#ifndef PHYSICS_H
#define PHYSICS_H

#include <btBulletDynamicsCommon.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
using namespace std;

class Physics
{
public:
	btBroadphaseInterface* m_broadphase;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btSequentialImpulseConstraintSolver* m_solver;
	btDiscreteDynamicsWorld* m_dynamicsWorld;

	btRigidBody* m_fallRigidBody;
	btRigidBody* m_camRigidBody;

	btCompoundShape* m_tableShape;

	std::vector<btRigidBody*> m_tables;


	int counter = 0;


	Physics();
	~Physics();
	void initDynamicScene();
	void initTableBodies();
};

#endif