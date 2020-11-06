/*
 * This header file contains drawing primitives for the objects in the scene.
 */

#ifndef PRIMITIVES_H
#define PRIMITIVES_H


std::vector<float> grassVertices{ 0.0f, 0.0f, 0.0f,
								   0.1f, 0.0f, 0.0f,
								   0.2f, 0.5f, 0.0f };

std::vector<unsigned int> grassIndices{ 0, 1, 2 };

std::vector<float> grassColors{ .46f, .50f, .17f, 1.f,
								 .46f, .50f, .17f, 1.f,
								.38f, .47f, .17f, 1.f};

std::vector<float> grassNormals{ 0.0f, 0.0f, 1.0f,
								   0.0f, 0.0f, 1.0f,
								   0.0f, 0.0f, 1.0f };

std::vector<float> grassPatchVertices{ -5.0f, 0.0f, -5.0f,
								   5.0f, 0.0f, -5.0f,
								   5.0f, 0.0f, 5.0f,
								   -5.0f, 0.0f, 5.0f};

std::vector<unsigned int> grassPatchIndices{ 0, 1, 3, 
											 1, 2, 3};

std::vector<float> grassPatchColors{ .6f, .58f, .27f, 1.f,
								.6f, .58f, .27f, 1.f,
								.6f, .58f, .27f, 1.f,
								.6f, .58f, .27f, 1.f, };

std::vector<float> grassPatchNormals{ 0.0f, 1.0f, 0.0f,
								   0.0f, 1.0f, 0.0f,
								   0.0f, 1.0f, 0.0f,
								   0.0f, 1.0f, 0.0f };

std::vector<float> billboardSquareVertices{ 0.5f,  0.5f, 0.0f,  // top right
											0.5f, -0.5f, 0.0f,  // bottom right
											-0.5f, -0.5f, 0.0f,  // bottom left
											-0.5f,  0.5f, 0.0f };   // top left 
							

std::vector<unsigned int> billboardSquareIndices{	0, 1, 3,  
													1, 2, 3 };  


std::vector<float> billboardSquareUVs{	1.0f, 0.0f,  // bottom right
										1.0f, 1.0f,  // top right
										0.0f, 1.0f,	 // top left
										0.0f, 0.0f };  // bottom left 

std::vector<float> billboardSquareNormals{	0.0f, 0.0f, 1.0f,  // top right
											0.0f, 0.0f, 1.0f,  // bottom right
											0.0f, 0.0f, 1.0f,  // bottom left
											0.0f, 0.0f, 1.0f };   // top left 

std::vector<float> skyboxVertices{
	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f };

/* Skybox Texture Order
 * +X (right)		ft
 * -X (left)		bk
 * +Y (top)			up
 * -Y (bottom)		dn
 * +Z (front)		rt
 * -Z (back)		lf
 */

std::vector<std::string> facesDay
{
		"assets/textures/skybox_day/hills_ft.tga",
		"assets/textures/skybox_day/hills_bk.tga",
		"assets/textures/skybox_day/hills_up.tga",
		"assets/textures/skybox_day/hills_dn.tga",
		"assets/textures/skybox_day/hills_rt.tga",
		"assets/textures/skybox_day/hills_lf.tga" };


std::vector<std::string> facesNight
{
	"assets/textures/skybox_night/hills_ft.png",
	"assets/textures/skybox_night/hills_bk.png",
	"assets/textures/skybox_night/hills_up.png",
	"assets/textures/skybox_night/hills_dn.png",
	"assets/textures/skybox_night/hills_rt.png",
	"assets/textures/skybox_night/hills_lf.png" };

#endif // PRIMITIVES_H
