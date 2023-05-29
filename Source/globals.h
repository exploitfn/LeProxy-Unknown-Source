#pragma once
#include <Windows.h>

namespace Globals
{
	HWND hWnd = NULL;
	uintptr_t modBase = NULL;
	HANDLE processID = NULL;
	int killKey = VK_END;
}
