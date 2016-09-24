#include <stdio.h>



// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"
#include "physics.h"
#include "main.h"
#include "game.h"

#include <GLFW/glfw3.h>
extern Game* game; // The "extern" keyword here is to access the variable "game->getWindow()" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}


// Initial camera_pos : on +Z
//glm::vec3 camera_pos = glm::vec3( x_vals[numTables], 2, y_vals[numTables] );
glm::vec3 camera_pos;
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 5.0f; // 5 units / second
float mouseSpeed = 0.005f;

bool jumped;

btTransform cameraTrans;
glm::vec3 camDirection;

btVector3 glmToBT(glm::vec3 glmVec)
{
	return btVector3(glmVec.x, glmVec.y, glmVec.z);
}

void computeMatricesFromInputs(){

	cameraTrans = physics->m_camRigidBody->getWorldTransform();

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse camera_pos
	double xpos, ypos;
	glfwGetCursorPos(game->getWindow(), &xpos, &ypos);

	// Reset mouse camera_pos for next frame
	glfwSetCursorPos(game->getWindow(), 1024/2, 768/2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(1024/2 - xpos );
	verticalAngle   += mouseSpeed * float( 768/2 - ypos );

	if (verticalAngle > 3.15f / 2) {
		verticalAngle = 3.15 / 2;
	}
	if (verticalAngle < -3.15 / 2) {
		verticalAngle = -3.15 / 2;
	}

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	camDirection = glm::vec3(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, camDirection );

	glm::vec3 forward = glm::vec3(camDirection.x, 0, camDirection.z);
	forward = normalize(forward);
	//printf("%f, %f, %f\n", forward.x, forward.y, forward.z);

	// Move forward
	if (glfwGetKey(game->getWindow(), GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		jumped = false;
	}
	if (glfwGetKey(game->getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
		camera_pos += forward * deltaTime * speed;
		//physics->m_camRigidBody->setCenterOfMassTransform(cameraTrans);
	}
	// Move backward
	if (glfwGetKey(game->getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
		camera_pos -= forward * deltaTime * speed;
		//physics->m_camRigidBody->setCenterOfMassTransform(cameraTrans);
	}
	// Strafe right
	if (glfwGetKey(game->getWindow(), GLFW_KEY_D) == GLFW_PRESS) {
		camera_pos += right * deltaTime * speed;
		//physics->m_camRigidBody->setCenterOfMassTransform(cameraTrans);
	}
	// Strafe left
	if (glfwGetKey(game->getWindow(), GLFW_KEY_A) == GLFW_PRESS) {

		camera_pos -= right * deltaTime * speed;
		//physics->m_camRigidBody->setCenterOfMassTransform(cameraTrans);
	} 

	
	if (glfwGetKey(game->getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS && !jumped) {
		jumped = true;

		jump.play();
		physics->m_camRigidBody->activate(true);
		physics->m_camRigidBody->applyCentralImpulse(btVector3(0, 5, 0));
	}
	

	if (glfwGetKey(game->getWindow(), GLFW_KEY_Q) == GLFW_PRESS) {
		printf("%f %f %f", camera_pos.x, camera_pos.y, camera_pos.z);
	}
	if (glfwGetKey(game->getWindow(), GLFW_KEY_E) == GLFW_PRESS) {
		//system("cls");
	}

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(FoV, 16.0f / 9.0f, 0.1f, 100.0f);
	// Camera matrix

	// Update camera rigid body position and y value of camera position
	//printf("x: %f, z: %f\n", camera_pos.x, camera_pos.z);
	cameraTrans.setOrigin(btVector3(camera_pos.x, cameraTrans.getOrigin().getY(), camera_pos.z));
	camera_pos.y = cameraTrans.getOrigin().getY();
	physics->m_camRigidBody->setWorldTransform(cameraTrans);



	(camera_pos.x > 15.0f) ? camera_pos.x = 15.0f : (void(0));
	(camera_pos.x < -15.0f) ? camera_pos.x = -15.0f : (void(0));

	(camera_pos.z > 15.0f) ? camera_pos.z = 15.0f : (void(0));
	(camera_pos.z < -15.0f) ? camera_pos.z = -15.0f : (void(0));


	

	
	ViewMatrix       = glm::lookAt(
								camera_pos,           // Camera is here
								camera_pos+camDirection, // and looks here : at the same camera_pos, plus "camDirection"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
	//printf("%f %f %f\n", camDirection.x, camDirection.y, camDirection.z);

	btTransform test;

	physics->m_camRigidBody->getMotionState()->getWorldTransform(test);
	//printf("%f, %f, %f\n", test.getOrigin().getX(), test.getOrigin().getY(), test.getOrigin().getZ());
}