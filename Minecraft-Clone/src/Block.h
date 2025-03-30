#pragma once
#include <cstdint>

#include <RealEngine.h>

namespace RealEngine {
	typedef uint32_t BlockType;

	enum BasicBlockTypes : BlockType {
		Air = 0,
		Bedrock = 1,
		Grass = 2,
		Dirt = 3,
	};

	struct BlockTexture {
		uint32_t BottomFaceID = 0;
		uint32_t SideFaceID = 0;
		uint32_t TopFaceID = 0;
	};

	struct Block {
		std::string InternalName;
		BlockType Id;

		std::string DisplayName;
		BlockTexture Texture;
	};

	class BlockHelper {
	public:
		static BlockType GetBlockType(const std::string& blockName);
		
		static inline uint32_t GetBlockTextureSideID(BlockType blockId)   { return s_BlockMap[blockId].Texture.SideFaceID;   }
		static inline uint32_t GetBlockTextureTopID(BlockType blockId)    { return s_BlockMap[blockId].Texture.TopFaceID;    }
		static inline uint32_t GetBlockTextureBottomID(BlockType blockId) { return s_BlockMap[blockId].Texture.BottomFaceID; }

		static void ReadBlockDataYaml(const std::filesystem::path& yamlFile);

		//Bind texture
		static inline void BindBlockTextureArray() {
			RE_ASSERT(s_BlockTextureArray, "Block texture array is not initialized");

			s_BlockTextureArray->Bind();
		}
	private:
		static std::vector<Block> s_BlockMap;
		static std::unordered_map<StringHash, BlockType> s_BlockLookup;
		static Ref<Texture2DArray> s_BlockTextureArray;
		static Ref<ShaderStorageBuffer> s_SideIDArray;
	};
}
