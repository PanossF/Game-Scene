#pragma once
#include "shader.h"
#include <GL/glew.h> // for GLuint etc..

class Skybox
{
public:
	GLuint cubemaptextureID = 0;
	GLuint vao = 0;			// vertex array object id
	GLuint vbo = 0;			// vertex buffer object id

	Skybox(std::string path, std::string xposimg,std::string xnegimg,std::string yposimg,std::string ynegimg,std::string zposimg,	std::string znegimg );
	~Skybox();

	void Init();
	void Draw( glm::mat4x4 View, glm::mat4x4 Projection, Shader& shader );
};

