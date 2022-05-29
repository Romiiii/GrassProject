//#define USE_ORIGINAL_IMPL

#define INDEX(i,j) ((i)+(N+2)*(j))
// Swap two array pointers
#define SWAP(x0,x) {float *tmp=x0;x0=x;x=tmp;}

#include "fluid_grid.h"
#include <stdlib.h>
#include "glm/glm.hpp"

#define IX(i,j) ((i)+(N+2)*(j))
#define SWAP(x0,x) {float * tmp=x0;x0=x;x=tmp;}
#define FOR_EACH_CELL for ( i=1 ; i<=N ; i++ ) { for ( j=1 ; j<=N ; j++ ) {
#define END_FOR }}
namespace JS
{

void add_source(int N, float *x, float *s, float dt)
{
	int i, size = (N + 2) * (N + 2);
	for (i = 0; i < size; i++) x[i] += dt * s[i];
}

void set_bnd(int N, int b, float *x)
{
	int i;

	for (i = 1; i <= N; i++) {
		x[IX(0, i)] = b == 1 ? -x[IX(1, i)] : x[IX(1, i)];
		x[IX(N + 1, i)] = b == 1 ? -x[IX(N, i)] : x[IX(N, i)];
		x[IX(i, 0)] = b == 2 ? -x[IX(i, 1)] : x[IX(i, 1)];
		x[IX(i, N + 1)] = b == 2 ? -x[IX(i, N)] : x[IX(i, N)];
	}
	x[IX(0, 0)] = 0.5f * (x[IX(1, 0)] + x[IX(0, 1)]);
	x[IX(0, N + 1)] = 0.5f * (x[IX(1, N + 1)] + x[IX(0, N)]);
	x[IX(N + 1, 0)] = 0.5f * (x[IX(N, 0)] + x[IX(N + 1, 1)]);
	x[IX(N + 1, N + 1)] = 0.5f * (x[IX(N, N + 1)] + x[IX(N + 1, N)]);
}

void lin_solve(int N, int b, float *x, float *x0, float a, float c)
{
	int i, j, k;

	for (k = 0; k < 20; k++) {
		FOR_EACH_CELL
			x[IX(i, j)] = (x0[IX(i, j)] + a * (x[IX(i - 1, j)] + x[IX(i + 1, j)] + x[IX(i, j - 1)] + x[IX(i, j + 1)])) / c;
		END_FOR
			set_bnd(N, b, x);
	}
}

void diffuse(int N, int b, float *x, float *x0, float diff, float dt)
{
	float a = dt * diff * N * N;
	lin_solve(N, b, x, x0, a, 1 + 4 * a);
}

void advect(int N, int b, float *d, float *d0, float *u, float *v, float dt)
{
	int i, j, i0, j0, i1, j1;
	float x, y, s0, t0, s1, t1, dt0;

	dt0 = dt * N;
	FOR_EACH_CELL
		x = i - dt0 * u[IX(i, j)]; y = j - dt0 * v[IX(i, j)];
	if (x < 0.5f) x = 0.5f; if (x > N + 0.5f) x = N + 0.5f; i0 = (int)x; i1 = i0 + 1;
	if (y < 0.5f) y = 0.5f; if (y > N + 0.5f) y = N + 0.5f; j0 = (int)y; j1 = j0 + 1;
	s1 = x - i0; s0 = 1 - s1; t1 = y - j0; t0 = 1 - t1;
	d[IX(i, j)] = s0 * (t0 * d0[IX(i0, j0)] + t1 * d0[IX(i0, j1)]) +
		s1 * (t0 * d0[IX(i1, j0)] + t1 * d0[IX(i1, j1)]);
	END_FOR
		set_bnd(N, b, d);
}

void project(int N, float *u, float *v, float *p, float *div)
{
	int i, j;

	FOR_EACH_CELL
		div[IX(i, j)] = -0.5f * (u[IX(i + 1, j)] - u[IX(i - 1, j)] + v[IX(i, j + 1)] - v[IX(i, j - 1)]) / N;
	p[IX(i, j)] = 0;
	END_FOR
		set_bnd(N, 0, div); set_bnd(N, 0, p);

	lin_solve(N, 0, p, div, 1, 4);

	FOR_EACH_CELL
		u[IX(i, j)] -= 0.5f * N * (p[IX(i + 1, j)] - p[IX(i - 1, j)]);
	v[IX(i, j)] -= 0.5f * N * (p[IX(i, j + 1)] - p[IX(i, j - 1)]);
	END_FOR
		set_bnd(N, 1, u); set_bnd(N, 2, v);
}

void dens_step(int N, float *x, float *x0, float *u, float *v, float diff, float dt)
{
	add_source(N, x, x0, dt);
	SWAP(x0, x); diffuse(N, 0, x, x0, diff, dt);
	SWAP(x0, x); advect(N, 0, x, x0, u, v, dt);
}

void vel_step(int N, float *u, float *v, float *u0, float *v0, float visc, float dt)
{
	add_source(N, u, u0, dt); add_source(N, v, v0, dt);
	SWAP(u0, u); diffuse(N, 1, u, u0, visc, dt);
	SWAP(v0, v); diffuse(N, 2, v, v0, visc, dt);
	project(N, u, v, u0, v0);
	SWAP(u0, u); SWAP(v0, v);
	advect(N, 1, u, u0, u0, v0, dt); advect(N, 2, v, v0, u0, v0, dt);
	project(N, u, v, u0, v0);
}
}



