
#pragma once

#include <functional>
#include <math.h>

#include "../context.h"

namespace inp {
	class ShaderReloadKE :KeyEvent
	{
	public:
		ShaderReloadKE() : KeyEvent(GLFW_KEY_R) {}
		void onJustPressed()
		{
			reload();
		}
		void onPressed() {}
		void onReleased() {}

		void reload()
		{
			ctx::shader = sh::loadShader(ctx::logger, "shaders/vertex.glsl", "shaders/fragment.glsl");
			glUseProgram(ctx::shader);
			ctx::logger << lg::inf << "shader reloaded" << lg::endl;
		}

		const char* getDiscription() const
		{
			return "[r] to reload shaders";
		}
	} shader_reload_event;

	class TimeResetKE :KeyEvent
	{
	public:
		TimeResetKE() : KeyEvent(GLFW_KEY_T) {}
		void onJustPressed()
		{
			ctx::time_on_pause = 0.0;
			ctx::timer = 0.0;
			ctx::paused_time = glfwGetTime();
			ctx::start_time = glfwGetTime();
		}
		void onPressed() {}
		void onReleased() {}

		const char* getDiscription() const
		{
			return "[t] to reset time";
		}
	} time_reset_event;

	class ExitKE :KeyEvent
	{
	public:
		ExitKE() : KeyEvent(GLFW_KEY_ESCAPE) {}
		void onJustPressed()
		{
			glfwSetWindowShouldClose(ctx::window, true);
		}
		void onPressed() {}
		void onReleased() {}

		const char* getDiscription() const
		{
			return "[esc] to exit";
		}
	} exit_event;

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
	} help_event;

	class PauseKE :KeyEvent
	{
	public:
		bool is_timer_run = true;

		PauseKE() : KeyEvent(GLFW_KEY_SPACE) {}
		void onJustPressed()
		{
			ctx::logger << lg::inf << "keeek" << lg::endl;
			if (is_timer_run)
			{
				ctx::logger << lg::inf << "timer paused" << lg::endl;
				ctx::paused_time = glfwGetTime();
			}
			else
			{
				ctx::logger << lg::inf << "timer unpaused" << lg::endl;
				ctx::time_on_pause += glfwGetTime() - ctx::paused_time;
			}

			is_timer_run = !is_timer_run;
		}
		void onPressed() {}
		void onReleased() {}

		const char* getDiscription() const
		{
			return "[space] to pause";
		}
	} pause_event;

	class FPSLockKE :KeyEvent
	{
	public:
		bool is_fps_locked = true;

		FPSLockKE() : KeyEvent(GLFW_KEY_F2) {}
		void onJustPressed()
		{
			if (is_fps_locked)ctx::logger << lg::inf << "fps unlocked" << lg::endl;
			if (!is_fps_locked)ctx::logger << lg::inf << "fps locked" << lg::endl;
			is_fps_locked = !is_fps_locked;
		}
		void onPressed() {}
		void onReleased() {}

		const char* getDiscription() const
		{
			return "[f2] to switch fps lock";
		}
	} fps_lock_event;

	class PressedKE :KeyEvent
	{
	public:
		PressedKE(const int &key, std::function<void()> cb ) : KeyEvent(key), cb(cb) {}
		void onJustPressed() {}
		void onPressed()
		{
			cb();
		}
		void onReleased() {}

		const char* getDiscription() const
		{
			return "";
		}
	private:
		std::function<void()> cb;
	}
	move_forward_event(GLFW_KEY_W, [](){
		double factor = ctx::scroll_y * .1 + 1.;
		factor = pow(factor, factor);
		ctx::cam.move(ctx::cam.getFront() * glm::vec3(ctx::delta_time/factor));
	}),
	move_back_event(GLFW_KEY_S, [](){
		double factor = ctx::scroll_y * .1 + 1.;
		factor = pow(factor, factor);
		ctx::cam.move(ctx::cam.getFront() * glm::vec3(-ctx::delta_time/factor));
	}),
	move_right_event(GLFW_KEY_D, [](){
		double factor = ctx::scroll_y * .1 + 1.;
		factor = pow(factor, factor);
		ctx::cam.move(ctx::cam.getRight() * glm::vec3(ctx::delta_time/factor));
	}),
	move_left_event(GLFW_KEY_A, [](){
		double factor = ctx::scroll_y * .1 + 1.;
		factor = pow(factor, factor);
		ctx::cam.move(ctx::cam.getRight() * glm::vec3(-ctx::delta_time/factor));
	}),
	move_up_event(GLFW_KEY_E, [](){
		double factor = ctx::scroll_y * .1 + 1.;
		factor = pow(factor, factor);
		ctx::cam.move(ctx::cam.getTop() * glm::vec3(ctx::delta_time/factor));
	}),
	move_down_event(GLFW_KEY_Q, [](){
		double factor = ctx::scroll_y * .1 + 1.;
		factor = pow(factor, factor);
		ctx::cam.move(ctx::cam.getTop() * glm::vec3(-ctx::delta_time/factor));
	});

	void registerAllEvents()
	{
		addEvent((KeyEvent*)&shader_reload_event);
		addEvent((KeyEvent*)&time_reset_event);
		addEvent((KeyEvent*)&help_event);
		addEvent((KeyEvent*)&exit_event);
		addEvent((KeyEvent*)&pause_event);
		addEvent((KeyEvent*)&fps_lock_event);
		addEvent((KeyEvent*)&move_forward_event);
		addEvent((KeyEvent*)&move_back_event);
		addEvent((KeyEvent*)&move_right_event);
		addEvent((KeyEvent*)&move_left_event);
		addEvent((KeyEvent*)&move_up_event);
		addEvent((KeyEvent*)&move_down_event);
	}
}