#include "ShaderLoader.h"

#include <spdlog/spdlog.h>

#include "../../Globals/Globals.h"

namespace ShaderLoader {
	void LoadShaders(AssetStore& assetStore) {
		std::string path = std::filesystem::current_path().string();
		std::replace(path.begin(), path.end(), '\\', '/');
		path += "/src/Engine/AssetStore/Scripts/shaders.lua";

		sol::load_result script = Globals::lua.load_file(path);

		if (!script.valid()) {
			sol::error err = script;
			std::string errorMsg = err.what();
			spdlog::error("SHADER LOADER : Error loading shader script: {0}", errorMsg);
			return;
		}

		script.call();
		sol::optional<sol::table> hasAssets = Globals::lua["assets"];
		if (hasAssets == sol::nullopt) {
			spdlog::warn("SHADER LOADER : No shaders to load");
			return;
		}
		sol::table assets = hasAssets.value();

		for (auto& asset : assets) {
			std::string shaderName = asset.first.as<std::string>();
			sol::table shaderFiles = asset.second.as<sol::table>();

			std::shared_ptr<Shader> shader = std::make_shared<Shader>(
				shaderFiles["vert"],
				shaderFiles["frag"],
				shaderFiles["geo"].get_or(std::string(""))
			);

			assetStore.AddShader(shaderName, shader);
		}
	}
}