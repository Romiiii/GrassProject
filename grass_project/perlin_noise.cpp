#include "perlin_noise.h"
#include <stdlib.h>  


void PerlinNoise2DCPU(int nWidth, int nHeight, int nOctaves, float fBias, float* fOutput, float* fSeed)

{

	//float* fSeed = new float[(long)nWidth * (long)nHeight];

	//for (int i = 0; i < nWidth * nHeight; i++) fSeed[i] = (float)rand() / (float)RAND_MAX;

	//for (int i = 0; i < nWidth * nHeight; i++) fSeed[i] = i/((float)nWidth*nHeight);


	for (int x = 0; x < nWidth; x++)
	{
		for (int y = 0; y < nHeight; y++)
		{
			float fNoise = 0.0f;
			float fScaleAcc = 0.0f;
			float fScale = 1.0f;

			for (int o = 0; o < nOctaves; o++)
			{
				int nPitch = nWidth >> o;
				int nSampleX1 = (x / nPitch) * nPitch;
				int nSampleY1 = (y / nPitch) * nPitch;

				int nSampleX2 = (nSampleX1 + nPitch) % nWidth;
				int nSampleY2 = (nSampleY1 + nPitch) % nWidth;

				float fBlendX = (float)(x - nSampleX1) / (float)nPitch;
				float fBlendY = (float)(y - nSampleY1) / (float)nPitch;

				float fSampleT = (1.0f - fBlendX) * fSeed[nSampleY1 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY1 * nWidth + nSampleX2];
				float fSampleB = (1.0f - fBlendX) * fSeed[nSampleY2 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY2 * nWidth + nSampleX2];

				fScaleAcc += fScale;
				fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;
				fScale = fScale / fBias;
			}

			// Scale to seed range
			fOutput[y * nWidth + x] = fNoise / fScaleAcc;

		}
	}
	//delete[] fSeed;

}


void PerlinNoise2DGPU(Texture& seedTexture, float* seedTextureData, ShaderProgram* computeShaderProgram, GLuint computeShaderTexture, int octaves, float bias) {
	seedTexture.loadTextureData(seedTextureData, 512, 512, GL_RED);

	computeShaderProgram->use();

	GLCall(glBindImageTexture(0, computeShaderTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8));
	GLCall(glBindImageTexture(1, seedTexture.getTextureID(), 0, GL_FALSE, 0, GL_READ_ONLY, GL_R8));

	//scene.currentTexture->bindTextureCubeMap();
	//shaderProgram.setInt("skybox", scene.currentTexture->getTextureID());

	computeShaderProgram->setInt("height", 512);
	computeShaderProgram->setInt("width", 512);
	computeShaderProgram->setInt("octaves", octaves);
	computeShaderProgram->setFloat("bias", bias);

	GLCall(glDispatchCompute(512 / 16, 512 / 16, 1));
}