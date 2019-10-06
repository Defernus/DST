#pragma once
#include <GLFW/glfw3.h>

class KeyEvent
{
protected:
	int button;
public:
	const int keyCode;
	
	KeyEvent(int key):keyCode(key){}

	virtual void onJustPressed	() = 0;
	virtual void onPressed		() = 0;
	virtual void onReleased		() = 0;
};

void initInputHandler(GLFWwindow *window);
void addEvent(KeyEvent *e);
void processInput(GLFWwindow *window);