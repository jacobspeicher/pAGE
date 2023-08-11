#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glew/glew.h>
#include <spdlog/spdlog.h>
#include <stb/stb_image.h>

#include <string>
#include <vector>

#include "Mesh.h"
#include "../Utils/shader.h"

class Model {
public:
	Model(const char* path) {
		LoadModel(path);
	}

	void Draw(Shader& shader) {
		for (unsigned int i = 0; i < meshes.size(); ++i) {
			meshes[i].Draw(shader);
		}
	}

private:
	// model data
	std::vector<Mesh> meshes;
	std::vector<AssimpTexture> texturesLoaded;
	std::string directory;

	void LoadModel(std::string path) {
		Assimp::Importer importer;
		// aiProcess_Triangulate - transform all the models primitives to triangles first
		// aiProcess_FlipUVs - flip the texture coordinates on the y-axis (OpenGL appreciates this)
		const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			spdlog::error("{0}", importer.GetErrorString());
			return;
		}

		directory = path.substr(0, path.find_last_of('/'));

		ProcessNode(scene->mRootNode, scene);
	}

	void ProcessNode(aiNode* node, const aiScene* scene) {
		// process all the node's meshes
		for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			meshes.push_back(ProcessMesh(mesh, scene));
		}
		// do the same for each child
		for (unsigned int i = 0; i < node->mNumChildren; ++i) {
			ProcessNode(node->mChildren[i], scene);
		}
	}

	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene) {
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<AssimpTexture> textures;
		
		for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
			Vertex vertex;

			glm::vec3 vector;
			vector.x = mesh->mVertices[i].x;
			vector.y = mesh->mVertices[i].y;
			vector.z = mesh->mVertices[i].z;
			vertex.Position = vector;

			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = vector;

			if (mesh->mTextureCoords[0]) {
				glm::vec2 vec;
				vec.x = mesh->mTextureCoords[0][i].x;
				vec.y = mesh->mTextureCoords[0][i].y;
				vertex.TexCoords = vec;
			}
			else
				vertex.TexCoords = glm::vec2(0.0f);

			vertices.push_back(vertex);
		}

		// process indices
		for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; ++j) {
				indices.push_back(face.mIndices[j]);
			}
		}

		// process material
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			std::vector<AssimpTexture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			std::vector<AssimpTexture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		}

		return Mesh(vertices, indices, textures);
	}

	std::vector<AssimpTexture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
		std::vector<AssimpTexture> textures;

		for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			for (unsigned int j = 0; j < texturesLoaded.size(); ++j) {
				if (std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0) {
					textures.push_back(texturesLoaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip) {
				AssimpTexture texture;
				texture.id = TextureFromFile(str.C_Str(), directory);
				texture.type = typeName;
				texture.path = str.data;
				textures.push_back(texture);
				texturesLoaded.push_back(texture);
			}
		}

		return textures;
	}

	unsigned int TextureFromFile(const char* filename, std::string directory) {
		stbi_set_flip_vertically_on_load(true);

		unsigned int texture;

		int width, height, numChannels;
		unsigned char* data;

		glGenTextures(1, &texture);

		glBindTexture(GL_TEXTURE_2D, texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		std::string texturePath = directory + "/" + std::string(filename);
		spdlog::info("{0}", filename);

		data = stbi_load(texturePath.c_str(), &width, &height, &numChannels, 0);
		if (data) {
			GLenum format[4] = { GL_RED, GL_RG, GL_RGB, GL_RGBA };

			glTexImage2D(
				GL_TEXTURE_2D, 0, format[numChannels - 1], 
				width, height, 0, format[numChannels - 1], 
				GL_UNSIGNED_BYTE, data
			);
			GLenum error = glGetError();
			if (error != GL_NO_ERROR) {
				spdlog::error("{0}", error);
			}
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			spdlog::error("Issue loading texture ({0})", filename);
		}

		stbi_image_free(data);

		return texture;
	}
};