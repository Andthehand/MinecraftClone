#include "MinecraftLayer.h"

#include <imgui.h>

namespace RealEngine {
	MinecraftLayer::MinecraftLayer()
		: Layer("MinecraftLayer") {
	}


	void MinecraftLayer::OnAttach() {
		m_Shader = Shader::Create("assets/shaders/basic.shader");

		glm::vec3 color = { 0.0f, 0.0f, 1.0f };
		m_UniformBuffer = UniformBuffer::Create(&color, sizeof(glm::mat4), 0);

		m_Camera = EditorCamera(90.0f, 1.778f, 0.1f, 1000.0f);
	}

	void MinecraftLayer::OnUpdate(float deltaTime) {
		m_Camera.OnUpdate(deltaTime);

		RenderCommands::Clear();

		glm::mat4 viewProjection = m_Camera.GetViewProjection();
		m_UniformBuffer->SetData(&viewProjection, sizeof(glm::mat4));

		m_Shader->Bind();
		m_Chunk.Render();
	}

	void MinecraftLayer::OnImGui() {
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
	}

	void MinecraftLayer::OnEvent(Event& e) {
		m_Camera.OnEvent(e);
	}
}
