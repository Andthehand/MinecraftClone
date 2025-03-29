#pragma once 
#include <RealEngine.h>

#include <cstdint>

#include "Block.h"

#define CHUNK_SIZE 1

namespace RealEngine {
	class Chunk {
	public:
		Chunk(const glm::vec3& chunkOffset);
		~Chunk() = default;

		void GenerateMesh();

		void Render(Shader* chunkShader);
	private:
		BlockType m_Blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];

		glm::vec3 m_ChunkOffset;

		//Render Data
		Ref<VertexArray> m_VAO;
		Ref<ShaderStorageBuffer> m_SSBO;

		// Used for sending IDs to the GPU for rendering
		Ref<Texture2DArray> m_BlockArray;
	};
}