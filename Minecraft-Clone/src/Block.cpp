#include "Block.h"

#include <ryml.hpp>
#include <ryml_std.hpp>

namespace RealEngine {
	std::vector<Block> BlockHelper::s_BlockMap;
	std::unordered_map<StringHash, BlockType> BlockHelper::s_BlockLookup;

	BlockType BlockHelper::GetBlockType(const std::string& blockName) {
		RE_PROFILE_FUNCTION();
		
		auto it = s_BlockLookup.find(StringHash(blockName));
		if (it != s_BlockLookup.end()) {
			return it->second;
		}

		RE_ASSERT(false, "Block type not found for name: {}", blockName);
		return 0; // Return an invalid block type/air if not found
	}

	void BlockHelper::ReadBlockDataYaml(const std::filesystem::path& yamlFile) {
		RE_PROFILE_FUNCTION();


		std::ifstream in(yamlFile, std::ifstream::in | std::ifstream::binary);
		if (!in.is_open()) {
			RE_ASSERT(false, "Failed to open block data YAML file");
			return;
		}
		auto size = std::filesystem::file_size(yamlFile);
		std::string content(size, '\0');
		in.read(&content[0], size);


		std::string fileName = yamlFile.filename().string();
		const ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(fileName), ryml::to_substr(content));


	}
}