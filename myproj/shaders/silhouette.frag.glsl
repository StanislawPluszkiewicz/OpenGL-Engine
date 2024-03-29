#version 330 core

out vec4 output_color;

uniform mat4 mymodel_matrix;
uniform mat4 myview_matrix;
uniform mat3 mynormal_matrix;

in vec4 v_modelspace;
in vec3 v_normal;

void main (void)
{   
	vec4 model_eyespace = myview_matrix * mymodel_matrix * v_modelspace;
	
	vec3 normal = normalize(mynormal_matrix * v_normal);
	vec3 model_eyespace3 = model_eyespace.xyz / model_eyespace.w;

	if (abs(dot(-model_eyespace3, normal)) < 0.2f)
	{
		output_color = vec4(1, 1, 1, 1);
	}
	else
	{
		output_color = vec4(0, 0, 0, 1);
	}
	

}