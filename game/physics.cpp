#include <btBulletDynamicsCommon.h>
#include <ctime>

#include "physics.h"
#include "main.h"

btRigidBody* m_fallRigidBody;
btRigidBody* m_camRigidBody;


Physics::Physics()
{
	// Build the m_broadphase
	m_broadphase = new btDbvtBroadphase();

	// Set up the collision configuration and m_dispatcher
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);


	// The actual physics m_solver
	m_solver = new btSequentialImpulseConstraintSolver;

	// The world.
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
	m_dynamicsWorld->setGravity(btVector3(0, -10, 0));
}

Physics::~Physics()
{
	for (int i = 0; i < m_tables.size(); i++)
	{
		m_dynamicsWorld->removeRigidBody(m_tables[i]);
		delete m_tables[i]->getMotionState();
		delete m_tables[i];
	}

	m_dynamicsWorld->removeRigidBody(m_camRigidBody);
	delete m_camRigidBody->getMotionState();
	delete m_camRigidBody;

	delete m_dynamicsWorld;
	delete m_solver;
	delete m_collisionConfiguration;
	delete m_dispatcher;
	delete m_broadphase;

	
}

void Physics::initDynamicScene()
{
	// Call this function after the models have been loaded or else the max/min values won't be correct values

	btCollisionShape* m_groundShape = new btBoxShape(btVector3(floor_model->max_x, floor_model->max_y / 2, floor_model->max_z));
	btCollisionShape* m_wallShape = new btBoxShape(btVector3(floor_model->max_x, floor_model->max_y, floor_model->max_z));
	btCollisionShape* m_cameraShape = new btBoxShape(btVector3(1, 2, 1));



	//Setup table shape
	m_tableShape = new btCompoundShape();
	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(0, 0, 0));
	//btCollisionShape* box1 = new btBoxShape(btVector3(table->meshes[1].meshMax.x, table->meshes[1].meshMax.y / 2, table->meshes[1].meshMax.z));
	btCollisionShape* box1 = new btCylinderShape(btVector3(table->meshes[1].meshMax.x, table->meshes[1].meshMax.y / 2, table->meshes[1].meshMax.z));
	m_tableShape->addChildShape(transform, box1);

	transform.setIdentity();
	transform.setOrigin(btVector3(0, 0.6, 0));
	//btCollisionShape* box2 = new btBoxShape(btVector3(table->meshes[0].meshMax.x, (table->meshes[0].meshMax.y - table->meshes[1].meshMax.y) / 2, table->meshes[0].meshMax.z));
	btCollisionShape* box2 = new btCylinderShape(btVector3(table->meshes[0].meshMax.x, (table->meshes[0].meshMax.y - table->meshes[1].meshMax.y) / 2, table->meshes[0].meshMax.z));
	m_tableShape->addChildShape(transform, box2);

	//btCollisionShape* m_tableShape = new btBoxShape(btVector3(0.2, 1.2/2, 0.2));
	/*
	btConvexHullShape* m_tableShape = new btConvexHullShape();

	int numMeshes = table->meshes.size();

	for (int i = 0; i < numMeshes; i++)
	{
	Mesh _tempMesh = table->meshes[i];
	int numVertices = table->meshes[i].vertices.size();

	for (int j = 0; j < numVertices; j++)
	{
	glm::vec3 _tempPos = _tempMesh.vertices[j].Position;
	m_tableShape->addPoint(btVector3(_tempPos.x, _tempPos.y, _tempPos.z));
	}
	}
	*/






	//Setup Rigid Bodies

	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -(floor_model->max_y / 2), 0)));
	btRigidBody::btRigidBodyConstructionInfo
		groundRigidBodyCI(0, groundMotionState, m_groundShape, btVector3(0, 0, 0));
	groundRigidBodyCI.m_restitution = 0;
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	groundRigidBody->setFriction(0.4f);
	groundRigidBody->setRollingFriction(0.4f);
	this->m_dynamicsWorld->addRigidBody(groundRigidBody);

	btDefaultMotionState* roofMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 3 * (floor_model->max_y / 2), 0)));
	btRigidBody::btRigidBodyConstructionInfo
		roofRigidBodyCI(0, roofMotionState, m_groundShape, btVector3(0, 0, 0));
	roofRigidBodyCI.m_restitution = 0;
	btRigidBody* roofRigidBody = new btRigidBody(roofRigidBodyCI);
	roofRigidBody->setFriction(0.4f);
	roofRigidBody->setRollingFriction(0.4f);
	this->m_dynamicsWorld->addRigidBody(roofRigidBody);

	//Wall 1
	btDefaultMotionState* wallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 2 * floor_model->max_z)));
	btRigidBody::btRigidBodyConstructionInfo
		wallRigidBodyCI(0, wallMotionState, m_wallShape, btVector3(0, 0, 0));
	wallRigidBodyCI.m_restitution = 0;
	btRigidBody* wallRigidBody = new btRigidBody(wallRigidBodyCI);
	wallRigidBody->setFriction(0.4f);
	wallRigidBody->setRollingFriction(0.4f);
	this->m_dynamicsWorld->addRigidBody(wallRigidBody);

	//Wall 2
	btDefaultMotionState* wall2MotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, -2 * floor_model->max_z)));
	btRigidBody::btRigidBodyConstructionInfo
		wall2RigidBodyCI(0, wall2MotionState, m_wallShape, btVector3(0, 0, 0));
	wall2RigidBodyCI.m_restitution = 0;
	btRigidBody* wall2RigidBody = new btRigidBody(wall2RigidBodyCI);
	wall2RigidBody->setFriction(0.4f);
	wall2RigidBody->setRollingFriction(0.4f);
	this->m_dynamicsWorld->addRigidBody(wall2RigidBody);

	//Wall 3
	btDefaultMotionState* wall3MotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(2 * floor_model->max_x, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo
		wall3RigidBodyCI(0, wall3MotionState, m_wallShape, btVector3(0, 0, 0));
	wall3RigidBodyCI.m_restitution = 0;
	btRigidBody* wall3RigidBody = new btRigidBody(wall3RigidBodyCI);
	wall3RigidBody->setFriction(0.4f);
	wall3RigidBody->setRollingFriction(0.4f);
	this->m_dynamicsWorld->addRigidBody(wall3RigidBody);

	//Wall 3
	btDefaultMotionState* wall4MotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(-2 * floor_model->max_x, 0, 0)));
	btRigidBody::btRigidBodyConstructionInfo
		wall4RigidBodyCI(0, wall4MotionState, m_wallShape, btVector3(0, 0, 0));
	wall4RigidBodyCI.m_restitution = 0;
	btRigidBody* wall4RigidBody = new btRigidBody(wall4RigidBodyCI);
	wall4RigidBody->setFriction(0.4f);
	wall4RigidBody->setRollingFriction(0.4f);
	this->m_dynamicsWorld->addRigidBody(wall4RigidBody);

	/*
	btDefaultMotionState* fallMotionState =
		new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0.5, 0)));
	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	m_tableShape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo m_fallRigidBodyCI(mass, fallMotionState, m_tableShape, fallInertia);
	m_fallRigidBodyCI.m_restitution = 0;
	this->m_fallRigidBody = new btRigidBody(m_fallRigidBodyCI);
	this->m_fallRigidBody->setFriction(0.4f);
	this->m_fallRigidBody->setRollingFriction(0.4f);
	this->m_dynamicsWorld->addRigidBody(this->m_fallRigidBody);
	*/

	initTableBodies();

	btDefaultMotionState* camMotionState = 
		new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 2, 5)));
	btScalar cam_mass = 1;
	btVector3 camInertia(0, 0, 0);
	m_cameraShape->calculateLocalInertia(cam_mass, camInertia);
	btRigidBody::btRigidBodyConstructionInfo m_camRigidBodyCI(cam_mass, camMotionState, m_cameraShape, camInertia);
	m_camRigidBodyCI.m_restitution = 0;
	this->m_camRigidBody = new btRigidBody(m_camRigidBodyCI);
	this->m_camRigidBody->setFriction(0.4f);
	this->m_camRigidBody->setRollingFriction(0.4f);
	this->m_dynamicsWorld->addRigidBody(this->m_camRigidBody);


}

void Physics::initTableBodies()
{
	srand(time(NULL));
	for (int i = 0; i < numTables; i++)
	{
		
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(x_vals[i], 0.5, z_vals[i]));

		btDefaultMotionState* fallMotionState =
			new btDefaultMotionState(transform);
		btScalar mass = 15;
		btVector3 fallInertia(0, 0, 0);
		m_tableShape->calculateLocalInertia(mass, fallInertia);
		btRigidBody::btRigidBodyConstructionInfo m_fallRigidBodyCI(mass, fallMotionState, m_tableShape, fallInertia);
		m_fallRigidBodyCI.m_restitution = 0;

		btRigidBody* m_rigidBody = new btRigidBody(m_fallRigidBodyCI);
		m_tables.push_back(m_rigidBody);
		m_tables[counter]->setFriction(0.4f);
		m_tables[counter]->setRollingFriction(0.4f);
		m_dynamicsWorld->addRigidBody(m_tables[counter]);


		counter++;
		
	}
}

