#include "TextureLoader.h"

namespace TextureLoader {
	void LoadTextures(AssetStore& assetStore) {
		std::shared_ptr<Texture> container = std::make_shared<Texture>("Container/container_diffuse.png", "Container/container_specular.png");

		assetStore.AddTexture("container", container);
	}
}