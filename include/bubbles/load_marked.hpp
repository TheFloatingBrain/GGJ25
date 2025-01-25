#include <bubbles/common.hpp>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/types.h>
#include <assimp/postprocess.h>
#include <assimp/color4.h>
#include <stb_image.h>
#include <stdio.h>


struct MarkedModel {
	Model model;
	std::vector<std::string> mesh_tags;
};

void print_scene_node_names(aiNode* from, std::string tabs)
{
	std::string new_tabs = tabs + std::string{"\t"};
	std::string name;
	from->mName.Set(name);
	spdlog::info("{}Node: [{}]", tabs, name);
	spdlog::info("{}Child Count: [{}]", tabs, from->mNumChildren);
	spdlog::info("{}Mesh Count: [{}]", tabs, from->mNumMeshes);
	for(size_t ii = 0; ii < from->mNumChildren; ++ii)
		print_scene_node_names(from->mChildren[ii], new_tabs);
}

/********************************************************************************************
 * These functions yoinked/adapted from https://github.com/Gamerfiend/raylib-3D/blob/master/r3d.h,	*
 * thanks and credit to (c) 2020 Snowminx (github: @Gamerfiend), zlib/libpng license for	*
 * original code. **************************************************************************/


inline void setTextureFromAssimpMaterial(
		const struct aiScene* aiModel, 
		Model* model, 
		unsigned int materialIndex, 
		enum aiTextureType textureType, 
		MaterialMapIndex mapType
	)
{
    char buffer[512] = "unnamed";
    struct aiString path;

    // TODO: Support mutliple color for different types..
    // aiColor4D color = (aiColor4D){0.f, 0.f, 0.f, 0.f};
    // if (aiGetMaterialColor(aiModel->mMaterials[materialIndex], AI_MATKEY_COLOR_DIFFUSE, &color) == aiReturn_SUCCESS)
    // {
    //     model->materials[materialIndex].maps[MAP_ALBEDO].color = (Color){(unsigned char)(color.r * 255), (unsigned char)(color.g * 255), (unsigned char)(color.b * 255), (unsigned char)(color.a * 255)};
    // }

    unsigned int textureIndex = 0;

    if (aiGetMaterialTexture(aiModel->mMaterials[materialIndex], textureType, textureIndex, &path, NULL, NULL, NULL, NULL, NULL, NULL) == aiReturn_SUCCESS)
    {
        strncpy(buffer, path.data, sizeof(buffer));
        const char embedded[] = { "*" };

        // Embedded texture
        if (TextIsEqual(TextSubtext(buffer, 0, 1), embedded))
        {
            const char* cindex = TextSubtext(buffer, 1, TextLength(buffer));
            unsigned int index = atoi(cindex);
            struct aiTexture* embeddedTexture = aiModel->mTextures[index];

            unsigned char* imageData = NULL;
            int width;
            int height;
            Image rImage = { 0 };

            // Texture is compressed.. (jpg)
            if (embeddedTexture->mHeight == 0)
            {
                imageData = stbi_load_from_memory((const unsigned char*)embeddedTexture->pcData, embeddedTexture->mWidth, &width, &height, NULL, 4);
            }
            else
            {
                imageData = stbi_load_from_memory((const unsigned char*)embeddedTexture->pcData, embeddedTexture->mWidth * embeddedTexture->mHeight, &width, &height, NULL, 4);
            }

            rImage.data = imageData;
            rImage.width = width;
            rImage.height = height;
            rImage.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
            rImage.mipmaps = 1;

            model->materials[materialIndex].maps[mapType].texture = LoadTextureFromImage(rImage);
            UnloadImage(rImage);
        }
        else
        {
            model->materials[materialIndex].maps[mapType].texture = LoadTexture(buffer);
        }
    }
}

inline struct aiNode* FindMeshNode(struct aiNode* root, unsigned int mesh)
{
    for (unsigned int i = 0; i < root->mNumMeshes; ++i)
    {
        if (mesh == root->mMeshes[i])
        {
            return root;
        }
    }
    for (unsigned int i = 0; i < root->mNumChildren; ++i)
    {
        struct aiNode* child = root->mChildren[i];
        if (FindMeshNode(child, mesh) != NULL)
        {
            return child;
        }
    }
    return NULL;
}

