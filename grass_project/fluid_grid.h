#ifndef FLUID_GRID_H
#define FLUID_GRID_H

#include <glm/glm.hpp>

#include "texture.h"

class FluidGrid;

struct Fan
{
	bool active = true;
	float x = 0.2f;
	float y = 0.2f;
	float density = 250.0f;
	float velocityX = 300.0f;
	float velocityY = 0.0f;
};

struct FluidGridConfig
{
	Texture *  density            = nullptr;
	Texture *  velX               = nullptr;
	Texture *  velY               = nullptr;
	FluidGrid *fluidGrid          = nullptr;
	bool       visualizeDensity   = false;
	float      velocityMultiplier = 2.7f;
	glm::vec2  velocityClampRange = {0.5f, 0.5f};
	Fan fan;
	bool shouldDrawFans = true;
};

class FluidGrid
{
public:
	FluidGrid(int N, float diffusion, float viscosity, FluidGridConfig *fluidGridConfig);
	~FluidGrid();

	void initialize();

	void addSource(float *dst, float *sources, float deltaTime);
	void diffuse(int b, float *cur, float *prev, float deltaTime);
	void advect(int b, float *density, float *densityPrev, float *velX, float *velY, float deltaTime);
	void densityStep(float deltaTime);
	void velocityStep(float deltaTime);
	void setBounds(int b, float *x);
	void project(float *velX, float *velY, float *p, float *div);
	float totalDensity();

	void addDensityAt(int x, int y, float d);
	void addVelocityAt(int x, int y, float vX, float vY);

	void clearCurrent();

	void     drawStep();
	void     simulate(float deltaTime);
	int      getN();
	Texture *getTextureDen();
	Texture *getTextureVelX();
	Texture *getTextureVelY();
	float *  getDiffPointer();
	float *  getViscPointer();

private:
	int   size;
	int   N;
	float diff;
	float visc;

	float *density;
	float *densityPrev;

	float *velX; // u in paper
	float *velY; // v in paper

	float *velXPrev;
	float *velYPrev;

	float *densitySource;
	float *velXSource;
	float *velYSource;


	Texture *textureDen;
	Texture *textureVelX;
	Texture *textureVelY;
};

#endif
