#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "model.h"
#include "assimp/Importer.hpp"	// need the importer class from assimp so we can load the meshes from the scene!!
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string.h> // 
#include "SOIL2.h" // for texture loading!
#include "GLError.h"
using namespace std;


Mesh::Mesh(std::vector<MeshVertex> verts, std::vector<unsigned int> inds, std::vector<MeshTexture> texs)
{
	vertices = verts;
	indices = inds;
	textures = texs;
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	scale = glm::vec3(1.0f, 1.0f, 1.0f);
	SetupMesh();
}

void Mesh::Init(std::vector<MeshVertex> verts, std::vector<unsigned int> inds, std::vector<MeshTexture> texs)
{
	vertices = verts;
	indices = inds;
	textures = texs;
}


void Mesh::Draw(Shader & shader)
{
	// setup texture samplers... 
	// bind appropriate textures
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;
	unsigned int aoNr = 1;
	unsigned int emissiveNr = 1;

	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
										  // retrieve texture number (the N in diffuse_textureN)
		string number;
		string name = textures[i].type;
		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++); // transfer unsigned int to stream
		else if (name == "texture_normal")
			number = std::to_string(normalNr++); // transfer unsigned int to stream
		else if (name == "texture_height")
			number = std::to_string(heightNr++); // transfer unsigned int to stream
		else if (name == "texture_ao")
			number = std::to_string(aoNr++); // transfer unsigned int to stream
		else if (name == "texture_emissive")
			number = std::to_string(emissiveNr++); // transfer unsigned int to stream
		// now set the sampler to the correct texture unit
		std::string texturename = name + number;
		glUniform1i(glGetUniformLocation(shader.ID, (texturename).c_str()), i);
		//check_gl_error();

		// and finally bind the texture
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
		//check_gl_error();
	}

	// draw the mesh
	glBindVertexArray(VAO);
	//check_gl_error();

	glDrawElements(GL_TRIANGLES,(GLsizei)indices.size(), GL_UNSIGNED_INT, (void *)0);
	//check_gl_error();

	glBindVertexArray(0);
	//check_gl_error();
	
	// reset all texture samplers we've used to 0
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i); // activate the texture unit
		// bind texture unit back to 0
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	// reset to texture sample 0
	glActiveTexture(GL_TEXTURE0);
	//check_gl_error();
}

void Mesh::Clear()
{
}


bool Mesh::InitFromSceneMesh(const aiMesh * inputmesh)
{
	return false;
}

glm::mat4x4 Mesh::GetModelMatrix()
{
	// get the 4x4 matrix by extracting it from quaternion and then applying the translation and scale to it
	glm::mat4x4 mat;
	mat = glm::toMat4(rotation);
	mat = glm::translate(mat, position);
	mat = glm::scale(mat, scale);
	return mat;
}

void Mesh::SetupMesh()
{
	
	// create a vertex array object ID for this mesh
	//glewIsSupported("GL_ARB_Vertex_Array")
	glGenVertexArrays(1, &VAO);
	check_gl_error();
	
	// and create a vertex buffer object ID
	glGenBuffers(1, &VBO);
	check_gl_error();
	
	// and create an element (index) buffer object ID
	glGenBuffers(1, &EBO);
	check_gl_error();
	
	// make the VAO the current one (remember OGL is a state machine so any new buffers that are bound are bound to this VAO)
	glBindVertexArray(VAO);
	check_gl_error();

	// bind the VBO buffer so we can add data to it
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	check_gl_error();

	// and add the data
	unsigned int size = (unsigned int)vertices.size() * sizeof(MeshVertex);
	glBufferData(GL_ARRAY_BUFFER, (GLsizei)vertices.size() * sizeof(MeshVertex), &vertices[0], GL_STATIC_DRAW);
	check_gl_error();

	// bind the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	check_gl_error();
	// now add the data to it
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,(GLsizei)indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
	check_gl_error();

	// now we have to setup where the offsets into the vertex are for each attribute (position, normal, uv0, uv1 and colour)
	// attribute 0 - position!
	glEnableVertexAttribArray(0);
	check_gl_error();
	// setup the attrib pointer to start at 0 and end at 3 (i.e. three floats for position vector)
	// note that we use sizeof(MeshVertex) as the "stride" so it knows how to compute the offsets for each element (last element is 0)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)0);
	check_gl_error();

	// attribute 1 - Normal!
	glEnableVertexAttribArray(1);
	check_gl_error();
	// setup the attrib pointer to start at 3 end at correct offset stride is still size off the whole struct
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, Normal));
	check_gl_error();

	// attribute 2 - UV0!
	glEnableVertexAttribArray(2);
	check_gl_error();
	// setup the attrib pointer to start at correct offset stride is still size off the whole struct
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, UV0));
	check_gl_error();

	// attribute 3 - UV1!
	glEnableVertexAttribArray(3);
	check_gl_error();
	// setup the attrib pointer to start at correct offset stride is still size off the whole struct
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, UV1));
	check_gl_error();

	// attribute 4 - Colour!
	glEnableVertexAttribArray(4);
	check_gl_error();
	// setup the attrib pointer to start at correct offset stride is still size off the whole struct
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, Colour));
	check_gl_error();

	// attribute 5 - Tangent
	glEnableVertexAttribArray(5);
	check_gl_error();
	// setup the attrib pointer to start at correct offset stride is still size off the whole struct
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, Tangent));
	check_gl_error();

	// attribute 6 - Bitangent
	glEnableVertexAttribArray(6);
	check_gl_error();
	// setup the attrib pointer to start at correct offset stride is still size off the whole struct
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(MeshVertex), (void*)offsetof(MeshVertex, Bitangent));
	check_gl_error();

	// finally, bind vertex array 0 so that we know the state is reset
	glBindVertexArray(0);
	check_gl_error();
}


