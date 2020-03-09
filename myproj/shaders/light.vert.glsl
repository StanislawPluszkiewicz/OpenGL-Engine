#version 330 core

layout(location = 0) in vec4 vertex_modelspace;
layout(location = 1) in vec3 vertex_normal;

uniform mat4 myprojection_matrix;
uniform mat4 myview_matrix;
uniform mat4 mymodel_matrix;

out vec4 v_modelspace;
out vec3 v_normal;

void main() {
    gl_Position = myprojection_matrix * myview_matrix * mymodel_matrix * vertex_modelspace; 
	v_modelspace = vertex_modelspace;
	v_normal = vertex_normal;
}
