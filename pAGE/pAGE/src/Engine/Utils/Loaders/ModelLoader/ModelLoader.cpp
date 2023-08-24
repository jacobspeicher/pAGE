#include "ModelLoader.h"

namespace ModelLoader {
	void LoadModels(AssetStore& assetStore) {
		std::shared_ptr<Model> cube = std::make_shared<Model>("cube/cube.obj");
		std::shared_ptr<Model> sphere = std::make_shared<Model>("sphere/sphere.obj");
		std::shared_ptr<Model> cylinder = std::make_shared<Model>("cylinder/cylinder.obj");
		std::shared_ptr<Model> cone = std::make_shared<Model>("cone/cone.obj");
		std::shared_ptr<Model> backpack = std::make_shared<Model>("backpack/backpack.obj");
		//std::shared_ptr<Model> snail = std::make_shared<Model>("snail/garden snail.obj");

		assetStore.AddModel("cube", cube);
		assetStore.AddModel("sphere", sphere);
		assetStore.AddModel("cylinder", cylinder);
		assetStore.AddModel("cone", cone);
		assetStore.AddModel("backpack", backpack);
		//assetStore.AddModel("snail", snail);
	}
}