#include "ModelLoader.h"

namespace ModelLoader {
	void LoadModels(AssetStore& assetStore) {
		std::shared_ptr<Model> backpack = std::make_shared<Model>("backpack/backpack.obj");
		std::shared_ptr<Model> snail = std::make_shared<Model>("snail/garden snail.obj");

		assetStore.AddModel("backpack", backpack);
		assetStore.AddModel("snail", snail);
	}
}