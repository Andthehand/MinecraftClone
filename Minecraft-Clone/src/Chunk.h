#pragma once 
#include <RealEngine.h>

#include <cstdint>

#include "Block.h"

static constexpr int CS = 62;

static constexpr int CS_P = CS + 2;
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
		Chunk() = delete;
		Chunk(const glm::ivec3& chunkOffset);
		~Chunk();

		Chunk(const Chunk&) = delete;
		Chunk& operator= (const Chunk&) = delete;

		MeshData& Reuse();
		void GenerateMesh();

		glm::ivec3& GetChunkOffset() { return m_ChunkOffset; }
	private:
		BlockType m_Blocks[CS_P * CS_P * CS_P];
		MeshData m_MeshData;

		glm::ivec3 m_ChunkOffset;
	};
}