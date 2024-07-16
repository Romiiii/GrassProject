#include "logger.h"

#include <cstdarg>
#include <chrono>

#define NOGDI
#include <windows.h>
#include <cassert>

#include <GLFW/glfw3.h>


namespace Logger
{
	static Log logger;

	const char* severityToString(Severity severity);
	std::string getFilename(const std::string const& path);

	void init(const char* path)
	{
		logger.file = fopen(path, "w");
		logger.path = path;
	}

	void deinit()
	{
		fclose(logger.file);
	}

	void _log(Severity severity, const char* file, int line, const char* message...)
	{
		assert(logger.file);

		// Logging format
		// Example "24-07-15 10:44:12.0503 | INFO | FileRenderer.cpp:134 | Finished initializing renderer"

		const char* format = "%04d-%02d-%02d %02d:%02d:%02d.%04d | %s | %s:%d | %s\n";
		const char* severityText = severityToString(severity);
		DateTime now = nowDateTime();
		std::string filename = getFilename(file);

		va_list arguments;
		va_start(arguments, message);

		int msgLength = vsnprintf(nullptr, 0, message, arguments);
		auto msgBuffer = new char[msgLength + 1];
		vsnprintf(msgBuffer, msgLength + 1, message, arguments);

		va_end(arguments);
		auto length = snprintf(nullptr, 0, format,
			now.year, now.month, now.day, now.hour, now.minute, now.second, now.millisecond,
			severityText,
			filename.c_str(), line,
			msgBuffer);

		// Avoid allocations idiot
		auto mainBuffer = new char[length + 1];

		snprintf(mainBuffer, length + 1, format,
			now.year, now.month, now.day, now.hour, now.minute, now.second, now.millisecond,
			severityText,
			filename.c_str(), line,
			msgBuffer);

		Entry entry{};
		entry.message = std::string(msgBuffer);
		entry.time = now;
		entry.severity = severity;
		entry.lineNumber = line;
		entry.file = std::string(filename);


		// Output
		OutputDebugString(mainBuffer);
		printf(mainBuffer);
		fputs(mainBuffer, logger.file);
		logger.entries.push_back(entry);


		delete[] mainBuffer;
		delete[] msgBuffer;
	}

	std::string getFilename(const std::string const& path)
	{
		return path.substr(path.find_last_of("/\\") + 1);
	}

	const char* severityToString(Severity severity)
	{
		switch (severity)
		{
		case Severity::DEBUG:
			return "DEBUG";
		case Severity::INFO:
			return "INFO";
		case Severity::WARNING:
			return "WARN";
		case Severity::ERROR:
			return "ERROR";
		case Severity::FATAL:
			return "FATAL";
		default:
			break;
		}
	}
}
