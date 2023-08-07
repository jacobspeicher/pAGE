#pragma once

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "TransformComponent.h"
#include "ModelComponent.h"
#include "ShapeComponent.h"

namespace ComponentUI {
	void PopulateTransformComponent(TransformComponent& transform) {
		ImGui::Text(transform.name.c_str());
		ImGui::Separator();
		ImGui::DragFloat3("position", glm::value_ptr(transform.position), 0.1f);
		ImGui::DragFloat3("scale", glm::value_ptr(transform.scale), 0.1f);
		ImGui::DragFloat3("rotation", glm::value_ptr(transform.rotation), 0.1f, 0.0f, 360.0f);
	}

	void PopulateModelComponent(ModelComponent& model) {
		ImGui::Text(model.name.c_str());
		ImGui::Separator();
		ImGui::Text("Nothing to see here");
	}

	void PopulateShapeComponent(ShapeComponent& shape) {
		ImGui::Text(shape.name.c_str());
		ImGui::Separator();
		ImGui::Text("Nothing to see here");
	}
}