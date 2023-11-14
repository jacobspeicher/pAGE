#include "ModelLoader.h"

#include <spdlog/spdlog.h>

namespace ModelLoader {
	void LoadModels(AssetStore& assetStore) {
		std::string path = std::filesystem::current_path().string();
		std::replace(path.begin(), path.end(), '\\', '/');
		path += "/src/Engine/DataStore/models.json";

		rapidjson::Document document;

		std::ifstream ifs(path);
		rapidjson::IStreamWrapper isw(ifs);

		document.ParseStream(isw);

		if (document.HasParseError()) {
			std::string errorMsg = static_cast<std::string>(rapidjson::GetParseError_En(document.GetParseError()));
			spdlog::error("MODEL LOADER : Error loading model data: {0}", errorMsg);
			return;
		}

		for (
			rapidjson::Value::ConstMemberIterator itr = document.MemberBegin();
			itr != document.MemberEnd();
			++itr
			) {
			std::string assetName = itr->name.GetString();
			auto assetFiles = itr->value.GetObj();

			std::shared_ptr<Model> model = std::make_shared<Model>(
				assetFiles["model"].GetString()
			);

			assetStore.AddModel(assetName, model);
		}
	}
}