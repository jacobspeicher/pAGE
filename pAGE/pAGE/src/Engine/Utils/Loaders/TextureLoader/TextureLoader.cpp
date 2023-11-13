#include "TextureLoader.h"

#include <spdlog/spdlog.h>

#include "../../Globals/Globals.h"

namespace TextureLoader {
	void LoadTextures(AssetStore& assetStore) {
		std::string path = std::filesystem::current_path().string();
		std::replace(path.begin(), path.end(), '\\', '/');
		path += "/src/Engine/AssetStore/Scripts/textures.lua";

		sol::load_result script = Globals::lua.load_file(path);

		if (!script.valid()) {
			sol::error err = script;
			std::string errorMsg = err.what();
			spdlog::error("Error loading texture script: {0}", errorMsg);
		}

		script.call();
		sol::optional<sol::table> hasAssets = Globals::lua["assets"];
		if (hasAssets == sol::nullopt) {
			spdlog::warn("No textures to load");
		}
		sol::table assets = hasAssets.value();

		for (auto& asset : assets) {
			std::string textureName = asset.first.as<std::string>();
			sol::table textureFiles = asset.second.as<sol::table>();

			std::shared_ptr<Texture> texture = std::make_shared<Texture>(
				textureFiles["diffuse"].get_or(std::string("")),
				textureFiles["specular"].get_or(std::string(""))
			);

			assetStore.AddTexture(textureName, texture);
		}
	}
}