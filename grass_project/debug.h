#ifndef DEBUG_H
#define DEBUG_H 

#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <cstdarg>
#include "logger.h"

/**
 * \brief An assert.
 */
#if _DEBUG
#define ASSERT(x) if (!(x)) {__debugbreak();} else{}
#else
#define ASSERT(X)
#endif

 /**
 * \brief Wraps an OpenGL call in error handling, for debugging.
 */
#if _DEBUG
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
#define GLCall(x) x
#endif

 /**
  * \brief Clears the OpenGL error stack.
  */
static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

/**
 * \brief A lookup table for converting error codes to readable names.
 * Remember to subtract the base value (0x500) to get to the correct index.
 */
static const char* errorCodeToErrorNameLookupTable[] = {
	"GL_INVALID_ENUM",
	"GL_INVALID_VALUE",
	"GL_INVALID_OPERATION",
	"GL_STACK_OVERFLOW",
	"GL_STACK_UNDERFLOW",
	"GL_OUT_OF_MEMORY",
	"GL_INVALID_FRAMEBUFFER_OPERATION",
	"GL_CONTEXT_LOST"
};

/**
* \brief Reports all OpenGL errors.
* \param function The name of the function that is being tested
* \param file The file containing the tested function
* \param line The line number of the error
* \return True if there were no errors, false otherwise
*/
static bool GLLogCall(const char* function, const char* file, int line)
{
	bool hadErrors = false;
	while (GLenum error = glGetError())
	{
		hadErrors = true;
		bool hasErrorLookup = 0x0500 <= error && error <= 0x507;
		const char* errorStr = hasErrorLookup ?
			errorCodeToErrorNameLookupTable[error - 0x0500] :
			std::to_string(error).c_str();

		LOG_ERROR("[OpenGL] %s:%s (%s) %s", errorStr, error);
	}
	return !hadErrors;
}


inline std::vector<std::string> g_debugStrings;

inline void debugText(const char* fmt...)
{
	va_list args;
	va_start(args, fmt);

	int size = vsnprintf(nullptr, 0, fmt, args);
	char* buffer = new char[size + 1];
	memset(buffer, 0, (size_t)size + 1);
	vsnprintf(buffer, (size_t)size + 1, fmt, args);

	va_end(args);

	g_debugStrings.push_back(std::string(buffer));
	delete[] buffer;
}

#endif