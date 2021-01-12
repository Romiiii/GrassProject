#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H

#define PERLIN_NOISE_TEXTURE_WIDTH 512

#include "texture.h"
#include "shader_program.h"

struct PerlinConfig {
	int octaves = 9;
	float bias = 2.0f;
};

void PerlinNoise2DCPU(int nWidth, int nHeight, int nOctaves, float fBias, float* fOutput, float* fSeed);

void PerlinNoise2DGPU(Texture& seedTexture, float* seedTextureData, ShaderProgram* computeShaderProgram, GLuint computeShaderTexture, int octaves, float bias);

#endif