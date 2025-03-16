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
        static thread_local std::vector<VertexData> vertices;
        static thread_local std::vector<uint32_t> indices;
        uint32_t index = 0;
        size_t vertexCount = 0;
        size_t indexCount = 0;

        auto isBlockVisible = [&](int x, int y, int z) {
            if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) {
                return true;
            }
            return m_Blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] == Block::Air;
        };

        auto addFace = [&](VertexData v1, VertexData v2, VertexData v3, VertexData v4) {
            if (vertexCount + 4 > vertices.size()) vertices.resize(vertexCount + 4);
            vertices[vertexCount++] = v1;
            vertices[vertexCount++] = v2;
            vertices[vertexCount++] = v3;
            vertices[vertexCount++] = v4;

            if (indexCount + 6 > indices.size()) indices.resize(indexCount + 6);
            indices[indexCount++] = index;
            indices[indexCount++] = index + 1;
            indices[indexCount++] = index + 2;
            indices[indexCount++] = index;
            indices[indexCount++] = index + 2;
            indices[indexCount++] = index + 3;
            index += 4;
        };

        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    if (m_Blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] == Block::Air) {
                        continue;
                    }

                    // Check each face of the block
                    if (isBlockVisible(x + 1, y, z)) {
						uint8_t uv = 0;

						// Right face
                        if (isBlockVisible(x + 1, y, z)) {
                            uint8_t uv = 0;
                            addFace(PackageData(x + 1, y, z + 1, uv++, 0, 0, 0), PackageData(x + 1, y, z, uv++, 0, 0, 0), PackageData(x + 1, y + 1, z, uv++, 0, 0, 0), PackageData(x + 1, y + 1, z + 1, uv++, 0, 0, 0));
                        }

						// Left face
                        if (isBlockVisible(x - 1, y, z)) {
                            uint8_t uv = 0;
                            addFace(PackageData(x, y, z + 1, uv++, 0, 0, 0), PackageData(x, y, z, uv++, 0, 0, 0), PackageData(x, y + 1, z, uv++, 0, 0, 0), PackageData(x, y + 1, z + 1, uv++, 0, 0, 0));
                        }
                        
						// Top face
                        if (isBlockVisible(x, y + 1, z)) {
                            uint8_t uv = 0;
                            addFace(PackageData(x, y + 1, z, uv++, 0, 0, 0), PackageData(x + 1, y + 1, z, uv++, 0, 0, 0), PackageData(x + 1, y + 1, z + 1, uv++, 0, 0, 0), PackageData(x, y + 1, z + 1, uv++, 0, 0, 0));
                        }

						// Bottom face
                        if (isBlockVisible(x, y - 1, z)) {
                            uint8_t uv = 0;
                            addFace(PackageData(x, y, z, uv++, 0, 0, 0), PackageData(x + 1, y, z, uv++, 0, 0, 0), PackageData(x + 1, y, z + 1, uv++, 0, 0, 0), PackageData(x, y, z + 1, uv++, 0, 0, 0));
                        }

						// Front face
                        if (isBlockVisible(x, y, z + 1)) {
                            uint8_t uv = 0;
                            addFace(PackageData(x, y, z + 1, uv++, 0, 0, 0), PackageData(x + 1, y, z + 1, uv++, 0, 0, 0), PackageData(x + 1, y + 1, z + 1, uv++, 0, 0, 0), PackageData(x, y + 1, z + 1, uv++, 0, 0, 0));
                        }

						// Back face
                        if (isBlockVisible(x, y, z - 1)) {
                            uint8_t uv = 0;
                            addFace(PackageData(x, y, z, uv++, 0, 0, 0), PackageData(x + 1, y, z, uv++, 0, 0, 0), PackageData(x + 1, y + 1, z, uv++, 0, 0, 0), PackageData(x, y + 1, z, uv++, 0, 0, 0));
                        }
                    }
                }
            }
        }

        m_IBO = IndexBuffer::Create(indices.data(), (uint32_t)indexCount);
        m_VBO = VertexBuffer::Create(vertices.data(), (uint32_t)vertexCount * (uint32_t)sizeof(VertexData));
        m_VBO->SetLayout(BufferAttributes{
            { DataType::Uint },
			{ DataType::Uint }
        });

        m_VAO = VertexArray::Create();
        m_VAO->SetVertexBuffer(m_VBO);
        m_VAO->SetIndexBuffer(m_IBO);
    }

	void Chunk::Render() {
		RenderCommands::DrawIndexed(m_VAO);
	}

    VertexData Chunk::PackageData(uint8_t x_pos, uint8_t y_pos, uint8_t z_pos, uint8_t uv, uint8_t side, uint8_t ao, uint32_t block_id) {
        uint32_t tempx = (x_pos & 0x7F)     << 0;
		uint32_t tempy = (y_pos & 0x7F)     << 7;
		uint32_t tempz = (z_pos & 0x7F)     << 14;
		uint32_t tempuv = (uv & 0x3)        << 21;
		uint32_t tempside = (side & 0x3)    << 23;
        uint32_t tempeao = (ao & 0x3)       << 25;

		VertexData data;
		data.package = tempx | tempy | tempz | tempuv | tempside | tempeao;
		data.block_id = block_id;

		return data;
    }
}
