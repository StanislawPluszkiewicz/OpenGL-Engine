#pragma once

#include <ctime>
#include <string>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

// speeds up the reading part but ignores obj files with quad faces
// #define IGNORE_QUADS_OBJ_FILES

class myShader;
class myCamera;

class myObject3D
{
public:
	std::vector<glm::vec3> m_Vertices;
	std::vector<glm::ivec3> m_Indices;
	std::vector<glm::vec3> m_Normals;

	glm::mat4 model_matrix;

	myObject3D();
	~myObject3D();
	void clear(); 
	bool readMesh(std::string filename);
	void normalize();
	void computeNormals();
	void createObjectBuffers();
	void computeTexturecoordinates_plane();
	void computeTexturecoordinates_cylinder();
	void displayObject(myShader *);
	void displayNormals(myShader *);
	void translate(float x, float y, float z);
	void rotate(float axis_x, float axis_y, float axis_z, float angle);
	glm::vec3 closestVertex(glm::vec3 ray, glm::vec3 starting_point);
};
