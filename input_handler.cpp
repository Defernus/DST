#include "input_handler.h"

#include <set>
#include <map>
#include <iostream>
#include <cstring>

std::multimap<int, KeyEvent*> events;

std::set<int> used_keys;

bool is_key_pressed[GLFW_KEY_LAST + 1];

void initInputHandler(GLFWwindow *window)
{
	for (bool &i : is_key_pressed) i = false;
	//for (int i = 0; i <= GLFW_KEY_LAST; ++i) is_key_pressed[i] = glfwGetKey(window, i) == GLFW_PRESS;
}

void processInput(GLFWwindow *window)
{
	for (int i : used_keys)
	{
		if (i < 0 || i > GLFW_KEY_LAST)continue;

		std::pair<std::multimap< int, KeyEvent* >::iterator, std::multimap< int, KeyEvent* >::iterator> range = events.equal_range(i);
		if (glfwGetKey(window, i) == GLFW_PRESS)
		{
			if (!is_key_pressed[i])
			{
				is_key_pressed[i] = true;
				for (std::multimap< int, KeyEvent* >::iterator j = range.first; j != range.second; ++j)j->second->onJustPressed();
			}
			for (std::multimap< int, KeyEvent* >::iterator j = range.first; j != range.second; ++j)j->second->onPressed();

		}
		if (glfwGetKey(window, i) == GLFW_RELEASE)
		{
			is_key_pressed[i] = false;
			for (std::multimap< int, KeyEvent* >::iterator j = range.first; j != range.second; ++j)j->second->onReleased();
		}
	}
}

void addEvent(KeyEvent *e)
{
	events.insert({ e->keyCode, e });
	used_keys.insert(e->keyCode);
}

void printHelp()
{
	std::cout << "============HELP============\n";

	for (std::multimap<int, KeyEvent*>::iterator i = events.begin(); i != events.end(); ++i)
	{
		char str[] = "|                          |\n";
		for (int j = 0; j != strlen(i->second->getDiscription()) && j != 25; ++j)
		{
			str[j + 2] = i->second->getDiscription()[j];
		}
		std::cout << str;
	}

	std::cout << "============================" << std::endl;
}