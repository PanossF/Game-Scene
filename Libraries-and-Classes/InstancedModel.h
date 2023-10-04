#pragma once


#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <common/shader.h>
/* assimp include files */
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
// Include GLEW
#include <GL/glew.h>




struct InstancedMeshVertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 UV0;
	glm::vec2 UV1;
	glm::vec4 Colour;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
	glm::vec3 offset;
};

struct InstancedMeshTexture
{
	unsigned int id;
	std::string type;
	std::string path;
};

class InstancedMesh
{
protected:
	// store the mesh's position and rotation
	glm::quat rotation;
	glm::vec3 position;
	glm::vec3 scale;
	unsigned int VAO = 0;	// vertex array object ID
	unsigned int VBO = 0;	// vertex buffer object ID
	unsigned int EBO = 0;	// element buffer object ID
	int amountOfInstances = 0;

	void SetupMesh();
	void SetupInstances();

public: 
	std::vector<InstancedMeshVertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<InstancedMeshTexture> textures;


	InstancedMesh(std::vector<InstancedMeshVertex> verts, std::vector<unsigned int> inds, std::vector<InstancedMeshTexture> texs);
	void Draw(Shader& shadertouse);
	void Clear();
	void Init(std::vector<InstancedMeshVertex> verts, std::vector<unsigned int> inds, std::vector<InstancedMeshTexture> texs);
	

	glm::vec3 GetPosition() { return position; };
	void SetPosition(glm::vec3 newpos) { position = newpos; };

	glm::quat GetRotation() { return rotation; };
	void SetRotation(glm::quat newrot) { rotation = newrot; };

	glm::vec3 GetScale() { return scale; };
	void SetScale(glm::vec3 newscale) { scale = newscale; };

	void SetInstances(int instances) { amountOfInstances = instances; };
	// load a mesh from an assimp scene mesh
	bool InitFromSceneMesh(const aiMesh * inputmesh);
	
	// get the final model matrix for this mesh (we create it from the quaternion)
	glm::mat4x4 GetModelMatrix();
private:


};


class InstancedModel
{
private:
	std::string directory;
	std::vector<InstancedMesh> meshes;
	std::vector<InstancedMeshTexture> textures_loaded;
	bool bDumpDebugOnLoad = false;
public:
	// constructor, expects a filepath to a 3D model.
	InstancedModel(std::string const &path, bool bDebug = false)
	{
		bDumpDebugOnLoad = bDebug;
		LoadModelFromFile(path);
	}
	void LoadModelFromFile(std::string filename);
	void ProcessNode(aiNode * node, const aiScene *scene);
	void DebugWriteMesh(aiMesh * mesh, const aiScene * scene);
	InstancedMesh ProcessMesh(aiMesh * mesh, const aiScene *scene);
	std::vector<InstancedMeshTexture> loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName);
	void Render(glm::mat4x4 viewmat, glm::mat4x4 projmat, Shader & shader);
	void SetPosition(glm::vec3 newpos);
	void SetRotation(glm::quat newrotation);
	void SetScale(glm::vec3 newscale);
	void SetInstances(int instances);
	InstancedMesh * GetMesh(unsigned int index);
};

