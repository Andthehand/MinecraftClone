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

	struct RenderStats {
		uint32_t drawCalls = 0;
		uint32_t quads = 0;

		uint32_t chunks = 0;
	};

	class ChunkManager {
	public:
		ChunkManager(Ref<GameCamera> camera);
		~ChunkManager() = default;

		void Update();
		void Render();

		const RenderStats& GetRenderStats() const { return m_RenderStats; }
	private:
		Ref<GameCamera> m_Camera;
		Ref<Shader> m_Shader;

		std::vector<ChunkData> m_Chunks;

		ChunkRenderer m_ChunkRenderer;
		RenderStats m_RenderStats;
	};
}