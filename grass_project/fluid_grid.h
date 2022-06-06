#ifndef FLUID_GRID_H
#define FLUID_GRID_H

#include "texture.h"

class FluidGrid;

struct FluidGridConfig {
	Texture* density = nullptr;
	Texture* velX = nullptr;
	Texture* velY = nullptr;
	FluidGrid* fluidGrid = nullptr;
	bool visualizeDensity = false;
};

class FluidGrid {

public:
	FluidGrid(int N, float diffusion, float viscosity, float dt, FluidGridConfig* fluidGridConfig);
	~FluidGrid();

	void initialize();
	
	void addSource(float* dst, float* sources);
	void diffuse(int b, float* cur, float* prev);
	void advect(int b, float* density, float* densityPrev, float* velX, float* velY);
	void densityStep();
	void velocityStep();
	void setBounds(int b, float* x);
	void project(float* velX, float* velY, float* p, float* div);

	void addDensityAt(int x, int y, float d);
	void addVelocityAt(int x, int y, float vX, float vY);

	void clearCurrent();

	void drawStep();
	void simulate();
	int getN();
	Texture* getTextureDen();
	Texture* getTextureVelX();
	Texture* getTextureVelY();
	float* getDiffPointer();
	float* getViscPointer();

private:
	int size;
	int N;
	float dt;
	float diff;
	float visc;
	
	float *density;
	float* densityPrev;

	float* velX; // u in paper
	float* velY; // v in paper

	float* velXPrev;
	float* velYPrev;

	float* densitySource;
	float* velXSource;
	float* velYSource;


	Texture* textureDen;
	Texture* textureVelX;
	Texture* textureVelY;
};

#endif