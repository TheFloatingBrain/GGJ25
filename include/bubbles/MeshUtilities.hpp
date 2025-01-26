#include <bubbles/common.hpp>

#ifndef BUBBLES_MESH_UTILITIES_INCLUDE_GUARD
#define BUBBLES_MESH_UTILITIES_INCLUDE_GUARD
namespace Bubbles
{
	inline void info_has_not_null(auto* what, std::string_view name, std::string tabs) {
		spdlog::info("{}Has {}: {}", tabs, name, (what == NULL));
	}
	
	inline void print_mesh_basic_info(const Mesh& mesh, std::string tabs)
	{
		spdlog::info("{}Vertex Count: [{}]", tabs, mesh.vertexCount);
		info_has_not_null(mesh.vertices, "Vertices", tabs);
		info_has_not_null(mesh.texcoords, "Texcoords", tabs);
		info_has_not_null(mesh.texcoords2, "Texcoords2", tabs);
		info_has_not_null(mesh.normals, "Normals", tabs);
		info_has_not_null(mesh.tangents, "Tangents", tabs);
		info_has_not_null(mesh.colors, "Colors", tabs);
		info_has_not_null(mesh.indices, "Indices", tabs);
		info_has_not_null(mesh.animVertices, "AnimVertices", tabs);
		info_has_not_null(mesh.animNormals, "AnimNormals", tabs);
	}
	
	inline void print_mesh_bone_info(const Mesh& mesh, std::string tabs)
	{
		spdlog::info("{}Bone Count: [{}]", tabs, mesh.boneCount);
		info_has_not_null(mesh.boneIds, "BoneIds", tabs);
		info_has_not_null(mesh.boneWeights, "BoneWeights", tabs);
		info_has_not_null(mesh.boneMatrices, "BoneMatricies", tabs);
	}

	inline void print_mesh_info(const Mesh& mesh, std::string tabs) {
		print_mesh_basic_info(mesh, tabs);
		print_mesh_bone_info(mesh, tabs);
	}

	inline void print_model_info(const Model& model)
	{
		spdlog::info("Model Mesh Count: [{}]", model.meshCount);
    		spdlog::info("Model Material Count: [{}]: ", model.materialCount);
		info_has_not_null(model.materials, "Materials", "");
		info_has_not_null(model.meshMaterial, "Mesh Materials", "");
		for(size_t ii = 0; ii < model.meshCount; ++ii) {
			spdlog::info("Model Mesh: [{}]", ii);
			print_mesh_info(model.meshes[ii], "\t");
		}
	}
}
#endif // BUBBLES_MESH_UTILITIES_INCLUDE_GUARD

