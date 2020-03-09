#pragma once
#include <GL/glew.h>
#include <map>
#include <vector>
#include "VBO.h"
#include <glm/vec3.hpp>

class VAO
{
private:
	enum Attribute { POSITION, NORMAL, TANGENT, TEXTURECOORD };

public:
	GLuint id;

	std::map<Attribute, VBO *> attribute_buffers;
	VBO *indices_buffer;
	int num_triangles;

	VAO();
	~VAO();

	void storeAttribute(Attribute c, int num_dimensions, GLvoid *data, int size_in_bytes, GLuint shader_location);

	void storeIndices(std::vector<glm::ivec3>);
	void storePositions(std::vector<glm::vec3>, int shader_location);
	void storeNormals(std::vector<glm::vec3>, int shader_location);

	void draw();

	void bind();
	void unbind();
};
