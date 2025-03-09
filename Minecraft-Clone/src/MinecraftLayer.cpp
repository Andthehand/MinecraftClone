#include "MinecraftLayer.h"

namespace RealEngine {
	MinecraftLayer::MinecraftLayer()
		: Layer("MinecraftLayer") {
	}


	void MinecraftLayer::OnAttach() {
		m_Shader = Shader::Create("assets/shaders/basic.shader");

		glm::vec3 color = { 0.0f, 0.0f, 1.0f };
		m_UniformBuffer = UniformBuffer::Create(&color, sizeof(glm::vec3), 0);
	}

	void MinecraftLayer::OnUpdate(float deltaTime) {
		m_Shader->Bind();
		m_Chunk.Render();
	}

	void MinecraftLayer::OnImGui() {
	}

	void MinecraftLayer::OnEvent(Event& e) {}
}
