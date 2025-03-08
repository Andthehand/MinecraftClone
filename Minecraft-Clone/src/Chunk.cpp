#include "Chunk.h"

namespace RealEngine {
	Chunk::Chunk() {
		for (int x = 0; x < CHUNK_SIZE; x++) {
			for (int y = 0; y < CHUNK_SIZE; y++) {
				for (int z = 0; z < CHUNK_SIZE; z++) {
					m_Blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] = Block::Grass;
				}
			}
		}

		// Create a very stupid mesh with the m_Blocks data
		std::vector<float> vertices;
		std::vector<uint32_t> indices;
		uint32_t index = 0;

		for (float x = 0; x < CHUNK_SIZE; x++) {
			for (float y = 0; y < CHUNK_SIZE; y++) {
				for (float z = 0; z < CHUNK_SIZE; z++) {
					if (m_Blocks[(int)x + (int)y * CHUNK_SIZE + (int)z * CHUNK_SIZE * CHUNK_SIZE] != Block::Air) {
						// Add vertices for a cube
						vertices.insert(vertices.end(), {
							x, y, z,
							x + 1, y, z,
							x + 1, y + 1, z,
							x, y + 1, z,
							x, y, z + 1,
							x + 1, y, z + 1,
							x + 1, y + 1, z + 1,
							x, y + 1, z + 1
						});

						// Add indices for the cube
						indices.insert(indices.end(), {
							index, index + 1, index + 2, index, index + 2, index + 3,
							index + 4, index + 5, index + 6, index + 4, index + 6, index + 7,
							index, index + 1, index + 5, index, index + 5, index + 4,
							index + 2, index + 3, index + 7, index + 2, index + 7, index + 6,
							index + 1, index + 2, index + 6, index + 1, index + 6, index + 5,
							index, index + 3, index + 7, index, index + 7, index + 4
						});

						index += 8;
					}
				}
			}
		}

		m_IBO = IndexBuffer::Create(indices.data(), (uint32_t)indices.size());
		m_VBO = VertexBuffer::Create(vertices.data(), (uint32_t)vertices.size() * (uint32_t)sizeof(float));
		m_VBO->SetLayout(BufferAttributes{
			{ DataType::Float3 }
		});

		m_VAO = VertexArray::Create();
		m_VAO->SetVertexBuffer(m_VBO);
		m_VAO->SetIndexBuffer(m_IBO);
	}
	void Chunk::Render() {
		RenderCommands::DrawIndexed(m_VAO);
	}
}