// Should be reasonably understandable what this does

void Model::LoadModelFromFile(std::string filename)
{
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace );

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
		return;
	}
	directory = filename.substr(0, filename.find_last_of('/'));

	ProcessNode(scene->mRootNode, scene);

}

// processnode calls processmesh on each mesh in this aiScene node, then recurses on down into leaves
void Model::ProcessNode(aiNode * node, const aiScene * scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
		// dump load info to console so we can debug
		if(bDumpDebugOnLoad)
			DebugWriteMesh(mesh, scene);
		meshes.push_back(ProcessMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

// dump the mesh info to console log so we can determine if it loaded correctly
void Model::DebugWriteMesh(aiMesh * mesh, const aiScene * scene)
{

	std::cout << "Mesh: " << mesh->mName.C_Str() << " Num Verts: " << mesh->mNumVertices << " Num Faces: " << mesh->mNumFaces << std::endl;

	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		std::cout << "Face: " << i << ": ";
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			std::cout << face.mIndices[j] << " ";
		std::cout << std::endl;
	}

	// write Position
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		std::cout << "V: " << i << ": " << mesh->mVertices[i].x << " " << mesh->mVertices[i].y << " " << mesh->mVertices[i].z << std::endl;
	}
	// write normal
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		std::cout << "N: " << i << ": " << mesh->mNormals[i].x << " " << mesh->mNormals[i].y << " " << mesh->mNormals[i].z << std::endl;
	}
	if (mesh->mTextureCoords[0])
	{
	// write UV coordinate set 1
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			std::cout << "UV1: " << i << ": " << mesh->mTextureCoords[0][i].x << " " << mesh->mTextureCoords[0][i].y << std::endl;
		}
	}
	else
	{
		std::cout << "No UV Coordinates for Set 1" << std::endl;
	}
	if (mesh->mTextureCoords[1])
	{
	// write UV coordinate set 2
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			std::cout << "UV2: " << i << ": " << mesh->mTextureCoords[1][i].x << " " << mesh->mTextureCoords[1][i].y << std::endl;
		}
	}
	else
	{
		std::cout << "No UV Coordinates for Set 2" << std::endl;
	}
	if (mesh->mColors[0])
	{
		// write Vertex Colour
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			std::cout << "VC: " << i << ": " << mesh->mColors[0][i].r << " " << mesh->mColors[0][i].g << " " << mesh->mColors[0][i].b << " " << mesh->mColors[0][i].a << std::endl;
		}

	}
	else
	{
		std::cout << "No Vertex Colours defined" << std::endl;
	}


}


