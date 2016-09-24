#ifndef MODEL_H
#define MODEL_H

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "mesh.h"
#include <glm\glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
GLint TextureFromFile(const char* path, string directory);

class Model
{
public:

	glm::quat orientation;
	glm::vec3 position;
	glm::vec3 scale;

	float max_x = 0, min_x = 0;
	float max_y = 0, min_y = 0;
	float max_z = 0, min_z = 0;

	glm::vec3 model_center;

	/*  Functions   */
	// Constructor, expects a filepath to a 3D model.
	Model(GLchar* path);

	glm::mat4 model_mat;

	// Draws the model, and thus all its meshes
	void Draw(Shader shader);

	void updateModelMat();

	void updateModelCenter();

	void calcMaxMin();

	vector<Mesh> meshes;

	

private:
	//vector<Mesh> meshes;
	/*  Model Data  */
	
	string directory;


	vector<Texture> textures_loaded;	// Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.

										/*  Functions   */
										// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
	void loadModel(string path);

	// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

	// Checks all material textures of a given type and loads the textures if they're not loaded yet.
	// The required info is returned as a Texture struct.
	vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);

};




GLint TextureFromFile(const char* path, string directory);

#endif 