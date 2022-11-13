/*
 * This header file contains drawing primitives for the objects in the scene.
 */

#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include <vector>
#include <glm\glm.hpp>
#include <string>
static std::vector<float> grassPositions{ 0.0f, 0.0f, 0.0f,
											-0.02f, 0.1f, 0.0f,
											0.0f, 0.2f, 0.0f,
											0.02f, 0.1f, 0.0f,
											0.0f, 0.5f, 0.0f,
};


static std::vector<float> grassUVs{ 0.0f, 0.0f,
									0.0f, 0.2f,
									0.5f, 0.4f,
									1.0f, 0.2f,
									0.5f, 1.0f
};

//static std::vector<unsigned int> grassIndices{	0, 3, 2,
//												0, 2, 1,
//												1, 2, 4,
//												2, 3, 4};


static std::vector<unsigned int> grassIndices{ 0, 2, 3,
												0, 1, 2,
												1, 4, 2,
												2, 4, 3 };

static std::vector<float> grassColors{ .56f, .60f, .17f, 1.f,
										.46f, .50f, .17f, 1.f,
										.56f, .60f, .17f, 1.f,
										.46f, .50f, .17f, 1.f,
										.46f, .50f, .17f, 1.f,
};

static std::vector<float> grassNormals{ 0.0f, 0.0f, 1.0f,
										0.0f, 0.0f, 1.0f,
										0.0f, 0.0f, 1.0f,
										0.0f, 0.0f, 1.0f,
										0.0f, 0.0f, 1.0f };

static std::vector<float> grassPatchPositions{
	0.0f, 0.0f, 1.0f, // Bot left
	1.0f, 0.0f, 1.0f, // Bot right
	1.0f, 0.0f, 0.0f, // Top right
	0.0f, 0.0f, 0.0f, // Top left
};

//static std::vector<unsigned int> grassPatchIndices{ 0, 1, 3,
//											 1, 2, 3};


static std::vector<unsigned int> grassPatchIndices{ 0, 1, 2,
											 2, 3, 0 };

static std::vector<float> grassPatchColors{ .6f, .58f, .27f, 1.f,
								.6f, .58f, .27f, 1.f,
								.6f, .58f, .27f, 1.f,
								.6f, .58f, .27f, 1.f, };

static std::vector<float> grassPatchNormals{ 0.0f, 1.0f, 0.0f,
								   0.0f, 1.0f, 0.0f,
								   0.0f, 1.0f, 0.0f,
								   0.0f, 1.0f, 0.0f };

static std::vector<float> cubePositions{
	-0.5f, 0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,

	-0.5f, -0.5f, 0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,
	-0.5f, 0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f,

	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f,

	-0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, -0.5f,

	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, 0.5f };

static float length = 1.0;
//    v5----- v4
//   /|      /|
//  v1------v0|
//  | |     | |
//  | |v6---|-|v7
//  |/      |/
//  v2------v3
static glm::vec3 p[] = {
		glm::vec3{length, length, length},
		glm::vec3{-length, length, length},
		glm::vec3{-length, -length, length},
		glm::vec3{length, -length, length},

		glm::vec3{length, length, -length},
		glm::vec3{-length, length, -length},
		glm::vec3{-length, -length, -length},
		glm::vec3{length, -length, -length}

};
static std::vector<uint32_t> indices({
							0,1,2, 0,2,3,
							4,5,6, 4,6,7,
							8,9,10, 8,10,11,
							12,13,14, 12, 14,15,
							16,17,18, 16,18,19,
							20,21,22, 20,22,23
	});
static std::vector<glm::vec3> positions({ p[0],p[1],p[2], p[3], // v0-v1-v2-v3
						p[4],p[0],p[3], p[7], // v4-v0-v3-v7
						p[5],p[4],p[7], p[6], // v5-v4-v7-v6
						p[1],p[5],p[6], p[2], // v1-v5-v6-v2
						p[4],p[5],p[1], p[0], // v1-v5-v6-v2
						p[3],p[2],p[6], p[7], // v1-v5-v6-v2
	});

/* Skybox Texture Order
 * +X (right)		ft
 * -X (left)		bk
 * +Y (top)			up
 * -Y (bottom)		dn
 * +Z (front)		rt
 * -Z (back)		lf
 */

static std::vector<std::string> facesDay
{
		"assets/textures/skybox_day/hills_ft.tga",
		"assets/textures/skybox_day/hills_bk.tga",
		"assets/textures/skybox_day/hills_up.tga",
		"assets/textures/skybox_day/hills_dn.tga",
		"assets/textures/skybox_day/hills_rt.tga",
		"assets/textures/skybox_day/hills_lf.tga" };


static std::vector<std::string> facesNight
{
	"assets/textures/skybox_night/hills_ft.png",
	"assets/textures/skybox_night/hills_bk.png",
	"assets/textures/skybox_night/hills_up.png",
	"assets/textures/skybox_night/hills_dn.png",
	"assets/textures/skybox_night/hills_rt.png",
	"assets/textures/skybox_night/hills_lf.png" };

#endif // PRIMITIVES_H
