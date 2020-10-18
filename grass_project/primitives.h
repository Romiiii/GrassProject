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

std::vector<float> billboardSquareColors{ .0f, .5f, .0f, 1.f,
								.0f, .5f, .0f, 1.f,
								.0f, .5f, .0f, 1.f,
								.0f, .5f, .0f, 1.f };

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
		"skybox/hills_ft.tga",
		"skybox/hills_bk.tga",
		"skybox/hills_up.tga",
		"skybox/hills_dn.tga",
		"skybox/hills_rt.tga",
		"skybox/hills_lf.tga" };


std::vector<std::string> facesNight
{
	"skybox2/hills_ft.png",
	"skybox2/hills_bk.png",
	"skybox2/hills_up.png",
	"skybox2/hills_dn.png",
	"skybox2/hills_rt.png",
	"skybox2/hills_lf.png" };

#endif // PRIMITIVES_H
