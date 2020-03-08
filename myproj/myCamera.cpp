#include <math.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>    

#include "helperFunctions.h"
#include "myCamera.h"

using namespace std;


myCamera::myCamera()
{
	camera_eye = glm::vec3(0.0f, 0.0f, 2.0f);
	camera_up = glm::vec3(0.0f, 1.0f, 0.0f);
	camera_forward = glm::vec3(0.0f, 0.0f, -1.0f);

	fovy = 45.0f;
	zNear = 0.1f;
	zFar = 2000.0f;

	window_width = 640;
	window_height = 480;
}

myCamera::~myCamera()
{
}

void myCamera::crystalball_rotateView(int dx, int dy)
{
	float vx = static_cast<float>(dx) / static_cast<float>(window_width);
	float vy = -static_cast<float>(dy) / static_cast<float>(window_height);

	float theta = 4.0f * (fabs(vx) + fabs(vy));

	glm::vec3 camera_right = glm::normalize(glm::cross(camera_forward, camera_up));
	glm::vec3 tomovein_direction = -camera_right * vx + -camera_up * vy;

	glm::vec3 rotation_axis = glm::normalize(glm::cross(tomovein_direction, camera_forward));

	rotate(camera_forward, rotation_axis, theta, true);
	rotate(camera_up, rotation_axis, theta, true);
	rotate(camera_eye, rotation_axis, theta, false);
}

void myCamera::firstperson_rotateView(int dx, int dy)
{
	float vx = static_cast<float>(dx) / static_cast<float>(window_width);
	float vy = -static_cast<float>(dy) / static_cast<float>(window_height);

	float theta = 4.0f * (fabs(vx) + fabs(vy));

	glm::vec3 camera_right = glm::normalize(glm::cross(camera_forward, camera_up));
	glm::vec3 tomovein_direction = -camera_right * vx + -camera_up * vy;

	glm::vec3 rotation_axis = glm::normalize(glm::cross(tomovein_direction, camera_forward));

	rotate(camera_forward, rotation_axis, theta, true);
	rotate(camera_up, rotation_axis, theta, true);
}

void myCamera::panView(int dx, int dy)
{
	float vx = static_cast<float>(dx) / static_cast<float>(window_width);
	float vy = -static_cast<float>(dy) / static_cast<float>(window_height);

	glm::vec3 camera_right = glm::normalize(glm::cross(camera_forward, camera_up));
	glm::vec3 tomovein_direction = -camera_right * vx + -camera_up * vy;

	camera_eye += 1.6f * tomovein_direction;
}

glm::vec3 myCamera::constructRay(int x, int y)
{
	glm::mat4 projection_matrix = projectionMatrix( );
	glm::mat4 view_matrix = viewMatrix();

	y = window_height - y;

	float x_c = (2.0f*x) / static_cast<float>(window_width) - 1.0f;
	float y_c = (2.0f*y) / static_cast<float>(window_height) - 1.0f;

	glm::vec4 tmp = glm::vec4(x_c, y_c, -1.0f, 1.0f);
	tmp = glm::inverse(projection_matrix) * tmp;
	tmp.z = -1.0f; tmp.w = 0.0f;

	return glm::normalize(glm::vec3(glm::inverse(view_matrix) * tmp));
}

glm::mat4 myCamera::projectionMatrix() const
{
	return glm::perspective(glm::radians(fovy), static_cast<float>(window_width) / static_cast<float>(window_height), zNear, zFar);
}

glm::mat4 myCamera::viewMatrix() const
{
	return glm::lookAt(camera_eye, camera_eye + camera_forward, camera_up);
}

void myCamera::moveForward(float size)
{
	//TODO
}

void myCamera::moveBack(float size)
{
	//TODO
}

void myCamera::turnLeft(float size)
{
	//TODO
}

void myCamera::turnRight(float size)
{
	//TODO
}