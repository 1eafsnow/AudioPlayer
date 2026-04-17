#include "Time.h"

int64_t GetTime()
{
	auto time = std::chrono::system_clock::now();
	auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(time);
	return ms.time_since_epoch().count();
}