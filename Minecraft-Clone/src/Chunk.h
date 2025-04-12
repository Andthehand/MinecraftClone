#pragma once 
#include <RealEngine.h>

#include <cstdint>

#include "Block.h"

#define CHUNK_SIZE_WIDTH 62

static constexpr int CS = CHUNK_SIZE_WIDTH;

static constexpr int CS_P = CHUNK_SIZE_WIDTH + 2;
static constexpr int CS_2 = CS * CS;
static constexpr int CS_P2 = CS_P * CS_P;
static constexpr int CS_P3 = CS_P * CS_P * CS_P;

namespace RealEngine {
	struct MeshData {
		uint64_t* faceMasks = nullptr; // CS_2 * 6
		uint64_t* opaqueMask = nullptr; //CS_P2
		uint8_t* forwardMerged = nullptr; // CS_2
		uint8_t* rightMerged = nullptr; // CS
		std::vector<uint64_t>* vertices = nullptr;
		int vertexCount = 0;
		int maxVertices = 0;
		int faceVertexBegin[6] = { 0 };
		int faceVertexLength[6] = { 0 };
	};

	class Chunk {
	public:
		Chunk(const glm::ivec3& chunkOffset);
		~Chunk() = default;

		MeshData Generate();

		void GenerateMesh(MeshData& meshData);

		glm::ivec3& GetChunkOffset() { return m_ChunkOffset; }
	private:
		BlockType m_Blocks[CS_P * CS_P * CS_P];

		glm::ivec3 m_ChunkOffset;
	};
}