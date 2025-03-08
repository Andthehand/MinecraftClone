#include "MinecraftLayer.h"

namespace RealEngine {
	MinecraftLayer::MinecraftLayer()
		: Layer("MinecraftLayer") {
	}


	void MinecraftLayer::OnAttach() {
		m_Shader = Shader::Create("assets/shaders/basic.shader");
	}

	void MinecraftLayer::OnUpdate(float deltaTime) {
		m_Shader->Bind();
		m_Chunk.Render();
	}

	void MinecraftLayer::OnImGui() {
	}

	void MinecraftLayer::OnEvent(Event& e) {}
}
