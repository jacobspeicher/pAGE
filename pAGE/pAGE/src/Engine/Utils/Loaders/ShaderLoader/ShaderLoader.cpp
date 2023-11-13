#include "ShaderLoader.h"

namespace ShaderLoader {
	void LoadShaders(AssetStore& assetStore) {
		std::shared_ptr<Shader> basicShader2D = std::make_shared<Shader>("Basic/Basic2D.vert", "Basic/Basic2D.frag");
		std::shared_ptr<Shader> basicShader3D = std::make_shared<Shader>("Basic/Basic3D.vert", "Basic/Basic3D.frag");
		std::shared_ptr<Shader> basicShaderGeometry = std::make_shared<Shader>("Basic/BasicGeometry.vert", "Basic/BasicGeometry.frag", "Basic/BasicGeometry.shader");

		assetStore.AddShader("basic2D", basicShader2D);
		assetStore.AddShader("basic3D", basicShader3D);
		assetStore.AddShader("basicGeometry", basicShaderGeometry);
	}
}