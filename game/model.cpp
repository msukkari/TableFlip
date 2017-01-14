#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <SOIL.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

#include "shader.h"
#include "mesh.h"
#include "model.h"

GLint TextureFromFile(const char* path, string directory)
{
	//Generate texture ID and load texture data 
	string filename = string(path);
	filename = directory + '/' + filename;
	GLuint textureID;
	glGenTextures(1, &textureID);
	printf("Loading Texture: %s\n", filename.c_str());

	int width, height;
	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}



void Model::updateModelCenter()
{
	this->model_center.x = (this->max_x + this->min_x) / 2;
	this->model_center.y = (this->max_y + this->min_y) / 2;
	this->model_center.z = (this->max_z + this->min_z) / 2;
}

void Model::calcMaxMin()
{
	int numMeshes = this->meshes.size();

	for (int i = 0; i < numMeshes; i++)
	{
		if (this->meshes[i].meshMax.x > this->max_x)
			this->max_x = this->meshes[i].meshMax.x;
		else if (this->meshes[i].meshMin.x < this->min_x)
			this->min_x = this->meshes[i].meshMin.x;

		if (this->meshes[i].meshMax.y > this->max_y)
			this->max_y = this->meshes[i].meshMax.y;
		else if (this->meshes[i].meshMin.y < this->min_y)
			this->min_y = this->meshes[i].meshMin.y;

		if (this->meshes[i].meshMax.z > this->max_z)
			this->max_z = this->meshes[i].meshMax.z;
		else if (this->meshes[i].meshMin.z < this->min_z)
			this->min_z = this->meshes[i].meshMin.z;
	}
}

void Model::updateModelMat()
{
	glm::mat4 Translation = glm::translate(glm::mat4(1.0f), this->position);
	glm::mat4 Scale = glm::scale(glm::mat4(1.0f), this->scale);
	glm::mat4 Rotation = glm::toMat4(this->orientation);
	this->model_mat = Translation * Rotation * Scale;
}

Model::Model(GLchar* path)
{
	this->loadModel(path);
	this->calcMaxMin();
	this->updateModelCenter();
}

void Model::Draw(Shader shader)
{
	for (GLuint i = 0; i < this->meshes.size(); i++)
		this->meshes[i].Draw(shader);
}

void Model::loadModel(string path)
{
	// Read file using ASSIMP and place it in a aiScene object
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	// Check for errors
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
		return;
	}
	// Retrieve the directory path of the filepath
	this->directory = path.substr(0, path.find_last_of('/')); //Takes the sub-string up until the final '/', so ./res/objectfiles/ball.obj => ./res/objectfiles

	// Recursively process each node starting from the root node
	//printf("%u\n", scene->mRootNode->mNumChildren);
	this->processNode(scene->mRootNode, scene);

}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	//printf("Number of meshes: %u\n", node->mNumMeshes);
	// Loop through all the meshes in the nood and process them
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		// The node object only contains indices to index the actual objects in the scene. 
		// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->processMesh(mesh, scene));
	}
	// After meshes have been processed, recursively process each of the children nodes
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}

}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	// Data to fill
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	glm::vec3 max = glm::vec3(0.0f);
	glm::vec3 min = glm::vec3(0.0f);
	glm::vec3 center = glm::vec3(0.0f);
	
	// Extract all position/normals/texCoords from the aiMesh and push it back onto vertex vector
	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		glm::vec3 vector; // Declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so transfer the data to this placeholder glm::vec3 first.
	    //Positions
		vector.x = mesh->mVertices[i].x;
		if (vector.x > max.x)
			max.x = vector.x;
		else if (vector.x < min.x)
			min.x = vector.x;


		vector.y = mesh->mVertices[i].y;
		if (vector.y > max.y)
			max.y = vector.y;
		else if (vector.y < min.y)
			min.y = vector.y;

		vector.z = mesh->mVertices[i].z;
		if (vector.z > max.z)
			max.z = vector.z;
		else if (vector.z < min.z)
			min.z = vector.z;

		vertex.Position = vector;
		// Normals
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;
		// Texture Coordinates
		if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			//Assume the texture only has one set of texture coords, so take the first one
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	//Extract all of the mesh's faces and its indices
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// Retrieve all indices of the face and store them in the indices vector
		for (GLuint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	
	// Process materials
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		//printf("%u\n", material->GetTextureCount(aiTextureType_DIFFUSE));

		//Texture convention taken from learnOpenGL.com:
		// We assume a convention for sampler names in the shaders. Each diffuse texture should be named
		// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
		// Same applies to other texture as the following list summarizes:
		// Diffuse: texture_diffuseN
		// Specular: texture_specularN
		// Normal: texture_normalN

		// 1. Diffuse maps
		vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// 2. Specular maps
		vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	// Calculate the center of the mesh
	float x = (max.x + min.x) / 2;
	float y = (max.y + min.y) / 2;
	float z = (max.z + min.z) / 2;

	center = glm::vec3(x, y, z);


	// Return a mesh object created from the extracted mesh data
	return Mesh(vertices, indices, textures, min, max, center);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		GLboolean skip = false;
		for (GLuint j = 0; j < textures_loaded.size(); j++)
		{
			if (textures_loaded[j].path == str)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // If texture hasn't been loaded already, load it
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str;
			textures.push_back(texture);
			this->textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}