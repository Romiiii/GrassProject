#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H

#define PERLIN_NOISE_TEXTURE_WIDTH 512

#include "texture.h"
#include "shader_program.h"

/**
* Settings the user can use to manipulate the generated perlin noise.
* makeChecker :			If set to true no perlin noise will be generated and a
*						checkerboard pattern will be drawn for debug purposes.
* octaves		: 
* bias			:
* textureScale:	Dictates the sample scale used to sample the 
*						perlin noise texture. Effectively zooming in and
*						out on the texture.
*/
struct PerlinConfig {
	bool makeChecker = false;
	int octaves = 9;
	float bias = 2.0f;
	float textureScale = 1.0f;
	Texture* texture = nullptr;
	glm::vec2 windDirection = glm::normalize(glm::vec2(0.5f, 0.0f ));
	float windStrength = 10.0f;
};

/**
* Generates perlin noise on the CPU.
*/
void PerlinNoise2DCPU(int nWidth, int nHeight, int nOctaves, float fBias, float* fOutput, float* fSeed);

/**
* Generates perlin noise on the GPU.
*/
void PerlinNoise2DGPU(Texture& seedTexture, float* seedTextureData, ShaderProgram* computeShaderProgram, GLuint computeShaderTexture, int octaves, float bias, bool makeChecker);

#endif