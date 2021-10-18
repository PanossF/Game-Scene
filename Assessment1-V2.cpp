

#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <GLFW/glfw3.h>
GLFWwindow* window;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>	// for value_ptr used in lights array uniform
using namespace glm;

#include "common/model.h"
#include "common/shader.h"
#include "common/GLError.h"
#include "common/skybox.h"
#include "common/controls.hpp"
#include "common/Camera.h"
#include "common/InstancedModel.h"

void GLAPIENTRY
MessageCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

int main(void)
{



	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1920, 1080, "Assessment 1", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	check_gl_error();

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	check_gl_error();

	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);


	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwPollEvents();
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	check_gl_error();

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	check_gl_error();

	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	check_gl_error();


	// shader for our standard models with 1 diffuse texture
	Shader StandardShader("StandardShading.vertexshader", "StandardShading.fragmentshader");
	check_gl_error();

	Shader SimpleShading("SimpleShading.vertexshader", "SimpleShading.fragmentshader");
	check_gl_error();

	Shader In_SimpleShading("In_SimpleShading.vertexshader", "In_SimpleShading.fragmentshader");
	check_gl_error();
	// used to draw a full white light sphere!
	Shader lightShader("LightShading.vertexshader", "LightShading.fragmentshader");
	check_gl_error();

	// used by the skybox, basically uses cubemap texture to draw the correct albedo (diffuse colour)
	Shader skyboxShader("skybox.vertexshader", "skybox.fragmentshader");
	check_gl_error();

	//Skybox sky("xp.jpg", "xn.jpg", "yp.jpg", "yn.jpg", "zp.jpg", "zn.jpg");
	Skybox sky("../3dcontent/skybox/night/", "night_west.png", "night_east.png", "night_up.png", "night_down.png", "night_south.png", "night_north.png");

	// basically a sphere, but with a shader to make it entirely white
	Model lightModel2("../3dcontent/models/sphere/sphere.obj");

	Model terrain("../3dcontent/models/terrain/terrain.obj");
	terrain.SetPosition(glm::vec3(0, -8, 0));

	Model samurai("../3dcontent/models/samurai2/samurai2.obj");
	samurai.SetPosition(glm::vec3(1, 0.5, 1));
	samurai.SetScale(glm::vec3(20, 20, 20));
	

	Model gladiator("../3dcontent/models/gladiator/gladiator.obj");
	gladiator.SetPosition(glm::vec3(0, 0.5, -35));
	gladiator.SetScale(glm::vec3(3, 3, 3));
	gladiator.SetRotation(glm::quat(0,0,1,0));

	//Model water("../3dcontent/models/water/Ocean.obj");
	//water.SetScale(glm::vec3(10, 10, 10));
	//water.SetPosition(glm::vec3(-50, 1, -35));

	//Model rock("../3dcontent/models/rock/rock.obj");
	//rock.SetScale(glm::vec3(0.5, 0.5, 0.5));
	//rock.SetPosition(glm::vec3(10, 1, 10));

	InstancedModel flower("../3dcontent/models/grass/flower.obj");  //Instanced Model class for Instancing
	flower.SetScale(glm::vec3(7, 7, 7));

	lightModel2.GetMesh(0)->SetScale(glm::vec3(0.3f, 0.3f, 0.3f)); //moving lightmodel for samurai
	check_gl_error();
	Camera camera(4, 7, 3); //position.x, position.y, position.z
	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = camera.GetProjection();
	// Camera matrix
	glm::mat4 View = camera.GetView();

	check_gl_error();

	glm::vec4 light(0, 500, 0, 150000); //light for scene
	glm::vec4 light2(5, 1, 2, 25);  // light for samurai
	glm::vec4 light4(5, 1, 25, 95);  // light for gladiator
	glm::vec4 light3(-55, 30, -30, 1500); //light for water
	glm::vec3 lightColour(1, 1, 1);

	//declaring variables for the movement of lightModel2
	static float timeElapsed = 0.0f;

	double currentTime = glfwGetTime();
	double lastTime = currentTime;



	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Compute time difference between current and last frame
		currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);


		glm::vec3 lightpos2(light2);
		lightModel2.GetMesh(0)->SetPosition(lightpos2);

		//check_gl_error();
		camera.CameraMove(deltaTime);
		Projection = camera.GetProjection();
		View = camera.GetView();

		// skybox always comes first, so other models can overwrite colour buffer
		sky.Draw(View, Projection, skyboxShader);

		// now bind and use our "Standard phong lighting shader for both "normal" mat and the base mesh
		StandardShader.use();
		// global ambient light level for everything rendered with this shader
		StandardShader.setFloat("ambientlevel", 0.2f);
		StandardShader.setVec3("lightColour", lightColour);
		StandardShader.setVec4("light", light2);
		samurai.Render(View, Projection, StandardShader);

		StandardShader.setVec4("light", light4);
		gladiator.Render(View, Projection, StandardShader);

		//StandardShader.setVec4("light", light3);
		//water.Render(View, Projection, StandardShader);
		
		SimpleShading.use();
		SimpleShading.setFloat("ambientlevel", 0.2f);
		SimpleShading.setVec3("lightColour", lightColour);
		SimpleShading.setVec4("light", light);

		terrain.Render(View, Projection, SimpleShading);
		//rock.Render(View, Projection, SimpleShading);

		In_SimpleShading.use();
		In_SimpleShading.setFloat("ambientlevel", 0.5f);
		In_SimpleShading.setVec3("lightColour", lightColour);
		In_SimpleShading.setVec4("light", light);

		flower.Render(View, Projection, In_SimpleShading);

		// using a sphere for a light so we can vizualize the light position
		// it has a custom shader that basically emits solid white
		lightShader.use();
		lightModel2.Render(View, Projection, lightShader);

		// Swap buffers
		glfwSwapBuffers(window);

		// For the next frame, the "last time" will be "now"
		lastTime = currentTime;

		glfwPollEvents();

		//for changing the light position from inputs
		if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_PRESS)
		{
			light2.z -= 0.1f;
		}

		if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS)
		{
			light2.z += 0.1f;
		}

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

