#define INDEX(i,j) ((i)+(N+2)*(j))
// Swap two array pointers
#define SWAP(x0,x) {float *tmp=x0;x0=x;x=tmp;}

#include "fluid_grid.h"
#include <stdlib.h>


FluidGrid::FluidGrid(size_t N, int diffusion, int viscosity, float dt, FluidGridConfig* fluidGridConfig) {
	this->N = N;
	size = (N + 2) * (N + 2);

	this->dt = dt;
	diff = diffusion;
	visc = viscosity;

	density = new float[size]();
	densityPrev = new float[size]();

	velX = new float[size]();
	velY = new float[size]();

	velXPrev = new float[size]();
	velYPrev = new float[size]();

	textureDen = new Texture("Den", GL_TEXTURE_2D);
	textureVelX = new Texture("VelX", GL_TEXTURE_2D);
	textureVelY = new Texture("VelY", GL_TEXTURE_2D);

	fluidGridConfig->density = textureDen;
	fluidGridConfig->velX = textureVelX;
	fluidGridConfig->velY = textureVelY;

	initialize();
}

FluidGrid::~FluidGrid() {
	free(density);
	free(densityPrev);

	free(velX);
	free(velY);

	free(velXPrev);
	free(velYPrev);
}

void FluidGrid::initialize() {
	memset(density, 0, sizeof(float) * size);

	// Put something in the middle (for now)
	for (size_t i = N / 2; i < (N / 2) + 15; i++) {
		for (size_t j = N / 2; j < (N / 2) + 15; j++) {
			density[INDEX(j, i)] = 1.0f;
			//density[INDEX(N / 2, N / 2)] = 1.0f;
		}
	}
	//density[INDEX(N/2, N/2)] = 1.0f;
	memset(densityPrev, 0, sizeof(float)*size);
	memset(velX, 0, sizeof(float) * size);
	memset(velY, 0, sizeof(float) * size);
	memset(velXPrev, 0, sizeof(float) * size);
	memset(velYPrev, 0, sizeof(float) * size);

	drawStep();
}

// Add density and velocity
void FluidGrid::addSource(float* dst, float* sources) {
	for (int i = 0; i < size; i++) dst[i] += dt * sources[i];
}


void FluidGrid::diffuse(int b, float* cur, float* prev) {
	int i, j, k;
	float a = dt * diff * N * N;
	for (k = 0; k < 20; k++) {
		for (i = 1; i <= N; i++) {
			for (j = 1; j <= N; j++) {
				cur[INDEX(i, j)] = (prev[INDEX(i, j)] + a * (cur[INDEX(i - 1, j)] + cur[INDEX(i + 1, j)] +
					cur[INDEX(i, j - 1)] + cur[INDEX(i, j + 1)])) / (1 + 4 * a);
			}
		}
		setBounds(b, cur);
	}
}

void FluidGrid::advect(int b, float* density, float* densityPrev, float* velX, float* velY) {
	int i0, j0, i1, j1;
	float x, y, s0, t0, s1, t1, dt0;
	dt0 = dt * N;
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= N; j++) {
			x = i - dt0 * velX[INDEX(i, j)]; 
			y = j - dt0 * velY[INDEX(i, j)];

			if (x < 0.5) x = 0.5; 
			if (x > N + 0.5) x = N + 0.5; 

			i0 = (int)x; i1 = i0 + 1;
			if (y < 0.5) y = 0.5; 

			if (y > N + 0.5) y = N + 0.5; 
			j0 = (int)y; j1 = j0 + 1;

			s1 = x - i0; s0 = 1 - s1; 
			t1 = y - j0; t0 = 1 - t1;

			density[INDEX(i, j)] = s0 * (t0 * densityPrev[INDEX(i0, j0)] + t1 * densityPrev[INDEX(i0, j1)]) +
				s1 * (t0 * densityPrev[INDEX(i1, j0)] + t1 * densityPrev[INDEX(i1, j1)]);
		}
	}
	setBounds(b, density);
}

void FluidGrid::densityStep() {
	addSource(density, densityPrev);
	SWAP(densityPrev, density); diffuse(0, density, densityPrev);
	SWAP(densityPrev, density); advect(0, density, densityPrev, velX, velY);
}

