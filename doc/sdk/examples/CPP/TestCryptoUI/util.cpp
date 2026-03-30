#include "util.h"

#include <filesystem>
#include <vector>

std::wstring GetExecutableDirectory()
{
	std::vector<wchar_t> buffer(MAX_PATH, L'\0');
	DWORD length = 0;

	for (;;)
	{
		length = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
		if (length == 0)
		{
			return L"";
		}

		if (length < buffer.size() - 1)
		{
			break;
		}

		buffer.resize(buffer.size() * 2);
	}

	return std::filesystem::path(buffer.data()).parent_path().wstring();
}

std::wstring GetSystemMessage(DWORD error)
{
	wchar_t* buffer = nullptr;
	const DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
	const DWORD length = FormatMessageW(flags, nullptr, error, 0, reinterpret_cast<LPWSTR>(&buffer), 0, nullptr);
	if (length == 0 || buffer == nullptr)
	{
		return L"Unknown error";
	}

	std::wstring message(buffer, length);
	LocalFree(buffer);

	while (!message.empty() && (message.back() == L'\r' || message.back() == L'\n'))
	{
		message.pop_back();
	}

	return message;
}

ServiceState QueryCertificatePropagationService()
{
	ServiceState state;

	SC_HANDLE serviceManager = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CONNECT);
	if (!serviceManager)
	{
		state.error = GetLastError();
		state.message = std::wstring(L"Failed to open Service Control Manager: ") + GetSystemMessage(state.error);
		return state;
	}

	SC_HANDLE service = OpenServiceW(serviceManager, L"CertPropSvc", SERVICE_QUERY_CONFIG | SERVICE_QUERY_STATUS);
	if (!service)
	{
		state.error = GetLastError();
		if (state.error == ERROR_SERVICE_DOES_NOT_EXIST)
		{
			state.message = L"Certificate Propagation service (CertPropSvc) is not installed.";
		}
		else
		{
			state.message = std::wstring(L"Failed to open Certificate Propagation service: ") + GetSystemMessage(state.error);
		}

		CloseServiceHandle(serviceManager);
		return state;
	}

	state.installed = true;

	DWORD bytesNeeded = 0;
	std::vector<BYTE> configBuffer(2048);
	if (QueryServiceConfigW(service,
		reinterpret_cast<LPQUERY_SERVICE_CONFIGW>(configBuffer.data()),
		static_cast<DWORD>(configBuffer.size()),
		&bytesNeeded))
	{
		const auto* config = reinterpret_cast<const QUERY_SERVICE_CONFIGW*>(configBuffer.data());
		state.enabled = config->dwStartType != SERVICE_DISABLED;
	}
	else
	{
		state.error = GetLastError();
		state.message = L"Failed to query Certificate Propagation service configuration: "
			+ GetSystemMessage(state.error);
	}

	SERVICE_STATUS_PROCESS serviceStatus = {};
	bytesNeeded = 0;
	if (QueryServiceStatusEx(service,
		SC_STATUS_PROCESS_INFO,
		reinterpret_cast<LPBYTE>(&serviceStatus),
		sizeof(serviceStatus),
		&bytesNeeded))
	{
		state.running = serviceStatus.dwCurrentState == SERVICE_RUNNING;
	}
	else
	{
		state.error = GetLastError();
		state.message = L"Failed to query Certificate Propagation service status: "
			+ GetSystemMessage(state.error);
	}

	if (state.message.empty())
	{
		state.message = L"Certificate Propagation service (CertPropSvc): ";
		state.message += state.enabled ? L"enabled" : L"disabled";
		state.message += L", ";
		state.message += state.running ? L"running" : L"not running";
	}

	CloseServiceHandle(service);
	CloseServiceHandle(serviceManager);
	return state;
}
