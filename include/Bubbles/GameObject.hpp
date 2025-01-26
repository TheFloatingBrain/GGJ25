#include <Bubbles/Physics.hpp>
#include <Bubbles/VectorUtilities.hpp>

#ifndef BUBBLES_GAME_OBJECT_INCLUDE_GUARD
#define BUBBLES_GAME_OBJECT_INCLUDE_GUARD
namespace Bubbles
{
	enum class GeometryType
	{
		Convex, 
		Sphere, 
		Box
	};

	struct PhysicsCreationInfo
	{
		::Vector3 startLocation = { 0 }, orientation = { 0 };
		float mass = 0.f;
		bool isStatic = false;

		btDefaultMotionState makeMotionState(auto& shape)
		{
			spdlog::trace("PhysicsCreationInfo: Creating Motion State");
			btVector3 localInertia(0, 0, 0);
			if(isStatic == false) {
				spdlog::trace("PhysicsCreationInfo: Calculating Inertia with mass {}", mass);
				shape.calculateLocalInertia(mass, localInertia);
				spdlog::trace("PhysicsCreationInfo: Inertial Calculated");
			}
			btDefaultMotionState state(makeTransform());
			spdlog::trace("PhysicsCreationInfo: Motion state created");
			return state;
		}

		inline btRigidBody::btRigidBodyConstructionInfo makeBodyInfo(
				btCollisionShape& shape, 
				btDefaultMotionState& motionState
			)
		{
			spdlog::trace("PhysicsCreationInfo: Creating Rigid Body Construction Info");
			btVector3 localInertia(0, 0, 0);
			btRigidBody::btRigidBodyConstructionInfo info(
				mass, &motionState, &shape, localInertia
			);
			spdlog::trace("PhysicsCreationInfo: Creating Rigid Body Construction Info Created");
			return info;
		}

		inline btTransform makeTransform() const
		{
			return btTransform(
				btQuaternion(
					orientation.x, 
					orientation.y, 
					orientation.z
				), 
				r2bv(startLocation)
			);
		}
	};


	struct Shape
	{
		GeometryType type;
		union {
			btBoxShape* box;
			btSphereShape* sphere;
			btConvexHullShape* convex;
		};
		void copy(Shape& other)
		{
			other.owns = false;
			type = other.type;
			switch(type)
			{
				case GeometryType::Box : {
					box = other.box;
					break;
				}
				case GeometryType::Sphere: {
					sphere = other.sphere;
					break;
				}
				case GeometryType::Convex: {
					convex = other.convex;
					break;
				}
				default : {
					box = other.box;
					break;
				}
			}
		}
		bool owns;
		Shape(Shape& other) {
			copy(other);
		}
		Shape() = delete;
		Shape(btBoxShape box_) : owns(true), type(GeometryType::Box), box(new btBoxShape(box_)) {}
		Shape(btSphereShape sphere_) : owns(true), type(GeometryType::Sphere), sphere(new btSphereShape(sphere_)) {}
		Shape(btConvexHullShape convex_) : owns(true), type(GeometryType::Convex), convex(new btConvexHullShape(convex_)) {}

		Shape& operator=(Shape& other) {
			copy(other);
			return *this;
		}

		btCollisionShape& shapeRef()
		{
			switch(type)
			{
				case GeometryType::Box : {
					spdlog::trace("Shape: shapeRef returning box");
					return *box;
				}
				case GeometryType::Sphere: {
					spdlog::trace("Shape: shapeRef returning sphere");
					return *sphere;
				}
				case GeometryType::Convex: {
					spdlog::trace("Shape: shapeRef returning convex");
					return *convex;
				}
				default : {
					spdlog::warn("Shape: shapeRef defaulting");
					return *box;
				}
			}
		}
		~Shape()
		{
			if(owns == true)
			{
				spdlog::trace("Shape: Calling Destructor for CollisionShape");
				switch(type)
				{
					case GeometryType::Box : {
						delete box;//.~btBoxShape();
						break;
					}
					case GeometryType::Sphere: {
						delete sphere;//.~btSphereShape();
						break;
					}
					case GeometryType::Convex: {
						delete convex;//.~btConvexHullShape();
						break;
					}
					default : { delete box; break; }//.~btBoxShape(); break; }
				}
				spdlog::trace("Shape: Collision Shape Destructor Called");
			}
		}
		auto visit(auto visitor)
		{
			switch(type)
			{
				case GeometryType::Box : {
					return visitor(box);
				}
				case GeometryType::Sphere: {
					return visitor(sphere);
				}
				case GeometryType::Convex: {
					return visitor(convex);
				}
				default : { return visitor(box); }
			}
		}
	};