void FluidGrid::velocityStep() {
	addSource(velX, velXPrev); addSource(velY, velYPrev);
	SWAP(velXPrev, velX); diffuse(1, velX, velXPrev);
	SWAP(velYPrev, velY); diffuse(2, velY, velYPrev);
	project(velX, velY, velXPrev, velYPrev);
	SWAP(velXPrev, velX); SWAP(velYPrev, velY);
	advect(1, velX, velXPrev, velXPrev, velYPrev); advect(2, velY, velYPrev, velXPrev, velYPrev);
	project(velX, velY, velXPrev, velYPrev);
}

void FluidGrid::project(float* velX, float* velY, float* p, float* div) {
	int i, j, k;
	float h;
	h = 1.0 / N;
	for (i = 1; i <= N; i++) {
		for (j = 1; j <= N; j++) {
			div[INDEX(i, j)] = -0.5 * h * (velX[INDEX(i + 1, j)] - velX[INDEX(i - 1, j)] +
				velY[INDEX(i, j + 1)] - velY[INDEX(i, j - 1)]);
			p[INDEX(i, j)] = 0;
		}
	}
	setBounds(0, div); setBounds(0, p);
	for (k = 0; k < 20; k++) {
		for (i = 1; i <= N; i++) {
			for (j = 1; j <= N; j++) {
				p[INDEX(i, j)] = (div[INDEX(i, j)] + p[INDEX(i - 1, j)] + p[INDEX(i + 1, j)] +
					p[INDEX(i, j - 1)] + p[INDEX(i, j + 1)]) / 4;
			}
		}
		setBounds(0, p);
	}
	for (i = 1; i <= N; i++) {
		for (j = 1; j <= N; j++) {
			velX[INDEX(i, j)] -= 0.5 * (p[INDEX(i + 1, j)] - p[INDEX(i - 1, j)]) / h;
			velY[INDEX(i, j)] -= 0.5 * (p[INDEX(i, j + 1)] - p[INDEX(i, j - 1)]) / h;
		}
	}
	setBounds(1, velX); setBounds(2, velY);
}

// b = 0 no border, propogate change
// b = 1 y-axis border
// b = 2 x-axis border
void FluidGrid::setBounds(int b, float* x) {
	int i;
	for (i = 1; i <= N; i++) {
		x[INDEX(0, i)] = b == 1 ? -x[INDEX(1, i)] : x[INDEX(1, i)];
		x[INDEX(N + 1, i)] = b == 1 ? -x[INDEX(N, i)] : x[INDEX(N, i)];
		x[INDEX(i, 0)] = b == 2 ? -x[INDEX(i, 1)] : x[INDEX(i, 1)];
		x[INDEX(i, N + 1)] = b == 2 ? -x[INDEX(i, N)] : x[INDEX(i, N)];
	}
	x[INDEX(0, 0)] = 0.5 * (x[INDEX(1, 0)] + x[INDEX(0, 1)]);
	x[INDEX(0, N + 1)] = 0.5 * (x[INDEX(1, N + 1)] + x[INDEX(0, N)]);
	x[INDEX(N + 1, 0)] = 0.5 * (x[INDEX(N, 0)] + x[INDEX(N + 1, 1)]);
	x[INDEX(N + 1, N + 1)] = 0.5 * (x[INDEX(N, N + 1)] + x[INDEX(N + 1, N)]);
}

void FluidGrid::drawStep() {
	textureDen->generateTexture(density, N + 2, N + 2, GL_RED);
	textureVelX->generateTexture(velX, N + 2, N + 2, GL_RED);
	textureVelY->generateTexture(velY, N + 2, N + 2, GL_RED);
}

void FluidGrid::simulate() {
	velocityStep();
	densityStep();
	drawStep();
}

size_t FluidGrid::getN() {
	return N;
}

Texture* FluidGrid::getTextureDen() {
	return textureDen;
}

float* FluidGrid::getDiffPointer() {
	return &diff;
}
float* FluidGrid::getViscPointer() {
	return &visc;
}



