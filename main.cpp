#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "utils/shader.h"
#include "utils/logger.h"

int width = 900;
int height = 600;

bool is_key_pressed[GLFW_KEY_LAST + 1];

Logger logger;

Shader shader;

void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, w, h);
}

void reloadShaders()
{
	shader = Shader(logger, "shaders/vertex.glsl", "shaders/fragment.glsl");
	shader.use();
	logger << "shader reloaded" << std::endl;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !is_key_pressed[GLFW_KEY_R])
	{
		is_key_pressed[GLFW_KEY_R] = true;
		reloadShaders();
	}
	else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
	{
		is_key_pressed[GLFW_KEY_R] = false;
	}
}

int main()
{
	for (bool &i : is_key_pressed) i = false;

	logger.set("runtime.log");

	if (!glfwInit())
	{
		logger << "failed to initialize GLFW";
		return -1;
	}
	logger << "GLFW success initialized" << std::endl;


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Defernus's shader toy", nullptr, nullptr);
	if (!window)
	{
		logger << "failed to create window";
		return -1;
	}
	logger << "window success created" << std::endl;

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwGetWindowSize(window, &width, &height);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		logger << "failed to initialize GLAD" << std::endl;
		return -1;
	}
	logger << "GLAD success initialized" << std::endl;

	reloadShaders();

	float vertices[]
	{
		-1.0, -1.0,
		 1.0, -1.0,
		-1.0,  1.0,
		 1.0, -1.0,
		-1.0,  1.0,
		 1.0,  1.0,
	};

	GLuint VAO, VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 12*sizeof(float), &vertices[0], GL_STATIC_DRAW);


	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, (void*)0);

	float start_time = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		shader.setFloat("width", width);
		shader.setFloat("height", height);
		shader.setFloat("time", glfwGetTime() - start_time);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}