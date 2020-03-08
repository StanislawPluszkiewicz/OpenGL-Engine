#version 330 core

out vec4 output_color;

uniform vec4 input_color;

void main (void)
{   
	output_color = input_color;
}