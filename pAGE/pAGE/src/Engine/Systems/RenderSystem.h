#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <glew/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Components/TransformComponent.h"
#include "../Components/ModelComponent.h"
#include "../Components/ShapeComponent.h"
#include "../Objects/Camera.h"

void RenderSystem(entt::registry& registry, Camera& camera) {
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 1200.0f / 720.0f, 0.1f, 100.0f);

	/*const auto shapeView = registry.view<TransformComponent, ShapeComponent>();
	for (const entt::entity entity : shapeView) {
		const auto& transform = shapeView.get<TransformComponent>(entity);
		const auto& shapeComponent = shapeView.get<ShapeComponent>(entity);

		glm::vec3 rotation = glm::radians(transform.rotation);

		model = glm::mat4(1.0f);
		model = glm::translate(model, transform.position);
		model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, transform.scale);

		glBindVertexArray(shapeComponent.vao);

		shapeComponent.shader->Use();
		shapeComponent.shader->SetMat4("projection", projection);
		shapeComponent.shader->SetMat4("view", view);
		shapeComponent.shader->SetMat4("model", model);
		shapeComponent.shader->SetVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));

		glDrawArrays(GL_TRIANGLES, 0, 3);
	}*/

	const auto modelView = registry.view<TransformComponent, ModelComponent>();
	for (const entt::entity entity : modelView) {
		const auto& transform = modelView.get<TransformComponent>(entity);
		const auto& modelComponent = modelView.get<ModelComponent>(entity);

		glm::vec3 rotation = glm::radians(transform.rotation);

		model = glm::mat4(1.0f);
		model = glm::translate(model, transform.position);
		model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, transform.scale);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, modelComponent.texture->diffuse);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, modelComponent.texture->specular);

		glBindVertexArray(modelComponent.vao);

		modelComponent.shader->Use();
		modelComponent.shader->SetMat4("view", view);
		modelComponent.shader->SetMat4("projection", projection);
		modelComponent.shader->SetVec3("viewPos", camera.Position);
		modelComponent.shader->SetMat4("model", model);

		modelComponent.shader->SetInt("material.diffuse", 0);
		modelComponent.shader->SetInt("material.specular", 1);
		modelComponent.shader->SetFloat("material.shininess", 256.0f);
		modelComponent.shader->SetVec3("dirLight.direction", glm::vec3(-1.0f, -1.0f, 0.0f));
		modelComponent.shader->SetVec3("dirLight.ambient", glm::vec3(0.2f));
		modelComponent.shader->SetVec3("dirLight.diffuse", glm::vec3(0.5f));
		modelComponent.shader->SetVec3("dirLight.specular", glm::vec3(0.1f));

		glDrawArrays(GL_TRIANGLES, 0, modelComponent.triangles.size() * 3);
	}
}