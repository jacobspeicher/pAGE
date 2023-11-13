#pragma once

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../Components/ModelComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/UIEntityNameComponent.h"

namespace ComponentUIHelper {
	void PopulateEntityNameComponent(UIEntityNameComponent& nameComponent, long entityId) {
		std::string title = nameComponent.entityName + " (" + std::to_string(entityId) + ")";
		ImGui::SeparatorText(title.c_str());
		ImGui::InputText("name", &nameComponent.entityName);
		ImGui::Spacing();
	}

	void PopulateTransformComponent(TransformComponent& transform) {
		ImGui::SeparatorText(transform.name.c_str());
		ImGui::DragFloat3("position", glm::value_ptr(transform.position), 0.1f);
		ImGui::DragFloat3("scale", glm::value_ptr(transform.scale), 0.1f);
		ImGui::DragFloat3("rotation", glm::value_ptr(transform.rotation), 0.5f);
		ImGui::Spacing();
	}

	void PopulateModelComponent(ModelComponent& model) {
		ImGui::SeparatorText(model.name.c_str());
		ImGui::Text("Nothing to see here");
		ImGui::Spacing();
	}

	void PopulateDirectionalLightComponent(DirectionalLightComponent& directionalLight) {
		ImGui::SeparatorText(directionalLight.name.c_str());
		ImGui::DragFloat3("direction", glm::value_ptr(directionalLight.direction), 0.1f);
		ImGui::ColorEdit3("ambient", glm::value_ptr(directionalLight.ambient));
		ImGui::ColorEdit3("diffuse", glm::value_ptr(directionalLight.diffuse));
		ImGui::ColorEdit3("specular", glm::value_ptr(directionalLight.specular));
		ImGui::Spacing();
	}
}