#pragma once

#include <memory>

#include "../../shader.h"

#include "../../../AssetStore/AssetStore.h"

namespace ShaderLoader {
	void LoadShaders(AssetStore& assetStore);
}