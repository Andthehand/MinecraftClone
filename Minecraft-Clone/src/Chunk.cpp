#include "Chunk.h"

namespace RealEngine {
    namespace Utils {
        inline std::vector<uint32_t> PopulateIndices(uint32_t quadCount) {
			std::vector<uint32_t> indices;
			indices.reserve(quadCount * 6); // 6 indices per quad (2 triangles)
			for (uint32_t i = 0; i < quadCount; ++i) {
				uint32_t baseIndex = i * 4;
				indices.push_back(baseIndex);
				indices.push_back(baseIndex + 1);
				indices.push_back(baseIndex + 2);
                indices.push_back(baseIndex);
                indices.push_back(baseIndex + 2);
				indices.push_back(baseIndex + 3);
			}

			return indices;
        }
    }

	Chunk::Chunk() {
		RE_PROFILE_FUNCTION();

		BlockType airBlock = Grass;
		for (int x = 0; x < CHUNK_SIZE; x++) {
			for (int y = 0; y < CHUNK_SIZE; y++) {
				for (int z = 0; z < CHUNK_SIZE; z++) {
					m_Blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] = airBlock;
				}
			}
		}

		GenerateMesh();
	}
					
    void Chunk::GenerateMesh() {
        RE_PROFILE_FUNCTION();
		constexpr uint32_t worstCaseQuadCount = (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE * 6); // 6 quads per block in worst case (all sides visible)
        static thread_local std::vector<uint32_t> indices = Utils::PopulateIndices(worstCaseQuadCount);
        static thread_local std::vector<VertexData> vertices;
        static Ref<IndexBuffer> IBO = IndexBuffer::Create(indices.data(), (uint32_t)indices.size());;
        size_t vertexCount = 0;
        size_t indexCount = 0;

        auto isBlockVisible = [&](int x, int y, int z) {
            if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) {
                return true;
            }
            return m_Blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] == BasicBlockTypes::Air;
        };

        auto addFace = [&](VertexData v1, VertexData v2, VertexData v3, VertexData v4) {
            if (vertexCount + 4 > vertices.size()) vertices.resize(vertexCount + 4);
            vertices[vertexCount++] = v1;
            vertices[vertexCount++] = v2;
            vertices[vertexCount++] = v3;
            vertices[vertexCount++] = v4;

			indexCount += 6; // 2 triangles per face, 3 indices per triangle
        };

        for (uint8_t x = 0; x < CHUNK_SIZE; x++) {
            for (uint8_t y = 0; y < CHUNK_SIZE; y++) {
                for (uint8_t z = 0; z < CHUNK_SIZE; z++) {
                    if (m_Blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] == BasicBlockTypes::Air) {
                        continue;
                    }


					BlockType block = m_Blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE];
                    // Right face
                    if (isBlockVisible(x + 1, y, z)) {
                        uint8_t uv = 0;
                        uint32_t sideTextureID = BlockHelper::GetBlockTextureSideID(block);
                        addFace(PackageData(x + 1, y, z + 1, uv++, 0, sideTextureID), PackageData(x + 1, y, z, uv++, 0, sideTextureID), PackageData(x + 1, y + 1, z, uv++, 0, sideTextureID), PackageData(x + 1, y + 1, z + 1, uv++, 0, sideTextureID));
                    }

                    // Left face
                    if (isBlockVisible(x - 1, y, z)) {
                        uint8_t uv = 0;
                        uint32_t sideTextureID = BlockHelper::GetBlockTextureSideID(block);
                        addFace(PackageData(x, y, z + 1, uv++, 0, sideTextureID), PackageData(x, y, z, uv++, 0, sideTextureID), PackageData(x, y + 1, z, uv++, 0, sideTextureID), PackageData(x, y + 1, z + 1, uv++, 0, sideTextureID));
                    }
                        
                    // Top face
                    if (isBlockVisible(x, y + 1, z)) {
                        uint8_t uv = 0;
                        uint32_t sideTextureID = BlockHelper::GetBlockTextureTopID(block);
                        addFace(PackageData(x, y + 1, z, uv++, 0, sideTextureID), PackageData(x + 1, y + 1, z, uv++, 0, sideTextureID), PackageData(x + 1, y + 1, z + 1, uv++, 0, sideTextureID), PackageData(x, y + 1, z + 1, uv++, 0, sideTextureID));
                    }

                    // Bottom face
                    if (isBlockVisible(x, y - 1, z)) {
                        uint8_t uv = 0;
                        uint32_t sideTextureID = BlockHelper::GetBlockTextureBottomID(block);
                        addFace(PackageData(x, y, z, uv++, 0, sideTextureID), PackageData(x + 1, y, z, uv++, 0, sideTextureID), PackageData(x + 1, y, z + 1, uv++, 0, sideTextureID), PackageData(x, y, z + 1, uv++, 0, sideTextureID));
                    }

                    // Front face
                    if (isBlockVisible(x, y, z + 1)) {
                        uint8_t uv = 0;
                        uint32_t sideTextureID = BlockHelper::GetBlockTextureSideID(block);
                        addFace(PackageData(x, y, z + 1, uv++, 0, sideTextureID), PackageData(x + 1, y, z + 1, uv++, 0, sideTextureID), PackageData(x + 1, y + 1, z + 1, uv++, 0, sideTextureID), PackageData(x, y + 1, z + 1, uv++, 0, sideTextureID));
                    }

                    // Back face
                    if (isBlockVisible(x, y, z - 1)) {
                        uint8_t uv = 0;
                        uint32_t sideTextureID = BlockHelper::GetBlockTextureSideID(block);
                        addFace(PackageData(x, y, z, uv++, 0, sideTextureID), PackageData(x + 1, y, z, uv++, 0, sideTextureID), PackageData(x + 1, y + 1, z, uv++, 0, sideTextureID), PackageData(x, y + 1, z, uv++, 0, sideTextureID));
                    }
                }
            }
        }

		RE_ASSERT(indexCount <= indices.size(), "Index count exceeds pre-allocated IBO size");
        m_VBO = VertexBuffer::Create(vertices.data(), (uint32_t)vertexCount * (uint32_t)sizeof(VertexData));
        m_VBO->SetLayout(BufferAttributes{
            { DataType::Uint },
			{ DataType::Uint }
        });

        m_VAO = VertexArray::Create();
        m_VAO->SetVertexBuffer(m_VBO);
        m_VAO->SetIndexBuffer(IBO);
    }

	void Chunk::Render() {
		RenderCommands::DrawIndexed(m_VAO);
	}

    VertexData Chunk::PackageData(uint8_t x_pos, uint8_t y_pos, uint8_t z_pos, uint8_t uv, uint8_t ao, uint32_t block_id) {
        uint32_t tempx = (x_pos & 0x7F)     << 0;
		uint32_t tempy = (y_pos & 0x7F)     << 7;
		uint32_t tempz = (z_pos & 0x7F)     << 14;
		uint32_t tempuv = (uv & 0x3)        << 21;
        uint32_t tempeao = (ao & 0x3)       << 25;

		VertexData data;
		data.package = tempx | tempy | tempz | tempuv | tempeao;
		data.block_id = block_id;

		return data;
    }
}
