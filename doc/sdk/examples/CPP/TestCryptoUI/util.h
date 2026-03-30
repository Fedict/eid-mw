#pragma once

#include <windows.h>

#include <string>

struct ServiceState
{
	bool installed = false;
	bool enabled = false;
	bool running = false;
	DWORD error = ERROR_SUCCESS;
	std::wstring message;
};

std::wstring GetExecutableDirectory();
std::wstring GetSystemMessage(DWORD error);
ServiceState QueryCertificatePropagationService();
