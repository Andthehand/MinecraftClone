#include "ChunkManager.h"

#include "RealEngine.h"
#include <glm/gtc/matrix_access.hpp>

namespace RealEngine {
	namespace Utils {
		void ExtractFrustum(glm::vec4* frustumPlanes, const glm::mat4& viewProjectionMatrix) {
			RE_PROFILE_FUNCTION();

			// Extract the right plane
			frustumPlanes[0] = glm::row(viewProjectionMatrix, 3) - glm::row(viewProjectionMatrix, 0);

			// Extract the left plane
			frustumPlanes[1] = glm::row(viewProjectionMatrix, 3) + glm::row(viewProjectionMatrix, 0);

			// Extract the top plane
			frustumPlanes[2] = glm::row(viewProjectionMatrix, 3) - glm::row(viewProjectionMatrix, 1);

			// Extract the bottom plane
			frustumPlanes[3] = glm::row(viewProjectionMatrix, 3) + glm::row(viewProjectionMatrix, 1);

			// Extract the near plane
			frustumPlanes[4] = glm::row(viewProjectionMatrix, 3) + glm::row(viewProjectionMatrix, 2);

			// Extract the far plane
			frustumPlanes[5] = glm::row(viewProjectionMatrix, 3) - glm::row(viewProjectionMatrix, 2);

			for (int i = 0; i < 6; i++) {
				frustumPlanes[i] = glm::normalize(frustumPlanes[i]);
			}
		}

		//This is to check if the bounding boxes for the chunks are in the frustum
		bool IntersectFrustum(const glm::vec4* frustumPlanes, const glm::vec3& min, const glm::vec3& max) {
			RE_PROFILE_FUNCTION();

			// Check if each corner of the bounding box is inside the frustum
			for (int i = 0; i < 6; i++) {
				if (glm::dot(frustumPlanes[i], glm::vec4(min, 1.0f)) < 0 &&
					glm::dot(frustumPlanes[i], glm::vec4(max, 1.0f)) < 0 &&
					glm::dot(frustumPlanes[i], glm::vec4(min.x, min.y, max.z, 1.0f)) < 0 &&
					glm::dot(frustumPlanes[i], glm::vec4(min.x, max.y, min.z, 1.0f)) < 0 &&
					glm::dot(frustumPlanes[i], glm::vec4(min.x, max.y, max.z, 1.0f)) < 0 &&
					glm::dot(frustumPlanes[i], glm::vec4(max.x, min.y, min.z, 1.0f)) < 0 &&
					glm::dot(frustumPlanes[i], glm::vec4(max.x, min.y, max.z, 1.0f)) < 0 &&
					glm::dot(frustumPlanes[i], glm::vec4(max.x, max.y, min.z, 1.0f)) < 0) {
					return false;
				}
			}
			return true;
		}
	}

	ChunkManager::ChunkManager(Ref<GameCamera> camera)
		: m_Camera(camera) {
		RE_PROFILE_FUNCTION();
		m_Shader = Shader::Create("assets/shaders/basic.shader");
		
		m_ChunkRenderer.init();

		auto circleFunction = [](float x) {
			return sqrt(RenderDistance * RenderDistance - x * x);
		};
	
		glm::ivec3 cameraPosition = glm::ivec3(m_Camera->GetPosition()) / glm::ivec3(CS);
		for (float x = 0.5; x < RenderDistance; x += 1.0f) {
			uint8_t yChunkCnt = (uint8_t)std::roundf(circleFunction(x));

			//Right side of circle
			for (int i = -yChunkCnt; i < yChunkCnt; i++) {
				glm::ivec3 chunkOffset = { (int)(x - 0.5f + cameraPosition.x), 0, i + cameraPosition.z };
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
				glm::ivec3 chunkOffset = { cameraPosition.z - (int)(x + 0.5f), 0, i + cameraPosition.z };
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
	}

	void ChunkManager::Update() {
		// TODO: Implement
	}

	void ChunkManager::Render() {
		RE_PROFILE_FUNCTION();
		m_RenderStats.DrawCalls = 0;
		m_RenderStats.Quads = 0;
		m_RenderStats.Chunks = 0;

		glm::vec4 frustumPlanes[6];
		Utils::ExtractFrustum(frustumPlanes, m_Camera->GetViewProjection());

		for (const auto& chunk : m_Chunks) {
			if (Utils::IntersectFrustum(frustumPlanes, chunk.Object->GetMin(), chunk.Object->GetMax())) {
				for (int i = 0; i < chunk.RenderData.faceDrawCommands.size(); i++) {
					if (chunk.RenderData.faceDrawCommands[i]) {
						m_ChunkRenderer.addDrawCommand(*chunk.RenderData.faceDrawCommands[i]);
						m_RenderStats.DrawCalls++;
						m_RenderStats.Quads += chunk.RenderData.faceDrawCommands[i]->indexCount / 6;
					}
				}

				m_RenderStats.Chunks++;
			}
		}

		m_ChunkRenderer.render(m_Shader);
	}
}