#pragma once

#include <entt/entity/entity.hpp>
#include <entt/entity/registry.hpp>

#include <glm/glm.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>

#include <imfilebrowser/imfilebrowser.h>

#include "../Utils/ComponentUIHelper.h"
#include "../Components/DirectionalLightComponent.h"
#include "../Components/ModelComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/UIEntityNameComponent.h"

#include "../AssetStore/AssetStore.h"

#include "../Objects/Camera.h"

#include "../Utils/Globals/Globals.h"

void ShowSceneHierarchy(entt::registry& registry, AssetStore& assetStore);
void RenderSceneHierarchyList(entt::registry& registry);
void ShowScene(entt::registry& registry, AssetStore& assetStore, Camera& camera);
void SelectSceneObject(entt::registry& registry, AssetStore& assetStore, glm::vec2 regionMouse);
void ShowInspector(entt::registry& registry);

void RenderUISystem(entt::registry& registry, AssetStore& assetStore, Camera& camera) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow();

	ShowSceneHierarchy(registry, assetStore);

	ShowScene(registry, assetStore, camera);

	ShowInspector(registry);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ShowSceneHierarchy(entt::registry& registry, AssetStore& assetStore) {
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_MenuBar;
	ImGui::SetNextWindowPos(ImVec2(10, 10));
	ImGui::SetNextWindowSize(ImVec2(332, 755));

	ImGui::Begin("Scene Hierarchy", NULL, window_flags);

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("2D")) {
			if (ImGui::MenuItem("Triangle")) {
				static int num = 0;
				auto triangle = registry.create();
				std::string name = "Triangle (" + std::to_string((long)triangle) + ")";
				registry.emplace<UIEntityNameComponent>(triangle, name);
				registry.emplace<TransformComponent>(triangle, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f));
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("3D")) {
			ImGui::SeparatorText("Basic");
			if (ImGui::MenuItem("Cube")) {
				static int num = 0;
				std::shared_ptr<Shader> shader = assetStore.GetShader("basic3D");
				auto cube = registry.create();
				std::string name = "Cube " + std::to_string(num);
				registry.emplace<UIEntityNameComponent>(cube, name);
				registry.emplace<TransformComponent>(cube, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f));
				registry.emplace<ModelComponent>(cube, assetStore.GetModel("cube"), shader);
			}
			if (ImGui::MenuItem("Sphere")) {
				static int num = 0;
				std::shared_ptr<Shader> shader = assetStore.GetShader("basic3D");
				auto sphere = registry.create();
				std::string name = "Sphere " + std::to_string(num);
				registry.emplace<UIEntityNameComponent>(sphere, name);
				registry.emplace<TransformComponent>(sphere, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f));
				registry.emplace<ModelComponent>(sphere, assetStore.GetModel("sphere"), shader);
			}
			if (ImGui::MenuItem("Cylinder")) {
				static int num = 0;
				std::shared_ptr<Shader> shader = assetStore.GetShader("basic3D");
				auto cylinder = registry.create();
				std::string name = "Cylinder " + std::to_string(num);
				registry.emplace<UIEntityNameComponent>(cylinder, name);
				registry.emplace<TransformComponent>(cylinder, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f));
				registry.emplace<ModelComponent>(cylinder, assetStore.GetModel("cylinder"), shader);
			}
			if (ImGui::MenuItem("Cone")) {
				static int num = 0;
				std::shared_ptr<Shader> shader = assetStore.GetShader("basic3D");
				auto cone = registry.create();
				std::string name = "Cone " + std::to_string(num);
				registry.emplace<UIEntityNameComponent>(cone, name);
				registry.emplace<TransformComponent>(cone, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f));
				registry.emplace<ModelComponent>(cone, assetStore.GetModel("cone"), shader);
			}

			ImGui::SeparatorText("Custom");
			if (ImGui::MenuItem("Backpack")) {
				static int num = 0;
				std::shared_ptr<Shader> shader = assetStore.GetShader("basic3D");
				auto backpack = registry.create();
				std::string name = "Backpack " + std::to_string(num);
				registry.emplace<UIEntityNameComponent>(backpack, name);
				registry.emplace<TransformComponent>(backpack, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f));
				registry.emplace<ModelComponent>(backpack, assetStore.GetModel("backpack"), shader);
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Lights")) {
			if (ImGui::MenuItem("Directional")) {
				static int num = 0;
				auto directional = registry.create();
				std::string name = "Directional " + std::to_string(num);
				std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>();
				registry.emplace<UIEntityNameComponent>(directional, name);
				registry.emplace<TransformComponent>(directional, glm::vec3(0.0f), glm::vec3(1.0f), glm::vec3(0.0f));
				registry.emplace<DirectionalLightComponent>(directional, glm::vec3(0, 0, -1), glm::vec3(0.1f), glm::vec3(1.0f), glm::vec3(0.5f));
				registry.emplace<SpriteComponent>(directional, sprite, assetStore.GetTexture("directionalLight"), assetStore.GetShader("basic2D"));
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	RenderSceneHierarchyList(registry);

	ImGui::End();
}

