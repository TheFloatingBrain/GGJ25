#include <Bubbles/Common.hpp>
#include <Bubbles/GameObject.hpp>
#include <Bubbles/VectorUtilities.hpp>
#include <unordered_map>

#ifndef BUBBLES_LOAD_MARKED_INCLUDE_GUARD
#define BUBBLES_LOAD_MARKED_INCLUDE_GUARD
namespace Bubbles
{

	enum class ObjectTag {
		Ground
	};

	struct MarkedModel {
		Model model;
		std::vector<std::string> meshTags;
	};

	inline void resolvePhysicsInfoBasedOnType(PhysicsCreationInfo& toResolve, ObjectTag type) {
		toResolve.isStatic = true;
		toResolve.mass = 0;
	}

	inline Vector3 vector3FromJsonArray(auto jsonData) {
		return Vector3{jsonData[0], jsonData[1], jsonData[2]};
	}
	inline Color colorFromJsonArray(auto jsonData)
	{
		if(jsonData.size() > 3)
			return Color{jsonData[0], jsonData[1], jsonData[2], jsonData[3]};
		return Color{jsonData[0], jsonData[1], jsonData[2]};
	}

	inline ObjectTag objectTagFromString(const auto& from) {
		return ObjectTag::Ground;
	}

	inline Color colorFromJson(const auto& jsonNode)
	{
		const std::unordered_map<std::string, Color> colorStrings{
			{"LIGHTGRAY" ,  LIGHTGRAY }, 
			{"GRAY"      ,  GRAY      }, 
			{"DARKGRAY"  ,  DARKGRAY  }, 
			{"YELLOW"    ,  YELLOW    }, 
			{"GOLD"      ,  GOLD      }, 
			{"ORANGE"    ,  ORANGE    }, 
			{"PINK"      ,  PINK      }, 
			{"RED"       ,  RED       }, 
			{"MAROON"    ,  MAROON    }, 
			{"GREEN"     ,  GREEN     }, 
			{"LIME"      ,  LIME      }, 
			{"DARKGREEN" ,  DARKGREEN }, 
			{"SKYBLUE"   ,  SKYBLUE   }, 
			{"BLUE"      ,  BLUE      }, 
			{"DARKBLUE"  ,  DARKBLUE  }, 
			{"PURPLE"    ,  PURPLE    }, 
			{"VIOLET"    ,  VIOLET    }, 
			{"DARKPURPLE",  DARKPURPLE},
			{"BEIGE"     ,  BEIGE     }, 
			{"BROWN"     ,  BROWN     }, 
			{"DARKBROWN" ,  DARKBROWN }, 
			{"WHITE"     ,  WHITE     }, 
			{"BLACK"     ,  BLACK     }, 
			{"BLANK"     ,  BLANK     }, 
			{"MAGENTA"   ,  MAGENTA   }, 
			{"RAYWHITE"  ,  RAYWHITE  }
		};
		Color color = PINK;
		if(jsonNode.contains("colorString") == true) {
			if(colorStrings.contains(jsonNode["colorString"]) == true) {
				color = colorStrings.at(jsonNode["colorString"]);
			}
		}
		else if(jsonNode.contains("color") == true)
			color = colorFromJsonArray(jsonNode["color"]);
		return color;
	}

	struct BasicObjectInfo
	{
		PhysicsCreationInfo info;
		ObjectTag tag;
		Color color;
	};


	inline BasicObjectInfo basicObjectInfoFromJson(const auto& jsonNode)
	{
		PhysicsCreationInfo info{};
		ObjectTag type = ObjectTag::Ground;
		if(jsonNode.contains("type") == true)
			type = objectTagFromString(jsonNode["type"]);
		if(jsonNode.contains("position") == true)
			info.startLocation = vector3FromJsonArray(jsonNode["position"]);
		if(jsonNode.contains("orientation") == true)
			info.orientation = degrees2Radians(vector3FromJsonArray(jsonNode["orientation"]));
		resolvePhysicsInfoBasedOnType(info, type);
		if(jsonNode.contains("mass") == true)
			info.mass = jsonNode["mass"];
		if(jsonNode.contains("isStatic") == true)
			info.isStatic = jsonNode["isStatic"];
		return {info, type, colorFromJson(jsonNode)};
	}

