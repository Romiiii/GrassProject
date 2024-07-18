#ifndef UTIL_H
#define UTIL_H

#include <chrono>

/*
 * Generate a random number between lower and upper.
 */
inline float generateRandomNumber(float lower, float upper) {
	return rand() / (float)RAND_MAX * (upper - lower) + lower;
}

struct DateTime
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
	int millisecond;
};

inline DateTime nowDateTime() {
	using namespace std::chrono;

	auto duration = system_clock::now().time_since_epoch();

	auto t = std::time(0);
	auto now = std::localtime(&t);

	DateTime dateTime{};
	dateTime.year = now->tm_year + 1900;
	dateTime.month = now->tm_mon + 1;
	dateTime.day = now->tm_mday;
	dateTime.hour = now->tm_hour;
	dateTime.minute = now->tm_min;
	dateTime.second = now->tm_sec;
	dateTime.millisecond = duration_cast<milliseconds>(duration).count() % 1000;

	return dateTime;
}

#define UNUSED(x) (void)(x)
#endif