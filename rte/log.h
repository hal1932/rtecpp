#pragma once
#include <Windows.h>
#include <cstdio>
#include <string>
#include "common.h"

namespace rte {

inline void _error(const char* msg, const char* file, int line)
{
	static char sBuffer[1024];
	_snprintf_s(sBuffer, sizeof(sBuffer), _TRUNCATE, "[ERROR](%s: %d) %s\n", basename(file).c_str(), line, msg);
	OutputDebugStringA(sBuffer);
}

inline void _error(int code, const char* file, int line)
{
	LPVOID lpMessageBuffer;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMessageBuffer,
		0,
		NULL);
	_error(static_cast<const char*>(lpMessageBuffer), file, line);
	LocalFree(lpMessageBuffer);
}
#define log_error(msg) _error(msg, __FILE__, __LINE__)

}// namespace rte
