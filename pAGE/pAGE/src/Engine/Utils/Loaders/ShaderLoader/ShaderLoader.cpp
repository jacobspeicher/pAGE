#include "ShaderLoader.h"

namespace ShaderLoader {
	void LoadShaders(AssetStore& assetStore) {
		std::string path = std::filesystem::current_path().string();
		std::replace(path.begin(), path.end(), '\\', '/');
		path += "/src/Engine/DataStore/shaders.json";

		rapidjson::Document document;

		std::ifstream ifs(path);
		rapidjson::IStreamWrapper isw(ifs);

		document.ParseStream(isw);

		if (document.HasParseError()) {
			std::string errorMsg = static_cast<std::string>(rapidjson::GetParseError_En(document.GetParseError()));
			spdlog::error("SHADER LOADER : Error loading shader data: {0}", errorMsg);
			return;
		}

		for (
			rapidjson::Value::ConstMemberIterator itr = document.MemberBegin();
			itr != document.MemberEnd();
			++itr
			) {
			std::string assetName = itr->name.GetString();
			auto assetFiles = itr->value.GetObj();

			std::shared_ptr<Shader> shader = std::make_shared<Shader>(
				assetFiles["vert"].GetString(),
				assetFiles["frag"].GetString(),
				assetFiles.HasMember("geo") ? assetFiles["geo"].GetString() : ""
			);

			assetStore.AddShader(assetName, shader);
		}
	}
}