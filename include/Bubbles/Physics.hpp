#include <Bubbles/Common.hpp>

#ifndef BUBBLES_PHYSICS_INCLUDE_GUARD
#define BUBBLES_PHYSICS_INCLUDE_GUARD
namespace Bubbles
{

	/// This is a Hello World program for running a basic Bullet physics simulation
	struct Physics {
	    ///-----includes_end-----
	
	    btDefaultCollisionConfiguration collisionConfiguration;
	
	    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	    btCollisionDispatcher dispatcher;
	
	    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	    btBroadphaseInterface* overlappingPairCache;
	
	    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	    btSequentialImpulseConstraintSolver solver;
	
	    btDiscreteDynamicsWorld dynamicsWorld;
	    ///-----initialization_start-----
	    Physics() :
	        collisionConfiguration(btDefaultCollisionConfiguration()),
	        dispatcher(btCollisionDispatcher(&collisionConfiguration)),
	        overlappingPairCache(new btDbvtBroadphase()),
	        solver(),
	        dynamicsWorld(&dispatcher, overlappingPairCache, &solver, &collisionConfiguration)
	    {
	        ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	
	          ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	
	
	          ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	
	
	          ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	
	        dynamicsWorld.setGravity(btVector3(0, -10, 0));
	    }
	
	};
}
#endif //BUBBLES_PHYSICS_INCLUDE_GUARD

