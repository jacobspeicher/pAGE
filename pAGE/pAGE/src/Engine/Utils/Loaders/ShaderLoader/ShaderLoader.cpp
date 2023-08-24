#include "ShaderLoader.h"

namespace ShaderLoader {
	void LoadShaders(AssetStore& assetStore) {
		std::shared_ptr<Shader> basicShader2D = std::make_shared<Shader>("Basic/basic2D.vert", "Basic/basic2D.frag");
		std::shared_ptr<Shader> basicShader3D = std::make_shared<Shader>("Basic/basic3D.vert", "Basic/basic3D.frag");
		std::shared_ptr<Shader> basicShaderGeometry = std::make_shared<Shader>("Basic/basicGeometry.vert", "Basic/basicGeometry.frag", "Basic/basicGeometry.shader");

		assetStore.AddShader("basic2D", basicShader2D);
		assetStore.AddShader("basic3D", basicShader3D);
		assetStore.AddShader("basicGeometry", basicShaderGeometry);
	}
}