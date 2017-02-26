#include "Collision.hpp"

namespace Collision {    
    static btCollisionConfiguration * g_collisionConfig;
    static btCollisionDispatcher * g_dispatcher;
    static btBroadphaseInterface * g_broadphase;
    static btCollisionWorld * g_collisionWorld;

    namespace Context {
	void Init() {
	    g_collisionConfig = new btDefaultCollisionConfiguration();
	    g_dispatcher = new btCollisionDispatcher(g_collisionConfig);
	}

	void Dispose() {
	    delete g_collisionConfig;
	    delete g_dispatcher;
	}
    }
}
