#pragma once
#include <RealEngine.h>

#include "Chunk.h"
#include "ChunkRenderer.h"
#include "ChunkManager.h"
#include "GameCamera.h"

namespace RealEngine {
	struct CameraData {
		glm::mat4 ViewProjection;
		glm::vec3 CameraPosition;
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
		GameCamera m_Camera;
		Ref<UniformBuffer> m_CameraUniformBuffer;

		Scope<ChunkManager> m_ChunkManager;
	};
}