	struct PhysicsData
	{
		Shape shape;
		btDefaultMotionState motionState;
		btRigidBody body;
		PhysicsData(
				Shape& shape_, 
				PhysicsCreationInfo info
			) : 
			shape(shape_), 
			motionState(info.makeMotionState(shape.shapeRef())), 
			body(info.makeBodyInfo(shape.shapeRef(), motionState)) {}
	};


	inline PhysicsData makeRigidBody(
			PhysicsCreationInfo info, 
			const Mesh& from
		)
	{
		if(from.indices != NULL)
		{
			spdlog::error("Attempted conversion of Raylib Mesh to "
					"Bullet3 Convex Hull when indicies are present. "
					//"Rigid Body not constructed."
				);
			//return std::nullopt;
		}
		spdlog::trace("makeRigidBody: Creating convex shape from Mesh");
		Shape convex_shape(btConvexHullShape(
			from.vertices, 
			from.vertexCount, 
			sizeof(float) * 3
		));
		spdlog::trace("makeRigidBody: Shape Made, returing Physics Data");
		return PhysicsData(convex_shape, info);
	}

	inline PhysicsData makeRigidBody(
			PhysicsCreationInfo info, 
			::Vector3 halfExtents
		)
	{
		spdlog::trace("makeRigidBody: Creating box shape");
		Shape shape(btBoxShape(r2bv(halfExtents)));
		spdlog::trace("makeRigidBody: Shape Made, returing Physics Data");
		return PhysicsData(
			shape, 
			info
		);
	}

	inline PhysicsData makeRigidBody(
			PhysicsCreationInfo info, 
			float radius
		)
	{
		spdlog::trace("makeRigidBody: Creating sphere shape");
		Shape shape(btSphereShape{radius});
		spdlog::trace("makeRigidBody: Shape Made, returing Physics Data");
		return PhysicsData(shape, info);
	}

	class PhysicsGameObject
	{
		protected: 
			Mesh mesh;
			bool ownsMesh;
			Material* material;
		public:
			PhysicsData physicsData;

			PhysicsGameObject(
					bool ownsMesh_, 
					Mesh mesh_, 
					PhysicsCreationInfo info, 
					Material* material_ = nullptr
				) : 
				ownsMesh(ownsMesh_), 
				mesh(mesh_), 
				material(material_),
				physicsData(makeRigidBody(info, mesh)) {}

			PhysicsGameObject(
					::Vector3 halfExtents, 
					PhysicsCreationInfo info, 
					Material* material_ = nullptr
				) : 
				ownsMesh(true), 
				mesh(GenMeshCube(halfExtents.x, halfExtents.y, halfExtents.z)), 
				material(material_),
				physicsData(makeRigidBody(info, halfExtents)) {}

			PhysicsGameObject(
					float radius, 
					PhysicsCreationInfo info, 
					Material* material_ = nullptr, 
					size_t rings = 20, 
					size_t slices = 20
				) : 
				ownsMesh(true), 
				mesh(GenMeshSphere(radius, rings, slices)), 
				material(material_),
				physicsData(makeRigidBody(info, radius)) {}
			
			::Vector3 getPosition() const {
				return b2rv(physicsData.body.getCenterOfMassPosition());
			}

			::Vector3 getRotation() const {
				return toRotation(physicsData.body.getOrientation());
			}
	};
}
#endif // BUBBLES_GAME_OBJECT_INCLUDE_GUARD

