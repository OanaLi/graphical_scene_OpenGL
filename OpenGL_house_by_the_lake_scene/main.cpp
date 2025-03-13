//
//  main.cpp
//  OpenGL_Lighting_Example_step1
//
//  Created by CGIS on 21/11/16.
//  Copyright © 2016 CGIS. All rights reserved.
//

#if defined (__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
    #define GL_SILENCE_DEPRECATION
#else
    #define GLEW_STATIC
    #include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <algorithm> //???

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.hpp"
#include "Model3D.hpp"
#include "Camera.hpp"

#include <iostream>

int glWindowWidth = 900;
int glWindowHeight = 800;
int retina_width, retina_height;
GLFWwindow* glWindow = NULL;

gps::Model3D teapot, scene;

glm::mat4 model, view, projection;
GLuint modelLoc, viewLoc, projectionLoc; 
glm::mat3 normalMatrix;
GLuint normalMatrixLoc;

glm::vec3 lightDir, lightColor, viewPos, pointLightPos1, pointLightPos2, pointLightColor;
GLuint lightDirLoc, lightColorLoc, pointLightPos1Loc, pointLightPos2Loc, pointLightColorLoc;

gps::Camera myCamera(
	glm::vec3(0.8f, 0.6f, 0.4f),
	glm::vec3(0.0f, 0.0f, -10.0f),
	glm::vec3(0.0f, 1.0f, 0.0f)
);
GLfloat cameraSpeed = 0.05f;

bool pressedKeys[1024];
GLfloat angleY;
int wireframeView = 0;

gps::Shader myCustomShader;

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height)
{
	fprintf(stdout, "window resized to width: %d , and height: %d\n", width, height);
	//TODO
}


float presentationTime = 5.0f; //30s
glm::vec3 direction, newPosition;

std::vector<glm::vec3> points = {
	glm::vec3(10.8f, 18.6f, -0.4f),
	glm::vec3(24.3f, 9.7f, -0.4f),
	glm::vec3(10.7f, -11.6f, -0.4f),
	glm::vec3(0.4f, -18.7f, -0.4f),
	glm::vec3(-12.7f, -10.1f, -0.4f),
	glm::vec3(-6.2f, 4.2f, -0.4f)
};

void scenePresentation() {
	float distanceToTarget = length(myCamera.getCameraTarget() - myCamera.getCameraPosition());

	myCamera.setCameraPosition(points[0]);
	myCamera.setCameraTarget(points[1]);

	for (int i = 0; i < points.size() - 1; i++) {

		while (distanceToTarget > 0.1f) {
			myCamera.move(gps::MOVE_FORWARD, cameraSpeed);

			view = myCamera.getViewMatrix();
			normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

			myCustomShader.useShaderProgram();
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
			glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
			glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));
		}

		myCamera.setCameraPosition(points[i + 1]);
		myCamera.setCameraTarget(points[i + 2]);

	}
}


void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			pressedKeys[key] = true;
		else if (action == GLFW_RELEASE)
			pressedKeys[key] = false;
	}

	if (key == GLFW_KEY_M && action == GLFW_PRESS) {
		wireframeView = (wireframeView + 1) % 4;

		switch (wireframeView) {
		case 0:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //solid
			break;
		case 1:
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe
			break;
		case 2:
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); //polygonal
			break;
		case 3:
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //smooth
			glShadeModel(GL_SMOOTH); 
			break;
		}
	}

	if (key == GLFW_KEY_U && action == GLFW_PRESS) {
         scenePresentation();
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	
}

