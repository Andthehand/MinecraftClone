#include "Chunk.h"

namespace RealEngine {
    namespace Utils {
        enum FacingDirection : uint32_t {
            FRONT   = 0,
            BACK    = 1,
            LEFT    = 2,
            RIGHT   = 3,
            TOP     = 4,
            BOTTOM  = 5
        };

        PACKED_STRUCT(VertexData) {
            glm::vec3 Position;
			FacingDirection Direction;
        };
    }

	Chunk::Chunk(const glm::vec3& chunkOffset) 
        : m_ChunkOffset(chunkOffset) {
		RE_PROFILE_FUNCTION();

        for (uint16_t y = 0; y < CHUNK_SIZE_HEIGHT; y++) {
            for (uint8_t x = 0; x < CHUNK_SIZE_WIDTH; x++) {
				for (uint8_t z = 0; z < CHUNK_SIZE_WIDTH; z++) {
					if (y == 0) {
						m_Blocks[z + x * CHUNK_SIZE_WIDTH + y * CHUNK_SIZE_WIDTH * CHUNK_SIZE_WIDTH] = Bedrock;
					}
					else if (y % 2 == 0) {
                        m_Blocks[z + x * CHUNK_SIZE_WIDTH + y * CHUNK_SIZE_WIDTH * CHUNK_SIZE_WIDTH] = Dirt;
					}
                    else {
                        m_Blocks[z + x * CHUNK_SIZE_WIDTH + y * CHUNK_SIZE_WIDTH * CHUNK_SIZE_WIDTH] = Grass;
                    }
                }
			}
		}

		m_BlockArray = Texture2DArray::Create(Texture2DArrayCreateInfo(
            CHUNK_SIZE_WIDTH,
            CHUNK_SIZE_WIDTH,
            CHUNK_SIZE_HEIGHT,
			TextureDataType::RED32, 
			TextureFormat::RED                  
		));

        for (uint16_t y = 0; y < CHUNK_SIZE_HEIGHT; y++) {
            m_BlockArray->SetSubTextureData(&m_Blocks[y * CHUNK_SIZE_WIDTH * CHUNK_SIZE_WIDTH], y);
        }

		GenerateMesh();
	}
					
    void Chunk::GenerateMesh() {
        RE_PROFILE_FUNCTION();
        static thread_local std::vector<Utils::VertexData> vertices;
        size_t vertexCount = 0;

		constexpr glm::vec2 uvTextures[4] = {
            {1.0f, 1.0f}, 
            {0.0f, 1.0f}, 
            {0.0f, 0.0f}, 
            {1.0f, 0.0f}
		};

        auto isBlockVisible = [&](int x, int y, int z) {
            if (x < 0 || x >= CHUNK_SIZE_WIDTH || y < 0 || y >= CHUNK_SIZE_HEIGHT || z < 0 || z >= CHUNK_SIZE_WIDTH) {
                return true;
            }
            return m_Blocks[z + x * CHUNK_SIZE_WIDTH + y * CHUNK_SIZE_WIDTH * CHUNK_SIZE_WIDTH] == BasicBlockTypes::Air;
        };

        auto addFace = [&](Utils::VertexData v) {
            if (vertexCount + 1 > vertices.size()) {
                vertices.push_back(v);
                vertexCount++;
            }
            else {
                vertices[vertexCount] = v;
                vertexCount++;
            }
        };

        for (uint16_t y = 0; y < CHUNK_SIZE_HEIGHT; y++) {
            for (uint8_t x = 0; x < CHUNK_SIZE_WIDTH; x++) {
                for (uint8_t z = 0; z < CHUNK_SIZE_WIDTH; z++) {
					uint32_t index = z + x * CHUNK_SIZE_WIDTH + y * CHUNK_SIZE_WIDTH * CHUNK_SIZE_WIDTH;
                    if (m_Blocks[index] == BasicBlockTypes::Air) {
                        continue;
                    }


					//BlockType block = m_Blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE];
                    // Right face
                    if (isBlockVisible(x + 1, y, z)) {
                        // uint32_t sideTextureID = BlockHelper::GetBlockTextureSideID(block);
						addFace({ glm::vec3(x, y, z), Utils::FacingDirection::RIGHT });
                    }

                    // Left face
                    if (isBlockVisible(x - 1, y, z)) {
                        // uint32_t sideTextureID = BlockHelper::GetBlockTextureSideID(block);
                        addFace({ glm::vec3(x, y, z), Utils::FacingDirection::LEFT });
                    }
                        
                    // Top face
                    if (isBlockVisible(x, y + 1, z)) {
                        //uint32_t sideTextureID = BlockHelper::GetBlockTextureTopID(block);
                        addFace({ glm::vec3(x, y, z), Utils::FacingDirection::TOP });
                    }

                    // Bottom face
                    if (isBlockVisible(x, y - 1, z)) {
                        //uint32_t sideTextureID = BlockHelper::GetBlockTextureBottomID(block);
                        addFace({ glm::vec3(x, y, z), Utils::FacingDirection::BOTTOM });
                    }

                    // Front face
                    if (isBlockVisible(x, y, z + 1)) {
                        // uint32_t sideTextureID = BlockHelper::GetBlockTextureSideID(block);
                        addFace({ glm::vec3(x, y, z), Utils::FacingDirection::FRONT });
                    }

                    // Back face
                    if (isBlockVisible(x, y, z - 1)) {
                        // uint32_t sideTextureID = BlockHelper::GetBlockTextureSideID(block);
                        addFace({ glm::vec3(x, y, z), Utils::FacingDirection::BACK });
                    }
                }
            }
        }

        m_VAO = VertexArray::Create();
		m_SSBO = ShaderStorageBuffer::Create(vertices.data(), (uint32_t)(vertices.size() * sizeof(Utils::VertexData)), 2);
    }

	void Chunk::Render(Shader* chunkShader) {
		m_BlockArray->Bind(1);
		RenderCommands::DrawArrays(m_VAO, (m_SSBO->GetSize() / sizeof(Utils::VertexData)) * 6);
	}
}
