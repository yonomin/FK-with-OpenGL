#ifndef INDICATOR_H
#define INDICATOR_H
#include <glad/glad.h> 

#include <stb_image.h>
#include <iostream>
#include <glm/glm.hpp>


inline const float quadrangle[] = {
	// Pos       //Tex
	-0.5f, 0.5f, 0.0f, 1.0f, // Top-Left:  (0, 1)
	 0.5f, 0.5f, 1.0f, 1.0f, // Top-Right: (1, 1)
	-0.5f,-0.5f, 0.0f, 0.0f, // Bottom-Left: (0, 0)
	 0.5f,-0.5f, 1.0f, 0.0f  // Bottom-Right: (1, 0)
};
inline const unsigned int quadIndices[] = {
	0, 1, 2,
	1, 3, 2
};

class Indicator 
{
//变量== == == == == == == == == == == == == == 
public:
	GLuint quadVAO;
	unsigned int texUnselected = 0;
	unsigned int texSelected = 0;
private:
	int width = 0;
	int height = 0;
	int nrChannels = 0;
//函数== == == == == == == == == == == == == == 
public:
	Indicator()
	{
		GLuint quadVBO;
		glGenBuffers(1, &quadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadrangle), quadrangle, GL_STATIC_DRAW);
		//VAO
		glGenVertexArrays(1, &quadVAO);
		glBindVertexArray(quadVAO);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
		glEnableVertexAttribArray(2);
		GLuint quadEBO;
		glGenBuffers(1, &quadEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
		//解绑
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void loadTextureUnselected(const char *path)
	{
		unsigned char *texUnselectedData = stbi_load(path, &width, &height, &nrChannels, 0);
		glGenTextures(1, &texUnselected);
		glBindTexture(GL_TEXTURE_2D, texUnselected);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texUnselectedData);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(texUnselectedData);

	}

	void loadTextureSelected(const char *path)
	{
		unsigned char *texSelectedData = stbi_load(path, &width, &height, &nrChannels, 0);
		glGenTextures(1, &texSelected);
		glBindTexture(GL_TEXTURE_2D, texSelected);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texSelectedData);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(texSelectedData);
	}
private:

};
//四边形的缓冲


#endif // !INDICATOR_H