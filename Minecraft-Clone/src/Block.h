#pragma once
#include <cstdint>

#include <RealEngine.h>

namespace RealEngine {
	typedef uint32_t BlockType;

	enum BasicBlockTypes : BlockType {
		Air = 0,
		Grass = 1
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
		BlockHelper() = delete;
		~BlockHelper() = delete;

		static BlockType GetBlockType(const std::string& blockName);

		static void ReadBlockDataYaml(const std::filesystem::path& yamlFile);
	private:
		static std::vector<Block> s_BlockMap;
		static std::unordered_map<StringHash, BlockType> s_BlockLookup;
		static Ref<Texture2DArray> s_BlockTextureArray;
	};
}
