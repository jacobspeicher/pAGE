#pragma once

#include <memory>

#include "../../Shader.h"

#include "../../../AssetStore/AssetStore.h"

namespace ShaderLoader {
	void LoadShaders(AssetStore& assetStore);
}