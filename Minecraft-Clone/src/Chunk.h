#pragma once 
#include <RealEngine.h>

#include <cstdint>

#include "Block.h"

#define CHUNK_SIZE 64

namespace RealEngine {
	PACKED_STRUCT(VertexData) {
		/*
			7 bits for x						(0)
			7 bits for y						(7)
			7 bits for y						(14)
			2 bits for UV						(21)
			2 bits for side						(23)
			2 bits for ambient occlusion (unused)(25)
		*/
		uint32_t package;
		uint32_t block_id;
	};


	class Chunk {
	public:
		Chunk(const glm::vec3& chunkOffset);
		~Chunk() = default;

		void GenerateMesh();

		void Render(Shader* chunkShader);
	private:
		VertexData PackageData(uint8_t x_pos, uint8_t y_pos, uint8_t z_pos, uint8_t uv, uint8_t ao, uint32_t block_id);
	private:
		BlockType m_Blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];

		glm::vec3 m_ChunkOffset;

		//Render Data
		Ref<VertexArray> m_VAO;
		Ref<VertexBuffer> m_VBO;
	};
}