void RenderSceneHierarchyList(entt::registry& registry) {
	const auto entityView = registry.view<UIEntityNameComponent>();
	for (int i = entityView.size() - 1; i >= 0; i--) {
		const auto& entity = entityView[i];
		const auto& nameComponent = entityView.get<UIEntityNameComponent>(entity);
		std::string title = std::to_string((long)entity) + " : " + nameComponent.entityName;
		if (ImGui::Selectable(title.c_str(), Globals::selected == (long)entity)) {
			Globals::selected = (long)entity;
		}
		if (ImGui::BeginPopupContextItem()) {
			Globals::selected = (long)entity;
			if (ImGui::Button("Delete")) {
				registry.destroy(entity);
				Globals::selected = -1;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}
	/*for (const entt::entity entity : entityView) {
		const auto& nameComponent = entityView.get<UIEntityNameComponent>(entity);
		std::string title = std::to_string((long)entity) + " : " + nameComponent.entityName;
		if (ImGui::Selectable(title.c_str(), Globals::selected == (long)entity)) {
			Globals::selected = (long)entity;
		}
		if (ImGui::BeginPopupContextItem()) {
			Globals::selected = (long)entity;
			if (ImGui::Button("Delete")) {
				registry.destroy(entity);
				Globals::selected = -1;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}*/
}

void ShowScene(entt::registry& registry, AssetStore& assetStore, Camera& camera) {
	ImGuiIO* io = &ImGui::GetIO(); (void)io;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoBringToFrontOnFocus;
	ImGui::SetNextWindowPos(ImVec2(352, 10));
	ImGui::SetNextWindowSize(ImVec2(1216, 755));

	ImGuiWindowFlags overlay_window_flags = ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoNav |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoMouseInputs;

	ImGui::Begin("Scene", NULL, window_flags);

	ImTextureID texture;
	if (Globals::debug)
		texture = (ImTextureID)*assetStore.GetOpenGLBinding("selColorBuffer");
	else
		texture = (ImTextureID)*assetStore.GetOpenGLBinding("texColorBuffer");
	ImGui::Image(texture, ImVec2(1200, 720), ImVec2(0, 1), ImVec2(1, 0));

	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 childWindowPos, childWindowPosPivot;
	childWindowPos.x = windowPos.x + windowSize.x - 10.0f;
	childWindowPos.y = windowPos.y + 30.0f;
	childWindowPosPivot.x = 1.0f;
	childWindowPosPivot.y = 0.0f;
	ImGui::SetNextWindowPos(childWindowPos, ImGuiCond_Always, childWindowPosPivot);
	ImGui::SetNextWindowBgAlpha(0.35f);
	ImGui::Begin("Scene Overlay", NULL, overlay_window_flags);

	glm::vec2 regionMouse(0.0f);
	regionMouse.x = io->MousePos.x - windowPos.x - 8; // account for image window padding
	regionMouse.y = io->MousePos.y - windowPos.y - 27; // account for image window padding

	ImGui::Text("Camera Position\nx: %.2f, y: %.2f, z: %.2f", camera.Position.x, camera.Position.y, camera.Position.z);
	ImGui::Text("Mouse Position\nx: %.2f, y: %.2f", regionMouse.x, regionMouse.y);

	ImGui::End();

	SelectSceneObject(registry, assetStore, regionMouse);

	if (ImGui::IsItemHovered()) {
		if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
			Globals::mouseIsCaptured = true;
			SDL_SetRelativeMouseMode((SDL_bool)true);
		}
	}

	ImGui::End();
}

void SelectSceneObject(entt::registry& registry, AssetStore& assetStore, glm::vec2 regionMouse) {
	const auto entityView = registry.view<UIEntityNameComponent>();
	if (ImGui::IsItemHovered()) {
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left, false)) {
			glm::vec2 ndc(
				(2.0f * regionMouse.x) / 1200.0f - 1.0f,
				1.0f - (2.0f * regionMouse.y) / 720.f
			);

			glBindFramebuffer(GL_FRAMEBUFFER, *assetStore.GetOpenGLBinding("sceneFramebuffer"));
			glReadBuffer(GL_COLOR_ATTACHMENT1);
			unsigned char data[3];
			glReadPixels((GLint)regionMouse.x, (GLint)(720 - regionMouse.y), (GLint)1, (GLint)1, GL_RGB, GL_UNSIGNED_BYTE, data);

			if (data[2] != 0) {
				for (const entt::entity entity : entityView) {
					if ((long)entity == data[2] - 1.0f) {
						Globals::selected = (long)entity;
						break;
					}
				}
			}
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}
}

void ShowInspector(entt::registry& registry) {
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse;
	ImGui::SetNextWindowPos(ImVec2(1578, 10));
	ImGui::SetNextWindowSize(ImVec2(332, 755));

	ImGui::Begin("Inspector", NULL, window_flags);

	if (Globals::selected != -1) {
		const entt::entity entity = (entt::entity)Globals::selected;

		if (registry.all_of<UIEntityNameComponent>(entity)) {
			ComponentUIHelper::PopulateEntityNameComponent(registry.get<UIEntityNameComponent>(entity), Globals::selected);
		}
		if (registry.all_of<TransformComponent>(entity)) {
			ComponentUIHelper::PopulateTransformComponent(registry.get<TransformComponent>(entity));
		}
		if (registry.all_of<ModelComponent>(entity)) {
			ComponentUIHelper::PopulateModelComponent(registry.get<ModelComponent>(entity));
		}
		if (registry.all_of<DirectionalLightComponent>(entity)) {
			ComponentUIHelper::PopulateDirectionalLightComponent(registry.get<DirectionalLightComponent>(entity));
		}
	}

	ImGui::End();
}