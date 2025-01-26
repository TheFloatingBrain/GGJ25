#include <bubbles/Physics.hpp>
#include <bubbles/VectorUtilities.hpp>

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
		::Vector3 startLocation, orientation;
		float mass;
		bool isStatic;

		btDefaultMotionState makeMotionState(btCollisionShape& shape)
		{
			btVector3 localInertia(0, 0, 0);
			if(isStatic == false)
				shape.calculateLocalInertia(mass, localInertia);
			return btDefaultMotionState(makeTransform());
		}

		inline btRigidBody::btRigidBodyConstructionInfo makeBodyInfo(
				btCollisionShape& shape, 
				btDefaultMotionState& motionState
			)
		{
			btVector3 localInertia(0, 0, 0);
			return btRigidBody::btRigidBodyConstructionInfo(
				mass, &motionState, &shape, localInertia
			);
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
			btBoxShape box;
			btSphereShape sphere;
			btConvexHullShape convex;
		};
		void copy(const Shape& other)
		{
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
		Shape(const Shape& other) {
			copy(other);
		}
		Shape(btBoxShape box_) : type(GeometryType::Box), box(box_) {}
		Shape(btSphereShape sphere_) : type(GeometryType::Sphere), sphere(sphere_) {}
		Shape(btConvexHullShape convex_) : type(GeometryType::Convex), convex(convex_) {}

		Shape& operator=(const Shape& other) {
			copy(other);
			return *this;
		}

		btCollisionShape& shapeRef()
		{
			switch(type)
			{
				case GeometryType::Box : {
					return box;
				}
				case GeometryType::Sphere: {
					return sphere;
				}
				case GeometryType::Convex: {
					return convex;
				}
				default : { return box; }
			}
		}
		~Shape()
		{
			switch(type)
			{
				case GeometryType::Box : {
					box.~btBoxShape();
					break;
				}
				case GeometryType::Sphere: {
					sphere.~btSphereShape();
					break;
				}
				case GeometryType::Convex: {
					convex.~btConvexHullShape();
					break;
				}
				default : { box.~btBoxShape(); break; }
			}
		}
	};

	//void myfunc(Shape shape)
	//{
	//	switch(shape.type)
	//	{
	//		case GeometryType::Box : {
	//			// Do box things
	//		}
	//		case GeometryType::Sphere: {
	//			// Do sphere things
	//		}
	//		case GeometryType::Convex: {
	//			// Do convex hull things
	//		}
	//	}
	//}


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
		btDefaultMotionState motionState;
		Shape convex_shape(btConvexHullShape(
			from.vertices, 
			from.vertexCount, 
			sizeof(float) * 3
		));
		return PhysicsData(convex_shape, info);
	}

	inline PhysicsData makeRigidBody(
			PhysicsCreationInfo info, 
			::Vector3 halfExtents
		)
	{
		Shape shape(btBoxShape(r2bv(halfExtents)));
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
		btSphereShape sphere(radius);
		Shape shape(sphere);
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

