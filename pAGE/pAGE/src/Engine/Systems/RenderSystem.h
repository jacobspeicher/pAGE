#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <glew/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Components/TransformComponent.h"
#include "../Components/ModelComponent.h"

void RenderSystem(entt::registry& registry) {
	const auto view = registry.view<TransformComponent, ModelComponent>();
	for (const entt::entity entity : view) {
		const auto& transform = view.get<TransformComponent>(entity);
		const auto& modelComponent = view.get<ModelComponent>(entity);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, transform.position);
		model = glm::rotate(model, transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, transform.scale);

		glBindVertexArray(modelComponent.vao);

		modelComponent.shader->Use();
		modelComponent.shader->SetMat4("model", model);
		modelComponent.shader->SetVec3("color", glm::vec3(255, 255, 255));

		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
}