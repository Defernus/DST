#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <thread>
#include <chrono>

#include "input_handler.h"

#include "utils/shader.h"
#include "utils/logger.h"

int width = 900;
int height = 600;

float start_time;
float time_on_pause = 0.0;
float paused_time;

lg::Logger logger;

sh::Shader shader;

GLFWwindow* window;

float delta_time;
int fps;

float timer;

void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
	width = w;
	height = h;
	glViewport(0, 0, w, h);
}

void reloadShaders()
{
	shader = sh::loadShader(logger, "shaders/vertex.glsl", "shaders/fragment.glsl");
	glUseProgram(shader);
	logger << lg::inf << "shader reloaded" << lg::endl;
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
		time_on_pause = 0.0;
		timer = 0.0;
		paused_time = glfwGetTime();
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
	bool is_timer_run = true;

	PauseKE() : KeyEvent(GLFW_KEY_SPACE) {}
	void onJustPressed()
	{
		if (is_timer_run)
		{
			logger << lg::inf << "timer paused" << lg::endl;
			paused_time = glfwGetTime();
		}
		else
		{
			logger << lg::inf << "timer unpaused" << lg::endl;
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

class FPSLockKE :KeyEvent
{
public:
	bool is_fps_locked = true;

	FPSLockKE() : KeyEvent(GLFW_KEY_F2) {}
	void onJustPressed()
	{
		if (is_fps_locked)logger << lg::inf << "fps unlocked" << lg::endl;
		if (!is_fps_locked)logger << lg::inf << "fps locked" << lg::endl;
		is_fps_locked = !is_fps_locked;
	}
	void onPressed() {}
	void onReleased() {}

	const char* getDiscription() const
	{
		return "[f2] to switch fps lock";
	}
}fps_lock_event;

int main()
{
	logger.set("runtime.log");

	if (!glfwInit())
	{
		logger << lg::err << "failed to initialize GLFW";
		return -1;
	}
	logger << lg::inf << "GLFW success initialized" << lg::endl;


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, "Defernus's shader toy", nullptr, nullptr);
	if (!window)
	{
		logger << lg::err << "failed to create window";
		return -1;
	}
	logger << lg::inf << "window success created" << lg::endl;

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwGetWindowSize(window, &width, &height);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		logger << lg::err << "failed to initialize GLAD" << lg::endl;
		return -1;
	}
	logger << lg::inf << "GLAD success initialized" << lg::endl;

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

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, (void*)0);

	start_time = glfwGetTime();
	timer = start_time;

	initInputHandler(window);

	addEvent((KeyEvent*)&shader_reload_event);
	addEvent((KeyEvent*)&time_reset_event);
	addEvent((KeyEvent*)&help_event);
	addEvent((KeyEvent*)&exit_event);
	addEvent((KeyEvent*)&pause_event);
	addEvent((KeyEvent*)&fps_lock_event);

	printHelp();

	float last_frame = glfwGetTime();
	float second_timer = 0;
	int fps_counter = 0;

	float min_spf = 1.0f / 60.0f;

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

		if (fps_lock_event.is_fps_locked && delta_time < min_spf)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(long((min_spf - delta_time) * 1000000L)));
		}

		processInput(window);

		if (pause_event.is_timer_run)
		{
			timer =  glfwGetTime() - (start_time + time_on_pause);
		}

		std::string title = "Defernus's shader toy FPS: " + std::to_string(fps) + " timer: " + std::to_string(timer);
		glfwSetWindowTitle(window, title.c_str());

		sh::setVec2(shader, "WIN_SIZE", glm::vec2(width, height));
		sh::setFloat(shader, "TIME", timer);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	logger << lg::inf << "finished";

	return 0;
}