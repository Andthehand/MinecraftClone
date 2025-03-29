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

		BlockType defaultBlock = Grass;
		BlockType testBlock = Dirt;
		for (int x = 0; x < CHUNK_SIZE; x++) {
			for (int y = 0; y < CHUNK_SIZE; y++) {
				for (int z = 0; z < CHUNK_SIZE; z++) {
					if (x % 2 == 0) {
						m_Blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] = testBlock;
					}
                    else {
                        m_Blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] = defaultBlock;
                    }
				}
			}
		}

		m_BlockArray = Texture2DArray::Create(Texture2DArrayCreateInfo{
			CHUNK_SIZE, 
			CHUNK_SIZE, 
            CHUNK_SIZE,     
			TextureDataType::RED32, 
			TextureFormat::RED                  
		});

        for (uint16_t i = 0; i < CHUNK_SIZE; i++) {
            m_BlockArray->SetSubTextureData(&m_Blocks[i], i);
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
            if (x < 0 || x >= CHUNK_SIZE || y < 0 || y >= CHUNK_SIZE || z < 0 || z >= CHUNK_SIZE) {
                return true;
            }
            return m_Blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] == BasicBlockTypes::Air;
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

        for (uint8_t x = 0; x < CHUNK_SIZE; x++) {
            for (uint8_t y = 0; y < CHUNK_SIZE; y++) {
                for (uint8_t z = 0; z < CHUNK_SIZE; z++) {
                    if (m_Blocks[x + y * CHUNK_SIZE + z * CHUNK_SIZE * CHUNK_SIZE] == BasicBlockTypes::Air) {
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
