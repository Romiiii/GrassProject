#ifndef FLUID_GRID_H
#define FLUID_GRID_H

#include "texture.h"

class FluidGrid;

struct FluidGridConfig {
	Texture* density = nullptr;
	Texture* velX = nullptr;
	Texture* velY = nullptr;
	FluidGrid* fluidGrid = nullptr;
};

class FluidGrid {

public:
	FluidGrid(size_t N, int diffusion, int viscosity, float dt, FluidGridConfig* fluidGridConfig);
	~FluidGrid();

	void initialize();

	void addSource(float* dst, float* sources);
	void diffuse(int b, float* cur, float* prev);
	void advect(int b, float* density, float* densityPrev, float* velX, float* velY);
	void densityStep();
	void velocityStep();
	void setBounds(int b, float* x);
	void project(float* velX, float* velY, float* p, float* div);
	void drawStep();
	void simulate();
	size_t getN();
	Texture* getTextureDen();
	float* getDiffPointer();
	float* getViscPointer();

private:
	size_t size;
	size_t N;
	float dt;
	float diff;
	float visc;

	float* density;
	float* densityPrev;

	float* velX; // u in paper
	float* velY; // v in paper

	float* velXPrev;
	float* velYPrev;

	Texture* textureDen;
	Texture* textureVelX;
	Texture* textureVelY;
};

#endif