FluidGrid::FluidGrid(int N, float diffusion, float viscosity, float dt, FluidGridConfig *fluidGridConfig) {
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
	for (size_t i = N / 2; i < (N / 2); i++) {
		for (size_t j = N / 2; j < (N / 2); j++) {
			//density[INDEX(j, i)] = 1.0f;
		}
	}

	memset(densityPrev, 0, sizeof(float) * size);
	memset(velX, 0, sizeof(float) * size);
	memset(velY, 0, sizeof(float) * size);
	memset(velXPrev, 0, sizeof(float) * size);
	memset(velYPrev, 0, sizeof(float) * size);

	drawStep();
}


// Add for both density and velocity
void FluidGrid::addSource(float *dst, float *sources) {
	for (int i = 0; i < size; i++) dst[i] += dt * sources[i];
}


void FluidGrid::diffuse(int b, float *cur, float *prev) {
#ifdef USE_ORIGINAL_IMPL
	JS::diffuse(N, b, cur, prev, diff, dt);
#else
	float a = dt * diff * N * N;

	for (int k = 0; k < 20; k++) {
		for (int i = 1; i <= N; i++) {
			for (int j = 1; j <= N; j++) {
				cur[INDEX(i, j)] = (prev[INDEX(i, j)] + a * (cur[INDEX(i - 1, j)] + cur[INDEX(i + 1, j)] +
					cur[INDEX(i, j - 1)] + cur[INDEX(i, j + 1)])) / (1 + 4 * a);
			}
		}
		setBounds(b, cur);
	}
#endif
}

void FluidGrid::advect(int b, float *density, float *densityPrev, float *velX, float *velY) {
#ifdef USE_ORIGINAL_IMPL
	JS::advect(N, b, density, densityPrev, velX, velY, dt);
#else
	float dt0 = dt * N;
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= N; j++) {
			float x = i - dt0 * velX[INDEX(i, j)];
			x = glm::clamp(x, 0.5f, N + 0.5f);
			int i0 = (int)x;
			int i1 = i0 + 1;

			float y = j - dt0 * velY[INDEX(i, j)];
			y = glm::clamp(y, 0.5f, N + 0.5f);
			int j0 = (int)y;
			int j1 = j0 + 1;

			float s1 = x - i0;
			float s0 = 1 - s1;

			float t1 = y - j0;
			float t0 = 1 - t1;

			density[INDEX(i, j)] = s0 * (t0 * densityPrev[INDEX(i0, j0)] + t1 * densityPrev[INDEX(i0, j1)]) +
				s1 * (t0 * densityPrev[INDEX(i1, j0)] + t1 * densityPrev[INDEX(i1, j1)]);
		}
	}
	setBounds(b, density);
#endif
}

void FluidGrid::densityStep() {
	
#ifdef USE_ORIGINAL_IMPL
	JS::dens_step(N, density, densityPrev, velX, velY, diff, dt);
#else
	addSource(density, densityPrev);
	SWAP(densityPrev, density);
	diffuse(0, density, densityPrev);

	SWAP(densityPrev, density);
	advect(0, density, densityPrev, velX, velY);
#endif
}

void FluidGrid::velocityStep() {
#ifdef USE_ORIGINAL_IMPL
	JS::vel_step(N, velX, velY, velXPrev, velYPrev, visc, dt);
#else
	addSource(velX, velXPrev); addSource(velY, velYPrev);
	SWAP(velXPrev, velX); diffuse(1, velX, velXPrev);
	SWAP(velYPrev, velY); diffuse(2, velY, velYPrev);
	project(velX, velY, velXPrev, velYPrev);
	SWAP(velXPrev, velX); SWAP(velYPrev, velY);
	advect(1, velX, velXPrev, velXPrev, velYPrev); advect(2, velY, velYPrev, velXPrev, velYPrev);
	project(velX, velY, velXPrev, velYPrev);
#endif
}

void FluidGrid::project(float *velX, float *velY, float *p, float *div) {

#ifdef USE_ORIGINAL_IMPL
	JS::project(N, velX, velY, p, div);
#else
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
#endif
}

// b = 0 no border, propagate change
// b = 1 y-axis border
// b = 2 x-axis border
void FluidGrid::setBounds(int b, float *x) {
	
#ifdef USE_ORIGINAL_IMPL
	JS::set_bnd(N, b, x);
#else
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
#endif
}

void FluidGrid::addDensityAt(int x, int y, float d)
{
	densityPrev[INDEX(x, y)] = d;
}

void FluidGrid::addVelocityAt(int x, int y, float vX, float vY)
{
	velXPrev[INDEX(x, y)] = vX;
	velYPrev[INDEX(x, y)] = vY;
}

void FluidGrid::clearCurrent()
{
	memset(densityPrev, 0, sizeof(float) * size);
	memset(velXPrev, 0, sizeof(float) * size);
	memset(velYPrev, 0, sizeof(float) * size);
}

void FluidGrid::drawStep() {
	textureDen->loadTextureSingleChannel(N + 2, density);
	textureVelX->loadTextureSingleChannel(N + 2, velX);
	textureVelY->loadTextureSingleChannel(N + 2, velY);
}

void FluidGrid::simulate() {
	velocityStep();
	densityStep();
	drawStep();
}

int FluidGrid::getN() {
	return N;
}

Texture *FluidGrid::getTextureDen() {
	return textureDen;
}

float *FluidGrid::getDiffPointer() {
	return &diff;
}
float *FluidGrid::getViscPointer() {
	return &visc;
}



