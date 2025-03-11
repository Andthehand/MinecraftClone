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
		Block m_Blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];

		//Render Data
		Ref<VertexArray> m_VAO;
		Ref<VertexBuffer> m_VBO;
		Ref<IndexBuffer> m_IBO;
	};
}