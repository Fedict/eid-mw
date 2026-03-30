#pragma once

struct FaqEntry
{
	const wchar_t* title;
	const wchar_t* body;
};

inline constexpr FaqEntry kFaqEntries[] =
{
	{ L"How do I start?", L"This is a test application to test your remote signing certificate linked to your eID card or eID wallet using Base CSP." },
	{
		L"What does retrieve sign cert do?",
		L"Remote signing certificates linked to your eID/Wallet have a validity of max. 24h. In order to retrieve a new signing certificate, you need a valid signing account first. "
		L"When retrieving a new certificate, the new eID Middleware will ask you to authenticate first using your eID/Wallet. "
		L"If you have no valid account yet, you will be guided to create one and agree to the terms and conditions. "
		L"A new signing certificate will be created for you and downloaded to your system. "
	},
	{
		L"Why do I need to authenticate every day before I can use my remote signing key?",
		L"Your remote signing certificate expires after 24h. In order to retrieve and use your remote signing key, you need to authenticate yourself using your eID/Wallet. "
		L"After you have authenticated successfully, your login session is valid for 8 hours as long as you do not restart your system. "
		L"As long as your authenticated session is valid, you can use your remote signing key, but you still have to give your consent for each signature using your eID card/wallet. "
	},
	{
		L"Why do certificates appear here?",
		L"After you retrieve a signing certificate, the application searches the Windows personal certificate store and lists the certificates that have an associated private key. "
		L"These are the certificates that can potentially be used for signing with the Base CSP key that belongs to the selected certificate. "
		L"If no certificates are shown, it may mean that no valid signing certificate is currently available on the system."
	},
	{
		L"What if the certificate propagation service is not running?",
		L"If the Certificate Propagation service is not running, Windows may not automatically make your remote signing certificate available in the certificate store. "
		L"In that case, the application (using Base CSP) may not show the expected signing certificates. "
		L"Try starting the service first and then retrieve the signing certificate again before attempting to sign."
	},
	{
		L"What about pkcs11? (or java->sun-pkc11)",
		L"When using the eID pkcs11 library on windows (c:\\windows\\system32\\beidpkcs11.dll), you still need to retrieve a remote signing certificate first, but it does not need to be propagated to the windows certificate store, since you can load it directly from PKCS11 "
	}
};
