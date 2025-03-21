#include "Block.h"

#include <ryml.hpp>
#include <ryml_std.hpp>

namespace RealEngine {
	std::vector<Block> BlockHelper::s_BlockMap;
	std::unordered_map<StringHash, BlockType> BlockHelper::s_BlockLookup;


	void write(ryml::NodeRef* n, std::vector<Block>& seq) {
		*n |= ryml::SEQ;
		for (auto const& v : seq)
			n->append_child() << v;
	}

	void write(ryml::NodeRef* n, Block const& val) {
		*n |= ryml::MAP;

        ryml::NodeRef block_tree = n->append_child() << ryml::key(val.InternalName);
        block_tree |= ryml::MAP;
        block_tree.append_child() << ryml::key("DisplayName") << val.DisplayName;
        block_tree.append_child() << ryml::key("Id") << val.Id;
	}

	bool read(ryml::ConstNodeRef const& n, Block* val) {
		ryml::ConstNodeRef node = n.first_child(); // Get the first child node which contains the block data

		ryml::csubstr key = node.key();
		val->InternalName = std::string(key.begin(), key.end());
		node["DisplayName"] >> val->DisplayName;
		node["Id"] >> val->Id;

		return true;
	}

    bool read(ryml::ConstNodeRef const& n, std::vector<Block>* val) {
        val->resize(static_cast<size_t>(n.num_children())); // Taken from https://github.com/biojppm/rapidyaml/blob/master/samples/quickstart.cpp#L3664

        size_t pos = 0;
        for (auto const ch : n.children()) {
            ch >> (*val)[pos++];
        }

        return true;
    }

	BlockType BlockHelper::GetBlockType(const std::string& blockName) {
		RE_PROFILE_FUNCTION();
		
		auto it = s_BlockLookup.find(StringHash(blockName));
		if (it != s_BlockLookup.end()) {
			return it->second;
		}

		RE_WARN("Block type not found for name: {}", blockName);
		return 0; // Return an invalid block type/air if not found
	}

	void BlockHelper::ReadBlockDataYaml(const std::filesystem::path& yamlFile) {
		RE_PROFILE_FUNCTION();

		std::vector<Block> test;
		test.push_back({ "Grass", BasicBlockTypes::Grass, "Grass Block", { 1, 2, 3 } });
		test.push_back({ "Air", BasicBlockTypes::Air, "Air Block", { 0, 0, 0 } });

		ryml::Tree tree;
		ryml::NodeRef root = tree.rootref();
		
		root << test;

		std::string yamlFileStr = ryml::emitrs_yaml<std::string>(tree);

		std::vector<Block> blocks;
		root >> blocks; // Create a new Block object


		std::ifstream in(yamlFile, std::ifstream::in | std::ifstream::binary);
		if (!in.is_open()) {
			RE_ASSERT(false, "Failed to open block data YAML file");
			return;
		}
		auto size = std::filesystem::file_size(yamlFile);
		std::string content(size, '\0');
		in.read(&content[0], size);


		std::string fileName = yamlFile.filename().string();
		//const ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(fileName), ryml::to_substr(content));


	}
}