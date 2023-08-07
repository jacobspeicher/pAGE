#pragma once

#include <glew/glew.h>

#include <map>
#include <memory>
#include <string>

#include "../Utils/shader.h"
#include "../Utils/texture.h"

#include "../../structs.h"

class AssetStore {
public:
	AssetStore();
	~AssetStore();

	void ClearAssets();

	void AddOpenGLObject(const std::string& assetId, std::shared_ptr<IOpenGLObject> openglObject);
	std::shared_ptr<IOpenGLObject> GetOpenGLObject(const std::string& assetId) const;

	void AddShader(const std::string& assetId, std::shared_ptr<Shader> shader);
	std::shared_ptr<Shader> GetShader(const std::string& assetId) const;

	void AddTexture(const std::string& assetId, std::shared_ptr<Texture> texture);
	std::shared_ptr<Texture> GetTexture(const std::string& assetId) const;

private:
	std::map<std::string, std::shared_ptr<IOpenGLObject>> openglObjects;
	std::map<std::string, std::shared_ptr<Shader>> shaders;
	std::map<std::string, std::shared_ptr<Texture>> textures;
};