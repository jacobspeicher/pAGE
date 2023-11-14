#pragma once

#include <fstream>
#include <memory>

#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>
#include <spdlog/spdlog.h>

#include "../../Shader.h"

#include "../../../AssetStore/AssetStore.h"

namespace ShaderLoader {
	void LoadShaders(AssetStore& assetStore);
}