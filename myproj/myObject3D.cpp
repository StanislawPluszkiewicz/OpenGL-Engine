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

	vertices.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
	vertices.push_back(glm::vec3(0.5f, 0.0f, 0.0f));
	vertices.push_back(glm::vec3(0.5f, 0.5f, 0.0f));

	indices.push_back(glm::ivec3(0, 1, 2));

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
	vector<glm::vec3> empty; vertices.swap(empty);
	normals.swap(empty);
	vector<glm::ivec3> empty2; indices.swap(empty2);
}

bool myObject3D::readMesh(string filename)
{
	//TODO
	return false;
}

void myObject3D::normalize()
{
	unsigned int tmpxmin = 0, tmpymin = 0, tmpzmin = 0, tmpxmax = 0, tmpymax = 0, tmpzmax = 0;

	for (unsigned i = 0; i<vertices.size(); i++) {
		if (vertices[i].x < vertices[tmpxmin].x) tmpxmin = i;
		if (vertices[i].x > vertices[tmpxmax].x) tmpxmax = i;

		if (vertices[i].y < vertices[tmpymin].y) tmpymin = i;
		if (vertices[i].y > vertices[tmpymax].y) tmpymax = i;

		if (vertices[i].z < vertices[tmpzmin].z) tmpzmin = i;
		if (vertices[i].z > vertices[tmpzmax].z) tmpzmax = i;
	}

	float xmin = vertices[tmpxmin].x, xmax = vertices[tmpxmax].x,
		ymin = vertices[tmpymin].y, ymax = vertices[tmpymax].y,
		zmin = vertices[tmpzmin].z, zmax = vertices[tmpzmax].z;

	float scale = ((xmax - xmin) <= (ymax - ymin)) ? (xmax - xmin) : (ymax - ymin);
	scale = (scale >= (zmax - zmin)) ? scale : (zmax - zmin);

	for (unsigned int i = 0; i<vertices.size(); i++) {
		vertices[i].x -= (xmax + xmin) / 2;
		vertices[i].y -= (ymax + ymin) / 2;
		vertices[i].z -= (zmax + zmin) / 2;

		vertices[i].x /= scale;
		vertices[i].y /= scale;
		vertices[i].z /= scale;
	}
}

void myObject3D::computeNormals()
{
	normals.assign(vertices.size(), glm::vec3(0.0f, 0.0f, 0.0f));
	//TODO
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
	for (unsigned int i = 0; i < indices.size(); ++i)
	{
		glVertex3fv(&vertices[indices[i][0]][0]);
		glVertex3fv(&vertices[indices[i][1]][0]);
		glVertex3fv(&vertices[indices[i][2]][0]);
	}
	glEnd();
}

void myObject3D::displayNormals(myShader *shader)
{
	shader->setUniform("mymodel_matrix", model_matrix);
	shader->setUniform("input_color", glm::vec4(0, 0, 1, 0));

	glBegin(GL_LINES);
	for (unsigned int i = 0; i < vertices.size(); i++)
	{
		glm::vec3 v = vertices[i] + normals[i] / 10.0f;
		glVertex3fv(&vertices[i][0]);
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
	for (unsigned int i = 0;i < vertices.size();i++)
	{
		float dotp = glm::dot(ray, vertices[i] - starting_point);
		if (dotp < 0) continue;

		float oq = glm::distance(starting_point, vertices[i]);
		float d = oq*oq - dotp*dotp;
		if (d < min)
		{
			min = d;
			min_index = i;
		}
	}
	return vertices[min_index];
}

