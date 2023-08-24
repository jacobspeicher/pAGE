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
	std::vector<Mesh> meshes;
	std::vector<AssimpTexture> modelTextures;
	AABB box;
	unsigned int boxVao;
	std::vector<glm::vec3> boundingBoxVertices;
	std::vector<unsigned int> boundingBoxIndices;

	Model(const char* path) {
		LoadModel(path);
	}

private:
	// model data
	unsigned int boxVbo, boxEbo;
	std::string directory;

	void LoadModel(std::string path) {
		Assimp::Importer importer;

		std::string cwd = std::filesystem::current_path().string();
		std::replace(cwd.begin(), cwd.end(), '\\', '/');
		cwd += "/src/Engine/AssetStore/Models/" + path;
		// aiProcess_Triangulate - transform all the models primitives to triangles first
		// aiProcess_FlipUVs - flip the texture coordinates on the y-axis (OpenGL appreciates this)
		const aiScene* scene = importer.ReadFile(cwd, aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			spdlog::error("{0}", importer.GetErrorString());
			return;
		}

		directory = cwd.substr(0, cwd.find_last_of('/'));

		ProcessNode(scene->mRootNode, scene);
		GenerateAABB();
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
		std::vector<std::vector<glm::vec3>> triangles;
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
			std::vector<glm::vec3> triangle;
			for (unsigned int j = 0; j < face.mNumIndices; ++j) {
				indices.push_back(face.mIndices[j]);
				glm::vec3 vertex = vertices[face.mIndices[j]].Position;
				triangle.push_back(vertex);
			}
			triangles.push_back(triangle);
		}

		// process material
		if (mesh->mMaterialIndex >= 0) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			std::vector<AssimpTexture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

			std::vector<AssimpTexture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		}

		Mesh outputMesh(vertices, indices, triangles, textures);
		outputMesh.GenerateAABB();

		return outputMesh;
	}

	std::vector<AssimpTexture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
		std::vector<AssimpTexture> textures;

		for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			for (unsigned int j = 0; j < modelTextures.size(); ++j) {
				if (std::strcmp(modelTextures[j].path.data(), str.C_Str()) == 0) {
					textures.push_back(modelTextures[j]);
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
				modelTextures.push_back(texture);
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

	void GenerateAABB() {
		box.min = glm::vec3(1000.0f);
		box.max = glm::vec3(-1000.0f);

		for (unsigned int i = 0; i < meshes.size(); ++i) {
			const auto& mesh = meshes[i];

			if (mesh.box.min.x < box.min.x) {
				box.min.x = mesh.box.min.x;
			}
			if (mesh.box.max.x > box.max.x) {
				box.max.x = mesh.box.max.x;
			}

			if (mesh.box.min.y < box.min.y) {
				box.min.y = mesh.box.min.y;
			}
			if (mesh.box.max.y > box.max.y) {
				box.max.y = mesh.box.max.y;
			}

			if (mesh.box.min.z < box.min.z) {
				box.min.z = mesh.box.min.z;
			}
			if (mesh.box.max.z > box.max.z) {
				box.max.z = mesh.box.max.z;
			}
		}

		boundingBoxVertices.push_back(glm::vec3(box.min.x, box.min.y, box.min.z));
		boundingBoxVertices.push_back(glm::vec3(box.min.x, box.max.y, box.min.z));
		boundingBoxVertices.push_back(glm::vec3(box.max.x, box.max.y, box.min.z));
		boundingBoxVertices.push_back(glm::vec3(box.max.x, box.min.y, box.min.z));

		boundingBoxVertices.push_back(glm::vec3(box.min.x, box.min.y, box.max.z));
		boundingBoxVertices.push_back(glm::vec3(box.min.x, box.max.y, box.max.z));
		boundingBoxVertices.push_back(glm::vec3(box.max.x, box.max.y, box.max.z));
		boundingBoxVertices.push_back(glm::vec3(box.max.x, box.min.y, box.max.z));

		boundingBoxIndices = {
			0, 1, 3, // back face
			1, 2, 3,

			5, 1, 6, // top face
			1, 2, 6,

			4, 5, 7, // front face
			5, 6, 7,

			4, 0, 7, // bottom face
			0, 3, 7,

			0, 1, 4, // left face
			1, 5, 4,

			7, 6, 3, // right face
			6, 2, 3,
		};

		SetupBoundingBox();
	}

	void SetupBoundingBox() {
		glGenVertexArrays(1, &boxVao);
		glGenBuffers(1, &boxVbo);
		glGenBuffers(1, &boxEbo);

		glBindVertexArray(boxVao);

		glBindBuffer(GL_ARRAY_BUFFER, boxVbo);
		glBufferData(GL_ARRAY_BUFFER, boundingBoxVertices.size() * sizeof(glm::vec3), &boundingBoxVertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxEbo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, boundingBoxIndices.size() * sizeof(unsigned int), &boundingBoxIndices[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

		glBindVertexArray(0);
	}
};