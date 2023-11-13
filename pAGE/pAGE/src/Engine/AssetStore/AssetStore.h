#pragma once

#include <glew/glew.h>

#include <map>
#include <memory>
#include <string>

#include "../Objects/Model.h"
#include "../Utils/shader.h"
#include "../Utils/texture.h"

#include "../../structs.h"

class AssetStore {
public:
	AssetStore();
	~AssetStore();

	void ClearAssets();

	void AddOpenGLBinding(const std::string& assetId, std::shared_ptr<unsigned int> binding);
	std::shared_ptr<unsigned int> GetOpenGLBinding(const std::string& assetId);

	void AddOpenGLObject(const std::string& assetId, std::shared_ptr<IOpenGLObject> openglObject);
	std::shared_ptr<IOpenGLObject> GetOpenGLObject(const std::string& assetId) const;

	void AddModel(const std::string& assetId, std::shared_ptr<Model> model);
	std::shared_ptr<Model> GetModel(const std::string& assetId) const;

	void AddShader(const std::string& assetId, std::shared_ptr<Shader> shader);
	std::shared_ptr<Shader> GetShader(const std::string& assetId) const;

	void AddTexture(const std::string& assetId, std::shared_ptr<Texture> texture);
	std::shared_ptr<Texture> GetTexture(const std::string& assetId) const;

private:
	std::map<std::string, std::shared_ptr<unsigned int>> openglBindings;
	std::map<std::string, std::shared_ptr<IOpenGLObject>> openglObjects;
	std::map<std::string, std::shared_ptr<Model>> models;
	std::map<std::string, std::shared_ptr<Shader>> shaders;
	std::map<std::string, std::shared_ptr<Texture>> textures;

	template <typename T>
	std::shared_ptr<T> GetAsset(const std::string& assetId, const std::map<std::string, std::shared_ptr<T>>& map) const {
		if (map.contains(assetId)) {
			return map.at(assetId);
		}

		return NULL;
	}
};