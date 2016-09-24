#ifndef MAIN_H
#define MAIN_H

#include "model.h"
#include <GLFW\glfw3.h>
#include <SFML\Audio.hpp>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void applyForce(glm::vec3 inCamPos);
void loadModels();
btVector3 glmToBT(glm::vec3 glmVec);
void createRandPositions();

extern Model* floor_model;
extern Model* table;
extern std::vector<Model*> tables;
extern int numTables;
extern std::vector<int> x_vals;
extern std::vector<int> z_vals;
extern Physics* physics;
extern sf::Sound jump;

#endif // !MAIN_H