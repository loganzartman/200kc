#include <stdlib.h>
#include "util.h"

float randfl(float a, float b) {
	return (float)rand()/(float)(RAND_MAX/(b-a)) + a;
}

float qsin(float x) {
	x *= 0.159155;
	x -= (int)x;

	float xx = x*x;
	float y = -6.87897;
	y = y*xx + 33.7755;
	y = y*xx - 72.5257;
	y = y*xx + 80.5874;
	y = y*xx - 41.2408;
	y = y*xx + 6.28077;
	return x*y;
}

float qcos(float x) {
	return qsin(x + 1.5708);
}