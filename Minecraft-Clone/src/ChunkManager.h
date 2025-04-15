#pragma once

#include "Chunk.h"
#include "ChunkRenderer.h"
#include "GameCamera.h"

#include "RealEngine.h"

#define RenderDistance 40

namespace RealEngine {
	struct ChunkRenderData {
		std::array<DrawElementsIndirectCommand*, 6> faceDrawCommands = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	};

	struct ChunkData {
		Scope<Chunk> Object;
		ChunkRenderData RenderData;
	};

	class ChunkManager {
	public:
		ChunkManager(Ref<GameCamera> camera);
		~ChunkManager() = default;

		void Update();
		void Render();
	private:
		Ref<GameCamera> m_Camera;
		Ref<Shader> m_Shader;

		std::vector<ChunkData> m_Chunks;

		ChunkRenderer m_ChunkRenderer;
	};
}