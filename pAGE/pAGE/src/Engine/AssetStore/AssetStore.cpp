#include <spdlog/spdlog.h>

#include "AssetStore.h"

AssetStore::AssetStore() {
	spdlog::info("{0} Created", outputPrefix);
}

AssetStore::~AssetStore() {
	ClearAssets();
	spdlog::info("{0} Destroyed", outputPrefix);
}

void AssetStore::ClearAssets() {
	openglBindings.clear();
	models.clear();
	shaders.clear();
	textures.clear();
	spdlog::info("{0} Cleaned up", outputPrefix);
}

void AssetStore::AddOpenGLBinding(const std::string& assetId, std::shared_ptr<unsigned int> binding) {
	openglBindings.emplace(assetId, binding);
	spdlog::info("{0} OpenGLBinding {1} added", outputPrefix, assetId);
}

std::shared_ptr<unsigned int> AssetStore::GetOpenGLBinding(const std::string& assetId) {
	return GetAsset<unsigned int>(assetId, openglBindings);
}

void AssetStore::AddModel(const std::string& assetId, std::shared_ptr<Model> model) {
	models.emplace(assetId, model);
	spdlog::info("{0} Model {1} added", outputPrefix, assetId);
}

std::shared_ptr<Model> AssetStore::GetModel(const std::string& assetId) const {
	return GetAsset<Model>(assetId, models);
}

void AssetStore::AddShader(const std::string& assetId, std::shared_ptr<Shader> shader) {
	shaders.emplace(assetId, shader);
	spdlog::info("{0} Shader {1} added", outputPrefix, assetId);
}

std::shared_ptr<Shader> AssetStore::GetShader(const std::string& assetId) const {
	return GetAsset<Shader>(assetId, shaders);
}

void AssetStore::AddTexture(const std::string& assetId, std::shared_ptr<Texture> texture) {
	textures.emplace(assetId, texture);
	spdlog::info("{0} Texture {1} added", outputPrefix, assetId);
}

std::shared_ptr<Texture> AssetStore::GetTexture(const std::string& assetId) const {
	return GetAsset<Texture>(assetId, textures);
}