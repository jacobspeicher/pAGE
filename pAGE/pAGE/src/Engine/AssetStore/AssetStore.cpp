#include <spdlog/spdlog.h>

#include "AssetStore.h"

AssetStore::AssetStore() {
	spdlog::info("AssetStore created");
}

AssetStore::~AssetStore() {
	ClearAssets();
	spdlog::info("AssetStore destroyed");
}

void AssetStore::ClearAssets() {
	openglObjects.clear();
	models.clear();
	shaders.clear();
	textures.clear();
}

void AssetStore::AddOpenGLBinding(const std::string& assetId, std::shared_ptr<unsigned int> binding) {
	openglBindings.emplace(assetId, binding);
	spdlog::info("OpenGLBinding {0} added to asset store", assetId);
}

std::shared_ptr<unsigned int> AssetStore::GetOpenGLBinding(const std::string& assetId) {
	return GetAsset<unsigned int>(assetId, openglBindings);
}

void AssetStore::AddOpenGLObject(const std::string& assetId, std::shared_ptr<IOpenGLObject> openglObject) {
	openglObjects.emplace(assetId, openglObject);
	spdlog::info("OpenGLObject {0} added to asset store", assetId);
}

std::shared_ptr<IOpenGLObject> AssetStore::GetOpenGLObject(const std::string& assetId) const {
	return GetAsset<IOpenGLObject>(assetId, openglObjects);
}

void AssetStore::AddModel(const std::string& assetId, std::shared_ptr<Model> model) {
	models.emplace(assetId, model);
	spdlog::info("Model {0} added to asset store", assetId);
}

std::shared_ptr<Model> AssetStore::GetModel(const std::string& assetId) const {
	return GetAsset<Model>(assetId, models);
}

void AssetStore::AddShader(const std::string& assetId, std::shared_ptr<Shader> shader) {
	shaders.emplace(assetId, shader);
	spdlog::info("Shader {0} added to asset store", assetId);
}

std::shared_ptr<Shader> AssetStore::GetShader(const std::string& assetId) const {
	return GetAsset<Shader>(assetId, shaders);
}

void AssetStore::AddTexture(const std::string& assetId, std::shared_ptr<Texture> texture) {
	textures.emplace(assetId, texture);
	spdlog::info("Texture {0} added to asset store", assetId);
}

std::shared_ptr<Texture> AssetStore::GetTexture(const std::string& assetId) const {
	return GetAsset<Texture>(assetId, textures);
}