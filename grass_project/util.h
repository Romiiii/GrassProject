#ifndef UTIL_H
#define UTIL_H

/*
 * Generate a random number between lower and upper.
 */
inline float generateRandomNumber(float lower, float upper) {
	return rand() / (float)RAND_MAX * (upper - lower) + lower;
}

#endif