#include "MinecraftLayer.h"

#include "Block.h"

#include <imgui.h>
#include <ryml.hpp>

namespace RealEngine {
	MinecraftLayer::MinecraftLayer()
		: Layer("MinecraftLayer") { }

	void MinecraftLayer::OnAttach() {
		RE_PROFILE_FUNCTION();

		glm::vec3 color = { 0.0f, 0.0f, 1.0f };
		m_CameraUniformBuffer = UniformBuffer::Create(&color, sizeof(CameraData), 0);

		m_Camera = GameCamera(glm::vec3(0));
		Window& window = Application::Get().GetWindow();
		m_Camera.HandleResolution(window.GetWidth(), window.GetHeight());

		RenderCommands::SetFaceCulling(true);
		window.HideCursor(true);

		m_ChunkManager = CreateScope<ChunkManager>();
	}

	void MinecraftLayer::OnUpdate(float deltaTime) {
		RE_PROFILE_FUNCTION();
		RenderCommands::Clear();

		// Update Camera and UBO
		m_Camera.OnUpdate(deltaTime);
		glm::mat4 viewProjection = m_Camera.GetViewProjection();
		glm::vec3 cameraPosition = m_Camera.GetPosition();
		glm::ivec3 intCamPosition = glm::ivec3(floor(m_Camera.GetPosition()));
		CameraData cameraData = { viewProjection, cameraPosition, intCamPosition };
		m_CameraUniformBuffer->SetData(&cameraData, sizeof(CameraData));

		// Render Chunks
		m_ChunkManager->Update();
		m_ChunkManager->Render();
	}

	void MinecraftLayer::OnImGui() {
		RE_PROFILE_FUNCTION();

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("OpenGL")) {
				static bool wireframe = false;
				if (ImGui::MenuItem("Wireframe", nullptr, &wireframe)) {
					RenderCommands::SetWireframe(wireframe);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		ImGui::Begin("Minecraft Settings");
		
		ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", m_Camera.GetPosition().x, m_Camera.GetPosition().y, m_Camera.GetPosition().z);
		ImGui::Text("Camera Rotation: (%.2f, %.2f)", m_Camera.GetYaw(), m_Camera.GetPitch());

		ImGui::End();
	}

	void MinecraftLayer::OnEvent(Event& e) {
		m_Camera.OnEvent(e);
	}
}