// this is the function that actually retrieves the data from the ASSIMP aiScene/aiMesh nodes and stores it in mesh object as vertex data
Mesh Model::ProcessMesh(aiMesh * mesh, const aiScene * scene)
{
	vector<MeshVertex> vertices;
	vector<unsigned int> indices;
	vector<MeshTexture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		MeshVertex vertex;
		// process vertex positions, normals and texture coordinates

		// POSITION
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;
		// NORMAL
		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;
		// UV coordinate set 1
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.UV0 = vec;
		}
		else
			vertex.UV0 = glm::vec2(0.0f, 0.0f);

		// UV coordinate set 2
		if (mesh->mTextureCoords[1]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[1][i].x;
			vec.y = mesh->mTextureCoords[1][i].y;
			vertex.UV1 = vec;
		}
		else
			vertex.UV1 = glm::vec2(0.0f, 0.0f);

		// try to load colours.. if no vertex colour, then try and simply get it from material (this seems to fail a fair bit)
		if (mesh->mColors[0])
		{
			glm::vec4 col;
			col.x = mesh->mColors[0][i].r;
			col.y = mesh->mColors[0][i].g;
			col.z = mesh->mColors[0][i].b;
			col.w = mesh->mColors[0][i].a;
			vertex.Colour = col;
		}
		else if (mesh->mMaterialIndex >= 0)
		{
			// no vertex colour, so set each vertex to the colour of the material
			aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
			if (material)
			{
				aiColor3D color(1.f, 0.f, 0.f);
				material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
				glm::vec4 col;
				col.r = color.r;
				col.g = color.g;
				col.b = color.b;
				col.a = 1.0f;
				vertex.Colour = col;
			}
		}
		else
		{
			glm::vec4 col;
			col.r = 1.0f;
			col.g = 1.0f;
			col.b = 1.0f;
			col.a = 1.0f;
			vertex.Colour = col;
		}
		// according to the docs, if the mesh contains tangents, then it automatically contains bitangents.
		if (mesh->mTangents)
		{
			glm::vec3 tan(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
			vertex.Tangent = tan;
			glm::vec3 btan(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
			vertex.Bitangent = btan;
		}

		// add the vertex into the vertex array
		vertices.push_back(vertex);
	}

	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	
	// process materials (basically load textures associated with materials)
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
		// diffuse (aka albedo)
		std::vector<MeshTexture> diffuseMaps = loadMaterialTextures(material,aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// emissive maps (i.e. object glow)
		std::vector<MeshTexture> emissiveMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emissive");
		textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());
		// specular color or intensity??
		std::vector<MeshTexture> specularMaps = loadMaterialTextures(material,aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		// use ambient to mean AO maps
		std::vector<MeshTexture> aoMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ao");
		textures.insert(textures.end(), aoMaps.begin(), aoMaps.end());
		// normal maps!
		std::vector<MeshTexture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		// bump maps loaded as normals
		std::vector<MeshTexture> bnormalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		textures.insert(textures.end(), bnormalMaps.begin(), bnormalMaps.end());

	}

	return Mesh(vertices, indices, textures);
}

std::vector<MeshTexture> Model::loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName)
{
	std::vector<MeshTexture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			MeshTexture texture;
			std::string filename = std::string(str.C_Str());
			if(directory.length() > 0)
				filename = directory + '/' + filename;
			texture.id = -1;
			texture.id = SOIL_load_OGL_texture(filename.c_str(), SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_TEXTURE_REPEATS); //TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture); // add to loaded textures
		}
	}
	return textures;
}

// sets model, view, projection matrices into the shader uniforms and then calls draw on each child mesh
// which means that all meshes have to be rendered with the same shader! maybe this is a bad idea?
void Model::Render(glm::mat4x4 viewmat, glm::mat4x4 projmat, Shader & shader)
{
	shader.setMat4("V", viewmat);
	shader.setMat4("P", projmat);

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		glm::mat4 modelmat = meshes[i].GetModelMatrix();
		glm::mat4x4 mvp = projmat * viewmat * modelmat;
		// set up the modelviewprojection matrix for rendering this mesh...
		shader.setMat4("MVP", mvp);
		shader.setMat4("M", modelmat);
		// setup the upper left of the view * model (so we don't have translation)
		glm::mat4 mv = viewmat * modelmat;
		glm::mat3 mv3x3 = glm::mat3(mv);
		shader.setMat3("MVR", mv3x3);
		// draw the mesh
		meshes[i].Draw(shader);
	}
		
}

void Model::SetPosition(glm::vec3 newpos)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].SetPosition(newpos);
	}
}

void Model::SetRotation(glm::quat newrotation)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].SetRotation(newrotation);
	}
}

void Model::SetScale(glm::vec3 newscale)
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].SetScale(newscale);
	}
}

Mesh * Model::GetMesh(unsigned int index)
{
	if(meshes.size() == 0 || index > meshes.size())
		return nullptr;
	return &meshes[index];
}

