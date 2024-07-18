#include "input.h"

#include "util.h"

inline Inputs inputs;

inline bool isActionDown(int action)
{
	return action == GLFW_PRESS || action == GLFW_REPEAT;
}

void Inputs::init(GLFWwindow* window, int startFrame)
{
	frame = startFrame;
	glfwSetKeyCallback(window, keyCallback);
	glfwSetMouseButtonCallback(window, buttonCallback);
	glfwSetWindowUserPointer(window, this);
	glfwSetCursorPosCallback(window, mouseCallback);
	this->window = window;
}


void Inputs::advanceFrame()
{
	mouseDelta = {};
	frame++;
}

bool Inputs::keyIsHeld(int key)
{
	InputState state = getKeyInputState(key);

	return state.isUp;
}

bool Inputs::keyIsPressed(int key)
{
	InputState state = getKeyInputState(key);

	return !state.isUp && state.frameOfChange == frame;
}

bool Inputs::keyIsReleased(int key)
{
	InputState state = getKeyInputState(key);

	return state.isUp && state.frameOfChange == frame;
}

bool Inputs::buttonIsHeld(int button)
{
	InputState state = getButtonInputState(button);

	return !state.isUp;
}

bool Inputs::buttonIsPressed(int button)
{
	InputState state = getKeyInputState(button);

	return !state.isUp && state.frameOfChange == frame;
}

bool Inputs::buttonIsReleased(int button)
{
	InputState state = getKeyInputState(button);

	return state.isUp && state.frameOfChange == frame;
}

glm::vec2 Inputs::getMousePosition()
{
	return mousePosition;
}

glm::vec2 Inputs::getMouseDelta()
{
	return mouseDelta;
}

glm::vec2 Inputs::getNormalizedMousePosition()
{
	int width;
	int height;
	glfwGetFramebufferSize(window, &width, &height);
	glm::vec2 windowSize = { (float)width, (float)height };

	glm::vec2 cursorPos = mousePosition;

	cursorPos.x = (2.0f * mousePosition.x) / (float)width - 1.0f;
	cursorPos.y = 1.0f - (2.0f * mousePosition.y) / (float)height;

	return cursorPos;
}

void Inputs::keyCallback(GLFWwindow* window, int key, int scancode, int action, int modifiers)
{
	UNUSED(scancode);
	UNUSED(modifiers);

	auto inputs = (Inputs*)glfwGetWindowUserPointer(window);

	inputs->setKeyInputState(key, isActionDown(action));
}

void Inputs::setKeyInputState(int key, bool isDown)
{
	keys[key] = { key, frame, isDown };
}

InputState Inputs::getKeyInputState(int key)
{
	return keys[key];
}

void Inputs::buttonCallback(GLFWwindow* window, int button, int action, int modifiers)
{
	UNUSED(modifiers);

	auto inputs = (Inputs*)glfwGetWindowUserPointer(window);

	inputs->setButtonInputState(button, isActionDown(action));
}

void Inputs::setButtonInputState(int button, bool isDown)
{
	buttons[button] = { button, frame, isDown };
}

InputState Inputs::getButtonInputState(int button)
{
	return buttons[button];
}

void Inputs::mouseCallback(GLFWwindow* window, double x, double y)
{
	auto inputs = (Inputs*)glfwGetWindowUserPointer(window);

	glm::vec2 newPosition = { x, y };

	if (!inputs->isMousePositionInitialized)
	{
		inputs->mouseDelta = { 0, 0 };
		inputs->isMousePositionInitialized = true;
	}
	else
	{
		inputs->mouseDelta = newPosition - inputs->mousePosition;
		inputs->mouseDelta.y *= -1;
	}

	inputs->mousePosition = newPosition;
}


bool Inputs::cursorIsDisabled()
{
	int mode = glfwGetInputMode(window, GLFW_CURSOR);
	return mode == GLFW_CURSOR_DISABLED;
}

bool Inputs::cursorIsEnabled()
{
	int mode = glfwGetInputMode(window, GLFW_CURSOR);
	return mode == GLFW_CURSOR_NORMAL;
}

void Inputs::enableCursor()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Inputs::disableCursor()
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Inputs::toggleCursorEnabled()
{
	if (cursorIsDisabled()) {
		enableCursor();
	}
	else
	{
		disableCursor();
	}
}

Inputs& Inputs::instance()
{
	return inputs;
}