void processMovement()
{
	if (pressedKeys[GLFW_KEY_Q]) {  //rotire stanga
		myCamera.rotate(0.0f, 1.0f);
	}

	if (pressedKeys[GLFW_KEY_E]) {  //rotire dreapta
		myCamera.rotate(0.0f, -1.0f);
	}

	if (pressedKeys[GLFW_KEY_R]) {  //sus
		myCamera.rotate(1.0f, 0.0f);
	}

	if (pressedKeys[GLFW_KEY_T]) {  //jos
		myCamera.rotate(-1.0f, 0.0f);
	}


	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
	}

	view = myCamera.getViewMatrix();
	normalMatrix = glm::mat3(glm::inverseTranspose(view * model));

	myCustomShader.useShaderProgram();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));
}

bool initOpenGLWindow()
{
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //window scaling for HiDPI displays
    glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
    
    // for multisampling/antialising
    glfwWindowHint(GLFW_SAMPLES, 4);

	glWindow = glfwCreateWindow(glWindowWidth, glWindowHeight, "OpenGL Shader Example", NULL, NULL);
	if (!glWindow) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(glWindow, windowResizeCallback);
	glfwSetKeyCallback(glWindow, keyboardCallback);
	glfwSetCursorPosCallback(glWindow, mouseCallback);
	//glfwSetInputMode(glWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(glWindow);

	glfwSwapInterval(1);

#if not defined (__APPLE__)
    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();
#endif

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	//for RETINA display
	glfwGetFramebufferSize(glWindow, &retina_width, &retina_height);

	return true;
}

void initObjects()
{
	scene.LoadModel("objects/scene.obj", "textures/");
}

void initUniforms()
{
	myCustomShader.useShaderProgram();
	
	model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myCustomShader.shaderProgram, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myCustomShader.shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myCustomShader.shaderProgram, "normalMatrix");
	glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

	projection = glm::perspective(glm::radians(45.0f), (float)retina_width / (float)retina_height, 0.1f, 1000.0f);
	projectionLoc = glGetUniformLocation(myCustomShader.shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	//set the light direction
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightDir");
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view)) * lightDir));

	//set light color
	lightColor = glm::vec3(1.0f, 1.0f, 1.0f); //white light
	lightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "lightColor");
	glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));


	//set punctiform light direction
	pointLightPos1 = glm::vec3(5.0f, 5.0f, 5.0f);  
	pointLightPos1Loc = glGetUniformLocation(myCustomShader.shaderProgram, "pointLightPos1");
	glUniform3fv(pointLightPos1Loc, 1, glm::value_ptr(pointLightPos1));
	pointLightPos2 = glm::vec3(-5.0f, 5.0f, -5.0f);
	pointLightPos2Loc = glGetUniformLocation(myCustomShader.shaderProgram, "pointLightPos2");
	glUniform3fv(pointLightPos2Loc, 1, glm::value_ptr(pointLightPos2));


	// Setează valorile pentru lumini punctiforme
	pointLightColor = glm::vec3(1.0f, 1.0f, 1.0f);
	pointLightColorLoc = glGetUniformLocation(myCustomShader.shaderProgram, "pointLightColor");
	glUniform3fv(pointLightColorLoc, 1, glm::value_ptr(pointLightColor));


}

void initShaders()
{
	myCustomShader.loadShader("shaders/shaderPPL.vert", "shaders/shaderPPL.frag");
	myCustomShader.useShaderProgram();
}

void initOpenGLState()
{
	glClearColor(0.8, 0.8, 0.8, 1.0);
	glViewport(0, 0, retina_width, retina_height);

	glEnable(GL_DEPTH_TEST); // enable depth-testing	
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"

	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, retina_width, retina_height);
	scene.Draw(myCustomShader);
}



void cleanup() {

	glfwDestroyWindow(glWindow);
	glfwTerminate();
}

int main(int argc, const char * argv[]) {

	initOpenGLWindow();
	initOpenGLState();
	initObjects();
	initShaders();	
	initUniforms();

	
	while (!glfwWindowShouldClose(glWindow)) {
		processMovement(); 
		renderScene();

		glfwPollEvents();
		glfwSwapBuffers(glWindow);
	}

	cleanup();

	return 0;
}
