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

		modelComponent.shader->Use();
		modelComponent.shader->SetMat4("view", view);
		modelComponent.shader->SetMat4("projection", projection);
		modelComponent.shader->SetVec3("viewPos", camera.Position);
		modelComponent.shader->SetMat4("model", model);

		modelComponent.shader->SetVec3("selColor", glm::vec3((long)entity / 255.0f));

		modelComponent.shader->SetFloat("material.shininess", 256.0f);
		modelComponent.shader->SetVec3("dirLight.direction", glm::vec3(-1.0f, -1.0f, 0.0f));
		modelComponent.shader->SetVec3("dirLight.ambient", glm::vec3(0.2f));
		modelComponent.shader->SetVec3("dirLight.diffuse", glm::vec3(0.5f));
		modelComponent.shader->SetVec3("dirLight.specular", glm::vec3(0.1f));

		const auto& meshes = modelComponent.model->meshes;
		for (unsigned int i = 0; i < meshes.size(); ++i) {
			const auto& mesh = meshes[i];
			unsigned int diffuseNr = 1;
			unsigned int specularNr = 1;

			for (unsigned int j = 0; j < mesh.textures.size(); ++j) {
				glActiveTexture(GL_TEXTURE0 + j);

				std::string number;
				std::string name = mesh.textures[j].type;

				if (name == "texture_diffuse")
					number = std::to_string(diffuseNr++);
				else if (name == "texture_specular")
					number = std::to_string(specularNr++);

				modelComponent.shader->SetInt(("material." + name + number).c_str(), (int)j);
				glBindTexture(GL_TEXTURE_2D, mesh.textures[j].id);
			}

			glActiveTexture(GL_TEXTURE0);

			glBindVertexArray(mesh.vao);
			glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
		}
	}
}