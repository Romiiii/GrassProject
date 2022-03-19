#include "camera.h"

#include "debug.h"

Camera::Camera() {
	updateCameraVectors();
}

/* Processes input received from any keyboard-like input system.
	* Accepts input parameter in the form of camera defined by
	* ENUM (to abstract it from window-system specific inputs).
	* \param direction - the direction of the key that was pressed
	* \param deltaTime - time between current frame and last frame
	*/
void Camera::processKeyboard(cameraMovement direction, float deltaTime) {
	glm::vec3 forwardInXYZ = glm::normalize(glm::vec3(camForward.x,
		camForward.y, camForward.z));
	float cameraSpeed;
	if (sprinting) {
		 cameraSpeed = sprintSpeed * deltaTime;
	}
	else {
		cameraSpeed = movementSpeed * deltaTime;
	}

	if (direction == cameraMovement::FORWARD) {
		glm::vec3 newCamPosition = camPosition + forwardInXYZ * cameraSpeed;
		// Check if the player is still above the ground
		if (newCamPosition.y >= groundY) {
			camPosition = newCamPosition;
		}
		else {
			newCamPosition = camPosition + glm::normalize(glm::vec3(
				camForward.x, 0, camForward.z)) * cameraSpeed;
			camPosition.x = newCamPosition.x;
			camPosition.z = newCamPosition.z;
		}
	}
	if (direction == cameraMovement::BACKWARD) {
		glm::vec3 newCamPosition = camPosition - forwardInXYZ * cameraSpeed;
		// Check if the player is still above the ground
		if (newCamPosition.y >= groundY) {
			camPosition = newCamPosition;
		}
		else {
			newCamPosition = camPosition - glm::normalize(glm::vec3(
				camForward.x, 0, camForward.z)) * cameraSpeed;
			camPosition.x = newCamPosition.x;
			camPosition.z = newCamPosition.z;
		}
	}
	if (direction == cameraMovement::LEFT) {
		camPosition -= glm::cross(forwardInXYZ, 
			glm::vec3(0, 1, 0)) * cameraSpeed;
	}
	if (direction == cameraMovement::RIGHT) {
		camPosition += glm::cross(forwardInXYZ, 
			glm::vec3(0, 1, 0)) * cameraSpeed;
	}
}

/* Processes input received from a mouse input system based on the
	* x and y offset of the mouse.
	* ENUM (to abstract it from window-system specific inputs).
	* \param xoffset - offset on the x axis of mouse
	* \param yoffset - offset on the y axis of mouse
	* \param constrainPitch - whether pitch should be constrained to
	*						   prevent the screen from flipping or not
	*/
void Camera::processMouseMovement(float xoffset, float yoffset,
	GLboolean constrainPitch) {
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	yaw += xoffset;
	pitch += yoffset;


	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}
	updateCameraVectors();
}

glm::vec3 Camera::getCamForward() {
	return camForward;
}

glm::vec3 Camera::getCamPosition() {
	return camPosition;
}

/* Calculates the forward vector from the Camera's (updated) Euler Angles.
	*/
void Camera::updateCameraVectors() {
	ASSERT(pitch <= 89.9f && pitch >= -89.9f);

	// -90 to point us forward.
	float yawToUse = yaw - 90;

	glm::vec3 front;
	front.x = cos(glm::radians(yawToUse)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yawToUse)) * cos(glm::radians(pitch));
	camForward = glm::normalize(front);
}