	inline void boxesFromJson(
			std::vector<PhysicsGameObject*>& objects, 
			std::vector<ObjectTag>& objectTypes, 
			std::vector<Color>& colors, 
			const auto& jsonData
		)
	{
		if(jsonData.contains("boxes") == false)
			return;
		for(const auto& boxData : jsonData["boxes"])
		{
			Vector3 size = { 0 };
			if(boxData.contains("size") == false)
				continue;
			else
				size = vector3FromJsonArray(boxData["size"]);
			auto info = basicObjectInfoFromJson(boxData);
			objects.push_back(new PhysicsGameObject{size, info.info});
			objectTypes.push_back(info.tag);
			colors.push_back(info.color);
		}
	}

	inline void spheresFromJson(
			std::vector<PhysicsGameObject*>& objects, 
			std::vector<ObjectTag>& objectTypes, 
			std::vector<Color>& colors, 
			const auto& jsonData
		)
	{
		if(jsonData.contains("spheres") == false)
			return;
		for(const auto& sphereData : jsonData["spheres"])
		{
			float radius = 0.f;
			if(sphereData.contains("radius") == false)
				continue;
			else
				radius = sphereData["size"];
			auto info = basicObjectInfoFromJson(sphereData);
			objects.push_back(new PhysicsGameObject{radius, info.info});
			objectTypes.push_back(info.tag);
			colors.push_back(info.color);
		}
	}
	inline void convexFromJson(
			std::vector<PhysicsGameObject*>& objects, 
			std::vector<ObjectTag>& objectTypes, 
			std::vector<Color>& colors, 
			const auto& jsonData
		)
	{
		if(jsonData.contains("convex") == true)
			spdlog::error("convexFromJson: Stub, not implemented");
	}

	inline void objectsFromJson(
			std::vector<PhysicsGameObject*>& objects, 
			std::vector<ObjectTag>& objectTypes, 
			std::vector<Color>& colors, 
			const auto& jsonData
		)
	{
		boxesFromJson(objects, objectTypes, colors, jsonData);
		spheresFromJson(objects, objectTypes, colors, jsonData);
		convexFromJson(objects, objectTypes, colors, jsonData);
	}

	void registerObjects(std::vector<PhysicsGameObject*>& objects, 
			Physics& physicsWorld) {
		for(auto* object : objects)
			physicsWorld.addGameObject(*object);
	}

	void drawObjects(
			std::vector<PhysicsGameObject*>& objects, 
			std::vector<Color>& colors
		)
	{
		const size_t objectCount = objects.size();
		for(size_t objectIndex = 0; objectIndex < objectCount; ++objectIndex)
			objects[objectIndex]->drawColored(colors[objectIndex]);
	}

	struct Level
	{
		std::vector<PhysicsGameObject*> levelObjects;
		std::vector<ObjectTag> objectTypes;
		std::vector<Color> colors;

		Level(std::string_view path, Physics& physics) {
			loadObjectsFromFile(path);
			registerObjects(physics);
		}

		~Level() { for(auto* object : levelObjects) delete object; }

		void drawObjects() { Bubbles::drawObjects(levelObjects, colors); }

		void registerObjects(Physics& physicsWorld) {
			Bubbles::registerObjects(levelObjects, physicsWorld);
		}

		bool loadObjectsFromFile(std::string_view path)
		{
			std::ifstream levelStream(path.data());
			if(levelStream.is_open() == false) {
				spdlog::error("Failed to load level from file {}", path);
				return false;
			}
			auto data = json::parse(levelStream);
			loadObjects(data);
			return true;
		}
		void loadObjects(const auto& jsonData) {
			objectsFromJson(levelObjects, objectTypes, colors, jsonData);
		}
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

