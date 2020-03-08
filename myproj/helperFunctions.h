#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>    
#include <string>

#include <GL/glew.h>    

//rotates inputvec around rotation_axis by angle theta in three dimensions.
static void rotate(glm::vec3 & inputvec, glm::vec3 rotation_axis, float theta, bool tonormalize = false)
{
	const float cos_theta = cos(theta);
	const float dot = glm::dot(inputvec, rotation_axis);
	glm::vec3 cross = glm::cross(inputvec, rotation_axis);

	inputvec.x *= cos_theta; inputvec.y *= cos_theta; inputvec.z *= cos_theta;
	inputvec.x += rotation_axis.x * dot * static_cast<float>(1.0 - cos_theta);
	inputvec.y += rotation_axis.y * dot * static_cast<float>(1.0 - cos_theta);
	inputvec.z += rotation_axis.z * dot * static_cast<float>(1.0 - cos_theta);

	inputvec.x -= cross.x * sin(theta);
	inputvec.y -= cross.y * sin(theta);
	inputvec.z -= cross.z * sin(theta);

	if (tonormalize) inputvec = glm::normalize(inputvec);
}

//Reads a .PPM image file and returns it in a byte array together with its image width and height.
static GLubyte * readPPMfile(char *filename, int & width, int & height)
{
	FILE *inFile;
	char buffer[100];
	unsigned char c;
	int maxVal;

	GLubyte *mytexture;
	int pixelsize;

	if ((inFile = fopen(filename, "rb")) == NULL) {
		return 0;
	}

	//Read file type identifier (magic number)
	fgets(buffer, sizeof(buffer), inFile);
	if ((buffer[0] != 'P') || (buffer[1] != '6')) {
		fprintf(stderr, "not a binary ppm file %s\n", filename);
		return 0;
	}

	if (buffer[2] == 'A')
		pixelsize = 4;
	else
		pixelsize = 3;

	//Read image size
	do fgets(buffer, sizeof(buffer), inFile);
	while (buffer[0] == '#');
	sscanf(buffer, "%d %d", &width, &height);

	//Read maximum pixel value (usually 255)
	do fgets(buffer, sizeof(buffer), inFile);
	while (buffer[0] == '#');
	sscanf(buffer, "%d", &maxVal);

	//Allocate RGBA texture buffer
	int memSize = width * height * 4 * sizeof(GLubyte);
	mytexture = new GLubyte[memSize];

	// read RGB data and set alpha value
	for (int i = 0; i < memSize; i++) {
		if ((i % 4) < 3 || pixelsize == 4) {
			c = fgetc(inFile);
			mytexture[i] = (GLubyte)c;
		}
		else mytexture[i] = (GLubyte)255; //Set alpha to opaque
	}
	fclose(inFile);
	return mytexture;
}