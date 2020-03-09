#include <math.h>
#include <GL/glew.h>
#include <vector>
#include <string>
#include <fstream>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>    

#include "myObject3D.h"
#include <iostream>
#include <sstream>
#include "myShader.h"

using namespace std;

myObject3D::myObject3D() {
	model_matrix = glm::mat4(1.0f);

	// m_Vertices.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	// m_Vertices.push_back(glm::vec3(0.5f, 0.0f, 0.0f));
	// m_Vertices.push_back(glm::vec3(0.5f, 0.5f, 0.0f));
	// 
	// m_Indices.push_back(glm::ivec3(0, 1, 2));

	assert(glBindBuffer != 0);
	assert(glBindVertexArray != 0);
	assert(glBufferData != 0);
	assert(glClear != 0);
	assert(glClearColor != 0);
	assert(glCullFace != 0);
	assert(glDepthFunc != 0);
	assert(glDeleteBuffers != 0);
	assert(glDeleteVertexArrays != 0);
	assert(glDisableVertexAttribArray != 0);
	assert(glDrawArrays != 0);
	assert(glEnable != 0);
	assert(glGenVertexArrays != 0);
	assert(glGenBuffers != 0);
	assert(glUseProgram != 0);
	assert(glUniformMatrix4fv != 0);
	assert(glVertexAttribPointer != 0);
	assert(glViewport != 0);
}

myObject3D::~myObject3D()
{
	clear();
}

void myObject3D::clear() {
	vector<glm::vec3> empty; m_Vertices.swap(empty);
	m_Normals.swap(empty);
	vector<glm::ivec3> empty2; m_Indices.swap(empty2);
}

bool myObject3D::readMesh(string filename)
{
	// clock_t start_time = std::clock();
	try
	{
		int verticies_count = 0, faces_count = 0;
		std::ifstream stream(filename);

		stream >> verticies_count >> faces_count;
		this->m_Vertices.reserve(verticies_count);
		this->m_Indices.reserve(faces_count);
		// std::cout << verticies_count << " " << faces_count << std::endl;

#ifdef IGNORE_QUADS_OBJ_FILES

		char c = 0;
		float x, y, z;

		do
		{
			stream >> c >> x >> y >> z;
			m_Vertices.emplace_back(x, y, z);
		} while (stream.good() && c == 'v');

		do
		{
			stream >> c >> x >> y >> z;
			// indicies in obj file start from 1
			m_Indices.emplace_back(x - 1, y - 1, z - 1);
		} while (stream.good() && c == 'f');
		
#else 
		char c = 0;
		float x, y, z;
		int xint, yint, zint, wint;
		std::string type, typeVertice = "v", typeFace = "f";
		bool firstFace = true;
		bool facesAreQuads = false;
		std::string line;
		while (std::getline(stream, line))
		{
			std::stringstream myline(line);
			myline >> type;
			if (type == typeVertice)
			{
				myline >> x >> y >> z;
				m_Vertices.emplace_back(x, y, z);
			}
			else if (type == typeFace)
			{
				if (firstFace)
				{
					int elementCount = 0;
					int pos = 0;
					while ((pos = line.find(" ")) != line.npos)
					{
						line = line.substr(pos + 1);
						++elementCount;
					}
					facesAreQuads = (elementCount == 4);
					firstFace = false;
					std::cout << facesAreQuads;
				}
				if (facesAreQuads)
				{
					myline >> xint >> yint >> zint >> wint;
					m_Indices.emplace_back(xint - 1, yint - 1, zint - 1);
					m_Indices.emplace_back(xint - 1, zint - 1, wint - 1);
				}
				else // triangles
				{
					myline >> xint >> yint >> zint;
					m_Indices.emplace_back(xint - 1, yint - 1, zint - 1);
				}
			}
			type = "";
		}
#endif
		stream.close();
		// std::cout << "Read file " << filename << " in " << std::clock() << " ms." << std::endl;
		return true;
	}
	catch (...)
	{
		std::cerr << "[ERROR]: Failed to read " << filename << "." << std::endl;
	}
	return false;
}

void myObject3D::normalize()
{
	unsigned int tmpxmin = 0, tmpymin = 0, tmpzmin = 0, tmpxmax = 0, tmpymax = 0, tmpzmax = 0;

	for (unsigned i = 0; i<m_Vertices.size(); i++) {
		if (m_Vertices[i].x < m_Vertices[tmpxmin].x) tmpxmin = i;
		if (m_Vertices[i].x > m_Vertices[tmpxmax].x) tmpxmax = i;

		if (m_Vertices[i].y < m_Vertices[tmpymin].y) tmpymin = i;
		if (m_Vertices[i].y > m_Vertices[tmpymax].y) tmpymax = i;

		if (m_Vertices[i].z < m_Vertices[tmpzmin].z) tmpzmin = i;
		if (m_Vertices[i].z > m_Vertices[tmpzmax].z) tmpzmax = i;
	}

	float xmin = m_Vertices[tmpxmin].x, xmax = m_Vertices[tmpxmax].x,
		ymin = m_Vertices[tmpymin].y, ymax = m_Vertices[tmpymax].y,
		zmin = m_Vertices[tmpzmin].z, zmax = m_Vertices[tmpzmax].z;

	float scale = ((xmax - xmin) <= (ymax - ymin)) ? (xmax - xmin) : (ymax - ymin);
	scale = (scale >= (zmax - zmin)) ? scale : (zmax - zmin);

	for (unsigned int i = 0; i<m_Vertices.size(); i++) {
		m_Vertices[i].x -= (xmax + xmin) / 2;
		m_Vertices[i].y -= (ymax + ymin) / 2;
		m_Vertices[i].z -= (zmax + zmin) / 2;

		m_Vertices[i].x /= scale;
		m_Vertices[i].y /= scale;
		m_Vertices[i].z /= scale;
	}
}

