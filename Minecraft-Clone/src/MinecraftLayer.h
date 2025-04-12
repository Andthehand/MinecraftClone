#pragma once
#include <RealEngine.h>

#include "Chunk.h"
#include "ChunkRenderer.h"

namespace RealEngine {
	struct CameraData {
		glm::mat4 ViewProjection;
		glm::vec3 CameraPosition;
	};

	struct ChunkRenderData {
		std::array<DrawElementsIndirectCommand*, 6> faceDrawCommands = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	};

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

		Scope<Chunk> m_Chunk;
		ChunkRenderer m_ChunkRenderer;
		ChunkRenderData m_ChunkRenderData;

		EditorCamera m_Camera;
		Ref<UniformBuffer> m_CameraUniformBuffer;

	};
}
