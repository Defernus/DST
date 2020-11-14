#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "logger.h"

namespace sh
{
	typedef unsigned int Shader;
	std::string loadFile(lg::Logger &logger, const char* path)
	{
		std::ifstream file;

		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			file.open(path);
			std::stringstream srcStream;

			srcStream << file.rdbuf();

			file.close();

			return srcStream.str();
		}
		catch (std::ifstream::failure e)
		{
			logger << lg::war << "SHADER::FILE_NOT_SUCCESFULLY_READ\n" << e.what() << lg::endl;
			return "";
		}
	}

	void checkCompileErrors(lg::Logger &logger, Shader shader, std::string type)
	{
		GLint success;
		GLchar infoLog[1024];
		if (type != "PROGRAM")
		{
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(shader, 1024, NULL, infoLog);
				logger << lg::war << "SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << lg::endl;
			}
		}
		else
		{
			glGetProgramiv(shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(shader, 1024, NULL, infoLog);
				logger << lg::war << "PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << lg::endl;
			}
		}
	}

	void compileShader(Shader &id, const char *src, const int &TYPE)
	{
		id = glCreateShader(TYPE);
		glShaderSource(id, 1, &src, NULL);
		glCompileShader(id);
	}

	Shader loadShader(lg::Logger &logger, const char *vertexPath, const char *fragmentPath, const char *geometryPath = nullptr)
	{
		Shader id;

		Shader vertex, fragment;

		const char *fShaderCode = loadFile(logger, fragmentPath).c_str();

		compileShader(vertex, loadFile(logger, vertexPath).c_str(), GL_VERTEX_SHADER);
		checkCompileErrors(logger, vertex, "VERTEX");

		compileShader(fragment, loadFile(logger, fragmentPath).c_str(), GL_FRAGMENT_SHADER);
		checkCompileErrors(logger, fragment, "FRAGMENT");

		Shader geometry(0);
		if (geometryPath != nullptr)
		{
			compileShader(fragment, loadFile(logger, geometryPath).c_str(), GL_GEOMETRY_SHADER);
			checkCompileErrors(logger, geometry, "GEOMETRY");
		}

		id = glCreateProgram();
		glAttachShader(id, vertex);
		glAttachShader(id, fragment);
		if (geometryPath != nullptr)
			glAttachShader(id, geometry);
		glLinkProgram(id);
		checkCompileErrors(logger, id, "PROGRAM");
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		if (geometryPath != nullptr)
			glDeleteShader(geometry);

		return id;
	}

	typedef unsigned Shader;

	void setBool(const Shader &id, const std::string &name, const bool &value)
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
	}

	void setInt(const Shader &id, const std::string &name, const int &value)
	{
		glUniform1i(glGetUniformLocation(id, name.c_str()), value);
	}

	void setFloat(const Shader &id, const std::string &name, const  float &value)
	{
		glUniform1f(glGetUniformLocation(id, name.c_str()), value);
	}

	void setVec2(const Shader &id, const std::string &name, const glm::vec2 &value)
	{
		glUniform2fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}
	void setVec2(const Shader &id, const std::string &name, const float &x, const float &y)
	{
		glUniform2f(glGetUniformLocation(id, name.c_str()), x, y);
	}

	void setVec3(const Shader &id, const std::string &name, const glm::vec3 &value)
	{
		glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}
	void setVec3(const Shader &id, const std::string &name, const float &x, const float &y, const float &z)
	{
		glUniform3f(glGetUniformLocation(id, name.c_str()), x, y, z);
	}

	void setVec4(const Shader &id, const std::string &name, const glm::vec4 &value)
	{
		glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, &value[0]);
	}
	void setVec4(const Shader &id, const std::string &name, const float &x, const float &y, const float &z, const float &w)
	{
		glUniform4f(glGetUniformLocation(id, name.c_str()), x, y, z, w);
	}

	void setMat2(const Shader &id, const std::string &name, const glm::mat2 &mat)
	{
		glUniformMatrix2fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void setMat3(const Shader &id, const std::string &name, const glm::mat3 &mat)
	{
		glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	void setMat4(const Shader &id, const std::string &name, const glm::mat4 &mat)
	{
		glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
}
