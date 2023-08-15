#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <glew/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <memory>

#include "../Components/TransformComponent.h"
#include "../Components/ModelComponent.h"
#include "../AssetStore/AssetStore.h"
#include "../Objects/Camera.h"
#include "../Utils/shader.h"

void RenderDebugBoxSystem(entt::registry& registry, Camera& camera, AssetStore& assetStore) {
	std::shared_ptr<Shader> shader = assetStore.GetShader("basicGeometry");
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 1200.0f / 720.0f, 0.1f, 100.0f);

	const auto entityView = registry.view<TransformComponent, ModelComponent>();
	for (const entt::entity entity : entityView) {
		const auto& transform = entityView.get<TransformComponent>(entity);
		const auto& modelComponent = entityView.get<ModelComponent>(entity);

		glm::vec3 rotation = glm::radians(transform.rotation);
		
		model = glm::mat4(1.0f);
		model = glm::translate(model, transform.position);
		model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, transform.scale);

		shader->Use();
		shader->SetMat4("view", view);
		shader->SetMat4("projection", projection);
		shader->SetMat4("model", model);
		shader->SetVec3("color", glm::vec3(0.0f, 1.0f, 0.0f));

		const auto& meshes = modelComponent.model->meshes;
		for (unsigned int i = 0; i < meshes.size(); ++i) {
			const auto& mesh = meshes[i];
			
			glBindVertexArray(mesh.boxVao);
			glDrawElements(GL_LINE_LOOP, mesh.boundingBoxIndices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}

		shader->SetVec3("color", glm::vec3(0.0f, 0.0f, 1.0f));
		glBindVertexArray(modelComponent.model->boxVao);
		glDrawElements(GL_LINE_LOOP, modelComponent.model->boundingBoxIndices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}