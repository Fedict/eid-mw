#pragma once

#include <windows.h>
#include <wincrypt.h>
#include <ncrypt.h>

#include <string>
#include <vector>

struct CertificateEntry
{
	PCCERT_CONTEXT context = nullptr;
	std::wstring displayName;
	std::wstring issuerDisplayName;
};

bool CallRetrieveSignCert(HWND owner, std::wstring& message);
void FreeCertificates(std::vector<CertificateEntry>& certificates);
bool LoadSigningCertificates(std::vector<CertificateEntry>& certificates, std::wstring& message);
NCRYPT_KEY_HANDLE GetKeyHandleFromCertificate(PCCERT_CONTEXT certificate, std::wstring& message);
SECURITY_STATUS SignHashWithKey(NCRYPT_KEY_HANDLE key, std::wstring& message);
