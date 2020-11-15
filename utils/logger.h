#pragma once

#include <iostream>
#include <fstream>
#include <glm/glm.hpp>

namespace lg
{
	struct Logger
	{
		std::ofstream file;

		Logger() {}

		Logger(const char *path)
		{
			set(path);
		}

		void set(const char *path)
		{
			file = std::ofstream(path);
			std::cout << "LOG started at " << path << std::endl;
		}

		template<typename T>
		void print(T v)
		{
			if (is_first_print)
			{
				file << "[" << msg_index << "]:";
				std::cout << "[" << msg_index << "]:";
				is_first_print = false;
				if (!is_type_defined)
				{
					file << "[U] ";
					std::cout << "[U] ";
				}
			}

			file << v;
			std::cout << v;
		}

		void endl()
		{
			++msg_index;
			is_type_defined = false;
			is_first_print = true;
			std::cout << std::endl;
			file << std::endl;
		}

		void inf()
		{
			if (!is_first_print) endl();
			is_type_defined = true;
			print("[I] ");
		}

		void war()
		{
			if (!is_first_print) endl();
			is_type_defined = true;
			print("[W] ");
		}

		void err()
		{
			if (!is_first_print) endl();
			is_type_defined = true;
			print("[E] ");
		}

		~Logger()
		{
			file.close();
		}

		int getMsgIndex() const
		{
			return msg_index;
		}
	private:
		bool is_type_defined = false;
		int msg_index = 0;
		bool is_first_print = true;
	};

	Logger &inf(Logger &l)
	{
		l.inf();
		return l;
	}

	Logger &err(Logger &l)
	{
		l.err();
		return l;
	}

	Logger &war(Logger &l)
	{
		l.war();
		return l;
	}

	Logger &endl(Logger &l)
	{
		l.endl();
		return l;
	}

	template<typename T>
	Logger &operator<<(Logger &l, const T &v)
	{
		l.print(v);
		return l;
	}

	Logger &operator<<(Logger &l, Logger &(*fu)(Logger&))
	{
		fu(l);
		return l;
	}

	Logger &operator<<(Logger &l, glm::vec3 vec)
	{
		l << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
		return l;
	}
}