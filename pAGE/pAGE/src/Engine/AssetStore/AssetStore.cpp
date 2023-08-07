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
	shaders.clear();
}

void AssetStore::AddOpenGLObject(const std::string& assetId, std::shared_ptr<IOpenGLObject> openglObject) {
	openglObjects.emplace(assetId, openglObject);
	spdlog::info("OpenGLObject {0} added to asset store", assetId);
}

std::shared_ptr<IOpenGLObject> AssetStore::GetOpenGLObject(const std::string& assetId) const {
	return openglObjects.at(assetId);
}

void AssetStore::AddShader(const std::string& assetId, std::shared_ptr<Shader> shader) {
	shaders.emplace(assetId, shader);
	spdlog::info("Shader {0} added to asset store", assetId);
}

std::shared_ptr<Shader> AssetStore::GetShader(const std::string& assetId) const {
	return shaders.at(assetId);
}

void AssetStore::AddTexture(const std::string& assetId, std::shared_ptr<Texture> texture) {
	textures.emplace(assetId, texture);
	spdlog::info("Texture {0} added to asset store", assetId);
}

std::shared_ptr<Texture> AssetStore::GetTexture(const std::string& assetId) const {
	return textures.at(assetId);
}