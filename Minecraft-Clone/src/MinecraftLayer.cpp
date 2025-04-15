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

		m_Camera = CreateRef<GameCamera>(glm::vec3(std::pow(2, 9) * CS / 2, CS, std::pow(2, 9) * CS / 2));
		Window& window = Application::Get().GetWindow();
		m_Camera->HandleResolution(window.GetWidth(), window.GetHeight());

		RenderCommands::SetFaceCulling(true);
		window.HideCursor(true);

		m_ChunkManager = CreateScope<ChunkManager>(m_Camera);
	}

	void MinecraftLayer::OnUpdate(float deltaTime) {
		RE_PROFILE_FUNCTION();
		RenderCommands::Clear();

		// Update Camera and UBO
		m_Camera->OnUpdate(deltaTime);
		glm::mat4 viewProjection = m_Camera->GetViewProjection();
		glm::vec3 cameraPosition = m_Camera->GetPosition();
		CameraData cameraData = { viewProjection, cameraPosition };
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
		
		glm::vec3 cameraPosition = m_Camera->GetPosition();
		ImGui::Text("Camera Position: (%.2f, %.2f, %.2f)", cameraPosition.x, cameraPosition.y, cameraPosition.z);\
		const glm::vec3& cameraFront = m_Camera->GetFront();
		ImGui::Text("Camera Front: (%.2f, %.2f, %.2f)", cameraFront.x, cameraFront.y, cameraFront.z);

		ImGui::SliderFloat("Camera Speed", &m_Camera->GetSpeedRef(), 0.0f, 100.0f);

		ImGui::End();
	}

	void MinecraftLayer::OnEvent(Event& e) {
		m_Camera->OnEvent(e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(RE_BIND_EVENT_FN(MinecraftLayer::OnKeyPressed));
	}

	bool MinecraftLayer::OnKeyPressed(KeyPressedEvent& e) {
		if (e.GetKeyCode() == Key::Tab) {
			static bool cursorHidden = false;
			Application::Get().GetWindow().HideCursor(cursorHidden);
			cursorHidden = !cursorHidden;
		}

		return false;
	}
}
