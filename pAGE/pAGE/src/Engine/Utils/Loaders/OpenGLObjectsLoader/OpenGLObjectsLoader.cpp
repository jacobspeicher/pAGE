#include "OpenGLObjectsLoader.h"

namespace OpenGLObjectsLoader {
	void LoadOpenGLObjects(AssetStore& assetStore) {
		/* Shapes */
		std::shared_ptr<Triangle> triangle = std::make_shared<Triangle>();

		assetStore.AddOpenGLObject("triangle", triangle);

		/* Models */
		std::shared_ptr<Cube> cube = std::make_shared<Cube>();

		assetStore.AddOpenGLObject("cube", cube);
	}
}