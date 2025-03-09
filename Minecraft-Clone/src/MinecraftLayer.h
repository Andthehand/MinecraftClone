#pragma once
#include <RealEngine.h>

#include "Chunk.h"

namespace RealEngine {
	class MinecraftLayer : public Layer {
	public:
		MinecraftLayer();
		virtual ~MinecraftLayer() = default;
		virtual void OnAttach() override;
		virtual void OnUpdate(float deltaTime) override;
		virtual void OnImGui() override;
		virtual void OnEvent(Event& e) override;
	private:
		Ref<Shader> m_Shader;
		Ref<UniformBuffer> m_UniformBuffer;

		EditorCamera m_Camera;

		Chunk m_Chunk;
	};
}
