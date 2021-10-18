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




struct MeshVertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 UV0;
	glm::vec2 UV1;
	glm::vec4 Colour;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

struct MeshTexture
{
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
protected:
	// store the mesh's position and rotation
	glm::quat rotation;
	glm::vec3 position;
	glm::vec3 scale;
	unsigned int VAO = 0;	// vertex array object ID
	unsigned int VBO = 0;	// vertex buffer object ID
	unsigned int EBO = 0;	// element buffer object ID

	void SetupMesh();
public: 
	std::vector<MeshVertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<MeshTexture> textures;

	Mesh(std::vector<MeshVertex> verts, std::vector<unsigned int> inds, std::vector<MeshTexture> texs);
	void Draw(Shader& shadertouse);
	void Clear();
	void Init(std::vector<MeshVertex> verts, std::vector<unsigned int> inds, std::vector<MeshTexture> texs);
	

	glm::vec3 GetPosition() { return position; };
	void SetPosition(glm::vec3 newpos) { position = newpos; };

	glm::quat GetRotation() { return rotation; };
	void SetRotation(glm::quat newrot) { rotation = newrot; };

	glm::vec3 GetScale() { return scale; };
	void SetScale(glm::vec3 newscale) { scale = newscale; };

	// load a mesh from an assimp scene mesh
	bool InitFromSceneMesh(const aiMesh * inputmesh);
	
	// get the final model matrix for this mesh (we create it from the quaternion)
	glm::mat4x4 GetModelMatrix();
private:


};


class Model
{
private:
	std::string directory;
	std::vector<Mesh> meshes;
	std::vector<MeshTexture> textures_loaded;
	bool bDumpDebugOnLoad = false;
public:
	// constructor, expects a filepath to a 3D model.
	Model(std::string const &path, bool bDebug = false)
	{
		bDumpDebugOnLoad = bDebug;
		LoadModelFromFile(path);
	}
	void LoadModelFromFile(std::string filename);
	void ProcessNode(aiNode * node, const aiScene *scene);
	void DebugWriteMesh(aiMesh * mesh, const aiScene * scene);
	Mesh ProcessMesh(aiMesh * mesh, const aiScene *scene);
	std::vector<MeshTexture> loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName);
	void Render(glm::mat4x4 viewmat, glm::mat4x4 projmat, Shader & shader);
	void SetPosition(glm::vec3 newpos);
	void SetRotation(glm::quat newrotation);
	void SetScale(glm::vec3 newscale);
	Mesh * GetMesh(unsigned int index);
};

