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
	int octaves = 6;
	float bias = 1.0f;
	Texture* texture = nullptr;
};

/**
* Generates perlin noise on the CPU.
*/
void perlinNoise2DCPU(int nWidth, int nHeight, int nOctaves, float fBias, float* fOutput, float* fSeed);

/**
* Generates perlin noise on the GPU.
*/
void perlinNoise2DGPU(Texture& seedTexture, float* seedTextureData, ShaderProgram* computeShaderProgram, GLuint computeShaderTexture, int octaves, float bias);

#endif