inline Matrix ConvertAIMatrix4x4(aiMatrix4x4 mat)
{
    Matrix out;
    out.m0 = mat.a1; out.m4 = mat.a2; out.m8 = mat.a3; out.m12 = mat.a4;
    out.m1 = mat.b1; out.m5 = mat.b2; out.m9 = mat.b3; out.m13 = mat.b4;
    out.m2 = mat.c1; out.m6 = mat.c2; out.m10 = mat.c3; out.m14 = mat.c4;
    out.m3 = mat.d1; out.m7 = mat.d2; out.m11 = mat.d3; out.m15 = mat.d4;
    return out;
};

inline Vector3 ConvertAIVector3D(aiVector3D vert)
{
    Vector3 v = { 0 };
    v.x = vert.x;
    v.y = vert.y;
    v.z = vert.z;
    return v;
}

inline void load_model_materials_from_assimp(Model& model, const aiScene* aiModel)
{
	// Load Materials
	model.materialCount = aiModel->mNumMaterials;
	model.materials = (Material*)calloc(model.materialCount, sizeof(Material));
	spdlog::debug("load_model_materials_from_assimp::materials allocated");
	for (int i = 0; i < model.materialCount; i++)
	{
		model.materials[i] = LoadMaterialDefault();

		// TODO: Support Base Color texture type? It doesn't seem to be used, even in PBR material flows like GLTF
		if (aiGetMaterialTextureCount(aiModel->mMaterials[i], aiTextureType_DIFFUSE) > 0) {
			setTextureFromAssimpMaterial(aiModel, &model, i, aiTextureType_DIFFUSE, MATERIAL_MAP_ALBEDO);
		}
		if (aiGetMaterialTextureCount(aiModel->mMaterials[i], aiTextureType_NORMALS) > 0) {
			setTextureFromAssimpMaterial(aiModel, &model, i, aiTextureType_NORMALS, MATERIAL_MAP_NORMAL);
		}
		if (aiGetMaterialTextureCount(aiModel->mMaterials[i], aiTextureType_AMBIENT) > 0) {
			setTextureFromAssimpMaterial(aiModel, &model, i, aiTextureType_AMBIENT, MATERIAL_MAP_METALNESS);
		}
		if (aiGetMaterialTextureCount(aiModel->mMaterials[i], aiTextureType_SHININESS) > 0) {
			setTextureFromAssimpMaterial(aiModel, &model, i, aiTextureType_SHININESS, MATERIAL_MAP_ROUGHNESS);
		}
	}
	spdlog::debug("load_model_materials_from_assimp::material and texture initialization complete");
}


inline void load_mesh_verticies_from_assimp(Mesh& to, aiMesh* from, aiNode* parent)
{
	spdlog::debug("load_mesh_verticies_from_assimp::mesh verticies allocated");
	to.animVertices = NULL;
	to.vertexCount = from->mNumVertices;
	spdlog::debug("load_mesh_verticies_from_assimp::mesh vertex count {} ", to.vertexCount);
	// Assimp stores vertices in Vector3 (XYZ) Raylib stores vertices in float array, where every three is one vertex (XYZ)
	to.vertices =(float*)malloc((sizeof(float) * to.vertexCount) * 3); //new float[to.vertexCount * 3];
	unsigned int vectorCounter = 0;
	std::string mesh_name;
	from->mName.Set(mesh_name);
	spdlog::debug("load_mesh_verticies_from_assimp::mesh name [{}]", mesh_name);
	for (int j = 0; j < to.vertexCount * 3; j += 3)
	{
		Vector3 vert = Vector3Transform(
			ConvertAIVector3D(from->mVertices[vectorCounter]), 
			ConvertAIMatrix4x4(parent->mTransformation)
		);
		to.vertices[j] = vert.x;
		to.vertices[j + 1] = vert.y;
		to.vertices[j + 2] = vert.z;
		vectorCounter++;
	}
	spdlog::debug("load_mesh_verticies_from_assimp::mesh verticies initialized");
}

