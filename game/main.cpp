#define _USE_MATH_DEFINES
#define NUM_BUFFERS 3
#define BUFFER_SIZE 4096

#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL.h>

#include <btBulletDynamicsCommon.h>

#include "physics.h"
#include "shader.h"
#include "controls.hpp"
#include "model.h"
#include "main.h"
#include "game.h"

#include <SFML\Audio.hpp>


Game* game = new Game();
Physics* physics = new Physics();
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

Model* floor_model;
Model* table;
Model* table2;


std::vector<Model*> tables;
std::vector<int> x_vals;
std::vector<int> z_vals;
int numTables = 10;
bool pushed;

sf::Sound sound;
sf::Sound jump;

int main()
{
	game->initWindow();
	loadModels();
	createRandPositions();

	camera_pos.x = x_vals[numTables];
	camera_pos.z = z_vals[numTables];

	Shader myShader{ "vertex.vert", "frag.frag" };
	
	

	physics->initDynamicScene();

	glfwSetKeyCallback(game->getWindow(), key_callback);

	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile("kick.wav"))
		printf("unable to load wav");


	sound.setBuffer(buffer);

	sf::SoundBuffer jumpBuffer;
	jumpBuffer.loadFromFile("jump2.wav");
	jump.setBuffer(jumpBuffer);

	double initialTime = glfwGetTime();
	double secTimer = initialTime;
	int nbFrames = 0;

	while (!glfwWindowShouldClose(game->getWindow()))
	{
		// Measure speed
		double currentTime = glfwGetTime();
		double frameTime = currentTime - initialTime;
		initialTime = currentTime;
		nbFrames++;

		if ((currentTime - secTimer) >= 1.0)
		{
			printf("%f FPS\n", double(nbFrames));
			nbFrames = 0;
			secTimer += 1.0;
		}




		physics->m_dynamicsWorld->stepSimulation(frameTime, 10);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glfwPollEvents();

		

		//lightPos.x = lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;

		//Get View and Projection matrices
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix = getProjectionMatrix();
		glm::mat4 ViewMatrix = getViewMatrix();



		myShader.Use();  

		glUniformMatrix4fv(glGetUniformLocation(myShader.Program, "perspective"), 1, GL_FALSE, &ProjectionMatrix[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(myShader.Program, "view"), 1, GL_FALSE, &ViewMatrix[0][0]);

		// Set the lighting uniforms
		glUniform3f(glGetUniformLocation(myShader.Program, "viewPos"), camera_pos.x, camera_pos.y, camera_pos.z);
		// Point light 1
		glUniform3f(glGetUniformLocation(myShader.Program, "light.position"), 0.0f, 0.3f, 3.0f);
		glUniform3f(glGetUniformLocation(myShader.Program, "light.ambient"), 0.6f, 0.6f, 0.6f);
		glUniform3f(glGetUniformLocation(myShader.Program, "light.diffuse"), 1.0f, 1.0f, 1.0f);
		glUniform3f(glGetUniformLocation(myShader.Program, "light.specular"), 1.0f, 1.0f, 1.0f);
		glUniform1f(glGetUniformLocation(myShader.Program, "light.constant"), 1.0f);
		glUniform1f(glGetUniformLocation(myShader.Program, "light.linear"), (GLfloat)0.009);
		glUniform1f(glGetUniformLocation(myShader.Program, "light.quadratic"), (GLfloat)0.0032);
		
		


	



			//std::cout << "sphere height: " << ball_trans.getOrigin().getY() << std::endl;
		

		// Draw the loaded models
		//floor_model

		
		floor_model->updateModelMat();
		glUniformMatrix4fv(glGetUniformLocation(myShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(floor_model->model_mat));
		floor_model->Draw(myShader);

		
		/*
		if (glfwGetKey(game->getWindow(), GLFW_KEY_E) == GLFW_PRESS)
		{
			physics->m_tables[1]->activate(true);
			physics->m_tables[1]->applyForce(btVector3(0,100,50), btVector3(0, table->max_y - 0.2, 0));
		}
		if (glfwGetKey(game->getWindow(), GLFW_KEY_Q) == GLFW_PRESS)
		{
			trans.setIdentity();
			physics->m_tables[1]->clearForces();
			physics->m_tables[1]->setWorldTransform(trans);
		}
		*/

		
		applyForce(camDirection);
		//table	
		





		//printf("Pos2: %f, %f, %f\n", trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
	
		/*
		trans.getOpenGLMatrix(&tables[0]->model_mat[0][0]);
		tables[0]->model_mat = glm::translate(tables[0]->model_mat, glm::vec3(0, -tables[0]->model_center.y, 0));
		glUniformMatrix4fv(glGetUniformLocation(myShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(tables[0]->model_mat));
		tables[0]->Draw(myShader);
		*/



		for (int i = 0; i < numTables; i++)
		{
			btTransform transform;
			physics->m_tables[i]->getMotionState()->getWorldTransform(transform);




			//printf("Pos2: %f, %f, %f\n", trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());


			transform.getOpenGLMatrix(&tables[i]->model_mat[0][0]);
			tables[i]->model_mat = glm::translate(tables[i]->model_mat, glm::vec3(0, -tables[i]->model_center.y, 0));
			glUniformMatrix4fv(glGetUniformLocation(myShader.Program, "model"), 1, GL_FALSE, glm::value_ptr(tables[i]->model_mat));
			tables[i]->Draw(myShader);
		}



		glfwSwapBuffers(game->getWindow());
	}


	glfwTerminate();

	delete physics;
	delete floor_model;
	delete table;
	delete table2;


	

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}


void applyForce(glm::vec3 inCamDi)
{
	for (int i = 0; i < numTables; i++)
	{
		btTransform tempTrans;
		physics->m_tables[i]->getMotionState()->getWorldTransform(tempTrans);

		glm::vec3 tablePos = glm::vec3(tempTrans.getOrigin().getX(), tempTrans.getOrigin().getY(), tempTrans.getOrigin().getZ());
		//printf("%f, %f, %f\n", tablePos.x, tablePos.y, tablePos.z);
		glm::vec3 direction = tablePos - camera_pos;

		glm::vec2 ProjDir = glm::vec2(direction.x, direction.z);
		glm::vec2 ProjCamDir = glm::vec2(inCamDi.x, inCamDi.z);

		glm::vec2 normProjDir = glm::normalize(ProjDir);
		glm::vec2 normProjCamDir = glm::normalize(ProjCamDir);

		//printf("%f, %f\n", normProjDir.x, normProjDir.y);


		float dot = glm::dot(normProjCamDir, normProjDir);
		float angle = glm::acos(dot);


		glm::vec3 normalizedCamDir3 = glm::normalize(inCamDi);

		//printf("%f\n", angle);

		if (glfwGetKey(game->getWindow(), GLFW_KEY_E) == GLFW_PRESS && angle < M_PI / 4 && glm::length(ProjDir) < 3.5 && !pushed)
		{
			pushed = true;

			srand(time(NULL));
			int randForceStength = rand() % 10 + 1;

			printf("Strength: %i\n", randForceStength);

			sound.play();

			physics->m_tables[i]->activate(true);
			physics->m_tables[i]->applyForce(btVector3(randForceStength * 500 * normalizedCamDir3.x, 3 * randForceStength * 500, randForceStength * 500  * normalizedCamDir3.z), btVector3(0, table->max_y - 0.2, 0));
		}
		else if (glfwGetKey(game->getWindow(), GLFW_KEY_E) == GLFW_RELEASE)
		{
			pushed = false;
		}
	}
}

void loadModels()
{
	floor_model = new Model("./resources/floor.obj");

	float scaleX = 3.0f, scaleY = 1.0f, scaleZ = 3.0f;
	floor_model->scale = glm::vec3(scaleX, scaleY, scaleZ);
	floor_model->orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	floor_model->position = glm::vec3(0, 0, 0);
	floor_model->max_x *= scaleX;
	floor_model->max_y *= scaleY;
	floor_model->max_z *= scaleZ;

	/*
	printf("floor_model height: %f\n", floor_model->max_y);
	printf("floor_model width: %f\n", floor_model->max_x);
	printf("floor_model width z: %f\n", floor_model->max_z);
	*/



	table = new Model("./resources/table.obj");
	table->scale = glm::vec3(1.0f, 1.0f, 1.0f);
	table->orientation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	table->position = glm::vec3(0, 0, 0);

	for (int i = 0; i < numTables; i++)
	{
		tables.push_back(table);
	}

	/*
	printf("Table height: %f\n", table->max_y);
	printf("Table width: %f\n", table->max_x);
	printf("Table width z: %f\n", table->max_z);

	printf("Table center: %f,%f,%f. max x: %f\n", table->model_center.x, table->model_center.y, table->model_center.z, table->max_x);

	printf("Mesh 0 min: %f, %f, %f\n", table->meshes[0].meshMin.x, table->meshes[0].meshMin.y, table->meshes[0].meshMin.z);
	printf("Mesh 0 max: %f, %f, %f\n", table->meshes[0].meshMax.x, table->meshes[0].meshMax.y, table->meshes[0].meshMax.z);

	printf("Mesh 1 min: %f, %f, %f\n", table->meshes[1].meshMin.x, table->meshes[1].meshMin.y, table->meshes[1].meshMin.z);
	printf("Mesh 1 max: %f, %f, %f\n", table->meshes[1].meshMax.x, table->meshes[1].meshMax.y, table->meshes[1].meshMax.z);
	*/
}

void createRandPositions()
{
	srand(time(NULL));

	for (int i = 0; i < numTables + 1; i++)
	{
		bool check;
		int _x;


		do
		{
			int randomNum = rand() % 2;
			int sign = randomNum == 1 ? 1 : -1;


			_x = rand() % 7 * 2 * sign;
			check = true;
			for (int j = 0; j < i; j++)
			{
				if (_x == x_vals[j])
				{
					check = false;
					break;
				}
			}
		} while (!check);

		x_vals.push_back(_x);

	}

	for (int i = 0; i < numTables + 1; i++)
	{
		bool check;
		int _z;

		do
		{
			int randomNum = rand() % 2;
			int sign = randomNum == 1 ? 1 : -1;

			_z = rand() % 7 * 2 * sign;
			check = true;
			for (int j = 0; j < i; j++)
			{
				if (_z == z_vals[j])
				{
					check = false;
					break;
				}
			}
		} while (!check);

		z_vals.push_back(_z);

	}

}