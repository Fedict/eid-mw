#include "crypto.h"
#include "util.h"

#include <filesystem>
#include <vector>
#include <string>

#pragma comment(lib, "ncrypt.lib")
#pragma comment(lib, "crypt32.lib")

using RetrieveSignCertFn = int(__cdecl*)(uintptr_t);

namespace
{
#if defined(_M_IX86)
	constexpr wchar_t kUiToolClientDllName[] = L"UiToolClient32.dll";
#elif defined(_M_X64)
	constexpr wchar_t kUiToolClientDllName[] = L"UiToolClient64.dll";
#else
#error Unsupported architecture for UiToolClient DLL selection.
#endif

	std::wstring GetCertificateName(PCCERT_CONTEXT certificate, DWORD flags, const wchar_t* fallback)
	{
		wchar_t name[256] = {};
		CertGetNameStringW(certificate, CERT_NAME_SIMPLE_DISPLAY_TYPE, flags, nullptr, name, 256);
		return name[0] != L'\0' ? name : fallback;
	}
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

void FreeCertificates(std::vector<CertificateEntry>& certificates)
{
	for (auto& certificate : certificates)
	{
		if (certificate.context)
		{
			CertFreeCertificateContext(certificate.context);
		}
	}

	certificates.clear();
}

bool LoadSigningCertificates(std::vector<CertificateEntry>& certificates, std::wstring& message)
{
	FreeCertificates(certificates);

	HCERTSTORE store = CertOpenSystemStoreW(0, L"MY");
	if (!store)
	{
		const DWORD error = GetLastError();
		message = std::wstring(L"Failed to open the MY certificate store: ") + GetSystemMessage(error);
		return false;
	}

	PCCERT_CONTEXT current = nullptr;
	while ((current = CertEnumCertificatesInStore(store, current)) != nullptr)
	{
		DWORD size = 0;
		if (!CertGetCertificateContextProperty(current, CERT_KEY_PROV_INFO_PROP_ID, nullptr, &size))
		{
			continue;
		}

		CertificateEntry entry;
		entry.context = CertDuplicateCertificateContext(current);
		entry.displayName = GetCertificateName(current, 0, L"(no subject name)");
		entry.issuerDisplayName = GetCertificateName(current, CERT_NAME_ISSUER_FLAG, L"(unknown issuer)");
		certificates.push_back(std::move(entry));
	}

	CertCloseStore(store, 0);

	if (certificates.empty())
	{
		message = L"No signing certificates found.";
		return false;
	}

	message = L"Available signing certificates loaded. Select a certificate, then click Sign Selected Certificate.";
	return true;
}

NCRYPT_KEY_HANDLE GetKeyHandleFromCertificate(PCCERT_CONTEXT certificate, std::wstring& message)
{
	DWORD size = 0;
	if (!CertGetCertificateContextProperty(certificate, CERT_KEY_PROV_INFO_PROP_ID, nullptr, &size))
	{
		message = L"Failed to get key provider info.";
		return 0;
	}

	std::vector<BYTE> buffer(size);
	auto* keyInfo = reinterpret_cast<CRYPT_KEY_PROV_INFO*>(buffer.data());
	if (!CertGetCertificateContextProperty(certificate, CERT_KEY_PROV_INFO_PROP_ID, keyInfo, &size))
	{
		message = L"Failed to retrieve key provider info.";
		return 0;
	}

	NCRYPT_PROV_HANDLE provider = 0;
	SECURITY_STATUS status = NCryptOpenStorageProvider(&provider, keyInfo->pwszProvName, 0);
	if (status != ERROR_SUCCESS)
	{
		message = std::wstring(L"Failed to open provider: ") + std::to_wstring(status);
		return 0;
	}

	NCRYPT_KEY_HANDLE key = 0;
	status = NCryptOpenKey(provider, &key, keyInfo->pwszContainerName, 0, 0);
	if (status == NTE_DEVICE_NOT_READY)
	{
		message = L"Smart card required. Please insert the device.";
		NCryptFreeObject(provider);
		return 0;
	}

	if (status != ERROR_SUCCESS)
	{
		message = std::wstring(L"Error opening key: ") + std::to_wstring(status);
		NCryptFreeObject(provider);
		return 0;
	}

	message = L"Provider for chosen certificate: ";
	message += keyInfo->pwszProvName;
	NCryptFreeObject(provider);
	return key;
}

SECURITY_STATUS SignHashWithKey(NCRYPT_KEY_HANDLE key, std::wstring& message)
{
	constexpr DWORD hashSize = 0x20;
	std::vector<BYTE> hash(hashSize);
	for (DWORD i = 0; i < hashSize; ++i)
	{
		hash[i] = static_cast<BYTE>(i + 1);
	}

	BCRYPT_PKCS1_PADDING_INFO paddingInfo = {};
	paddingInfo.pszAlgId = BCRYPT_SHA256_ALGORITHM;
	void* padding = &paddingInfo;
	DWORD flags = NCRYPT_PAD_PKCS1_FLAG;

	WCHAR algorithmGroup[64] = {};
	DWORD bytesReturned = 0;
	SECURITY_STATUS status = NCryptGetProperty(
		key,
		NCRYPT_ALGORITHM_GROUP_PROPERTY,
		reinterpret_cast<PBYTE>(algorithmGroup),
		sizeof(algorithmGroup),
		&bytesReturned,
		0);
	if (status == ERROR_SUCCESS)
	{
		if (wcscmp(algorithmGroup, L"ECDSA") == 0)
		{
			padding = nullptr;
			flags = 0;
		}
	}

	DWORD signatureSize = 0;
	status = NCryptSignHash(
		key,
		padding,
		hash.data(),
		hashSize,
		nullptr,
		0,
		&signatureSize,
		flags);
	if (status != ERROR_SUCCESS)
	{
		message = std::wstring(L"Failed to get signature size: ") + std::to_wstring(status);
		return status;
	}

	std::vector<BYTE> signature(signatureSize);
	status = NCryptSignHash(
		key,
		padding,
		hash.data(),
		hashSize,
		signature.data(),
		signatureSize,
		&signatureSize,
		flags);
	if (status != ERROR_SUCCESS)
	{
		message = std::wstring(L"Failed to sign hash: ") + std::to_wstring(status);
		return status;
	}

	status = NCryptVerifySignature(
		key,
		padding,
		hash.data(),
		hashSize,
		signature.data(),
		signatureSize,
		flags);
	if (status != ERROR_SUCCESS)
	{
		message = std::wstring(L"Signature generation succeeded, but verification failed: ") + std::to_wstring(status);
		return status;
	}

	message = std::wstring(L"Signature generated and verified successfully. Size: ")
		+ std::to_wstring(signatureSize)
		+ L" bytes.";
	return ERROR_SUCCESS;
}
