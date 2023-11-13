#include "ModelLoader.h"

#include <spdlog/spdlog.h>

#include "../../Globals/Globals.h"

namespace ModelLoader {
	void LoadModels(AssetStore& assetStore) {
		std::string path = std::filesystem::current_path().string();
		std::replace(path.begin(), path.end(), '\\', '/');
		path += "/src/Engine/AssetStore/Scripts/models.lua";

		sol::load_result script = Globals::lua.load_file(path);

		if (!script.valid()) {
			sol::error err = script;
			std::string errorMsg = err.what();
			spdlog::error("MODEL LOADER : Error loading model script: {0}", errorMsg);
			return;
		}
		
		script.call();
		sol::optional<sol::table> hasAssets = Globals::lua["assets"];
		if (hasAssets == sol::nullopt) {
			spdlog::warn("MODEL LOADER : No models to load");
			return;
		}
		sol::table assets = hasAssets.value();

		for (auto& asset : assets) {
			std::string modelName = asset.first.as<std::string>();
			sol::table modelFiles = asset.second.as<sol::table>();
			std::shared_ptr<Model> model = std::make_shared<Model>(
				modelFiles.get<std::string>("model")
			);

			assetStore.AddModel(modelName, model);
		}
	}
}