inline void load_texture_coordinates_from_assimp(Mesh& mesh, aiMesh* importMesh)
{
	// Assimp stores texCoords in Vector3 (XYZ), Raylib uses (UV) float array
	if (importMesh->mTextureCoords[0])
	{
		mesh.texcoords = new float[mesh.vertexCount * 2];
		spdlog::debug("load_texture_coordinates_from_assimp::mesh texture coordinates 0 allocated");
		unsigned int texCoord = 0;
		for (int j = 0; j < mesh.vertexCount * 2; j += 2)
		{
			mesh.texcoords[j] = importMesh->mTextureCoords[0][texCoord].x;
			mesh.texcoords[j + 1] = importMesh->mTextureCoords[0][texCoord].y;
			texCoord++;
		}
		spdlog::debug("load_texture_coordinates_from_assimp::mesh texture coordinates 0 initialized");
	}
	else {
		spdlog::debug("load_texture_coordinates_from_assimp::no texcoords");
		mesh.texcoords = NULL;
	}

	// Raylib supports two layers of textureCoords
	if (importMesh->mTextureCoords[1])
	{
		mesh.texcoords2 = new float[mesh.vertexCount * 2];
		spdlog::debug("load_texture_coordinates_from_assimp::mesh texture coordinates 1 allocated");
		unsigned int texCoord = 0;
		for (int j = 0; j < mesh.vertexCount * 2; j += 2)
		{
			mesh.texcoords2[j] = importMesh->mTextureCoords[1][texCoord].x;
			mesh.texcoords2[j + 1] = importMesh->mTextureCoords[1][texCoord].y;
			texCoord++;
		}
		spdlog::debug("load_texture_coordinates_from_assimp::mesh texture coordinates 1 initialized");
	}
	else {
		spdlog::debug("load_texture_coordinates_from_assimp::no texcoords2");
		mesh.texcoords2 = NULL;
	}
}

inline void load_normals_from_assimp(Mesh& mesh, aiMesh* importMesh)
{
	mesh.normals = (float*)malloc((sizeof(float) * mesh.vertexCount) * 3);//new float[mesh.vertexCount * 3];
	mesh.animNormals = NULL;
	spdlog::debug("load_normals_from_assimp::mesh normals allocated");
	unsigned int normalCounter = 0;
	for (int j = 0; j < mesh.vertexCount * 3; j += 3)
	{
		mesh.normals[j] = importMesh->mNormals[normalCounter].x;
		mesh.normals[j + 1] = importMesh->mNormals[normalCounter].y;
		mesh.normals[j + 2] = importMesh->mNormals[normalCounter].z;
		normalCounter++;
	}
	spdlog::debug("load_normals_from_assimp::mesh normals initialized");
}

inline void load_faces_from_assimp(Mesh& mesh, aiMesh* importMesh)
{
	spdlog::debug("load_faces_from_assimp::start");
	unsigned int indiceTotal = 0;
	for (unsigned int j = 0; j < importMesh->mNumFaces; j++)
	{
		indiceTotal += importMesh->mFaces[j].mNumIndices;
	}
	spdlog::debug("load_faces_from_assimp::indice total {}", indiceTotal);

	mesh.indices = new unsigned short[indiceTotal];
	unsigned int indexCounter = 0;
	spdlog::debug("load_faces_from_assimp::indices allocated");
	for (unsigned int j = 0; j < importMesh->mNumFaces; j++)
	{
		for (unsigned int k = 0; k < importMesh->mFaces[j].mNumIndices; k++)
		{
			mesh.indices[indexCounter] = importMesh->mFaces[j].mIndices[k];
			indexCounter++;
		}
	}
	spdlog::debug("load_faces_from_assimp::indices initilized");

	mesh.triangleCount = importMesh->mNumFaces;
	spdlog::debug("load_faces_from_assimp::triangle count set");
}

