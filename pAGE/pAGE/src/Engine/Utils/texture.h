#pragma once

#include <glew/glew.h>
#include <stb/stb_image.h>

#include <algorithm>
#include <filesystem>
#include <string>

class Texture {
public:
	GLuint diffuse;
	GLuint specular;

	Texture(std::string diffuseFile, std::string specularFile) {
		if (diffuseFile == "") {
			createTexture();
		}
		else if (specularFile == "") {
			createTexture(diffuseFile);
		}
		else {
			createTexture(diffuseFile, specularFile);
		}
	}

	void createTexture() {
		diffuse = NULL;
		specular = NULL;
	}

	void createTexture(std::string file) {
		bindTexture(diffuse);
		loadTextureData(file);

		specular = NULL;
	}

	void createTexture(std::string diffuseFile, std::string specularFile) {
		bindTexture(diffuse);
		loadTextureData(diffuseFile);

		bindTexture(specular);
		loadTextureData(specularFile);
	}

	void bindTexture(unsigned int& binding) {
		glGenTextures(1, &binding);

		glBindTexture(GL_TEXTURE_2D, binding);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	void loadTextureData(std::string file) {
		std::string path = std::filesystem::current_path().string();
		std::replace(path.begin(), path.end(), '\\', '/');
		path += "/src/Engine/AssetStore/Textures/";

		stbi_set_flip_vertically_on_load(true);

		int width, height, numChannels;
		unsigned char* data;

		std::string filePath = path + file;
		data = stbi_load(filePath.c_str(), &width, &height, &numChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		stbi_image_free(data);
	}
};