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

		GenerateMesh();
	}
	
    void Chunk::GenerateMesh() {
        RE_PROFILE_FUNCTION();
        static thread_local std::vector<uint32_t> vertices;
        static thread_local std::vector<uint32_t> indices;
        uint32_t index = 0;
        size_t vertexCount = 0;
        size_t indexCount = 0;

        auto isBlockVisible = [&](int x, int y, int z) {
            if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) {
                return false;
            }
            return m_Blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] != Block::Air;
        };

        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    if (m_Blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] == Block::Air) {
                        continue;
                    }

                    // Check each face of the block
                    if (!isBlockVisible(x + 1, y, z)) {
                        // Add vertices and indices for the positive X face
                        if (vertexCount + 4 > vertices.size()) vertices.resize(vertexCount + 4);
                        vertices[vertexCount++] = packageData(x + 1, y, z);
                        vertices[vertexCount++] = packageData(x + 1, y + 1, z);
                        vertices[vertexCount++] = packageData(x + 1, y + 1, z + 1);
                        vertices[vertexCount++] = packageData(x + 1, y, z + 1);

                        if (indexCount + 6 > indices.size()) {
                            indices.resize(indexCount + 6);
                            indices[indexCount++] = index;
                            indices[indexCount++] = index + 1;
                            indices[indexCount++] = index + 2;
                            indices[indexCount++] = index;
                            indices[indexCount++] = index + 2;
                            indices[indexCount++] = index + 3;
                        }
                        index += 4;
                    }
                    if (!isBlockVisible(x - 1, y, z)) {
                        // Add vertices and indices for the negative X face
                        if (vertexCount + 4 > vertices.size()) vertices.resize(vertexCount + 4);
                        vertices[vertexCount++] = packageData(x, y, z);
                        vertices[vertexCount++] = packageData(x, y + 1, z);
                        vertices[vertexCount++] = packageData(x, y + 1, z + 1);
                        vertices[vertexCount++] = packageData(x, y, z + 1);

                        if (indexCount + 6 > indices.size()) {
                            indices.resize(indexCount + 6);
                            indices[indexCount++] = index;
                            indices[indexCount++] = index + 1;
                            indices[indexCount++] = index + 2;
                            indices[indexCount++] = index;
                            indices[indexCount++] = index + 2;
                            indices[indexCount++] = index + 3;
                        }
                        index += 4;
                    }
                    if (!isBlockVisible(x, y + 1, z)) {
                        // Add vertices and indices for the positive Y face
                        if (vertexCount + 4 > vertices.size()) vertices.resize(vertexCount + 4);
                        vertices[vertexCount++] = packageData(x, y + 1, z);
                        vertices[vertexCount++] = packageData(x + 1, y + 1, z);
                        vertices[vertexCount++] = packageData(x + 1, y + 1, z + 1);
                        vertices[vertexCount++] = packageData(x, y + 1, z + 1);

                        if (indexCount + 6 > indices.size()) {
                            indices.resize(indexCount + 6);
                            indices[indexCount++] = index;
                            indices[indexCount++] = index + 1;
                            indices[indexCount++] = index + 2;
                            indices[indexCount++] = index;
                            indices[indexCount++] = index + 2;
                            indices[indexCount++] = index + 3;
                        }
                        index += 4;
                    }
                    if (!isBlockVisible(x, y - 1, z)) {
                        // Add vertices and indices for the negative Y face
                        if (vertexCount + 4 > vertices.size()) vertices.resize(vertexCount + 4);
                        vertices[vertexCount++] = packageData(x, y, z);
                        vertices[vertexCount++] = packageData(x + 1, y, z);
                        vertices[vertexCount++] = packageData(x + 1, y, z + 1);
                        vertices[vertexCount++] = packageData(x, y, z + 1);

                        if (indexCount + 6 > indices.size()) {
                            indices.resize(indexCount + 6);
                            indices[indexCount++] = index;
                            indices[indexCount++] = index + 1;
                            indices[indexCount++] = index + 2;
                            indices[indexCount++] = index;
                            indices[indexCount++] = index + 2;
                            indices[indexCount++] = index + 3;
                        }
                        index += 4;
                    }
                    if (!isBlockVisible(x, y, z + 1)) {
                        // Add vertices and indices for the positive Z face
                        if (vertexCount + 4 > vertices.size()) vertices.resize(vertexCount + 4);
                        vertices[vertexCount++] = packageData(x, y, z + 1);
                        vertices[vertexCount++] = packageData(x + 1, y, z + 1);
                        vertices[vertexCount++] = packageData(x + 1, y + 1, z + 1);
                        vertices[vertexCount++] = packageData(x, y + 1, z + 1);

                        if (indexCount + 6 > indices.size()) {
                            indices.resize(indexCount + 6);
                            indices[indexCount++] = index;
                            indices[indexCount++] = index + 1;
                            indices[indexCount++] = index + 2;
                            indices[indexCount++] = index;
                            indices[indexCount++] = index + 2;
                            indices[indexCount++] = index + 3;
                        }
                        index += 4;
                    }
                    if (!isBlockVisible(x, y, z - 1)) {
                        // Add vertices and indices for the negative Z face
                        if (vertexCount + 4 > vertices.size()) vertices.resize(vertexCount + 4);
                        vertices[vertexCount++] = packageData(x, y, z);
                        vertices[vertexCount++] = packageData(x + 1, y, z);
                        vertices[vertexCount++] = packageData(x + 1, y + 1, z);
                        vertices[vertexCount++] = packageData(x, y + 1, z);

                        if (indexCount + 6 > indices.size()) {
                            indices.resize(indexCount + 6);
                            indices[indexCount++] = index;
                            indices[indexCount++] = index + 1;
                            indices[indexCount++] = index + 2;
                            indices[indexCount++] = index;
                            indices[indexCount++] = index + 2;
                            indices[indexCount++] = index + 3;
                        }
                        index += 4;
                    }
                }
            }
        }

        m_IBO = IndexBuffer::Create(indices.data(), (uint32_t)indexCount);
        m_VBO = VertexBuffer::Create(vertices.data(), (uint32_t)vertexCount * (uint32_t)sizeof(float));
        m_VBO->SetLayout(BufferAttributes{
            { DataType::Uint }
        });

        m_VAO = VertexArray::Create();
        m_VAO->SetVertexBuffer(m_VBO);
        m_VAO->SetIndexBuffer(m_IBO);
    }

	void Chunk::Render() {
		RenderCommands::DrawIndexed(m_VAO);
	}

    uint32_t Chunk::packageData(uint8_t x_pos, uint8_t y_pos, uint8_t z_pos, uint8_t block_id, uint8_t ao) {
        uint32_t tempx = (x_pos & 0x3F);
		uint32_t tempy = (y_pos & 0x3F) << 6;
		uint32_t tempz = (z_pos & 0x3F) << 12;
		uint32_t tempid = (block_id & 0xFF) << 18;
		uint32_t tempeo = (ao & 0x3) << 26;
		uint32_t ret = tempx | tempy | tempz | tempid | tempeo;
		return ret;
    }
}