void myObject3D::computeNormals()
{
	std::map <int, std::set<int> > fpv; // Faces per vertex
	std::vector <glm::vec3> faceNormals;
	faceNormals.reserve(m_Vertices.size());
	m_Normals.reserve(m_Indices.size());

	for (int iCurrentFace = 0;
		iCurrentFace != this->m_Indices.size();
		++iCurrentFace)
	{
		glm::ivec3 current = this->m_Indices[iCurrentFace];

		auto fill = [&](int i) -> void
		{
			if (fpv.find(current[i]) == fpv.end())
				fpv[current[i]] = std::set<int>();
			fpv[current[i]].insert(iCurrentFace); // next 
		};
		fill(0);
		fill(1);
		fill(2);
		
		glm::vec3 face_normal = glm::cross(
			this->m_Vertices[current[1]] - this->m_Vertices[current[0]],
			this->m_Vertices[current[2]] - this->m_Vertices[current[1]]);
		face_normal = glm::normalize(face_normal);
		faceNormals.emplace_back(face_normal);
	}

	for (auto it = fpv.begin();
		it != fpv.end();
		std::advance(it, 1))
	{
		const std::set<int>& adjacent_faces = it->second;
		glm::vec3 average;
		for (auto setIt = adjacent_faces.begin();
			setIt != adjacent_faces.end();
			std::advance(setIt, 1))
		{
			int face_index = *setIt;
			average += faceNormals[face_index];
		}
		average /= (float) adjacent_faces.size();

		float norm = average.length();
		average = glm::normalize(average);
		// average.x /= norm;
		// average.y /= norm;
		// average.z /= norm;

		m_Normals.emplace_back(average);
	}
}

void myObject3D::createObjectBuffers()
{}

void myObject3D::computeTexturecoordinates_plane()
{}

void myObject3D::computeTexturecoordinates_cylinder()
{}

void myObject3D::displayObject(myShader *shader)
{
	shader->setUniform("mymodel_matrix", model_matrix);
	shader->setUniform("input_color", glm::vec4(1, 1, 0, 0));

	glBegin(GL_TRIANGLES);
	for (unsigned int i = 0; i < m_Indices.size(); ++i)
	{
		glVertex3fv(&m_Vertices[m_Indices[i][0]][0]);
		glVertex3fv(&m_Vertices[m_Indices[i][1]][0]);
		glVertex3fv(&m_Vertices[m_Indices[i][2]][0]);
	}
	glEnd();
}

void myObject3D::displayNormals(myShader *shader)
{
	shader->setUniform("mymodel_matrix", model_matrix);
	shader->setUniform("input_color", glm::vec4(0, 0, 1, 0));

	glBegin(GL_LINES);
	for (unsigned int i = 0; i < m_Vertices.size(); i++)
	{
		glm::vec3 v = m_Vertices[i] + m_Normals[i] / 10.0f;
		glVertex3fv(&m_Vertices[i][0]);
		glVertex3fv(&v[0]);
	}
	glEnd();
}

void myObject3D::translate(float x, float y, float z)
{
	glm::mat4 tmp = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
	model_matrix = tmp * model_matrix;
}

void myObject3D::rotate(float axis_x, float axis_y, float axis_z, float angle)
{
	glm::mat4 tmp = glm::rotate(glm::mat4(1.0f), static_cast<float>(angle), glm::vec3(axis_x, axis_y, axis_z));
	model_matrix = tmp * model_matrix;
}

glm::vec3 myObject3D::closestVertex(glm::vec3 ray, glm::vec3 starting_point)
{
	float min = std::numeric_limits<float>::max();
	unsigned int min_index = 0;

	ray = glm::normalize(ray);
	for (unsigned int i = 0;i < m_Vertices.size();i++)
	{
		float dotp = glm::dot(ray, m_Vertices[i] - starting_point);
		if (dotp < 0) continue;

		float oq = glm::distance(starting_point, m_Vertices[i]);
		float d = oq*oq - dotp*dotp;
		if (d < min)
		{
			min = d;
			min_index = i;
		}
	}
	return m_Vertices[min_index];
}

