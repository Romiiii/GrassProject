#ifndef GRASS_MATH_H
#define GRASS_MATH_H

#include <glm/glm.hpp>
#include <optional>

struct Rectangle {
	glm::vec3 center;
	float width;
	float height;
};

class Ray
{
public:
	glm::vec3 origin;
	glm::vec3 direction;

	std::optional<glm::vec3> intersectsRectangle(const Rectangle& rectangle);
	std::optional<glm::vec3> intersectsLine(const glm::vec3& p1, const glm::vec3& p2);
};

inline float map(float s, float a1, float a2, float b1, float b2)
{
	return b1 + (s - a1) * (b2 - b1) / (a2 - a1);
}

inline glm::vec2 map(glm::vec2 s, float a1, float a2, float b1, float b2)
{
	return { map(s.x, a1, a2, b1, b2), map(s.y, a1, a2, b1, b2) };
}

#endif