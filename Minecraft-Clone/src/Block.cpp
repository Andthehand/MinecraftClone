#include "Block.h"

#include <fstream>

#include <ryml.hpp>
#include <ryml_std.hpp>

namespace RealEngine {
	// Static member initialization
	std::vector<Block> BlockHelper::s_BlockMap;
	std::unordered_map<StringHash, BlockType> BlockHelper::s_BlockLookup;
	Ref<Texture2DArray> BlockHelper::s_BlockTextureArray;

	struct YAMLBlockTexture {
		// These could be paths but it's easier to use strings
		std::string SideFace;
		std::string BottomFace;
		std::string TopFace;

		uint32_t NumTextures;
	};

	struct YAMLBlockData {
		std::string InternalName;
		BlockType Id;
		std::string DisplayName;
		YAMLBlockTexture Texture;
	};

	// This function reads a single YAMLBlockData from a YAML node
	bool read(ryml::ConstNodeRef const& n, YAMLBlockData* val) {
		ryml::ConstNodeRef node = n.first_child(); // Get the first child node which contains the block data

		// Read the block data
		ryml::csubstr key = node.key();
		val->InternalName = std::string(key.begin(), key.end());
		node["DisplayName"] >> val->DisplayName;
		node["Id"] >> val->Id;
		
		// Read the texture data
		ryml::ConstNodeRef texture_node = node["Textures"];
		val->Texture.NumTextures = (uint32_t)texture_node.num_children();
		switch (val->Texture.NumTextures) {
			case 1:
				texture_node["SideFace"] >> val->Texture.SideFace;
				break;
			case 2:
				texture_node["SideFace"] >> val->Texture.SideFace;
				texture_node["BottomFace"] >> val->Texture.BottomFace;
				break;
			case 3:
				texture_node["SideFace"] >> val->Texture.SideFace;
				texture_node["BottomFace"] >> val->Texture.BottomFace;
				texture_node["TopFace"] >> val->Texture.TopFace;
				break;
		}

		return true;
	}

	// This function reads a vector of YAMLBlockData from a YAML node
    bool read(ryml::ConstNodeRef const& n, std::vector<YAMLBlockData>* val) {
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

		RE_ASSERT(it != s_BlockLookup.end(), "Block name {} not found in BlockLookup", blockName);
		return Air; // Return an invalid block type/air if not found
	}

	void BlockHelper::ReadBlockDataYaml(const std::filesystem::path& yamlFile) {
		RE_PROFILE_FUNCTION();

		RE_INFO("Loading block data from YAML file: {}", yamlFile);

		std::string content;
		{
			RE_PROFILE_SCOPE("Read YAML File");

			// Read in file
			std::ifstream in(yamlFile, std::ifstream::in | std::ifstream::binary);
			if (!in.is_open()) {
				RE_ASSERT(false, "Failed to open block data YAML file");
				return;
			}
			auto size = std::filesystem::file_size(yamlFile);
			content = std::string(size, '\0');
			in.read(&content[0], size);
		}

		std::vector<YAMLBlockData> blocks;
		{
			RE_PROFILE_SCOPE("Parse YAML File");

			// Parse File
			std::string fileName = yamlFile.filename().string();
			const ryml::Tree tree = ryml::parse_in_place(ryml::to_substr(fileName), ryml::to_substr(content));
			ryml::ConstNodeRef root = tree.rootref();

			// Read in the blocks
			root >> blocks; // Create a new Block object
		}

		std::vector<std::filesystem::path> texturePaths;
		{
			RE_PROFILE_SCOPE("Process Block Data");

			// Populate the BlockMap and BlockLookup
			s_BlockMap.reserve(blocks.size());
			s_BlockLookup.reserve(blocks.size());

			uint32_t currrentTextureID = 0;
			std::string assetPath = "assets/blocks/";
			for (const auto& yamlBlock : blocks) {
				Block block;
				block.InternalName = yamlBlock.InternalName;
				block.Id = yamlBlock.Id;
				block.DisplayName = yamlBlock.DisplayName;

				// Assign the texture IDs
				switch (yamlBlock.Texture.NumTextures) {
				case 1:
					block.Texture.TopFaceID = currrentTextureID;
					block.Texture.BottomFaceID = currrentTextureID;
					block.Texture.SideFaceID = currrentTextureID++;

					texturePaths.push_back(assetPath + yamlBlock.Texture.SideFace);
					break;
				case 2:
					block.Texture.TopFaceID = currrentTextureID;
					block.Texture.BottomFaceID = currrentTextureID++;
					block.Texture.SideFaceID = currrentTextureID++;

					texturePaths.push_back(assetPath + yamlBlock.Texture.BottomFace);
					texturePaths.push_back(assetPath + yamlBlock.Texture.SideFace);
					break;
				case 3:
					block.Texture.TopFaceID = currrentTextureID++;
					block.Texture.BottomFaceID = currrentTextureID++;
					block.Texture.SideFaceID = currrentTextureID++;

					texturePaths.push_back(assetPath + yamlBlock.Texture.TopFace);
					texturePaths.push_back(assetPath + yamlBlock.Texture.BottomFace);
					texturePaths.push_back(assetPath + yamlBlock.Texture.SideFace);
					break;
				}

				s_BlockMap.push_back(block);
				s_BlockLookup[StringHash(block.InternalName)] = block.Id;

				RE_INFO("Loaded block: {} with ID: {} and {} textures", block.InternalName, block.Id, yamlBlock.Texture.NumTextures);
			}
		}

		// Load the textures
		RE_INFO("Loading block textures...");
		s_BlockTextureArray = Texture2DArray::Create(texturePaths, 5); // 16x16 textures down to 1x1 mipmap
	}
}