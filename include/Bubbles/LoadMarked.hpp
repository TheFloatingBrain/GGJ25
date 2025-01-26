#include <Bubbles/Common.hpp>

#ifndef BUBBLES_LOAD_MARKED_INCLUDE_GUARD
#define BUBBLES_LOAD_MARKED_INCLUDE_GUARD
namespace Bubbles
{
	struct MarkedModel {
		Model model;
		std::vector<std::string> meshTags;
	};
	
	std::optional<MarkedModel> LoadMarked(const char* path)
	{
		std::string check(path);
		if(check.ends_with(".gltf") == false) {
			spdlog::error("LoadMarked: File is not gltf");
			return std::nullopt;
		}
		MarkedModel model; 
		//model.model = LoadModel(path);
		std::ifstream gltfStream(path);
		if(gltfStream.is_open() == false) {
			spdlog::error("LoadMarked: Failed to open gltf");
			return std::nullopt;
		}
		auto data = json::parse(gltfStream);
		model.model = LoadModel(path);
		model.meshTags.resize(model.model.meshCount);
		for(const auto& node : data["nodes"]) {
			if(node.contains("mesh") == true)
				model.meshTags[node["mesh"]] = node["name"];
		}
		return model;
	}
}
#endif //BUBBLES_LOAD_MARKED_INCLUDE_GUARD

