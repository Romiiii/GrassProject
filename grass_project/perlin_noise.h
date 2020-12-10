#ifndef PERLIN_NOISE_H
#define PERLIN_NOISE_H

struct PerlinConfig {
	int octaves = 9;
	float bias = 2.0f;
};

void PerlinNoise2D(int nWidth, int nHeight, int nOctaves, float fBias, float* fOutput);

#endif