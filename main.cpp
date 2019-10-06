#include <glad/glad.h>

#include "input_handler.h"

#include "utils/shader.h"
#include "utils/logger.h"

int width = 900;
int height = 600;

float start_time;

Logger logger;

Shader shader;

GLFWwindow* window;

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

void printHelp()
{
	std::cout <<
		"===========HELP===========\n" <<
		"| [r] to reload shaders  |\n" <<
		"| [t] to reset time      |\n" <<
		"| [f1] to print this     |\n" <<
		"| [esc] to exit          |\n" <<
		"==========================" << std::endl;
}

class ShaderReloadKE :KeyEvent
{
public:
	ShaderReloadKE() : KeyEvent(GLFW_KEY_R) {}
	void onJustPressed()
	{
		reloadShaders();
	}
	void onPressed() {}
	void onReleased() {}
}shader_reload_event;

class TimeResetKE :KeyEvent
{
public:
	TimeResetKE() : KeyEvent(GLFW_KEY_T) {}
	void onJustPressed()
	{
		start_time = glfwGetTime();
		logger << "time reseted" << std::endl;
	}
	void onPressed() {}
	void onReleased() {}
}time_reset_event;

class ExitKE :KeyEvent
{
public:
	ExitKE() : KeyEvent(GLFW_KEY_ESCAPE) {}
	void onJustPressed()
	{
		glfwSetWindowShouldClose(window, true);
	}
	void onPressed() {}
	void onReleased() {}
}exit_event;

class HelpKE :KeyEvent
{
public:
	HelpKE() : KeyEvent(GLFW_KEY_F1) {}
	void onJustPressed()
	{
		printHelp;
	}
	void onPressed() {}
	void onReleased() {}
}help_event;

int main()
{
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

	window = glfwCreateWindow(width, height, "Defernus's shader toy", nullptr, nullptr);
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

	start_time = glfwGetTime();

	initInputHandler(window);

	addEvent((KeyEvent*)&shader_reload_event);
	addEvent((KeyEvent*)&time_reset_event);
	addEvent((KeyEvent*)&help_event);
	addEvent((KeyEvent*)&exit_event);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		shader.setFloat("WIDTH", width);
		shader.setFloat("HEIGHT", height);
		shader.setFloat("TIME", glfwGetTime() - start_time);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	logger << "finished";

	return 0;
}