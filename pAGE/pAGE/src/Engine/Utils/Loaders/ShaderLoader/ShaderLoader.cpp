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
			spdlog::error("Error loading shader script: {0}", errorMsg);
		}

		Globals::lua.script_file(path);
		sol::optional<sol::table> hasAssets = Globals::lua["assets"];
		if (hasAssets == sol::nullopt) {
			spdlog::warn("No shaders to load");
		}
		sol::table assets = hasAssets.value();

		for (auto& asset : assets) {
			std::string shaderName = asset.first.as<std::string>();
			sol::table shaderFiles = asset.second.as<sol::table>();

			std::shared_ptr<Shader> shader = std::make_shared<Shader>(
				shaderFiles["vert"],
				shaderFiles["frag"],
				shaderFiles["geo"].valid() ? shaderFiles["geo"] : std::string("")
			);

			assetStore.AddShader(shaderName, shader);
		}

		/*
		std::shared_ptr<Shader> basicShader2D = std::make_shared<Shader>("Basic/Basic2D.vert", "Basic/Basic2D.frag");
		std::shared_ptr<Shader> basicShader3D = std::make_shared<Shader>("Basic/Basic3D.vert", "Basic/Basic3D.frag");
		std::shared_ptr<Shader> basicShaderGeometry = std::make_shared<Shader>("Basic/BasicGeometry.vert", "Basic/BasicGeometry.frag", "Basic/BasicGeometry.shader");

		assetStore.AddShader("basic2D", basicShader2D);
		assetStore.AddShader("basic3D", basicShader3D);
		assetStore.AddShader("basicGeometry", basicShaderGeometry);*/
	}
}