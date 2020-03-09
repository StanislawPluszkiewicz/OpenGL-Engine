#include <fstream>
#include <string>
#include <vector>

#include <GL/glew.h>

#include <SDL2/SDL_main.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#undef main

#include "NFD/nfd.h"

#include "helperFunctions.h"

#include "myShader.h"
#include "myCamera.h"
#include "myObject3D.h"

#include <glm/glm.hpp>

using namespace std;

#define INITIAL_WINDOW_WIDTH 640
#define INITIAL_WINDOW_HEIGHT 480

// SDL variables
SDL_Window* window;
SDL_GLContext glContext;
 
int mouse_position[2];
bool mouse_button_pressed = false;
bool quit = false;
bool windowsize_changed = true;
bool crystalballorfirstperson_view = true;

// Camera parameters.
myCamera *cam1; 

// Mesh object
myObject3D *obj1;

//Shader
myShader *shader1;

//Point to draw to illustrate picking
glm::vec3 picked_point;

bool isSilhouetteOn = false;

// Process the event.  
void processEvents(SDL_Event current_event)
{
	switch (current_event.type)
	{
		// window close button is pressed
		case SDL_QUIT:
		{
			quit = true;
			break;
		}
		case SDL_KEYDOWN:
		{
			if (current_event.key.keysym.sym == SDLK_ESCAPE)
				quit = true;
			if (current_event.key.keysym.sym == SDLK_UP)
				cam1->moveForward(0.1f);
			if (current_event.key.keysym.sym == SDLK_DOWN)
				cam1->moveBack(0.1f);
			if (current_event.key.keysym.sym == SDLK_LEFT)
				cam1->turnLeft(0.1f);
			if (current_event.key.keysym.sym == SDLK_RIGHT)
				cam1->turnRight(0.1f);
			if (current_event.key.keysym.sym == SDLK_v)
				crystalballorfirstperson_view = !crystalballorfirstperson_view;
			else if (current_event.key.keysym.sym == SDLK_o)
			{
				nfdchar_t *outPath = NULL;
				nfdresult_t result = NFD_OpenDialog("obj", NULL, &outPath);
				if (result != NFD_OKAY) return;
				myObject3D *obj_tmp = new myObject3D();
				if (!obj_tmp->readMesh(outPath))
				{
					delete obj_tmp;
					return;
				}
				delete obj1;
				obj1 = obj_tmp;
				obj1->computeNormals();
				obj1->createObjectBuffers();
			}
			else if (current_event.key.keysym.sym == SDLK_s)
			{
	

				isSilhouetteOn = !isSilhouetteOn;
			}
			break;
		}
		case SDL_MOUSEBUTTONDOWN:
		{
			mouse_position[0] = current_event.button.x;
			mouse_position[1] = current_event.button.y;
			mouse_button_pressed = true;

			const Uint8 *state = SDL_GetKeyboardState(NULL);
			if (state[SDL_SCANCODE_LCTRL])
			{
				glm::vec3 ray = cam1->constructRay(mouse_position[0], mouse_position[1]);
				picked_point = obj1->closestVertex(ray, cam1->camera_eye);
			}
			break;
		}
		case SDL_MOUSEBUTTONUP:
		{
			mouse_button_pressed = false;
			break;
		}
		case SDL_MOUSEMOTION:
		{
			int x = current_event.motion.x;
			int y = current_event.motion.y;

			int dx = x - mouse_position[0];
			int dy = y - mouse_position[1];

			mouse_position[0] = x;
			mouse_position[1] = y;

			if ((dx == 0 && dy == 0) || !mouse_button_pressed) return;

			if ( (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) && crystalballorfirstperson_view)
				cam1->crystalball_rotateView(dx, dy);
			else if ((SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) && !crystalballorfirstperson_view)
				cam1->firstperson_rotateView(dx, dy);
			else if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_RIGHT))
				cam1->panView(dx, dy);

			break;
		}
		case SDL_WINDOWEVENT:
		{
			if (current_event.window.event == SDL_WINDOWEVENT_RESIZED)
				windowsize_changed = true;
			break;
		}
		case SDL_MOUSEWHEEL:
		{
			if (current_event.wheel.y < 0)
				cam1->moveBack(0.1f);
			else if (current_event.wheel.y > 0)
				cam1->moveForward(0.1f);
			break;
		}
		default : 
			break;
	}
}

int main(int argc, char *argv[])
{
	// Using OpenGL 3.1 core
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	// Initialize video subsystem
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);
	// Create window
	window = SDL_CreateWindow("IN4I24", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	// Create OpenGL context
	glContext = SDL_GL_CreateContext(window);

	// Initialize glew
	glewInit();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);

	// Setting up OpenGL shaders
	shader1 = new myShader("shaders/light.vert.glsl", "shaders/light.frag.glsl");
	myShader * shader2 = new myShader("shaders/light.vert.glsl", "shaders/silhouette.frag.glsl");
	myShader * shaderTmp;
	// Read up the scene
	obj1 = new myObject3D();
	obj1->readMesh("apple.obj"); 
	obj1->computeNormals();       
	obj1->createObjectBuffers();

	cam1 = new myCamera();

	SDL_GetWindowSize(window, &cam1->window_width, &cam1->window_height);

	// display loop
	while (!quit)
	{
		if (windowsize_changed)
		{
			SDL_GetWindowSize(window, &cam1->window_width, &cam1->window_height);
			windowsize_changed = false;
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, cam1->window_width, cam1->window_height);

		glm::mat4 projection_matrix = cam1->projectionMatrix();
		glm::mat4 view_matrix = cam1->viewMatrix();

		if (isSilhouetteOn)
		{
			shaderTmp = shader2;
			shaderTmp->start();
			shaderTmp->setUniform("myview_matrix", view_matrix);
			shaderTmp->setUniform("myprojection_matrix", projection_matrix);
			shaderTmp->setUniform("mynormal_matrix", glm::transpose(glm::inverse(glm::mat3(view_matrix))));
		}
		else 
		{
			shaderTmp = shader1;
			shaderTmp->start();
			shaderTmp->setUniform("input_color", glm::vec4(1, 1, 0, 0));
			shaderTmp->setUniform("myview_matrix", view_matrix);
			shaderTmp->setUniform("myprojection_matrix", projection_matrix);
		}

		obj1->displayObject(shaderTmp);
		obj1->displayNormals(shaderTmp);

		glPointSize(6.0f);
		glBegin(GL_POINTS);
		 glVertex3fv(&picked_point[0]);
  	    glEnd();

		SDL_GL_SwapWindow(window);

		SDL_Event current_event;
		while (SDL_PollEvent(&current_event) != 0)
			processEvents(current_event);
	}
	
	// Freeing resources before exiting.
	// Destroy window
	if (glContext) SDL_GL_DeleteContext(glContext);
	if (window) SDL_DestroyWindow(window);

	// Quit SDL subsystems
	SDL_Quit();

	return 0;
}