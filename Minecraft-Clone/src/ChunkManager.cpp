#include "ChunkManager.h"

#include "RealEngine.h"

namespace RealEngine {
	ChunkManager::ChunkManager(Ref<GameCamera> camera)
		: m_Camera(camera) {
		RE_PROFILE_FUNCTION();
		m_Shader = Shader::Create("assets/shaders/basic.shader");
		
		m_ChunkRenderer.init();

		auto circleFunction = [](float x) {
			return sqrt(RenderDistance * RenderDistance - x * x);
		};
	
		glm::ivec3 cameraposition = glm::ivec3(m_Camera->GetPosition()) / glm::ivec3(CS);
		for (float x = 0.5; x < RenderDistance; x += 1.0f) {
			uint8_t yChunkCnt = (uint8_t)std::roundf(circleFunction(x));

			//Right side of circle
			for (int i = -yChunkCnt; i < yChunkCnt; i++) {
				glm::ivec3 chunkOffset = { (int)(x - 0.5f + cameraposition.x), 0, i + cameraposition.z };
				Scope<Chunk> chunk = CreateScope<Chunk>(chunkOffset);

				MeshData meshData = chunk->Reuse();

				ChunkRenderData renderData;
				for (uint32_t j = 0; j < renderData.faceDrawCommands.size(); j++) {
					if (meshData.faceVertexLength[j]) {
						uint32_t baseInstance = (j << 28) | (chunkOffset.z << 18) | (chunkOffset.y << 9) | chunkOffset.x;

						auto drawCommand = m_ChunkRenderer.getDrawCommand(meshData.faceVertexLength[j], baseInstance);

						renderData.faceDrawCommands[j] = drawCommand;

						m_ChunkRenderer.buffer(*drawCommand, meshData.vertices->data() + meshData.faceVertexBegin[j]);
					}
				}

				m_Chunks.push_back({ std::move(chunk), renderData });
			}

			//Left side of circle
			for (int i = -yChunkCnt; i < yChunkCnt; i++) {
				glm::ivec3 chunkOffset = { cameraposition.z - (int)(x + 0.5f), 0, i + cameraposition.z };
				Scope<Chunk> chunk = CreateScope<Chunk>(chunkOffset);

				MeshData meshData = chunk->Reuse();

				ChunkRenderData renderData;
				for (uint32_t j = 0; j < renderData.faceDrawCommands.size(); j++) {
					if (meshData.faceVertexLength[j]) {
						uint32_t baseInstance = (j << 28) | (chunkOffset.z << 18) | (chunkOffset.y << 9) | chunkOffset.x;

						auto drawCommand = m_ChunkRenderer.getDrawCommand(meshData.faceVertexLength[j], baseInstance);

						renderData.faceDrawCommands[j] = drawCommand;

						m_ChunkRenderer.buffer(*drawCommand, meshData.vertices->data() + meshData.faceVertexBegin[j]);
					}
				}

				m_Chunks.push_back({ std::move(chunk), renderData });
			}
		}

		m_RenderStats.chunks = (uint32_t)m_Chunks.size();
	}

	void ChunkManager::Update() {
		// TODO: Implement
	}

	void ChunkManager::Render() {
		RE_PROFILE_FUNCTION();
		m_RenderStats.drawCalls = 0;
		m_RenderStats.quads = 0;

		for (const auto& chunk : m_Chunks) {
			for (int i = 0; i < chunk.RenderData.faceDrawCommands.size(); i++) {
				if (chunk.RenderData.faceDrawCommands[i]) {
					m_ChunkRenderer.addDrawCommand(*chunk.RenderData.faceDrawCommands[i]);
					m_RenderStats.drawCalls++;
					m_RenderStats.quads += chunk.RenderData.faceDrawCommands[i]->indexCount / 6;
				}
			}
		}

		m_ChunkRenderer.render(m_Shader);
	}
}