#include "grass_math.h"

std::optional<glm::vec3> Ray::intersectsRectangle(const Rectangle& rectangle) {
	// Compute half the width and height of the rectangle
	float halfWidth = rectangle.width / 2.0f;
	float halfHeight = rectangle.height / 2.0f;

	// Compute the four corner glm::vec3s of the rectangle
	glm::vec3 topLeft = { rectangle.center.x - halfWidth, rectangle.center.y + halfHeight, rectangle.center.z };
	glm::vec3 topRight = { rectangle.center.x + halfWidth, rectangle.center.y + halfHeight, rectangle.center.z };
	glm::vec3 bottomLeft = { rectangle.center.x - halfWidth, rectangle.center.y - halfHeight, rectangle.center.z };
	glm::vec3 bottomRight = { rectangle.center.x + halfWidth, rectangle.center.y - halfHeight, rectangle.center.z };

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


std::optional<glm::vec3> Ray::intersectsLine(const glm::vec3& p1, const glm::vec3& p2) {
	// Compute the direction of the line segment
	float segmentX = p2.x - p1.x;
	float segmentY = p2.y - p1.y;
	float segmentZ = p2.z - p1.z;

	// Compute the normal of the line segment
	float segmentLength = sqrt(segmentX * segmentX + segmentY * segmentY + segmentZ * segmentZ);
	float segmentNormalX = segmentX / segmentLength;
	float segmentNormalY = segmentY / segmentLength;
	float segmentNormalZ = segmentZ / segmentLength;

	// Compute the dot product of the line segment normal and the ray direction
	float dotProduct = segmentNormalX * direction.x + segmentNormalY * direction.y + segmentNormalZ * direction.z;

	// If the dot product is close to zero, the ray is parallel to the line segment
	if (fabs(dotProduct) < 0.000001) {
		return std::nullopt;
	}

	// Compute the vector between the origin of the ray and one of the line segment's points
	float vectorX = origin.x - p1.x;
	float vectorY = origin.y - p1.y;
	float vectorZ = origin.z - p1.z;

	// Compute the distance from the origin of the ray to the intersection point
	float distance = (vectorX * segmentNormalX + vectorY * segmentNormalY + vectorZ * segmentNormalZ) / dotProduct;

	// Check if the intersection point is within the line segment
	if (distance >= 0 && distance <= segmentLength) {
		// Compute the point of intersection
		glm::vec3 intersectionPoint = origin + distance * direction;

		return intersectionPoint;
	}

	return std::nullopt;
}

