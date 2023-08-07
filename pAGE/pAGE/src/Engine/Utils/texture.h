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

	Texture() {
		diffuse = NULL;
		specular = NULL;
	}

	Texture(std::string diffuseFile, std::string specularFile) {
		std::string path = std::filesystem::current_path().string();
		std::replace(path.begin(), path.end(), '\\', '/');
		path += "/src/Engine/AssetStore/Textures/";
		
		stbi_set_flip_vertically_on_load(true);

		int width, height, numChannels;
		unsigned char* data;

		glGenTextures(1, &diffuse);
		glGenTextures(1, &specular);

		glBindTexture(GL_TEXTURE_2D, diffuse);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		std::string diffusePath = path + diffuseFile;
		data = stbi_load(diffusePath.c_str(), &width, &height, &numChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		stbi_image_free(data);

		glBindTexture(GL_TEXTURE_2D, specular);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		std::string specularPath = path + specularFile;
		data = stbi_load(specularPath.c_str(), &width, &height, &numChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		stbi_image_free(data);
	}
};