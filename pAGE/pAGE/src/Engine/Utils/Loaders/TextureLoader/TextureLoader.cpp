#include "TextureLoader.h"

namespace TextureLoader {
	void LoadTextures(AssetStore& assetStore) {
		std::string path = std::filesystem::current_path().string();
		std::replace(path.begin(), path.end(), '\\', '/');
		path += "/src/Engine/DataStore/textures.json";

		rapidjson::Document document;

		std::ifstream ifs(path);
		rapidjson::IStreamWrapper isw(ifs);

		document.ParseStream(isw);

		if (document.HasParseError()) {
			std::string errorMsg = static_cast<std::string>(rapidjson::GetParseError_En(document.GetParseError()));
			spdlog::error("TEXTURE LOADER : Error loading texture data: {0}", errorMsg);
			return;
		}

		for (
			rapidjson::Value::ConstMemberIterator itr = document.MemberBegin();
			itr != document.MemberEnd();
			++itr
			) {
			std::string assetName = itr->name.GetString();
			auto assetFiles = itr->value.GetObj();

			std::shared_ptr<Texture> texture = std::make_shared<Texture>(
				assetFiles.HasMember("diffuse") ? assetFiles["diffuse"].GetString() : "",
				assetFiles.HasMember("specular") ? assetFiles["specular"].GetString() : ""
			);

			assetStore.AddTexture(assetName, texture);
		}
	}
}