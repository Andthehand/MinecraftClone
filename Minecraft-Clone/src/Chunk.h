#pragma once 
#include <RealEngine.h>

#include <cstdint>

#include "Block.h"

#define CHUNK_SIZE_WIDTH 32
#define CHUNK_SIZE_HEIGHT 256

namespace RealEngine {
	class Chunk {
	public:
		Chunk(const glm::vec3& chunkOffset);
		~Chunk() = default;

		void GenerateMesh();

		void Render(Shader* chunkShader);
	private:
		BlockType m_Blocks[CHUNK_SIZE_WIDTH * CHUNK_SIZE_WIDTH * CHUNK_SIZE_HEIGHT];

		glm::vec3 m_ChunkOffset;

		//Render Data
		Ref<VertexArray> m_VAO;
		Ref<ShaderStorageBuffer> m_SSBO;

		// Used for sending IDs to the GPU for rendering
		Ref<Texture2DArray> m_BlockArray;
	};
}