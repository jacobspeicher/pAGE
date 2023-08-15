#pragma once

#include <glew/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

class Shader {
public:
	unsigned int id;

	Shader(const char* vertPath, const char* fragPath) {
		// retrieve vert/frag source code
		std::string vertString, fragString;
		std::ifstream vertFile, fragFile;
		// ensure ifstreams can throw exceptions
		vertFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		std::string path = std::filesystem::current_path().string();
		std::replace(path.begin(), path.end(), '\\', '/');
		path += "/src/Engine/Shaders/";

		char infoLog[512];
		try {
			vertFile.open(path + vertPath);
			fragFile.open(path + fragPath);

			// read the files contents into streams
			std::stringstream vertStream, fragStream;
			vertStream << vertFile.rdbuf();
			fragStream << fragFile.rdbuf();

			vertFile.close();
			fragFile.close();

			// convert streams into strings
			vertString = vertStream.str();
			fragString = fragStream.str();
		}
		catch (std::ifstream::failure e) {
			strerror_s(infoLog, 512, errno);
			spdlog::error("Shader files not successfully read\n{0}", infoLog);
			return;
		}

		const char* vertCode = vertString.c_str();
		const char* fragCode = fragString.c_str();

		// compile shaders
		unsigned int vertex, fragment;
		int success;

		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertCode, NULL);
		glCompileShader(vertex);

		// print any errors
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			spdlog::error("Vertex shader compilation failed\n{0}", infoLog);
		}

		// fragment shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragCode, NULL);
		glCompileShader(fragment);

		// print any errors
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			spdlog::error("Fragment shader compilation failed\n{0}", infoLog);
		}

		// create shader program
		id = glCreateProgram();
		glAttachShader(id, vertex);
		glAttachShader(id, fragment);
		glLinkProgram(id);

		// print any linking errors
		glGetProgramiv(id, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(id, 512, NULL, infoLog);
			spdlog::error("Program linking failed\n{0}", infoLog);
		}

		// delete shaders now that the program is created
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	Shader(const char* vertPath, const char* fragPath, const char* geometryPath) {
		// retrieve vert/frag source code
		std::string vertString, fragString, geometryString;
		std::ifstream vertFile, fragFile, geometryFile;
		// ensure ifstreams can throw exceptions
		vertFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fragFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		geometryFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		std::string path = std::filesystem::current_path().string();
		std::replace(path.begin(), path.end(), '\\', '/');
		path += "/src/Engine/Shaders/";

		char infoLog[512];
		try {
			vertFile.open(path + vertPath);
			fragFile.open(path + fragPath);
			geometryFile.open(path + geometryPath);

			// read the files contents into streams
			std::stringstream vertStream, fragStream, geometryStream;
			vertStream << vertFile.rdbuf();
			fragStream << fragFile.rdbuf();
			geometryStream << geometryFile.rdbuf();

			vertFile.close();
			fragFile.close();
			geometryFile.close();

			// convert streams into strings
			vertString = vertStream.str();
			fragString = fragStream.str();
			geometryString = geometryStream.str();
		}
		catch (std::ifstream::failure e) {
			strerror_s(infoLog, 512, errno);
			spdlog::error("Shader files not successfully read\n{0}", infoLog);
			return;
		}

		const char* vertCode = vertString.c_str();
		const char* fragCode = fragString.c_str();
		const char* geometryCode = geometryString.c_str();

		// compile shaders
		unsigned int vertex, fragment, geometry;
		int success;

		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vertCode, NULL);
		glCompileShader(vertex);

		// print any errors
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			spdlog::error("Vertex shader compilation failed\n{0}", infoLog);
		}

		// fragment shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fragCode, NULL);
		glCompileShader(fragment);

		// print any errors
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			spdlog::error("Fragment shader compilation failed\n{0}", infoLog);
		}

		// fragment shader
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &geometryCode, NULL);
		glCompileShader(geometry);

		// print any errors
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(geometry, 512, NULL, infoLog);
			spdlog::error("Geometry shader compilation failed\n{0}", infoLog);
		}

		// create shader program
		id = glCreateProgram();
		glAttachShader(id, vertex);
		glAttachShader(id, fragment);
		glAttachShader(id, geometry);
		glLinkProgram(id);

		// print any linking errors
		glGetProgramiv(id, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(id, 512, NULL, infoLog);
			spdlog::error("Program linking failed\n{0}", infoLog);
		}

		// delete shaders now that the program is created
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		glDeleteShader(geometry);
	}

	void Use() {
		glUseProgram(id);
	}

	void SetBool(const std::string& name, bool value) const {
		glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
	}

	void SetInt(const std::string& name, int value) const {
		glUniform1i(glGetUniformLocation(id, name.c_str()), value);
	}

	void SetFloat(const std::string& name, float value) const {
		glUniform1f(glGetUniformLocation(id, name.c_str()), value);
	}

	void SetVec3(const std::string& name, glm::vec3 value) const {
		glUniform3fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
	}

	void SetVec4(const std::string& name, glm::vec4 value) const {
		glUniform4fv(glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value));
	}

	void SetMat4(const std::string& name, glm::mat4 value) const {
		glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}
};