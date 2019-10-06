#pragma once

#include <iostream>
#include <fstream>

struct Logger
{
	std::ofstream file;

	Logger(){}

	void set(const char *path)
	{
		file = std::ofstream(path);
		std::cout << "LOG started at " << path << std::endl;
	}

	Logger(const char *path)
	{
		set(path);
	}

	~Logger()
	{
		file.close();
	}
};


template<typename T>
Logger &operator<<(Logger &log, const T &t)
{
	log.file << t;
	std::cout << t;
	return log;
}

Logger &operator<<(Logger &log, std::ostream &(*var)(std::ostream&))
{
	log.file << var;
	std::cout << var;
	return log;
}