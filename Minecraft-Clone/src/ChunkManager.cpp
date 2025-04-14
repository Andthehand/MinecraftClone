#include "ChunkManager.h"

#include "RealEngine.h"

namespace RealEngine {
	ChunkManager::ChunkManager() {
		RE_PROFILE_FUNCTION();
		m_Shader = Shader::Create("assets/shaders/basic.shader");
		
		m_ChunkRenderer.init();

		auto circleFunction = [](float x) {
			return sqrt(RenderDistance * RenderDistance - x * x);
		};
	
		for (float x = 0.5; x < RenderDistance; x += 1.0f) {
			uint8_t yChunkCnt = (uint8_t)std::roundf(circleFunction(x));

			//Right side of circle
			for (int i = -yChunkCnt; i < yChunkCnt; i++) {
				glm::ivec3 chunkOffset = { (int)(x - 0.5f), 0, -i };
				Scope<Chunk> chunk = CreateScope<Chunk>(chunkOffset);

				MeshData meshData = chunk->Reuse();

				ChunkRenderData renderData;
				for (uint32_t j = 0; j < renderData.faceDrawCommands.size(); j++) {
					if (meshData.faceVertexLength[j]) {
						uint32_t baseInstance = (j << 24) | (chunkOffset.z << 16) | (chunkOffset.y << 8) | chunkOffset.x;

						auto drawCommand = m_ChunkRenderer.getDrawCommand(meshData.faceVertexLength[j], baseInstance);

						renderData.faceDrawCommands[j] = drawCommand;

						m_ChunkRenderer.buffer(*drawCommand, meshData.vertices->data() + meshData.faceVertexBegin[j]);
					}
				}

				m_Chunks.push_back({ std::move(chunk), renderData });
			}

			//Left side of circle
			for (int i = -yChunkCnt; i < yChunkCnt; i++) {
				glm::ivec3 chunkOffset = { -(int)(x + 0.5f), 0, i };
				Scope<Chunk> chunk = CreateScope<Chunk>(chunkOffset);

				MeshData meshData = chunk->Reuse();

				ChunkRenderData renderData;
				for (uint32_t j = 0; j < renderData.faceDrawCommands.size(); j++) {
					if (meshData.faceVertexLength[j]) {
						uint32_t baseInstance = (j << 24) | (chunkOffset.z << 16) | (chunkOffset.y << 8) | chunkOffset.x;

						auto drawCommand = m_ChunkRenderer.getDrawCommand(meshData.faceVertexLength[j], baseInstance);

						renderData.faceDrawCommands[j] = drawCommand;

						m_ChunkRenderer.buffer(*drawCommand, meshData.vertices->data() + meshData.faceVertexBegin[j]);
					}
				}

				m_Chunks.push_back({ std::move(chunk), renderData });
			}
		}
	}

	void ChunkManager::Update() {
		// TODO: Implement
	}

	void ChunkManager::Render() {
		for (const auto& chunk : m_Chunks) {
			for (int i = 0; i < chunk.ChunkRenderData.faceDrawCommands.size(); i++) {
				if (chunk.ChunkRenderData.faceDrawCommands[i]) {
					m_ChunkRenderer.addDrawCommand(*chunk.ChunkRenderData.faceDrawCommands[i]);
				}
			}
		}

		m_ChunkRenderer.render(m_Shader);
	}
}