#include <Bubbles/GameObject.hpp>

#ifndef BUBBLES_PHYSICS_INCLUDE_GUARD
#define BUBBLES_PHYSICS_INCLUDE_GUARD
namespace Bubbles
{
	struct Physics
	{
	    btDefaultCollisionConfiguration collisionConfiguration;
	    btCollisionDispatcher dispatcher;
	    btBroadphaseInterface* overlappingPairCache;
	    btSequentialImpulseConstraintSolver solver;
	    btDiscreteDynamicsWorld dynamicsWorld;
	    Physics() :
	        collisionConfiguration(btDefaultCollisionConfiguration()),
	        dispatcher(btCollisionDispatcher(&collisionConfiguration)),
	        overlappingPairCache(new btDbvtBroadphase()),
	        solver(),
	        dynamicsWorld(&dispatcher, overlappingPairCache, &solver, &collisionConfiguration)
	    {
	        dynamicsWorld.setGravity(btVector3(0, -10, 0));
	    }
	    void addGameObject(PhysicsGameObject& object) {
		    dynamicsWorld.addRigidBody(&object.physicsData.body);
	    }
	    void step() {
		    dynamicsWorld.stepSimulation(1.f / 60.f, 10);
	    }
	
	};
}
#endif //BUBBLES_PHYSICS_INCLUDE_GUARD

