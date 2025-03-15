#pragma once 
#include <RealEngine.h>

#include <cstdint>

#define CHUNK_SIZE 64

namespace RealEngine {
	enum Block : uint8_t {
		Air = 0,
		Grass = 1,
		Dirt = 2,
		Stone = 3,
		Sand = 4,
		Water = 5,
		Leaves = 6,
		Wood = 7
	};

	class Chunk {
	public:
		Chunk();
		~Chunk() = default;

		void GenerateMesh();

		void Render();
	private:
		/* 
			6 bits for x
			6 bits for y
			6 bits for y
			8 bits for id (unused)
			2 bits for ambient occlusion (unused)
		*/
		uint32_t packageData(uint8_t x_pos, uint8_t y_pos, uint8_t z_pos, uint8_t block_id = 0, uint8_t ao = 0);
	private:
		Block m_Blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];

		//Render Data
		Ref<VertexArray> m_VAO;
		Ref<VertexBuffer> m_VBO;
		Ref<IndexBuffer> m_IBO;
	};
}