#pragma once

#include "Chunk.h"
#include "ChunkRenderer.h"

#include "RealEngine.h"

#define RenderDistance 1

namespace RealEngine {
	struct ChunkRenderData {
		std::array<DrawElementsIndirectCommand*, 6> faceDrawCommands = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
	};

	struct ChunkData {
		Scope<Chunk> Chunk;
		ChunkRenderData ChunkRenderData;
	};

	class ChunkManager {
	public:
		ChunkManager();
		~ChunkManager() = default;

		void Update();
		void Render();
	private:
		Ref<Shader> m_Shader;

		std::vector<ChunkData> m_Chunks;

		ChunkRenderer m_ChunkRenderer;
	};
}