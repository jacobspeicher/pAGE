#pragma once

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "TransformComponent.h"
#include "ModelComponent.h"

namespace ComponentUI {
	void PopulateTransformComponent(TransformComponent& transform) {
		ImGui::Text(transform.name.c_str());
		ImGui::Separator();
		ImGui::SliderFloat3("position", glm::value_ptr(transform.position), -1.0f, 1.0f);
		ImGui::SliderFloat3("scale", glm::value_ptr(transform.scale), 0.0f, 1.0f);
		ImGui::SliderFloat3("rotation", glm::value_ptr(transform.rotation), 0.0f, 360.0f);
	}

	void PopulateModelComponent(ModelComponent& model) {
		ImGui::Text(model.name.c_str());
		ImGui::Separator();
		ImGui::Text("Nothing to see here");
	}
}