#include "Skybox.h"
#include "SOIL2.h"
#include "GLError.h"


//#define _QUADS_

Skybox::Skybox(std::string path, std::string xp, std::string xn, std::string yp, std::string yn, std::string zp, std::string zn)
{
	// create cubemap texture using SOIL NOTE: We don't want mipmaps as the sky is tied to the camera hence no minification
	cubemaptextureID = SOIL_load_OGL_cubemap
	(
		(path+xp).c_str(),	(path+xn).c_str(),	(path+yp).c_str(),	(path+yn).c_str(),	(path+zp).c_str(),	(path+zn).c_str(),
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID,
		0
	);
	// initialize vertex array object and buffers
	Init();
}

Skybox::~Skybox()
{
}

void Skybox::Init()
{
	float vertices[] = {
		//              vertex1         vertex2         vertex3         vertex4
		/* xpos */		+1, -1, +1,		+1, +1, +1,		+1, +1, -1,		+1, -1, -1,
		/* xneg */		-1, -1, -1,		-1, +1, -1,		-1, +1, +1,		-1, -1, +1,
		/* ypos */		-1, +1, -1,		+1, +1, -1,		+1, +1, +1,		-1, +1, +1,
		/* yneg */		-1, -1, +1,		+1, -1, +1,		+1, -1, -1,		-1, -1, -1,
		/* zpos */		+1, -1, +1,		-1, -1, +1,		-1, +1, +1,		+1, +1, +1,
		/* zneg */		-1, -1, -1,		+1, -1, -1,		+1, +1, -1,		-1, +1, -1,
	};

	float points[] = {
		-10.0f,  10.0f, -10.0f,
		-10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,
		10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,

		-10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f, -10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,

		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,

		-10.0f, -10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f, -10.0f,  10.0f,
		-10.0f, -10.0f,  10.0f,

		-10.0f,  10.0f, -10.0f,
		10.0f,  10.0f, -10.0f,
		10.0f,  10.0f,  10.0f,
		10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f,  10.0f,
		-10.0f,  10.0f, -10.0f,

		-10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		10.0f, -10.0f, -10.0f,
		10.0f, -10.0f, -10.0f,
		-10.0f, -10.0f,  10.0f,
		10.0f, -10.0f,  10.0f
	};


	// vertexarray
	glGenVertexArrays(1, &vao);
	check_gl_error();

	glBindVertexArray(vao);
	check_gl_error();

	glGenBuffers(1, &vbo);
	check_gl_error();

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	check_gl_error();

#ifdef _QUADS_
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
#else
	glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(float), points, GL_STATIC_DRAW);
#endif
	check_gl_error();

#ifdef _QUADS_
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)(sizeof(float) * 0));
#else
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0,(void *) 0);
#endif
	check_gl_error();

	glEnableVertexAttribArray(0);
	check_gl_error();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	check_gl_error();

	glBindVertexArray(0);
	check_gl_error();
}

void Skybox::Draw(glm::mat4x4 View, glm::mat4x4 Projection, Shader& shader)
{
	// seamless cubemap
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	//check_gl_error();
	// skybox will be drawn first, so disallow writing into depth buffer
	glDepthMask(GL_FALSE);
	//check_gl_error();
	// and don't depth test
	//glDepthFunc(GL_ALWAYS);
 
	shader.use();
	//check_gl_error();

	glm::mat4x4 Model;

	// create a view matrix that doesn't have translation by getting the 3x3 from the view and sticking it into a 4x4
	glm::mat3x3 rotview = glm::mat3(View);
	glm::mat4x4 rotmat = glm::mat4(rotview);


	shader.setMat4("MVP", Projection * rotmat);
	shader.setMat4("P", Projection);


	shader.setMat4("V", rotmat);
 
	shader.setInt("skybox", 0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemaptextureID);
 
	glBindVertexArray(vao);
	//check_gl_error();
#ifdef _QUADS_
	glDrawArrays(GL_QUADS, 0, 24);
#else
	glDrawArrays(GL_TRIANGLES, 0, 36);
#endif

	glBindVertexArray(0);
	//check_gl_error();
	// unbind our cubemap texture
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	//check_gl_error();
 
	glUseProgram(0);
	//check_gl_error();
 
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
}
