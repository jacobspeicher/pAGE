#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>
#include <glm/glm.hpp>

#include "../Components/ModelComponent.h"
#include "../Components/TransformComponent.h"
#include "../Objects/Camera.h"

bool RayIntersectsTriangle(glm::vec3 origin, glm::vec3 direction, glm::mat4 model, std::vector<glm::vec3> triangle, float& distance);

void SelectObjectSystem(entt::registry& registry, Camera& camera, glm::vec2 ndcMouse, int& outSelected) {
	glm::mat4 inverseProjection = glm::inverse(glm::perspective(glm::radians(camera.Zoom), 1200.0f / 720.0f, 0.1f, 100.0f));
	glm::mat4 inverseView = glm::inverse(camera.GetViewMatrix());
	glm::vec4 viewMouse = inverseProjection * glm::vec4(ndcMouse.x, ndcMouse.y, -1.0f, 1.0f);
	glm::vec3 worldMouse = inverseView * glm::vec4(viewMouse.x, viewMouse.y, -1.0f, 0.0f);
	worldMouse = glm::normalize(worldMouse);

	glm::mat4 model(1.0f);

	float minDistance = 1000.0f;
	long selected = -1;
	const auto view = registry.view<TransformComponent, ModelComponent>();
	for (const entt::entity entity : view) {
		const auto& transform = registry.get<TransformComponent>(entity);
		const auto& modelComponent = registry.get<ModelComponent>(entity);
		glm::vec3 rotation = glm::radians(transform.rotation);

		model = glm::mat4(1.0f);
		model = glm::translate(model, transform.position);
		model = glm::rotate(model, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, transform.scale);

		for (Mesh mesh : modelComponent.model->meshes) {
			for (std::vector<glm::vec3> triangle : mesh.triangles) {
				float distance;
				if (RayIntersectsTriangle(camera.Position, worldMouse, model, triangle, distance)) {
					if (distance < minDistance) {
						minDistance = distance;
						selected = (long)entity;
						break;
					}
				}
			}
			if (selected == (long)entity)
				break;
		}
		// TODO : this is gonna have to get changed when object deletion is supported
		if (selected != -1)
			outSelected = selected;
	}
}

// Moller-Trumbore
bool RayIntersectsTriangle(glm::vec3 origin, glm::vec3 direction, glm::mat4 model, std::vector<glm::vec3> triangle, float& distance) {
	glm::vec3 vert0 = model * glm::vec4(triangle[0], 1.0f);
	glm::vec3 vert1 = model * glm::vec4(triangle[1], 1.0f);
	glm::vec3 vert2 = model * glm::vec4(triangle[2], 1.0f);
	glm::vec3 edge1, edge2, h, s, q;
	float a, f, u, v;
	edge1 = vert1 - vert0;
	edge2 = vert2 - vert0;
	h = glm::cross(direction, edge2);
	a = glm::dot(edge1, h);

	// ray and triangle are parllel or degenerate
	if (a > -glm::epsilon<float>() && a < glm::epsilon<float>())
		return false;

	f = 1.0f / a;
	s = origin - vert0;
	u = f * glm::dot(s, h);

	if (u < 0.0f || u > 1.0f)
		return false;

	q = glm::cross(s, edge1);
	v = f * glm::dot(direction, q);

	if (v < 0.0f || u + v > 1.0f)
		return false;

	// compute t to find out where along the line it intersects
	float t = f * glm::dot(edge2, q);

	if (t > glm::epsilon<float>()) {
		// intersection = origin + direction * t;
		distance = glm::length(direction * t);
		return true;
	}

	return false;
}