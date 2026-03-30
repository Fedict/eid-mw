Retrieval of a (new) Remote Signing Certificate
=======================================================

For retrieving a remote signing certificate, a user can use the BeIDSignApp in the notification area to create an account and retrieve a certificate.
For integrators that want to provide these options to their customers, there is a library that can be integrated easily.
There is only one (synchronous) function to be called, retrieve_sign_cert() from a library we provide called UiToolClient.dll.

The owner HWND is for the handle to the parent window 

see crypto.cpp for base CSP sample signing

namespace
{
#if defined(_M_IX86)
	constexpr wchar_t kUiToolClientDllName[] = L"UiToolClient32.dll";
#elif defined(_M_X64)
	constexpr wchar_t kUiToolClientDllName[] = L"UiToolClient64.dll";
#else
#error Unsupported architecture for UiToolClient DLL selection.
#endif
}

bool CallRetrieveSignCert(HWND owner, std::wstring& message)
{
	const std::wstring dllPath =
		std::filesystem::path(GetExecutableDirectory()).append(kUiToolClientDllName).wstring();
	HMODULE dll = LoadLibraryW(dllPath.c_str());
	if (!dll)
	{
		const DWORD error = GetLastError();
		message = std::wstring(L"Failed to load ") + dllPath + L": " + GetSystemMessage(error);
		return false;
	}

	auto retrieveSignCert = reinterpret_cast<RetrieveSignCertFn>(GetProcAddress(dll, "retrieve_sign_cert"));
	if (!retrieveSignCert)
	{
		message = std::wstring(L"Failed to find retrieve_sign_cert in ") + kUiToolClientDllName + L".";
		FreeLibrary(dll);
		return false;
	}

	const int rc = retrieveSignCert(reinterpret_cast<uintptr_t>(owner));
	FreeLibrary(dll);

	message = std::wstring(L"retrieve_sign_cert returned: ") + std::to_wstring(rc);
	return true;
}


How it works:
===============
This retrieve_sign_cert sends a message to the installed BeIDSignApp that is installed as part of the new eID Middleware.

First, the BeIdSignApp process will ask the user to login/authenticate using a valid BE eID card.
At this stage a check is also done if the user already has a valid remote signing account.

If no account is found yet for the user, the enrollment process will start and the user will be asked to sign an agreement first.

If an account already exists, or after an account is created, a signing certificate will be issued and BeIDSignApp process will download the certificate and install it in the “MY" certificate store in windows.

The application can then continue and use Base CSP or PKS11 to sign a hash, or Adobe can be used to sign a PDF document.
On windows, you can also use the virtual card that has become present after this call succeeded and returned.


