#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <map>

class myShader
{
public:
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint geometry_shader;

	GLuint shaderprogram;

	std::map<std::string, GLint> text_to_id;

	myShader(std::string file_vertexshader, std::string file_fragmentshader);
	~myShader();

	void clear();
	void start();
	void stop();

	GLint getUniformLocation(std::string name);

	void setUniform(std::string name, glm::mat4 &);
	void setUniform(std::string name, glm::mat3 &);
	void setUniform(std::string name, float);
	void setUniform(std::string name, int);
	void setUniform(std::string name, glm::vec2);
	void setUniform(std::string name, glm::vec3);
	void setUniform(std::string name, glm::vec4);

private:
	GLuint _initShader(GLenum type, std::string filename);
	bool _initProgram();
	void _programErrors(const GLint program);
	void _shaderErrors(const GLint shader);
};


