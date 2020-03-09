#version 330 core

out vec4 output_color;

uniform mat4 mymodel_matrix;
uniform mat4 myview_matrix;
uniform mat3 mynormal_matrix;
uniform vec4 input_color;

in vec4 v_modelspace;
in vec3 v_normal;

void main (void)
{   
	mat4 model_eyespace = myview_matrix * mymodel_matrix * v_modelspace;
	
	mat3 normal = mynormal_matrix * v_normal;
	mat3 model_eyespace3 = model_eyespace.xyz / model_eyespace.w;

	if (abs(dot(-model_eyespace3, normal)) < 0.2f)
	{
		output_color = vec4(1, 1, 1, 1);
	}
	else
	{
		output_color = vec4(0, 0, 0, 1);
	}
	

}