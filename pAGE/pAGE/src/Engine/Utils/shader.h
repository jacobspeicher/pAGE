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
	enum Type {
		VERTEX = 0,
		FRAGMENT,
		GEOMETRY
	};

	Shader(std::string vertPath, std::string fragPath, std::string geometryPath) {
		if (geometryPath == "") {
			createShader(vertPath.c_str(), fragPath.c_str());
		}
		else {
			createShader(vertPath.c_str(), fragPath.c_str(), geometryPath.c_str());
		}
	}

	void createShader(const char* vertPath, const char* fragPath) {
		// retrieve vert/frag source code
		std::string vertString, fragString;

		vertString = readFile(vertPath);
		fragString = readFile(fragPath);

		/*std::ifstream vertFile, fragFile;
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
		}*/

		unsigned int vertex, fragment;
		compileShader(vertex, vertString.c_str(), Type::VERTEX);
		compileShader(fragment, fragString.c_str(), Type::FRAGMENT);
		createShaderProgram(id, vertex, fragment);

		/*const char* vertCode = vertString.c_str();
		const char* fragCode = fragString.c_str();

		// compile shaders
		char infoLog[512];
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
		glDeleteShader(fragment);*/
	}

	void createShader(const char* vertPath, const char* fragPath, const char* geometryPath) {
		// retrieve vert/frag source code
		std::string vertString, fragString, geometryString;
		vertString = readFile(vertPath);
		fragString = readFile(fragPath);
		geometryString = readFile(geometryPath);
		/*std::ifstream vertFile, fragFile, geometryFile;
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
		const char* geometryCode = geometryString.c_str();*/

		// compile shaders
		unsigned int vertex, fragment, geometry;
		compileShader(vertex, vertString.c_str(), Type::VERTEX);
		compileShader(fragment, fragString.c_str(), Type::FRAGMENT);
		compileShader(geometry, geometryString.c_str(), Type::GEOMETRY);
		createShaderProgram(id, vertex, fragment, geometry);
		/*int success;

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
		glDeleteShader(geometry);*/
	}

	std::string readFile(const char* filePath) {
		std::string fileString;
		std::ifstream file;

		// ensure ifstreams can throw exceptions
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		std::string path = std::filesystem::current_path().string();
		std::replace(path.begin(), path.end(), '\\', '/');
		path += "/src/Engine/Shaders/";

		char infoLog[512];
		try {
			file.open(path + filePath);

			// read the file's contents into streams
			std::stringstream stream;
			stream << file.rdbuf();

			file.close();

			fileString = stream.str();
		}
		catch (std::ifstream::failure e) {
			strerror_s(infoLog, 512, errno);
			spdlog::error("Shader file not successfully read\n{0}", infoLog);
			return "";
		}

		return fileString;
	}

	void compileShader(unsigned int& binding, const char* code, Type type) {
		char infoLog[512];
		int success;

		switch (type) {
		case Type::VERTEX:
			binding = glCreateShader(GL_VERTEX_SHADER);
			break;
		case Type::FRAGMENT:
			binding = glCreateShader(GL_FRAGMENT_SHADER);
			break;
		case Type::GEOMETRY:
			binding = glCreateShader(GL_GEOMETRY_SHADER);
			break;
		}
		glShaderSource(binding, 1, &code, NULL);
		glCompileShader(binding);

		glGetShaderiv(binding, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(binding, 512, NULL, infoLog);
			spdlog::error("Shader compilation failed\n{0}", infoLog);
		}
	}

	void createShaderProgram(unsigned int& program, unsigned int& vertex, unsigned int& fragment) {
		char infoLog[512];
		int success;

		program = glCreateProgram();
		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glLinkProgram(program);

		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			spdlog::error("Program linking failed\n{0}", infoLog);
		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}

	void createShaderProgram(unsigned int& program, unsigned int& vertex, unsigned int& fragment, unsigned int& geometry) {
		char infoLog[512];
		int success;

		program = glCreateProgram();
		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		glAttachShader(program, geometry);
		glLinkProgram(program);

		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(program, 512, NULL, infoLog);
			spdlog::error("Program linking failed\n{0}", infoLog);
		}

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