#pragma once

#include <cstdint>
#include <thread>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

inline uint32_t osSetThreadAffinityMask(std::thread& thread, uint32_t mask) {
#ifdef _WIN32
	DWORD_PTR dw = SetThreadAffinityMask(thread.native_handle(), mask);
	if (dw == 0) {
		DWORD dwErr = GetLastError();
		return dwErr;
	}
	return 0;
#endif // _WIN32
	return -1;
}