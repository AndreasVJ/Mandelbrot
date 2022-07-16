#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shaders/shader.h"


float windowWidth = 900;
float windowHeight = 600;

double previousMouseX = 0;
double previousMouseY = 0;

float realAxisOffset = -0.7;
float imaginaryAxisOffset = 0;
float zoom = 1.0;
float zoomFactor = 1.5;

float realAxisMousePosition;
float imaginaryAxisMousePosition;

bool render = true;
bool firstClick = true;

float vertices[] = {
   -1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,
   -1.0f,  1.0f, 0.0f,
};

unsigned int indices[] = {
	0, 1, 2,
	0, 3, 2
};

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
	windowWidth = width;
	windowHeight = height;
	glViewport(0, 0, width, height);
	render = true;
}

static void cursor_position_callback(GLFWwindow* window, double x, double y){

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

		// Make mouse wrap around window when dragging
		if (x <= 0) {
			x += windowWidth;
			previousMouseX += windowWidth;
		}
		else if (x >= windowWidth-1) {
			x -= windowWidth;
			previousMouseX -= windowWidth;
		}
		if (y <= 0) {
			y += windowHeight;
			previousMouseY += windowHeight;
		}
		else if (y >= windowHeight-1) {
			y -= windowHeight;
			previousMouseY -= windowHeight;
		}

		glfwSetCursorPos (window, x, y);

		realAxisOffset -= 2*windowWidth/windowHeight*((x-previousMouseX+windowWidth/2)/windowWidth-0.5)/zoom;
		imaginaryAxisOffset += 2*((y-previousMouseY+windowHeight/2)/windowHeight-0.5)/zoom;

		render = true;
	}

	previousMouseX = x;
	previousMouseY = y;
}

void scrollCallback(GLFWwindow* window, double xOffset, double yOffset) {

	render = true;

	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	realAxisMousePosition = realAxisOffset + 2*windowWidth/windowHeight*(xpos/windowWidth-0.5)/zoom;
	imaginaryAxisMousePosition = imaginaryAxisOffset - 2*(ypos/windowHeight-0.5)/zoom;

	realAxisOffset = realAxisMousePosition;
	imaginaryAxisOffset = imaginaryAxisMousePosition;

	if (yOffset == 1) {
		zoom *= zoomFactor;
	}
	else {
		zoom /= zoomFactor;
	}

	realAxisMousePosition = realAxisOffset + 2*windowWidth/windowHeight*(xpos/windowWidth-0.5)/zoom;
	imaginaryAxisMousePosition = imaginaryAxisOffset - 2*(ypos/windowHeight-0.5)/zoom;

	realAxisOffset -= realAxisMousePosition - realAxisOffset;
	imaginaryAxisOffset -= imaginaryAxisMousePosition - imaginaryAxisOffset;
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		firstClick = true;
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		realAxisOffset = -0.7;
		imaginaryAxisOffset = 0;
		zoom = 1.0;
		zoomFactor = 1.5;
		render = true;
	}
}

int main() {

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Mandelbrot", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, windowWidth, windowHeight);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetScrollCallback(window, scrollCallback);


	Shader shader("src/shaders/default.vs", "src/shaders/default.fs");
	glUseProgram(shader.ID);


	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// Place vertices in a buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Place indices in a buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Specify how to read vertex data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind vertex buffer and array
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	double previousTime = 0;
	double currentTime;


	while (!glfwWindowShouldClose(window)) {

		currentTime = glfwGetTime();
		const double deltaTime = currentTime - previousTime;
		if (deltaTime > 1 / 60) {

			processInput(window);
			previousTime = currentTime;

			shader.setFloat("windowWidth", windowWidth);
			shader.setFloat("windowHeight", windowHeight);

			shader.setFloat("realAxisOffset", realAxisOffset);
			shader.setFloat("imaginaryAxisOffset", imaginaryAxisOffset);
			shader.setFloat("zoom", zoom);

			if (render == true) {
				render = false;
				glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT);

				glDrawElements(GL_TRIANGLES, sizeof(indices)/(sizeof(indices[0])), GL_UNSIGNED_INT, 0);

				glfwSwapBuffers(window);
			}

		}

		glfwPollEvents();
	}


	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	shader.deleteShader();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}