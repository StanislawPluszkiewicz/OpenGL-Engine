#include "VAO.h"
#include <iostream>
#include <glm/vec3.hpp>


VAO::VAO()
{
	glGenVertexArrays(1, &id);
	indices_buffer = NULL;
	num_triangles = 0;
}


VAO::~VAO()
{
}

void VAO::storeIndices(std::vector<glm::ivec3> indices)
{
	if (indices_buffer)
		delete indices_buffer;

	indices_buffer = new VBO(GL_ELEMENT_ARRAY_BUFFER);

	bind();
	indices_buffer->bind();
	indices_buffer->setData(&indices[0], indices.size() * sizeof(glm::ivec3));
	unbind();

	num_triangles = indices.size();
}

void VAO::storeAttribute(Attribute c, int num_dimensions, GLvoid* data, int size_in_bytes, GLuint shader_location)
{
	if (attribute_buffers.find(c) != attribute_buffers.end())
		delete attribute_buffers[c];

	attribute_buffers[c] = new VBO(GL_ARRAY_BUFFER);

	bind();
	attribute_buffers[c]->bind();
	attribute_buffers[c]->setData(data, size_in_bytes);
	glVertexAttribPointer(shader_location, num_dimensions, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(shader_location);
	unbind();
}


void VAO::storePositions(std::vector<glm::vec3> data, int shader_location)
{
	storeAttribute(POSITION, 3, &data[0], data.size() * sizeof(glm::vec3), shader_location);
}

void VAO::storeNormals(std::vector<glm::vec3> data, int shader_location)
{
	storeAttribute(NORMAL, 3, &data[0], data.size() * sizeof(glm::vec3), shader_location);
}

void VAO::draw()
{
	bind();
	glDrawElements(GL_TRIANGLES, num_triangles * 3, GL_UNSIGNED_INT, 0);
	unbind();
}

void VAO::bind()
{
	glBindVertexArray(id);
}

void VAO::unbind()
{
	glBindVertexArray(0);
}