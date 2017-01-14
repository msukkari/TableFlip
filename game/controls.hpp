#ifndef CONTROLS_HPP
#define CONTROLS_HPP

void computeMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();

extern glm::vec3 camDirection;
extern glm::vec3 camera_pos;
#endif