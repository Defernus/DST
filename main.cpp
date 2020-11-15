#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>
#include <thread>
#include <chrono>

#include "input/input_handler.h"
#include "input/input_events.h"

#include "utils/shader.h"
#include "utils/logger.h"

#include "context.h"

void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
	ctx::width = w;
	ctx::height = h;
	glViewport(0, 0, w, h);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	ctx::scroll_x += xoffset;
	ctx::scroll_y += yoffset;
	if(ctx::scroll_x < 1)ctx::scroll_x=1.;
	if(ctx::scroll_y < 1)ctx::scroll_y=1.;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    ctx::is_lmb_pressed = button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS;
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	double mx = xpos/ctx::width*2. - 1.;
	double my = 1. - ypos/ctx::height*2.;
	if(ctx::is_lmb_pressed) {
		ctx::rot_yaw += mx - ctx::mouse_x;
		ctx::rot_pitch -= my - ctx::mouse_y;
	}
	ctx::mouse_x = mx;
	ctx::mouse_y = my;
}

int main()
{
	ctx::logger.set("runtime.log");

	if (!glfwInit())
	{
		ctx::logger << lg::err << "failed to initialize GLFW";
		return -1;
	}
	ctx::logger << lg::inf << "GLFW success initialized" << lg::endl;


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	ctx::window = glfwCreateWindow(ctx::width, ctx::height, "Defernus's shader toy", nullptr, nullptr);
	if (!ctx::window)
	{
		ctx::logger << lg::err << "failed to create window";
		return -1;
	}
	ctx::logger << lg::inf << "window success created" << lg::endl;

	glfwMakeContextCurrent(ctx::window);
	glfwSetMouseButtonCallback(ctx::window, mouse_button_callback);
	glfwSetScrollCallback(ctx::window, scroll_callback);
	glfwSetCursorPosCallback(ctx::window, cursor_position_callback);
	glfwSetFramebufferSizeCallback(ctx::window, framebuffer_size_callback);
	glfwGetWindowSize(ctx::window, &ctx::width, &ctx::height);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		ctx::logger << lg::err << "failed to initialize GLAD" << lg::endl;
		return -1;
	}
	ctx::logger << lg::inf << "GLAD success initialized" << lg::endl;

	inp::shader_reload_event.reload();

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

	ctx::start_time = glfwGetTime();
	ctx::timer = ctx::start_time;

	initInputHandler(ctx::window);

	inp::registerAllEvents();

	printHelp();

	double last_frame = glfwGetTime();
	double second_timer = 0;
	int fps_counter = 0;

	double min_spf = 1. / 60.;

	while (!glfwWindowShouldClose(ctx::window))
	{
		double current_frame = glfwGetTime();
		ctx::delta_time = current_frame - last_frame;
		last_frame = current_frame;

		second_timer += ctx::delta_time;
		++fps_counter;
		if (second_timer > 1)
		{
			ctx::fps = fps_counter / second_timer;
			fps_counter = 0;
			second_timer = 0;
		}

		if (inp::fps_lock_event.is_fps_locked && ctx::delta_time < min_spf)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(long((min_spf - ctx::delta_time) * 1000000L)));
		}

		processInput(ctx::window);

		if (inp::pause_event.is_timer_run)
		{
			ctx::timer =  glfwGetTime() - (ctx::start_time + ctx::time_on_pause);
		}

		std::string title =
			  "Defernus's shader toy FPS: " + std::to_string(ctx::fps)
			+ " timer: " + std::to_string(ctx::timer)
			+ " res: " + std::to_string(ctx::width) + "x" + std::to_string(ctx::height);
		glfwSetWindowTitle(ctx::window, title.c_str());


		sh::setVec2(ctx::shader, "WIN_SIZE", glm::vec2(ctx::width, ctx::height));
		sh::setVec2(ctx::shader, "MOUSE", glm::vec2(ctx::mouse_x, ctx::mouse_y));
		sh::setVec2(ctx::shader, "SCROLL", glm::vec2(ctx::scroll_x, ctx::scroll_y));

		sh::setVec3(ctx::shader, "POSITION", glm::vec3(ctx::pos_x, ctx::pos_y, ctx::pos_z));
		sh::setVec3(ctx::shader, "ROTATION", glm::vec3(ctx::rot_yaw, ctx::rot_pitch, ctx::rot_roll));

		sh::setFloat(ctx::shader, "TIME", ctx::timer);


		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(ctx::window);
		glfwPollEvents();
	}

	glfwTerminate();

	ctx::logger << lg::inf << "finished";

	return 0;
}