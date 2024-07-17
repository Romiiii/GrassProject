#include "grass_math.h"

#include "glm/gtc/epsilon.hpp"

std::optional<glm::vec3> Ray::intersectsRectangle(const Rectangle& rectangle) {
	
	glm::vec3 dir = glm::normalize(direction);
	glm::vec3 invDirection = { 1.0f / dir.x, 1.0f / dir.y, 1.0f / dir.z };

	// Compute half the width and height of the rectangle
	float halfWidth = rectangle.width / 2.0f;
	float halfHeight = rectangle.height / 2.0f;

	// Compute the four corner glm::vec3s of the rectangle
	glm::vec3 aabbMin = { rectangle.center.x - halfWidth, rectangle.center.y - 0.0001f, rectangle.center.z - halfHeight };
	glm::vec3 aabbMax = { rectangle.center.x + halfWidth, rectangle.center.y + 0.0001f, rectangle.center.z + halfHeight };

	float t1 = ( aabbMin.x - origin.x) * invDirection.x;
	float t2 = ( aabbMax.x - origin.x) * invDirection.x;
	float t3 = ( aabbMin.y - origin.y) * invDirection.y;
	float t4 = ( aabbMax.y - origin.y) * invDirection.y;
	float t5 = ( aabbMin.z - origin.z) * invDirection.z;
	float t6 = ( aabbMax.z - origin.z) * invDirection.z;

	float tMin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tMax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));


	if (tMax >= tMin && tMin > 0.0f)
	{
		return origin + tMin * direction;
	}
	else
	{
		return std::nullopt;
	}
}

#if 0
std::optional<glm::vec3> Ray::intersectsRectangle(const Rectangle& rectangle) {
	// Compute half the width and height of the rectangle
	float halfWidth = rectangle.width / 2.0f;
	float halfHeight = rectangle.height / 2.0f;

	// Compute the four corner glm::vec3s of the rectangle
	glm::vec3 topLeft = { rectangle.center.x - halfWidth, rectangle.center.y, rectangle.center.z + halfHeight };
	glm::vec3 topRight = { rectangle.center.x + halfWidth, rectangle.center.y, rectangle.center.z + halfHeight };
	glm::vec3 bottomLeft = { rectangle.center.x - halfWidth, rectangle.center.y, rectangle.center.z - halfHeight };
	glm::vec3 bottomRight = { rectangle.center.x + halfWidth, rectangle.center.y, rectangle.center.z - halfHeight };

	// Check if the ray intersects with any of the rectangle's edges
	std::optional<glm::vec3> intersection;
	intersection = intersectsLine(topLeft, topRight);
	if (intersection) {
		return intersection;
	}

	intersection = intersectsLine(topRight, bottomRight);
	if (intersection) {
		return intersection;
	}

	intersection = intersectsLine(bottomRight, bottomLeft);
	if (intersection) {
		return intersection;
	}

	intersection = intersectsLine(bottomLeft, topLeft);
	if (intersection) {
		return intersection;
	}

	return std::nullopt;
}


#endif
std::optional<glm::vec3> Ray::intersectsLine(const glm::vec3& p1, const glm::vec3& p2) {
	// Compute the direction of the line segment
	glm::vec3 segment = p2 - p1;

	// Compute the normal of the line segment
	float segmentLength = glm::length(segment);
	segment = glm::normalize(segment);

	// Compute the dot product of the line segment normal and the ray direction

	float dotProduct = glm::dot(segment, direction);

	// If the dot product is close to zero, the ray is perpendicular to the line segment
	if (glm::epsilonEqual(dotProduct, 1.0f, 0.000001f)) {
		return std::nullopt;
	}

	// Compute the vector between the origin of the ray and one of the line segment's points
	glm::vec3 vector = origin - p1;

	// Compute the distance from the origin of the ray to the intersection point
	float distance = glm::distance(origin, segment);

	// Check if the intersection point is within the line segment
	if (distance >= 0 && distance <= segmentLength) {
		// Compute the point of intersection
		glm::vec3 intersectionPoint = origin + distance * direction;

		return intersectionPoint;
	}

	return std::nullopt;
}