inline void load_tangents_from_assimp(Mesh& mesh, aiMesh* importMesh)
{
	spdlog::debug("load_tangents_from_assimp::start");
	if (importMesh->mTangents)
	{
		spdlog::debug("load_tangents_from_assimp::mesh has tangents");
		mesh.tangents = new float[mesh.vertexCount * 4];
		unsigned int tangentCounter = 0;
		spdlog::debug("load_tangents_from_assimp::tangents allocated");
		for (int j = 0; j < mesh.vertexCount * 4; j += 4)
		{
			mesh.tangents[j] = importMesh->mTangents[tangentCounter].x;
			mesh.tangents[j + 1] = importMesh->mTangents[tangentCounter].y;
			mesh.tangents[j + 2] = importMesh->mTangents[tangentCounter].z;
			mesh.tangents[j + 3] = 0;
			tangentCounter++;
		}
		spdlog::debug("load_tangents_from_assimp::tangents initalized");
	}
	else {
		spdlog::debug("load_tangents_from_assimp::no tangents");
		mesh.tangents = NULL;
	}
}

inline void load_colors_from_assimp(Mesh& mesh, aiMesh* importMesh)
{
	spdlog::debug("load_colors_from_assimp::start");
	if (importMesh->mColors[0])
	{
		spdlog::debug("load_colors_from_assimp::mesh has colors");
		mesh.colors = new unsigned char[mesh.vertexCount * 4];
		unsigned int colorCounter = 0;
		spdlog::debug("load_colors_from_assimp::colors allocated");
		for (int j = 0; j < mesh.vertexCount * 4; j += 4)
		{
			mesh.colors[j] = importMesh->mColors[0][colorCounter].r;
			mesh.colors[j + 1] = importMesh->mColors[0][colorCounter].g;
			mesh.colors[j + 2] = importMesh->mColors[0][colorCounter].b;
			mesh.colors[j + 3] = importMesh->mColors[0][colorCounter].a;
			colorCounter++;
		}
		spdlog::debug("load_colors_from_assimp::colors initialized");
	}
	else {
		spdlog::debug("load_colors_from_assimp::no colors");
		mesh.colors = NULL;
	}
}

inline std::optional<MarkedModel> load_marked(const char* filename)
{
	spdlog::debug("load_marked::begin");
	MarkedModel marked;
	marked.model = { 0 };
	::Model& model = marked.model;
	const struct aiScene* aiModel = aiImportFile(filename, aiProcess_Triangulate);
	//TODO Error handling for when a model isn't loaded successfully
	if (!aiModel)
	{
		spdlog::critical("load_marked::unable to load model");
		TraceLog(LOG_WARNING, "LoadModelAdvanced: Unable able to load model %s", filename);
		return std::nullopt;
	}

	model.transform = MatrixIdentity();


	load_model_materials_from_assimp(model, aiModel);

	//Load Meshes for Model
	model.meshCount = aiModel->mNumMeshes;
	model.meshes = new Mesh[model.meshCount];
	model.meshMaterial = new int[model.meshCount];
	spdlog::debug("load_marked::meshes allocated");
	for (int i = 0; i < model.meshCount; i++)
	{
		struct aiMesh* importMesh = aiModel->mMeshes[i];
		struct aiNode* parentNode = FindMeshNode(aiModel->mRootNode, i);
		std::string mesh_name;
		importMesh->mName.Set(mesh_name);
		marked.mesh_tags.push_back(mesh_name);
		Mesh& mesh = model.meshes[i];
		mesh.boneIds = NULL;
		mesh.boneWeights = NULL;
		load_mesh_verticies_from_assimp(mesh, importMesh, parentNode);
		load_texture_coordinates_from_assimp(mesh, importMesh);
		load_normals_from_assimp(mesh, importMesh);
		load_faces_from_assimp(mesh, importMesh);
		load_tangents_from_assimp(mesh, importMesh);
		load_colors_from_assimp(mesh, importMesh);

		// Mark this material index as active, so we can coorelate it to a material
		if (importMesh->mMaterialIndex >= 0)
		{
			model.meshMaterial[i] = importMesh->mMaterialIndex;
		}

		//mesh.vboId = new unsigned int[7]; // Done in UploadMesh
	}
	spdlog::debug("load_marked::meshes initialized");

	for (int i = 0; i < model.meshCount; i++)
	{
		UploadMesh(&model.meshes[i], false);
		spdlog::debug("load_marked::mesh {} uploaded successfully", i);
	}
	spdlog::debug("load_marked::meshes uploaded");

	aiReleaseImport(aiModel);
	spdlog::debug("load_marked::model import released");

	return marked;
}

