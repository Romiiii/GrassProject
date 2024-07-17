#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>
#include "util.h"

namespace Logger
{
	enum class Severity
	{
		DEBUG,
		INFO,
		WARNING,
		ERROR,
		FATAL
	};

	struct Entry
	{
		Severity severity;
		std::string file;
		int lineNumber;
		std::string message;
		DateTime time;
	};

	struct Log
	{
		std::string path;
		FILE* file;
		std::vector<Entry> entries;
	};

	void _log(Severity severity, const char* file, int line, const char* message...);
	void init(const char* path);
	void deinit();
}



#if _DEBUG
#define LOG_DEBUG(message) Logger::_log(Logger::Severity::DEBUG,  __FILE_, __LINE__, message)
#else
#define LOG_DEBUG(message)
#endif

#define LOG_INFO(...) Logger::_log(Logger::Severity::INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...) Logger::_log(Logger::Severity::WARNING,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) Logger::_log(Logger::Severity::ERROR,  __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) Logger::_log(Logger::Severity::FATAL,  __FILE__, __LINE__, __VA_ARGS__)

#endif