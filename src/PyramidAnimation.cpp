/*
 * PyramidAnimation.cpp
 *
 *  Created on: Aug 15, 2020
 *      Author: Admin
 */

// Header includes and namespace
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../SOIL2/SOIL2.h"
using namespace std;

// Window Title and Shader program macros
#define WINDOW_TITLE "Pyramid Animation"
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

// Defines for window properties, shader, buffer and array objects
GLint shaderProgram, WindowWidth = 800, WindowHeight = 600;
GLuint VBO, VAO, texture;

// Defines for functions
void UResizeWindow(int, int);
void URenderGraphics(void);
void UCreateShader(void);
void UCreateBuffers(void);
void UGenerateTexture(void);

// Vertex shader source code
const GLchar* vertexShaderSource = GLSL(330,
	layout(location = 0) in vec3 position;
	layout(location = 2) in vec2 textureCoordinates;
	out vec2 mobileTextureCoordinate;

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;
	void main() {
		gl_Position = projection * view * model * vec4(position, 1.0f);
		mobileTextureCoordinate = vec2(textureCoordinates.x, 1.0f - textureCoordinates.y);
	}
);

// Fragment shader source code
const GLchar* fragmentShaderSource = GLSL(330,
	in vec2 mobileTextureCoordinate;
	out vec4 gpuTexture;
	uniform sampler2D uTexture;
	void main() {
		gpuTexture = texture(uTexture, mobileTextureCoordinate);
	}
);

// Main function
int main(int argc, char* argv[]) {

	// Create OpenGL Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutCreateWindow(WINDOW_TITLE);

	// Set Window Resize callback
	glutReshapeFunc(UResizeWindow);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		cout << "Failed to initialize GLEW" << endl;
		return -1;
	}

	// Create Shader and Buffer objects
	UCreateShader();
	UCreateBuffers();
	UGenerateTexture();

	// Use shader program
	glUseProgram(shaderProgram);

	// Set clear color, display callback, and start main GLUT loop
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glutDisplayFunc(URenderGraphics);
	glutMainLoop();

	// Destroy Buffer objects
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	return 0;

}

// Resizes Window
void UResizeWindow(int w, int h) {
	WindowWidth = w;
	WindowHeight = h;
	glViewport(0, 0, WindowWidth, WindowHeight);
}

// Render Graphics
void URenderGraphics(void) {

	glEnable(GL_DEPTH_TEST); // Enables z-depth

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen

	glBindVertexArray(VAO); // Activate vertex array object

	glBindTexture(GL_TEXTURE_2D, texture);

	// Transforms model
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // Center model
	model = glm::rotate(model, glutGet(GLUT_ELAPSED_TIME) * -0.0005f, glm::vec3(1.0f, 1.0f, 1.0f));
	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

	// Transform camera
	glm::mat4 view;
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -5.0f)); // Move camera -0.2 in y, -2 in z

	// Creates perspective projection
	glm::mat4 projection;
	projection = glm::perspective(45.0f, (GLfloat)WindowWidth / (GLfloat)WindowHeight, 0.1f, 100.0f);

	// Send transformation to Vertex shader
	GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
	GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLuint projLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glutPostRedisplay();

	// Draws triangles
	glDrawArrays(GL_TRIANGLES, 0, 18);

	glBindVertexArray(0); // Deactive Vertex Array object

	glutSwapBuffers(); // Flips front and back buffers
}

void UCreateShader() {

	// Vertex Shader
	GLint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// Fragment Shader
	GLint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	// Shader Program
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	// Delete shaders once linked
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void UCreateBuffers() {

	// Position and Color data
	GLfloat vertices[] = {
		 // Vertex Positions   // Texture Coordinates
		  0.0f,  1.0f,  0.0f,     0.5f, 1.0f,
		 -0.5f,  0.0f,  0.5f,     1.0f, 0.0f,    // Left Side
		 -0.5f,  0.0f, -0.5f,     0.0f, 0.0f,

		  0.0f,  1.0f,  0.0f,     0.5f, 1.0f,
		 -0.5f,  0.0f, -0.5f,     1.0f, 0.0f,   // Back Side
		  0.5f,  0.0f, -0.5f,     0.0f, 0.0f,

		  0.0f,  1.0f,  0.0f,     0.5f, 1.0f,
		  0.5f,  0.0f, -0.5f,     1.0f, 0.0f,   // Right Side
		  0.5f,  0.0f,  0.5f,     0.0f, 0.0f,

		  0.0f,  1.0f,  0.0f,     0.5f, 1.0f,
		 -0.5f,  0.0f,  0.5f,     0.0f, 0.0f,    // Front Side
		  0.5f,  0.0f,  0.5f,     1.0f, 0.0f,

		 -0.5f,  0.0f,  0.5f,     0.0f, 1.0f,
		 -0.5f,  0.0f, -0.5f,     0.0f, 0.0f,    // Bottom Left
		  0.5f,  0.0f,  0.5f,     1.0f, 1.0f,
		 -0.5f,  0.0f, -0.5f,     0.0f, 0.0f,
		  0.5f,  0.0f, -0.5f,     1.0f, 0.0f,    // Bottom Right
		  0.5f,  0.0f,  0.5f,     1.0f, 1.0f,

	};

	// Generate buffer IDs
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Activate Vertex Array Object
	glBindVertexArray(VAO);

	// Activate VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Copy vertices to VBO

	// Set attribute pointer for position data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Set attribute pointer for texture data
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0); // Deactivate VAO
}

void UGenerateTexture() {

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	int width, height;
	unsigned char* image = SOIL_load_image("snhu.jpg", &width, &height, 0, SOIL_LOAD_RGB); // Load Texture File

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0); // Unbind Texture
}



