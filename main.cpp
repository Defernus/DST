#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

#include "input_handler.h"

#include "utils/shader.h"
#include "utils/logger.h"

int width = 900;
int height = 600;

float start_time;
float time_on_pause = 0.0;
float paused_time;

Logger logger;

Shader shader;

GLFWwindow* window;

float delta_time;
int fps;

float time;

bool is_timer_run = true;


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

	const char* getDiscription() const
	{
		return "[r] to reload shaders";
	}
}shader_reload_event;

class TimeResetKE :KeyEvent
{
public:
	TimeResetKE() : KeyEvent(GLFW_KEY_T) {}
	void onJustPressed()
	{
		start_time = glfwGetTime();
	}
	void onPressed() {}
	void onReleased() {}

	const char* getDiscription() const
	{
		return "[t] to reset time";
	}
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

	const char* getDiscription() const
	{
		return "[esc] to exit";
	}
}exit_event;

class HelpKE :KeyEvent
{
public:
	HelpKE() : KeyEvent(GLFW_KEY_F1) {}
	void onJustPressed()
	{
		printHelp();
	}
	void onPressed() {}
	void onReleased() {}

	const char* getDiscription() const
	{
		return "[f1] to print this";
	}
}help_event;

class PauseKE :KeyEvent
{
public:
	PauseKE() : KeyEvent(GLFW_KEY_SPACE) {}
	void onJustPressed()
	{
		if (is_timer_run)
		{
			paused_time = glfwGetTime();
		}
		else
		{
			time_on_pause += glfwGetTime() - paused_time;
		}

		is_timer_run = !is_timer_run;
	}
	void onPressed() {}
	void onReleased() {}

	const char* getDiscription() const
	{
		return "[space] to pause";
	}
}pause_event;

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
	time = start_time;

	initInputHandler(window);

	addEvent((KeyEvent*)&shader_reload_event);
	addEvent((KeyEvent*)&time_reset_event);
	addEvent((KeyEvent*)&help_event);
	addEvent((KeyEvent*)&exit_event);
	addEvent((KeyEvent*)&pause_event);

	printHelp();

	float last_frame = glfwGetTime();
	float second_timer = 0;
	int fps_counter = 0;

	while (!glfwWindowShouldClose(window))
	{
		float current_frame = glfwGetTime();
		delta_time = current_frame - last_frame;
		last_frame = current_frame;

		second_timer += delta_time;
		++fps_counter;
		if (second_timer > 1)
		{
			fps = fps_counter / second_timer;
			fps_counter = 0;
			second_timer = 0;
		}

		processInput(window);

		if (is_timer_run)
		{
			time =  glfwGetTime() - (start_time + time_on_pause);
		}

		std::string title = "Defernus's shader toy FPS: " + std::to_string(fps) + " timer: " + std::to_string(time);
		glfwSetWindowTitle(window, title.c_str());

		shader.setVec2("WIN_SIZE", glm::vec2(width, height));
		shader.setFloat("TIME", time);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	logger << "finished";

	return 0;
}