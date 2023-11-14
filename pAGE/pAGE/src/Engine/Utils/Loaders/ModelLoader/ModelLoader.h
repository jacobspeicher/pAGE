#pragma once

#include <fstream>
#include <memory>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>
#include <spdlog/spdlog.h>

#include "../../../AssetStore/AssetStore.h"

#include "../../../Objects/Model.h"

namespace ModelLoader {
	void LoadModels(AssetStore& assetStore);
}
