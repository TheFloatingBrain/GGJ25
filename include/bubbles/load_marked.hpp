#include <bubbles/common.hpp>


struct MarkedModel {
	Model model;
	std::vector<std::string> mesh_tags;
};

std::optional<MarkedModel> load_marked(const char* path)
{
	std::string check(path);
	if(check.ends_with(".gltf") == false) {
		spdlog::error("load_marked: File is not gltf");
		return std::nullopt;
	}
	MarkedModel model; 
	model.model = LoadModel(path);
	std::ifstream gltf_stream(path);
	if(gltf_stream.is_open() == false) {
		spdlog::error("load_marked: Failed to open gltf");
		return std::nullopt;
	}
	auto data = json::parse(gltf_stream);
	model.mesh_tags.resize(model.model.meshCount);
	for(const auto& node : data["nodes"]) {
		if(node.contains("mesh") == true)
			model.mesh_tags[node["mesh"]] = node["name"];
	}
	return model;
}

