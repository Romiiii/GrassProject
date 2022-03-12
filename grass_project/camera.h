/*
 * An abstract camera class that processes input and calculates the
 * corresponding camera position for use in OpenGL.
 * The camera can fly, but cannot go below the ground.
 * Inspired by:
 * https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/camera.h
 */
#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

/** Defines several possible options for camera movement.
 * Used as abstraction to stay away from window-system specific input methods.
 */
enum class cameraMovement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

/**
* Processes mouse and keyboard input and updates the camera accordingly.
*/
class Camera {
public:
	Camera();

	/* Processes input received from any keyboard-like input system.
	 * Accepts input parameter in the form of camera defined by
	 * ENUM (to abstract it from window-system specific inputs).
	 * \param direction - the direction of the key that was pressed
	 * \param deltaTime - time between current frame and last frame
	 */
	void processKeyboard(cameraMovement direction, float deltaTime);

	/* Processes input received from a mouse input system based on the
	 * x and y offset of the mouse.
	 * ENUM (to abstract it from window-system specific inputs).
	 * \param xoffset - offset on the x axis of mouse
	 * \param yoffset - offset on the y axis of mouse
	 * \param constrainPitch - whether pitch should be constrained to
	 *						   prevent the screen from flipping or not
	 */
	void processMouseMovement(float xoffset, float yoffset,
		GLboolean constrainPitch = true);

	glm::vec3 getCamForward();
	glm::vec3 getCamPosition();

	/* Calculates the forward vector from the Camera's (updated) Euler Angles.
	*/
	void updateCameraVectors();

	// Angles are Euler Angles

	/* 
	* Yaw is initialized to -90.0 degrees since a yaw of 0.0
	* results in a direction vector pointing to the right so
	* we initially rotate a bit to the left.
	 */
	float yaw = -90.f;
	float pitch = 0.0f;

	// Camera options
	float movementSpeed = 2.5f;
	float sprintSpeed = movementSpeed * 10;
	bool sprinting = false;

	float mouseSensitivity = 0.05f;
	float groundY = 0.0f;

	// Camera Attributes
	glm::vec3 camForward = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 camPosition = glm::vec3(0.0f, groundY, 0.0f);
};

#endif