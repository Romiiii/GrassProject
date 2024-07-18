#ifndef INPUT_H
#define	INPUT_H

#include <GLFW/glfw3.h>

#include <array>
#include <glm/glm.hpp>

struct InputState
{
	int keyOrButton;
	int frameOfChange;
	bool isUp;
};

class Inputs {
public:
	void init(GLFWwindow* window, int startFrame);
	void advanceFrame();

	bool keyIsHeld(int key);
	bool keyIsPressed(int key);
	bool keyIsReleased(int key);

	bool buttonIsHeld(int button);
	bool buttonIsPressed(int button);
	bool buttonIsReleased(int button);

	glm::vec2 getMousePosition();
	glm::vec2 getMouseDelta();
	glm::vec2 getNormalizedMousePosition();

	bool cursorIsDisabled();
	bool cursorIsEnabled();

	void enableCursor();
	void disableCursor();
	void toggleCursorEnabled();

	static Inputs& instance();


private:
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int modifiers);
	void setKeyInputState(int key, bool isDown);
	InputState getKeyInputState(int key);

	static void buttonCallback(GLFWwindow* window, int button, int action, int modifiers);
	void setButtonInputState(int button, bool isDown);
	InputState getButtonInputState(int button);

	static void mouseCallback(GLFWwindow* window, double x, double y);

	GLFWwindow* window;
	int frame = 0;
	std::array<InputState, GLFW_KEY_LAST> keys{};
	std::array<InputState, GLFW_MOUSE_BUTTON_LAST> buttons{};
	bool isMousePositionInitialized = false;
	glm::vec2 mousePosition;
	glm::vec2 mouseDelta;
};

#endif // !INPUT_H
