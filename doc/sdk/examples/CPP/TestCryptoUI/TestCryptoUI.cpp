#include <windows.h>
#include <dwmapi.h>
#include <commctrl.h>
#include <wincrypt.h>
#include <cryptuiapi.h>
#include <ncrypt.h>

#include "crypto.h"
#include "faq_entries.h"
#include "util.h"

#include <cwchar>
#include <string>
#include <thread>
#include <vector>

#pragma comment(lib, "ncrypt.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "cryptui.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "comctl32.lib")

#if defined(_M_X64)
#pragma comment(linker, \
	"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined(_M_IX86)
#pragma comment(linker, \
	"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined(_M_ARM64)
#pragma comment(linker, \
	"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='arm64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker, \
	"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

LRESULT CALLBACK FaqWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ThemedResultDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ThemedChoiceDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

namespace
{
	constexpr int kControlMargin = 16;
	constexpr int kTopButtonGap = 12;
	constexpr int kButtonHeight = 30;
	constexpr int kCertificateListWidth = 700;
	constexpr int kCertificateListTop = 136;
	constexpr int kCertificateListHeight = 256;
	constexpr int kLogWidth = 700;
	constexpr int kLogHeight = 152;
	constexpr int kBottomButtonGap = 16;
	constexpr int kBottomButtonWidth = (kLogWidth - kBottomButtonGap) / 2;
	constexpr int kBottomButtonTop = kCertificateListTop + kCertificateListHeight + 16;
	constexpr int kLogTop = kBottomButtonTop + kButtonHeight + 14;
	constexpr int kBusyIndicatorWidth = kLogWidth;
	constexpr int kBusyIndicatorHeight = 14;
	constexpr int kBusyIndicatorTop = kLogTop + kLogHeight + 10;
	constexpr int kServiceWarningIconSize = 16;
	constexpr int kClientWidth = kLogWidth + (kControlMargin * 2);
	constexpr int kClientHeight = kBusyIndicatorTop + kBusyIndicatorHeight + kControlMargin;
	constexpr COLORREF kColorBackground = RGB(12, 30, 58);
	constexpr COLORREF kColorSurface = RGB(20, 44, 79);
	constexpr COLORREF kColorLogSurface = RGB(14, 36, 66);
	constexpr COLORREF kColorSurfaceBorder = RGB(53, 88, 134);
	constexpr COLORREF kColorButton = RGB(47, 95, 160);
	constexpr COLORREF kColorButtonPressed = RGB(35, 73, 125);
	constexpr COLORREF kColorButtonDisabled = RGB(57, 72, 96);
	constexpr COLORREF kColorTextPrimary = RGB(234, 242, 255);
	constexpr COLORREF kColorTextLog = RGB(200, 216, 240);
	constexpr COLORREF kColorTextMuted = RGB(178, 197, 228);
	constexpr COLORREF kColorTextIssuer = RGB(132, 148, 170);
	constexpr UINT_PTR kRetrieveTimeoutTimerId = 1;
	constexpr UINT_PTR kBusyAnimationTimerId = 2;
	constexpr UINT kRetrieveTimeoutMs = 60000;
	constexpr UINT kBusyAnimationIntervalMs = 40;
	constexpr UINT WM_APP_RETRIEVE_COMPLETE = WM_APP + 1;
	constexpr UINT WM_APP_SIGN_COMPLETE = WM_APP + 2;
	constexpr int kTopButtonCount = 4;
	constexpr int kTopButtonWidth = (kClientWidth - (kControlMargin * 2) - (kTopButtonGap * (kTopButtonCount - 1))) / kTopButtonCount;
	constexpr int kFaqHorizontalMargin = 18;
	constexpr int kFaqContentWidth = 700;
	constexpr int kFaqWindowWidth = kFaqContentWidth + (kFaqHorizontalMargin * 2) + 24;
	constexpr int kResultDialogHorizontalMargin = 18;
	constexpr int kResultDialogContentWidth = 500;
	constexpr int kResultDialogButtonWidth = 100;

	enum ControlId
	{
		IdRefreshService = 1001,
		IdRetrieveSignCert = 1002,
		IdPrepareSign = 1003,
		IdCertificateList = 1004,
		IdSignSelected = 1005,
		IdStatusLog = 1006,
		IdServiceStatus = 1007,
		IdCertificateLabel = 1008,
		IdBusyIndicator = 1009,
		IdFaq = 1010,
		IdResultDialogOk = 1011,
		IdChoiceDialogPrimary = 1012,
		IdChoiceDialogSecondary = 1013,
		IdViewCertificate = 1014,
		IdServiceWarningIcon = 1015
	};

	struct AppState
	{
		HWND hwnd = nullptr;
		HWND refreshServiceButton = nullptr;
		HWND retrieveSignCertButton = nullptr;
		HWND reloadCertificatesButton = nullptr;
		HWND faqButton = nullptr;
		HWND busyIndicator = nullptr;
		HWND serviceStatus = nullptr;
		HWND serviceWarningIcon = nullptr;
		HWND certificateList = nullptr;
		HWND signSelectedButton = nullptr;
		HWND viewCertificateButton = nullptr;
		HWND statusLog = nullptr;
		HFONT headingFont = nullptr;
		HFONT bodyFont = nullptr;
		HFONT smallBodyFont = nullptr;
		HBRUSH backgroundBrush = nullptr;
		HBRUSH surfaceBrush = nullptr;
		HBRUSH logBrush = nullptr;
		HBRUSH buttonBrush = nullptr;
		HBRUSH buttonPressedBrush = nullptr;
		HBRUSH buttonDisabledBrush = nullptr;
		HBRUSH borderBrush = nullptr;
		int busyAnimationOffset = 0;
		bool retrieveInProgress = false;
		bool retrieveDetached = false;
		bool retrieveTimedOut = false;
		UINT retrieveRequestId = 0;
		bool signInProgress = false;
		bool signDetached = false;
		bool signTimedOut = false;
		UINT signRequestId = 0;
		std::vector<CertificateEntry> certificates;
	};

	struct RetrieveSignCertResult
	{
		UINT requestId = 0;
		bool success = false;
		std::wstring message;
	};

	struct SignHashResult
	{
		UINT requestId = 0;
		bool success = false;
		std::wstring message;
	};

	struct ResultDialogData
	{
		std::wstring title;
		std::wstring message;
		HWND owner = nullptr;
	};

	struct ChoiceDialogData
	{
		std::wstring title;
		std::wstring message;
		std::wstring primaryButtonText;
		std::wstring secondaryButtonText;
		HWND owner = nullptr;
		int result = IDCANCEL;
	};

	AppState g_app;

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#ifndef DWMWA_BORDER_COLOR
#define DWMWA_BORDER_COLOR 34
#endif

#ifndef DWMWA_CAPTION_COLOR
#define DWMWA_CAPTION_COLOR 35
#endif

#ifndef DWMWA_TEXT_COLOR
#define DWMWA_TEXT_COLOR 36
#endif

	HFONT CreateUiFont(HWND hwnd, int pointSize, int weight, const wchar_t* faceName)
	{
		HDC dc = GetDC(hwnd);
		const int dpi = dc ? GetDeviceCaps(dc, LOGPIXELSY) : 96;
		if (dc)
		{
			ReleaseDC(hwnd, dc);
		}

		return CreateFontW(
			-MulDiv(pointSize, dpi, 72),
			0,
			0,
			0,
			weight,
			FALSE,
			FALSE,
			FALSE,
			DEFAULT_CHARSET,
			OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY,
			VARIABLE_PITCH,
			faceName);
	}

	void AppendLog(const std::wstring& line)
	{
		if (!g_app.statusLog)
		{
			return;
		}

		const int textLength = GetWindowTextLengthW(g_app.statusLog);
		SendMessageW(g_app.statusLog, EM_SETSEL, textLength, textLength);

		std::wstring text = line;
		if (!text.empty() && text.back() != L'\n')
		{
			text += L"\r\n";
		}

		SendMessageW(g_app.statusLog, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(text.c_str()));
	}

	bool IsAsyncOperationInProgress()
	{
		return g_app.retrieveInProgress || g_app.signInProgress;
	}

	bool HasDetachedAsyncOperation()
	{
		return g_app.retrieveDetached || g_app.signDetached;
	}

	const wchar_t* GetDetachedOperationName()
	{
		if (g_app.retrieveDetached)
		{
			return L"retrieve_sign_cert";
		}

		if (g_app.signDetached)
		{
			return L"test hash signature";
		}

		return L"background operation";
	}

	void SetServiceLabel(const std::wstring& text)
	{
		if (g_app.serviceStatus)
		{
			SetWindowTextW(g_app.serviceStatus, text.c_str());
		}

		if (g_app.serviceStatus && g_app.serviceWarningIcon && g_app.hwnd)
		{
			RECT labelRect = {};
			GetWindowRect(g_app.serviceStatus, &labelRect);
			MapWindowPoints(HWND_DESKTOP, g_app.hwnd, reinterpret_cast<LPPOINT>(&labelRect), 2);

			HDC dc = GetDC(g_app.serviceStatus);
			SIZE textSize = {};
			if (dc)
			{
				HFONT font = reinterpret_cast<HFONT>(SendMessageW(g_app.serviceStatus, WM_GETFONT, 0, 0));
				HGDIOBJ oldFont = nullptr;
				if (font)
				{
					oldFont = SelectObject(dc, font);
				}

				GetTextExtentPoint32W(dc, text.c_str(), static_cast<int>(text.size()), &textSize);
				if (oldFont)
				{
					SelectObject(dc, oldFont);
				}

				ReleaseDC(g_app.serviceStatus, dc);
			}

			const int iconX = min(labelRect.left + textSize.cx + 6, kClientWidth - kControlMargin - kServiceWarningIconSize);
			const int iconY = labelRect.top + ((labelRect.bottom - labelRect.top) - kServiceWarningIconSize) / 2;
			SetWindowPos(
				g_app.serviceWarningIcon,
				nullptr,
				iconX,
				iconY,
				kServiceWarningIconSize,
				kServiceWarningIconSize,
				SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	void SetServiceWarningIconVisible(bool visible)
	{
		if (!g_app.serviceWarningIcon)
		{
			return;
		}

		ShowWindow(g_app.serviceWarningIcon, visible ? SW_SHOW : SW_HIDE);
		InvalidateRect(g_app.serviceWarningIcon, nullptr, TRUE);
		UpdateWindow(g_app.serviceWarningIcon);
	}

	void UpdateSignButtonState();

	void UpdateActionButtons()
	{
		const bool busy = IsAsyncOperationInProgress();

		if (g_app.refreshServiceButton)
		{
			EnableWindow(g_app.refreshServiceButton, !busy);
		}

		if (g_app.retrieveSignCertButton)
		{
			EnableWindow(g_app.retrieveSignCertButton, !busy);
		}

		if (g_app.reloadCertificatesButton)
		{
			EnableWindow(g_app.reloadCertificatesButton, !busy);
		}

		if (g_app.faqButton)
		{
			EnableWindow(g_app.faqButton, TRUE);
		}

		if (g_app.busyIndicator)
		{
			ShowWindow(g_app.busyIndicator, busy ? SW_SHOW : SW_HIDE);
			if (busy)
			{
				SetTimer(g_app.hwnd, kBusyAnimationTimerId, kBusyAnimationIntervalMs, nullptr);
			}
			else
			{
				KillTimer(g_app.hwnd, kBusyAnimationTimerId);
				g_app.busyAnimationOffset = 0;
				InvalidateRect(g_app.busyIndicator, nullptr, TRUE);
			}
		}

		UpdateSignButtonState();
	}

	bool ConfirmCloseIfBusy(HWND hwnd)
	{
		if (!IsAsyncOperationInProgress() && !HasDetachedAsyncOperation())
		{
			return true;
		}

		const wchar_t* message = nullptr;
		if (g_app.retrieveInProgress)
		{
			message =
				L"retrieve_sign_cert is still running. Closing now will terminate the application while the external tray call is still in progress.\n\nDo you want to quit anyway?";
		}
		else if (g_app.retrieveDetached)
		{
			message =
				L"retrieve_sign_cert is still finishing in the background even though waiting was cancelled.\n\nDo you want to quit anyway?";
		}
		else if (g_app.signInProgress)
		{
			message =
				L"The test hash signature is still running. Closing now will terminate the application while the signing call is still in progress.\n\nDo you want to quit anyway?";
		}
		else
		{
			message =
				L"The test hash signature is still finishing in the background even though waiting was cancelled.\n\nDo you want to quit anyway?";
		}

		const int result = MessageBoxW(
			hwnd,
			message,
			L"Operation Still Running",
			MB_ICONWARNING | MB_YESNO | MB_DEFBUTTON2);
		return result == IDYES;
	}

	void CancelRetrieveWait()
	{
		KillTimer(g_app.hwnd, kRetrieveTimeoutTimerId);
		g_app.retrieveInProgress = false;
		g_app.retrieveDetached = true;
		g_app.retrieveTimedOut = false;
		UpdateActionButtons();
		AppendLog(L"Stopped waiting for retrieve_sign_cert. The external call may still finish in the background, and its result will be ignored.");
	}

	void CancelSignWait()
	{
		KillTimer(g_app.hwnd, kRetrieveTimeoutTimerId);
		g_app.signInProgress = false;
		g_app.signDetached = true;
		g_app.signTimedOut = false;
		UpdateActionButtons();
		AppendLog(L"Stopped waiting for the test hash signature. The signing call may still finish in the background, and its result will be ignored.");
	}

	void ApplyWindowTheme(HWND hwnd)
	{
		const BOOL darkMode = TRUE;
		const COLORREF captionColor = kColorBackground;
		const COLORREF textColor = kColorTextPrimary;
		const COLORREF borderColor = kColorSurfaceBorder;

		DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkMode, sizeof(darkMode));
		DwmSetWindowAttribute(hwnd, DWMWA_CAPTION_COLOR, &captionColor, sizeof(captionColor));
		DwmSetWindowAttribute(hwnd, DWMWA_TEXT_COLOR, &textColor, sizeof(textColor));
		DwmSetWindowAttribute(hwnd, DWMWA_BORDER_COLOR, &borderColor, sizeof(borderColor));
	}

	int MeasureWrappedTextHeight(HWND hwnd, const wchar_t* text, HFONT font, int width)
	{
		HDC dc = GetDC(hwnd);
		if (!dc)
		{
			return 42;
		}

		HGDIOBJ oldFont = SelectObject(dc, font ? font : GetStockObject(DEFAULT_GUI_FONT));
		RECT rect = { 0, 0, width, 0 };
		DrawTextW(dc, text, -1, &rect, DT_CALCRECT | DT_WORDBREAK);
		SelectObject(dc, oldFont);
		ReleaseDC(hwnd, dc);

		return (rect.bottom - rect.top) + 2;
	}

	void OpenFaqWindow(HWND owner)
	{
		const wchar_t faqClassName[] = L"TestCryptoUiFaqWindowClass";
		static bool classRegistered = false;
		if (!classRegistered)
		{
			WNDCLASSEXW faqClass = {};
			faqClass.cbSize = sizeof(faqClass);
			faqClass.lpfnWndProc = ::FaqWindowProc;
			faqClass.hInstance = GetModuleHandleW(nullptr);
			faqClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
			faqClass.hbrBackground = g_app.backgroundBrush;
			faqClass.lpszClassName = faqClassName;
			RegisterClassExW(&faqClass);
			classRegistered = true;
		}

		HWND existingWindow = FindWindowW(faqClassName, L"FAQ");
		if (existingWindow)
		{
			ShowWindow(existingWindow, SW_SHOW);
			SetForegroundWindow(existingWindow);
			return;
		}

		HWND faqWindow = CreateWindowExW(
			WS_EX_DLGMODALFRAME,
			faqClassName,
			L"FAQ",
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			kFaqWindowWidth,
			500,
			owner,
			nullptr,
			GetModuleHandleW(nullptr),
			nullptr);

		if (!faqWindow)
		{
			MessageBoxW(owner, L"Failed to open the FAQ window.", L"FAQ", MB_OK | MB_ICONERROR);
			return;
		}

		ApplyWindowTheme(faqWindow);
		ShowWindow(faqWindow, SW_SHOW);
		UpdateWindow(faqWindow);
	}

	void ShowThemedResultDialog(HWND owner, const std::wstring& title, const std::wstring& message)
	{
		const wchar_t dialogClassName[] = L"TestCryptoUiResultDialogClass";
		static bool classRegistered = false;
		if (!classRegistered)
		{
			WNDCLASSEXW dialogClass = {};
			dialogClass.cbSize = sizeof(dialogClass);
			dialogClass.lpfnWndProc = ::ThemedResultDialogProc;
			dialogClass.hInstance = GetModuleHandleW(nullptr);
			dialogClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
			dialogClass.hbrBackground = g_app.backgroundBrush;
			dialogClass.lpszClassName = dialogClassName;
			RegisterClassExW(&dialogClass);
			classRegistered = true;
		}

		auto* data = new ResultDialogData{ title, message, owner };
		HWND dialog = CreateWindowExW(
			WS_EX_DLGMODALFRAME,
			dialogClassName,
			title.c_str(),
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			580,
			220,
			owner,
			nullptr,
			GetModuleHandleW(nullptr),
			data);

		if (!dialog)
		{
			delete data;
			MessageBoxW(owner, message.c_str(), title.c_str(), MB_OK | MB_ICONINFORMATION);
			return;
		}

		ApplyWindowTheme(dialog);

		RECT ownerRect = {};
		RECT dialogRect = {};
		GetWindowRect(owner, &ownerRect);
		GetWindowRect(dialog, &dialogRect);
		const int dialogWidth = dialogRect.right - dialogRect.left;
		const int dialogHeight = dialogRect.bottom - dialogRect.top;
		const int x = ownerRect.left + ((ownerRect.right - ownerRect.left) - dialogWidth) / 2;
		const int y = ownerRect.top + ((ownerRect.bottom - ownerRect.top) - dialogHeight) / 2;
		SetWindowPos(dialog, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

		EnableWindow(owner, FALSE);
		ShowWindow(dialog, SW_SHOW);
		UpdateWindow(dialog);

		MSG msg = {};
		while (IsWindow(dialog) && GetMessageW(&msg, nullptr, 0, 0) > 0)
		{
			if (!IsDialogMessageW(dialog, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
		}

		if (IsWindow(owner))
		{
			EnableWindow(owner, TRUE);
			SetForegroundWindow(owner);
		}
	}

	int ShowThemedChoiceDialog(
		HWND owner,
		const std::wstring& title,
		const std::wstring& message,
		const std::wstring& primaryButtonText,
		const std::wstring& secondaryButtonText)
	{
		const wchar_t dialogClassName[] = L"TestCryptoUiChoiceDialogClass";
		static bool classRegistered = false;
		if (!classRegistered)
		{
			WNDCLASSEXW dialogClass = {};
			dialogClass.cbSize = sizeof(dialogClass);
			dialogClass.lpfnWndProc = ::ThemedChoiceDialogProc;
			dialogClass.hInstance = GetModuleHandleW(nullptr);
			dialogClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
			dialogClass.hbrBackground = g_app.backgroundBrush;
			dialogClass.lpszClassName = dialogClassName;
			RegisterClassExW(&dialogClass);
			classRegistered = true;
		}

		auto* data = new ChoiceDialogData{ title, message, primaryButtonText, secondaryButtonText, owner, IDCANCEL };
		HWND dialog = CreateWindowExW(
			WS_EX_DLGMODALFRAME,
			dialogClassName,
			title.c_str(),
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			700,
			250,
			owner,
			nullptr,
			GetModuleHandleW(nullptr),
			data);

		if (!dialog)
		{
			delete data;
			return IDCANCEL;
		}

		ApplyWindowTheme(dialog);

		RECT ownerRect = {};
		RECT dialogRect = {};
		GetWindowRect(owner, &ownerRect);
		GetWindowRect(dialog, &dialogRect);
		const int dialogWidth = dialogRect.right - dialogRect.left;
		const int dialogHeight = dialogRect.bottom - dialogRect.top;
		const int x = ownerRect.left + ((ownerRect.right - ownerRect.left) - dialogWidth) / 2;
		const int y = ownerRect.top + ((ownerRect.bottom - ownerRect.top) - dialogHeight) / 2;
		SetWindowPos(dialog, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

		EnableWindow(owner, FALSE);
		ShowWindow(dialog, SW_SHOW);
		UpdateWindow(dialog);

		MSG msg = {};
		while (IsWindow(dialog) && GetMessageW(&msg, nullptr, 0, 0) > 0)
		{
			if (!IsDialogMessageW(dialog, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessageW(&msg);
			}
		}

		const int result = data->result;
		if (IsWindow(owner))
		{
			EnableWindow(owner, TRUE);
			SetForegroundWindow(owner);
		}

		delete data;
		return result;
	}

	void ClearCertificates()
	{
		FreeCertificates(g_app.certificates);
		if (g_app.certificateList)
		{
			SendMessageW(g_app.certificateList, LB_RESETCONTENT, 0, 0);
		}

		if (g_app.signSelectedButton)
		{
			EnableWindow(g_app.signSelectedButton, FALSE);
		}
	}

	void DestroyUiFonts()
	{
		if (g_app.headingFont)
		{
			DeleteObject(g_app.headingFont);
			g_app.headingFont = nullptr;
		}

		if (g_app.bodyFont)
		{
			DeleteObject(g_app.bodyFont);
			g_app.bodyFont = nullptr;
		}

		if (g_app.smallBodyFont)
		{
			DeleteObject(g_app.smallBodyFont);
			g_app.smallBodyFont = nullptr;
		}
	}

	void DestroyUiBrushes()
	{
		if (g_app.backgroundBrush)
		{
			DeleteObject(g_app.backgroundBrush);
			g_app.backgroundBrush = nullptr;
		}

		if (g_app.surfaceBrush)
		{
			DeleteObject(g_app.surfaceBrush);
			g_app.surfaceBrush = nullptr;
		}

		if (g_app.logBrush)
		{
			DeleteObject(g_app.logBrush);
			g_app.logBrush = nullptr;
		}

		if (g_app.buttonBrush)
		{
			DeleteObject(g_app.buttonBrush);
			g_app.buttonBrush = nullptr;
		}

		if (g_app.buttonPressedBrush)
		{
			DeleteObject(g_app.buttonPressedBrush);
			g_app.buttonPressedBrush = nullptr;
		}

		if (g_app.buttonDisabledBrush)
		{
			DeleteObject(g_app.buttonDisabledBrush);
			g_app.buttonDisabledBrush = nullptr;
		}

		if (g_app.borderBrush)
		{
			DeleteObject(g_app.borderBrush);
			g_app.borderBrush = nullptr;
		}
	}

	void DrawButton(const DRAWITEMSTRUCT* drawItem)
	{
		const bool isDisabled = (drawItem->itemState & ODS_DISABLED) != 0;
		const bool isPressed = (drawItem->itemState & ODS_SELECTED) != 0;
		const bool hasFocus = (drawItem->itemState & ODS_FOCUS) != 0;

		HBRUSH fillBrush = g_app.buttonBrush;
		if (isDisabled)
		{
			fillBrush = g_app.buttonDisabledBrush;
		}
		else if (isPressed)
		{
			fillBrush = g_app.buttonPressedBrush;
		}

		FillRect(drawItem->hDC, &drawItem->rcItem, fillBrush ? fillBrush : g_app.backgroundBrush);
		FrameRect(drawItem->hDC, &drawItem->rcItem, g_app.borderBrush ? g_app.borderBrush : g_app.backgroundBrush);

		RECT textRect = drawItem->rcItem;
		textRect.left += 10;
		textRect.right -= 10;

		SetBkMode(drawItem->hDC, TRANSPARENT);
		SetTextColor(drawItem->hDC, isDisabled ? kColorTextMuted : kColorTextPrimary);
		SelectObject(drawItem->hDC, g_app.bodyFont ? g_app.bodyFont : GetStockObject(DEFAULT_GUI_FONT));

		wchar_t text[256] = {};
		GetWindowTextW(drawItem->hwndItem, text, static_cast<int>(std::size(text)));
		DrawTextW(drawItem->hDC, text, -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

		if (hasFocus)
		{
			RECT focusRect = drawItem->rcItem;
			InflateRect(&focusRect, -5, -5);
			DrawFocusRect(drawItem->hDC, &focusRect);
		}
	}

	void DrawBusyIndicator(const DRAWITEMSTRUCT* drawItem)
	{
		FillRect(drawItem->hDC, &drawItem->rcItem, g_app.logBrush ? g_app.logBrush : g_app.backgroundBrush);
		FrameRect(drawItem->hDC, &drawItem->rcItem, g_app.borderBrush ? g_app.borderBrush : g_app.backgroundBrush);

		RECT fillRect = drawItem->rcItem;
		InflateRect(&fillRect, -1, -1);
		if (fillRect.right <= fillRect.left || fillRect.bottom <= fillRect.top)
		{
			return;
		}

		HBRUSH trackBrush = g_app.surfaceBrush ? g_app.surfaceBrush : g_app.backgroundBrush;
		FillRect(drawItem->hDC, &fillRect, trackBrush);

		const int trackWidth = fillRect.right - fillRect.left;
		const int segmentWidth = max(36, trackWidth / 5);
		const int travelWidth = trackWidth + segmentWidth;
		const int segmentLeft = fillRect.left + (g_app.busyAnimationOffset % max(1, travelWidth)) - segmentWidth;
		RECT segmentRect = { segmentLeft, fillRect.top, segmentLeft + segmentWidth, fillRect.bottom };
		RECT visibleRect = {};
		if (IntersectRect(&visibleRect, &fillRect, &segmentRect))
		{
			FillRect(drawItem->hDC, &visibleRect, g_app.buttonBrush ? g_app.buttonBrush : g_app.backgroundBrush);
		}
	}

	void DrawCertificateListItem(const DRAWITEMSTRUCT* drawItem)
	{
		if (drawItem->itemID == static_cast<UINT>(-1))
		{
			return;
		}

		const bool isSelected = (drawItem->itemState & ODS_SELECTED) != 0;
		const bool hasFocus = (drawItem->itemState & ODS_FOCUS) != 0;
		const HBRUSH backgroundBrush = isSelected
			? (g_app.buttonPressedBrush ? g_app.buttonPressedBrush : g_app.surfaceBrush)
			: (g_app.surfaceBrush ? g_app.surfaceBrush : g_app.backgroundBrush);

		FillRect(drawItem->hDC, &drawItem->rcItem, backgroundBrush);

		if (drawItem->itemID >= g_app.certificates.size())
		{
			return;
		}

		const CertificateEntry& certificate = g_app.certificates[drawItem->itemID];
		const std::wstring issuerText = certificate.issuerDisplayName.empty()
			? std::wstring()
			: std::wstring(L"(issued by ") + certificate.issuerDisplayName + L")";

		RECT textRect = drawItem->rcItem;
		textRect.left += 10;
		textRect.right -= 10;

		SetBkMode(drawItem->hDC, TRANSPARENT);

		HFONT subjectFont = g_app.bodyFont ? g_app.bodyFont : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
		HFONT issuerFont = g_app.smallBodyFont ? g_app.smallBodyFont : subjectFont;
		HGDIOBJ oldFont = SelectObject(drawItem->hDC, subjectFont);

		SIZE issuerSize = {};
		int issuerWidth = 0;
		if (!issuerText.empty())
		{
			SelectObject(drawItem->hDC, issuerFont);
			GetTextExtentPoint32W(drawItem->hDC, issuerText.c_str(), static_cast<int>(issuerText.size()), &issuerSize);
			issuerWidth = min(issuerSize.cx, max(120, (textRect.right - textRect.left) / 2));
			SelectObject(drawItem->hDC, subjectFont);
		}

		RECT subjectRect = textRect;
		if (issuerWidth > 0)
		{
			subjectRect.right = max(subjectRect.left + 80, textRect.right - issuerWidth - 8);
		}

		SetTextColor(drawItem->hDC, kColorTextPrimary);
		DrawTextW(
			drawItem->hDC,
			certificate.displayName.c_str(),
			-1,
			&subjectRect,
			DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);

		if (issuerWidth > 0 && subjectRect.right + 8 < textRect.right)
		{
			RECT issuerRect = textRect;
			issuerRect.left = subjectRect.right + 8;
			SelectObject(drawItem->hDC, issuerFont);
			SetTextColor(drawItem->hDC, kColorTextIssuer);
			DrawTextW(
				drawItem->hDC,
				issuerText.c_str(),
				-1,
				&issuerRect,
				DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
		}

		SelectObject(drawItem->hDC, oldFont);

		if (hasFocus)
		{
			RECT focusRect = drawItem->rcItem;
			InflateRect(&focusRect, -3, -3);
			DrawFocusRect(drawItem->hDC, &focusRect);
		}
	}

	void RefreshServiceState()
	{
		const ServiceState state = QueryCertificatePropagationService();
		SetServiceLabel(state.message);
		SetServiceWarningIconVisible(state.message.find(L"not running") != std::wstring::npos);
		AppendLog(state.message);
	}

	void UpdateSignButtonState()
	{
		if (!g_app.certificateList)
		{
			return;
		}

		const LRESULT selectedIndex = SendMessageW(g_app.certificateList, LB_GETCURSEL, 0, 0);
		const BOOL enabled = !IsAsyncOperationInProgress() && selectedIndex != LB_ERR;
		if (g_app.signSelectedButton)
		{
			EnableWindow(g_app.signSelectedButton, enabled);
		}

		if (g_app.viewCertificateButton)
		{
			EnableWindow(g_app.viewCertificateButton, enabled);
		}
	}

	void StartRetrieveSignCertAsync(HWND owner)
	{
		if (IsAsyncOperationInProgress())
		{
			AppendLog(L"Another operation is already running. Waiting for the current request to finish.");
			return;
		}

		if (HasDetachedAsyncOperation())
		{
			const std::wstring message =
				std::wstring(L"The previous ")
				+ GetDetachedOperationName()
				+ L" is still finishing in the background. "
				L"The application is no longer waiting for it, but a new retrieve request cannot be started until that earlier call returns.";
			AppendLog(message);
			ShowThemedResultDialog(owner, L"Operation Still Finishing", message);
			return;
		}

		g_app.retrieveInProgress = true;
		g_app.retrieveDetached = false;
		g_app.retrieveTimedOut = false;
		++g_app.retrieveRequestId;
		UpdateActionButtons();

		AllowSetForegroundWindow(ASFW_ANY);
		SetForegroundWindow(owner);
		AppendLog(L"Calling retrieve_sign_cert...");
		SetTimer(owner, kRetrieveTimeoutTimerId, kRetrieveTimeoutMs, nullptr);
		const UINT requestId = g_app.retrieveRequestId;

		std::thread([owner, requestId]()
			{
				auto* result = new RetrieveSignCertResult();
				result->requestId = requestId;
				result->success = CallRetrieveSignCert(owner, result->message);
				PostMessageW(owner, WM_APP_RETRIEVE_COMPLETE, 0, reinterpret_cast<LPARAM>(result));
			}).detach();
	}

	void RunPrepareSignFlow()
	{
		ClearCertificates();

		std::wstring message;
		if (LoadSigningCertificates(g_app.certificates, message))
		{
			for (const auto& certificate : g_app.certificates)
			{
				SendMessageW(g_app.certificateList, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(certificate.displayName.c_str()));
			}

			if (!g_app.certificates.empty())
			{
				SendMessageW(g_app.certificateList, LB_SETCURSEL, 0, 0);
				UpdateSignButtonState();
			}
		}
		else
		{
			UpdateSignButtonState();
		}

		AppendLog(message);
	}

	void StartSignSelectedAsync(HWND owner)
	{
		if (IsAsyncOperationInProgress())
		{
			AppendLog(L"Another operation is already running. Waiting for the current request to finish.");
			return;
		}

		if (HasDetachedAsyncOperation())
		{
			const std::wstring message =
				std::wstring(L"The previous ")
				+ GetDetachedOperationName()
				+ L" is still finishing in the background. "
				L"The application is no longer waiting for it, but a new signing request cannot be started until that earlier call returns.";
			AppendLog(message);
			ShowThemedResultDialog(owner, L"Operation Still Finishing", message);
			return;
		}

		const LRESULT selectedIndex = SendMessageW(g_app.certificateList, LB_GETCURSEL, 0, 0);
		if (selectedIndex == LB_ERR || static_cast<size_t>(selectedIndex) >= g_app.certificates.size())
		{
			AppendLog(L"Select a signing certificate before signing.");
			return;
		}

		const CertificateEntry& certificate = g_app.certificates[static_cast<size_t>(selectedIndex)];
		std::wstring message = std::wstring(L"Selected certificate: ") + certificate.displayName;
		AppendLog(message);

		PCCERT_CONTEXT certificateCopy = CertDuplicateCertificateContext(certificate.context);
		if (!certificateCopy)
		{
			AppendLog(L"Failed to duplicate the selected certificate context.");
			return;
		}

		g_app.signInProgress = true;
		g_app.signDetached = false;
		g_app.signTimedOut = false;
		++g_app.signRequestId;
		UpdateActionButtons();

		AllowSetForegroundWindow(ASFW_ANY);
		SetForegroundWindow(owner);
		AppendLog(L"Starting test hash signature...");
		SetTimer(owner, kRetrieveTimeoutTimerId, kRetrieveTimeoutMs, nullptr);
		const UINT requestId = g_app.signRequestId;

		std::thread([owner, requestId, certificateCopy]()
			{
				auto* result = new SignHashResult();
				result->requestId = requestId;

				std::wstring keyMessage;
				NCRYPT_KEY_HANDLE key = GetKeyHandleFromCertificate(certificateCopy, keyMessage);
				if (!key)
				{
					result->success = false;
					result->message = keyMessage.empty()
						? L"Failed to get a key handle from the selected certificate."
						: keyMessage;
				}
				else
				{
					std::wstring signMessage;
					const SECURITY_STATUS status = SignHashWithKey(key, signMessage);
					NCryptFreeObject(key);

					result->success = (status == ERROR_SUCCESS);
					result->message = keyMessage;
					if (!result->message.empty() && !signMessage.empty())
					{
						result->message += L"\r\n";
					}

					result->message += signMessage;
				}

				CertFreeCertificateContext(certificateCopy);
				PostMessageW(owner, WM_APP_SIGN_COMPLETE, 0, reinterpret_cast<LPARAM>(result));
			}).detach();
	}

	void RunViewSelectedCertificateFlow()
	{
		const LRESULT selectedIndex = SendMessageW(g_app.certificateList, LB_GETCURSEL, 0, 0);
		if (selectedIndex == LB_ERR || static_cast<size_t>(selectedIndex) >= g_app.certificates.size())
		{
			AppendLog(L"Select a signing certificate before opening the certificate view.");
			return;
		}

		const CertificateEntry& certificate = g_app.certificates[static_cast<size_t>(selectedIndex)];
		AppendLog(std::wstring(L"Opening certificate view for: ") + certificate.displayName);
		if (!CryptUIDlgViewContext(CERT_STORE_CERTIFICATE_CONTEXT, certificate.context, g_app.hwnd, nullptr, 0, nullptr))
		{
			const DWORD error = GetLastError();
			AppendLog(std::wstring(L"Failed to open the Windows certificate view: ") + GetSystemMessage(error));
		}
	}

	void CreateUi(HWND hwnd)
	{
		g_app.headingFont = CreateUiFont(hwnd, 12, FW_SEMIBOLD, L"Segoe UI");
		g_app.bodyFont = CreateUiFont(hwnd, 10, FW_NORMAL, L"Segoe UI");
		g_app.smallBodyFont = CreateUiFont(hwnd, 9, FW_NORMAL, L"Segoe UI");
		g_app.backgroundBrush = CreateSolidBrush(kColorBackground);
		g_app.surfaceBrush = CreateSolidBrush(kColorSurface);
		g_app.logBrush = CreateSolidBrush(kColorLogSurface);
		g_app.buttonBrush = CreateSolidBrush(kColorButton);
		g_app.buttonPressedBrush = CreateSolidBrush(kColorButtonPressed);
		g_app.buttonDisabledBrush = CreateSolidBrush(kColorButtonDisabled);
		g_app.borderBrush = CreateSolidBrush(kColorSurfaceBorder);
		HFONT headingFont = g_app.headingFont ? g_app.headingFont : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
		HFONT bodyFont = g_app.bodyFont ? g_app.bodyFont : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));

		CreateWindowExW(
			0,
			L"STATIC",
			L"Certificate Propagation service (CertPropSvc): checking...",
			WS_CHILD | WS_VISIBLE,
			kControlMargin,
			kControlMargin,
			668,
			28,
			hwnd,
			reinterpret_cast<HMENU>(IdServiceStatus),
			nullptr,
			nullptr);

		CreateWindowExW(
			0,
			L"STATIC",
			nullptr,
			WS_CHILD | SS_ICON,
			kControlMargin + 640,
			kControlMargin + 6,
			kServiceWarningIconSize,
			kServiceWarningIconSize,
			hwnd,
			reinterpret_cast<HMENU>(IdServiceWarningIcon),
			nullptr,
			nullptr);

		CreateWindowExW(
			0,
			L"BUTTON",
			L"Refresh Service Status",
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			kControlMargin,
			48,
			kTopButtonWidth,
			kButtonHeight,
			hwnd,
			reinterpret_cast<HMENU>(IdRefreshService),
			nullptr,
			nullptr);

		CreateWindowExW(
			0,
			L"BUTTON",
			L"Retrieve Sign Cert",
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			kControlMargin + kTopButtonWidth + kTopButtonGap,
			48,
			kTopButtonWidth,
			kButtonHeight,
			hwnd,
			reinterpret_cast<HMENU>(IdRetrieveSignCert),
			nullptr,
			nullptr);

		CreateWindowExW(
			0,
			L"BUTTON",
			L"Reload Certificates",
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			kControlMargin + ((kTopButtonWidth + kTopButtonGap) * 2),
			48,
			kTopButtonWidth,
			kButtonHeight,
			hwnd,
			reinterpret_cast<HMENU>(IdPrepareSign),
			nullptr,
			nullptr);

		CreateWindowExW(
			0,
			L"BUTTON",
			L"FAQ",
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			kControlMargin + ((kTopButtonWidth + kTopButtonGap) * 3),
			48,
			kTopButtonWidth,
			kButtonHeight,
			hwnd,
			reinterpret_cast<HMENU>(IdFaq),
			nullptr,
			nullptr);

		CreateWindowExW(
			0,
			L"STATIC",
			nullptr,
			WS_CHILD | SS_OWNERDRAW,
			kControlMargin,
			kBusyIndicatorTop,
			kBusyIndicatorWidth,
			kBusyIndicatorHeight,
			hwnd,
			reinterpret_cast<HMENU>(IdBusyIndicator),
			nullptr,
			nullptr);

		CreateWindowExW(
			0,
			L"STATIC",
			L"Available signing certificates",
			WS_CHILD | WS_VISIBLE,
			kControlMargin,
			104,
			320,
			28,
			hwnd,
			reinterpret_cast<HMENU>(IdCertificateLabel),
			nullptr,
			nullptr);

		CreateWindowExW(
			0,
			L"LISTBOX",
			nullptr,
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_NOTIFY | LBS_NOINTEGRALHEIGHT | LBS_OWNERDRAWFIXED | LBS_HASSTRINGS,
			kControlMargin,
			kCertificateListTop,
			kCertificateListWidth,
			kCertificateListHeight,
			hwnd,
			reinterpret_cast<HMENU>(IdCertificateList),
			nullptr,
			nullptr);

		CreateWindowExW(
			0,
			L"BUTTON",
			L"Sign Using the Selected Signing Certificate",
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			kControlMargin,
			kBottomButtonTop,
			kBottomButtonWidth,
			kButtonHeight,
			hwnd,
			reinterpret_cast<HMENU>(IdSignSelected),
			nullptr,
			nullptr);

		CreateWindowExW(
			0,
			L"BUTTON",
			L"View Certificate",
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
			kControlMargin + kBottomButtonWidth + kBottomButtonGap,
			kBottomButtonTop,
			kBottomButtonWidth,
			kButtonHeight,
			hwnd,
			reinterpret_cast<HMENU>(IdViewCertificate),
			nullptr,
			nullptr);

		CreateWindowExW(
			0,
			L"EDIT",
			nullptr,
			WS_CHILD | WS_VISIBLE | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
			kControlMargin,
			kLogTop,
			kLogWidth,
			kLogHeight,
			hwnd,
			reinterpret_cast<HMENU>(IdStatusLog),
			nullptr,
			nullptr);

		const HWND headingControls[] =
		{
			GetDlgItem(hwnd, IdServiceStatus),
			GetDlgItem(hwnd, IdCertificateLabel)
		};

		const HWND bodyControls[] =
		{
			GetDlgItem(hwnd, IdRefreshService),
			GetDlgItem(hwnd, IdRetrieveSignCert),
			GetDlgItem(hwnd, IdPrepareSign),
			GetDlgItem(hwnd, IdFaq),
			GetDlgItem(hwnd, IdBusyIndicator),
			GetDlgItem(hwnd, IdCertificateList),
			GetDlgItem(hwnd, IdSignSelected),
			GetDlgItem(hwnd, IdViewCertificate),
			GetDlgItem(hwnd, IdStatusLog)
		};

		for (HWND child : headingControls)
		{
			SendMessageW(child, WM_SETFONT, reinterpret_cast<WPARAM>(headingFont), TRUE);
		}

		for (HWND child : bodyControls)
		{
			SendMessageW(child, WM_SETFONT, reinterpret_cast<WPARAM>(bodyFont), TRUE);
		}

		g_app.serviceStatus = GetDlgItem(hwnd, IdServiceStatus);
		g_app.serviceWarningIcon = GetDlgItem(hwnd, IdServiceWarningIcon);
		g_app.refreshServiceButton = GetDlgItem(hwnd, IdRefreshService);
		g_app.retrieveSignCertButton = GetDlgItem(hwnd, IdRetrieveSignCert);
		g_app.reloadCertificatesButton = GetDlgItem(hwnd, IdPrepareSign);
		g_app.faqButton = GetDlgItem(hwnd, IdFaq);
		g_app.busyIndicator = GetDlgItem(hwnd, IdBusyIndicator);
		g_app.certificateList = GetDlgItem(hwnd, IdCertificateList);
		g_app.signSelectedButton = GetDlgItem(hwnd, IdSignSelected);
		g_app.viewCertificateButton = GetDlgItem(hwnd, IdViewCertificate);
		g_app.statusLog = GetDlgItem(hwnd, IdStatusLog);

		if (g_app.serviceWarningIcon)
		{
			SendMessageW(
				g_app.serviceWarningIcon,
				STM_SETICON,
				reinterpret_cast<WPARAM>(LoadImageW(nullptr, IDI_WARNING, IMAGE_ICON, kServiceWarningIconSize, kServiceWarningIconSize, LR_SHARED)),
				0);
			SetServiceWarningIconVisible(false);
		}

		UpdateActionButtons();
	}
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		g_app.hwnd = hwnd;
		CreateUi(hwnd);
		RefreshServiceState();
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IdRefreshService:
			RefreshServiceState();
			return 0;

		case IdRetrieveSignCert:
			StartRetrieveSignCertAsync(hwnd);
			return 0;

		case IdPrepareSign:
			RunPrepareSignFlow();
			return 0;

		case IdFaq:
			OpenFaqWindow(hwnd);
			return 0;

		case IdSignSelected:
			StartSignSelectedAsync(hwnd);
			return 0;

		case IdViewCertificate:
			RunViewSelectedCertificateFlow();
			return 0;

		case IdCertificateList:
			if (HIWORD(wParam) == LBN_SELCHANGE)
			{
				UpdateSignButtonState();
			}
			return 0;
		}
		break;

	case WM_TIMER:
		if (wParam == kRetrieveTimeoutTimerId)
		{
			KillTimer(hwnd, kRetrieveTimeoutTimerId);
			if (g_app.retrieveInProgress)
			{
				g_app.retrieveTimedOut = true;
				const wchar_t* prompt =
					L"retrieve_sign_cert is still waiting for the external tray application. "
					L"The tray dialog may be hidden behind another window or the tray app may be hung.\n\n"
					L"You can keep waiting for another 30 seconds, or cancel and return to the application.";
				AppendLog(L"retrieve_sign_cert is still waiting for the external tray application.");

				const int choice = ShowThemedChoiceDialog(
					hwnd,
					L"Retrieve Sign Certificate Still Running",
					prompt,
					L"Wait Another 60 Seconds",
					L"Cancel");
				if (choice == IDYES)
				{
					AppendLog(L"Waiting another 60 seconds for retrieve_sign_cert...");
					SetTimer(hwnd, kRetrieveTimeoutTimerId, kRetrieveTimeoutMs, nullptr);
				}
				else
				{
					CancelRetrieveWait();
				}
			}
			else if (g_app.signInProgress)
			{
				g_app.signTimedOut = true;
				const wchar_t* prompt =
					L"The test hash signature is still waiting for the signer to return. "
					L"The PIN or consent dialog may be hidden behind another window, or the signing provider may be hung.\n\n"
					L"You can keep waiting for another 30 seconds, or cancel and return to the application.";
				AppendLog(L"The test hash signature is still waiting for the signer to return.");

				const int choice = ShowThemedChoiceDialog(
					hwnd,
					L"Signature Still Running",
					prompt,
					L"Wait Another 30 Seconds",
					L"Cancel");
				if (choice == IDYES)
				{
					AppendLog(L"Waiting another 30 seconds for the test hash signature...");
					SetTimer(hwnd, kRetrieveTimeoutTimerId, kRetrieveTimeoutMs, nullptr);
				}
				else
				{
					CancelSignWait();
				}
			}
			return 0;
		}

		if (wParam == kBusyAnimationTimerId)
		{
			if (IsAsyncOperationInProgress() && g_app.busyIndicator)
			{
				const RECT clientRect = { 0, 0, kBusyIndicatorWidth, kBusyIndicatorHeight };
				const int trackWidth = (clientRect.right - clientRect.left) - 2;
				const int segmentWidth = max(36, trackWidth / 5);
				g_app.busyAnimationOffset = (g_app.busyAnimationOffset + 10) % max(1, trackWidth + segmentWidth);
				InvalidateRect(g_app.busyIndicator, nullptr, TRUE);
			}
			return 0;
		}
		break;

	case WM_DESTROY:
		ClearCertificates();
		DestroyUiFonts();
		DestroyUiBrushes();
		PostQuitMessage(0);
		return 0;

	case WM_CLOSE:
		if (ConfirmCloseIfBusy(hwnd))
		{
			DestroyWindow(hwnd);
		}
		return 0;

	case WM_CTLCOLORSTATIC:
		if (reinterpret_cast<HWND>(lParam) == g_app.statusLog)
		{
			SetBkColor(reinterpret_cast<HDC>(wParam), kColorLogSurface);
			SetTextColor(reinterpret_cast<HDC>(wParam), kColorTextLog);
			if (g_app.logBrush)
			{
				return reinterpret_cast<INT_PTR>(g_app.logBrush);
			}
		}
		else
		{
			SetBkMode(reinterpret_cast<HDC>(wParam), TRANSPARENT);
			SetTextColor(reinterpret_cast<HDC>(wParam), kColorTextPrimary);
			if (g_app.backgroundBrush)
			{
				return reinterpret_cast<INT_PTR>(g_app.backgroundBrush);
			}
		}
		break;

	case WM_MEASUREITEM:
		if (wParam == IdCertificateList)
		{
			auto* measureItem = reinterpret_cast<MEASUREITEMSTRUCT*>(lParam);
			if (measureItem)
			{
				measureItem->itemHeight = 24;
				return TRUE;
			}
		}
		break;

	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORLISTBOX:
		SetBkColor(reinterpret_cast<HDC>(wParam), kColorSurface);
		SetTextColor(reinterpret_cast<HDC>(wParam), kColorTextPrimary);
		if (g_app.surfaceBrush)
		{
			return reinterpret_cast<INT_PTR>(g_app.surfaceBrush);
		}
		break;

	case WM_DRAWITEM:
		if (wParam == IdRefreshService ||
			wParam == IdRetrieveSignCert ||
			wParam == IdPrepareSign ||
			wParam == IdFaq ||
			wParam == IdSignSelected ||
			wParam == IdViewCertificate)
		{
			DrawButton(reinterpret_cast<const DRAWITEMSTRUCT*>(lParam));
			return TRUE;
		}

		if (wParam == IdBusyIndicator)
		{
			DrawBusyIndicator(reinterpret_cast<const DRAWITEMSTRUCT*>(lParam));
			return TRUE;
		}

		if (wParam == IdCertificateList)
		{
			DrawCertificateListItem(reinterpret_cast<const DRAWITEMSTRUCT*>(lParam));
			return TRUE;
		}
		break;

	case WM_APP_RETRIEVE_COMPLETE:
	{
		auto* result = reinterpret_cast<RetrieveSignCertResult*>(lParam);

		if (result)
		{
			if (result->requestId != g_app.retrieveRequestId)
			{
				AppendLog(L"Ignored a late retrieve_sign_cert result from an older request.");
				delete result;
				return 0;
			}

			KillTimer(hwnd, kRetrieveTimeoutTimerId);
			const bool hadTimeout = g_app.retrieveTimedOut;
			const bool wasDetached = g_app.retrieveDetached;
			g_app.retrieveInProgress = false;
			g_app.retrieveDetached = false;
			g_app.retrieveTimedOut = false;
			UpdateActionButtons();

			if (wasDetached)
			{
				AppendLog(L"retrieve_sign_cert finished after waiting was cancelled. The result was ignored.");
				delete result;
				return 0;
			}

			AppendLog(result->message);
			if (!result->success)
			{
				MessageBoxW(hwnd, result->message.c_str(), L"Retrieve Sign Certificate Failed", MB_OK | MB_ICONERROR);
			}
			else
			{
				if (hadTimeout)
				{
					AppendLog(L"retrieve_sign_cert eventually returned after the timeout warning.");
				}
				RunPrepareSignFlow();
			}

			delete result;
		}

		return 0;
	}

	case WM_APP_SIGN_COMPLETE:
	{
		auto* result = reinterpret_cast<SignHashResult*>(lParam);

		if (result)
		{
			if (result->requestId != g_app.signRequestId)
			{
				AppendLog(L"Ignored a late test hash signature result from an older request.");
				delete result;
				return 0;
			}

			KillTimer(hwnd, kRetrieveTimeoutTimerId);
			const bool hadTimeout = g_app.signTimedOut;
			const bool wasDetached = g_app.signDetached;
			g_app.signInProgress = false;
			g_app.signDetached = false;
			g_app.signTimedOut = false;
			UpdateActionButtons();

			if (wasDetached)
			{
				AppendLog(L"The test hash signature finished after waiting was cancelled. The result was ignored.");
				delete result;
				return 0;
			}

			AppendLog(result->message);
			if (!result->success)
			{
				ShowThemedResultDialog(hwnd, L"Signature Failed", result->message);
			}
			else
			{
				if (hadTimeout)
				{
					AppendLog(L"The test hash signature eventually returned after the timeout warning.");
				}

				ShowThemedResultDialog(hwnd, L"Signature Complete", L"Test hash signature succeeded.");
			}

			delete result;
		}

		return 0;
	}

	case WM_ERASEBKGND:
		if (g_app.backgroundBrush)
		{
			RECT clientRect = {};
			GetClientRect(hwnd, &clientRect);
			FillRect(reinterpret_cast<HDC>(wParam), &clientRect, g_app.backgroundBrush);
			return TRUE;
		}
		break;
	}

	return DefWindowProcW(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK FaqWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		const HFONT headingFont = g_app.headingFont ? g_app.headingFont : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
		const HFONT bodyFont = g_app.bodyFont ? g_app.bodyFont : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
		int top = 18;

		for (const auto& entry : kFaqEntries)
		{
			HWND title = CreateWindowExW(
				0,
				L"STATIC",
				entry.title,
				WS_CHILD | WS_VISIBLE,
				kFaqHorizontalMargin,
				top,
				kFaqContentWidth,
				22,
				hwnd,
				nullptr,
				nullptr,
				nullptr);
			SendMessageW(title, WM_SETFONT, reinterpret_cast<WPARAM>(headingFont), TRUE);
			top += 24;

			const int bodyHeight = MeasureWrappedTextHeight(hwnd, entry.body, bodyFont, kFaqContentWidth);

			HWND body = CreateWindowExW(
				0,
				L"STATIC",
				entry.body,
				WS_CHILD | WS_VISIBLE,
				kFaqHorizontalMargin,
				top,
				kFaqContentWidth,
				bodyHeight,
				hwnd,
				nullptr,
				nullptr,
				nullptr);
			SendMessageW(body, WM_SETFONT, reinterpret_cast<WPARAM>(bodyFont), TRUE);
			top += bodyHeight + 16;
		}

		RECT windowRect = { 0, 0, kFaqWindowWidth, top + 18 };
		const LONG style = GetWindowLongW(hwnd, GWL_STYLE);
		const LONG exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
		AdjustWindowRectEx(&windowRect, style, FALSE, exStyle);
		SetWindowPos(hwnd, nullptr, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		return 0;
	}

	case WM_CTLCOLORSTATIC:
		SetBkMode(reinterpret_cast<HDC>(wParam), TRANSPARENT);
		SetTextColor(reinterpret_cast<HDC>(wParam), kColorTextPrimary);
		if (g_app.backgroundBrush)
		{
			return reinterpret_cast<INT_PTR>(g_app.backgroundBrush);
		}
		break;
	}

	return DefWindowProcW(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK ThemedResultDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	auto* data = reinterpret_cast<ResultDialogData*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

	switch (message)
	{
	case WM_NCCREATE:
	{
		const auto* create = reinterpret_cast<const CREATESTRUCTW*>(lParam);
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create->lpCreateParams));
		return TRUE;
	}

	case WM_CREATE:
	{
		data = reinterpret_cast<ResultDialogData*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
		const HFONT headingFont = g_app.headingFont ? g_app.headingFont : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
		const HFONT bodyFont = g_app.bodyFont ? g_app.bodyFont : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));

		int top = 18;
		HWND title = CreateWindowExW(
			0,
			L"STATIC",
			data ? data->title.c_str() : L"",
			WS_CHILD | WS_VISIBLE,
			kResultDialogHorizontalMargin,
			top,
			kResultDialogContentWidth,
			24,
			hwnd,
			nullptr,
			nullptr,
			nullptr);
		SendMessageW(title, WM_SETFONT, reinterpret_cast<WPARAM>(headingFont), TRUE);
		top += 30;

		const std::wstring bodyText = data ? data->message : L"";
		const int bodyHeight = MeasureWrappedTextHeight(hwnd, bodyText.c_str(), bodyFont, kResultDialogContentWidth);
		HWND body = CreateWindowExW(
			0,
			L"STATIC",
			bodyText.c_str(),
			WS_CHILD | WS_VISIBLE,
			kResultDialogHorizontalMargin,
			top,
			kResultDialogContentWidth,
			bodyHeight,
			hwnd,
			nullptr,
			nullptr,
			nullptr);
		SendMessageW(body, WM_SETFONT, reinterpret_cast<WPARAM>(bodyFont), TRUE);
		top += bodyHeight + 18;

		HWND okButton = CreateWindowExW(
			0,
			L"BUTTON",
			L"OK",
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | WS_TABSTOP,
			kResultDialogHorizontalMargin + kResultDialogContentWidth - kResultDialogButtonWidth,
			top,
			kResultDialogButtonWidth,
			kButtonHeight,
			hwnd,
			reinterpret_cast<HMENU>(IdResultDialogOk),
			nullptr,
			nullptr);
		SendMessageW(okButton, WM_SETFONT, reinterpret_cast<WPARAM>(bodyFont), TRUE);

		RECT rect = { 0, 0, kResultDialogContentWidth + (kResultDialogHorizontalMargin * 2), top + kButtonHeight + 18 };
		const LONG style = GetWindowLongW(hwnd, GWL_STYLE);
		const LONG exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
		AdjustWindowRectEx(&rect, style, FALSE, exStyle);
		SetWindowPos(hwnd, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		return 0;
	}

	case WM_COMMAND:
		if (LOWORD(wParam) == IdResultDialogOk)
		{
			DestroyWindow(hwnd);
			return 0;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;

	case WM_NCDESTROY:
		delete data;
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
		return 0;

	case WM_CTLCOLORSTATIC:
		SetBkMode(reinterpret_cast<HDC>(wParam), TRANSPARENT);
		SetTextColor(reinterpret_cast<HDC>(wParam), kColorTextPrimary);
		if (g_app.backgroundBrush)
		{
			return reinterpret_cast<INT_PTR>(g_app.backgroundBrush);
		}
		break;

	case WM_DRAWITEM:
		if (wParam == IdResultDialogOk)
		{
			DrawButton(reinterpret_cast<const DRAWITEMSTRUCT*>(lParam));
			return TRUE;
		}
		break;

	case WM_ERASEBKGND:
		if (g_app.backgroundBrush)
		{
			RECT clientRect = {};
			GetClientRect(hwnd, &clientRect);
			FillRect(reinterpret_cast<HDC>(wParam), &clientRect, g_app.backgroundBrush);
			return TRUE;
		}
		break;
	}

	return DefWindowProcW(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK ThemedChoiceDialogProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	auto* data = reinterpret_cast<ChoiceDialogData*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));

	switch (message)
	{
	case WM_NCCREATE:
	{
		const auto* create = reinterpret_cast<const CREATESTRUCTW*>(lParam);
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create->lpCreateParams));
		return TRUE;
	}

	case WM_CREATE:
	{
		data = reinterpret_cast<ChoiceDialogData*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
		const HFONT headingFont = g_app.headingFont ? g_app.headingFont : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
		const HFONT bodyFont = g_app.bodyFont ? g_app.bodyFont : static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT));
		const int buttonWidth = 200;
		const int buttonGap = 12;

		int top = 18;
		HWND title = CreateWindowExW(
			0,
			L"STATIC",
			data ? data->title.c_str() : L"",
			WS_CHILD | WS_VISIBLE,
			kResultDialogHorizontalMargin,
			top,
			kResultDialogContentWidth + 120,
			24,
			hwnd,
			nullptr,
			nullptr,
			nullptr);
		SendMessageW(title, WM_SETFONT, reinterpret_cast<WPARAM>(headingFont), TRUE);
		top += 30;

		const std::wstring bodyText = data ? data->message : L"";
		const int contentWidth = kResultDialogContentWidth + 120;
		const int bodyHeight = MeasureWrappedTextHeight(hwnd, bodyText.c_str(), bodyFont, contentWidth);
		HWND body = CreateWindowExW(
			0,
			L"STATIC",
			bodyText.c_str(),
			WS_CHILD | WS_VISIBLE,
			kResultDialogHorizontalMargin,
			top,
			contentWidth,
			bodyHeight,
			hwnd,
			nullptr,
			nullptr,
			nullptr);
		SendMessageW(body, WM_SETFONT, reinterpret_cast<WPARAM>(bodyFont), TRUE);
		top += bodyHeight + 18;

		const int buttonsLeft = kResultDialogHorizontalMargin + contentWidth - ((buttonWidth * 2) + buttonGap);
		HWND secondaryButton = CreateWindowExW(
			0,
			L"BUTTON",
			data ? data->secondaryButtonText.c_str() : L"Cancel",
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | WS_TABSTOP,
			buttonsLeft,
			top,
			buttonWidth,
			kButtonHeight,
			hwnd,
			reinterpret_cast<HMENU>(IdChoiceDialogSecondary),
			nullptr,
			nullptr);
		SendMessageW(secondaryButton, WM_SETFONT, reinterpret_cast<WPARAM>(bodyFont), TRUE);

		HWND primaryButton = CreateWindowExW(
			0,
			L"BUTTON",
			data ? data->primaryButtonText.c_str() : L"OK",
			WS_CHILD | WS_VISIBLE | BS_OWNERDRAW | WS_TABSTOP,
			buttonsLeft + buttonWidth + buttonGap,
			top,
			buttonWidth,
			kButtonHeight,
			hwnd,
			reinterpret_cast<HMENU>(IdChoiceDialogPrimary),
			nullptr,
			nullptr);
		SendMessageW(primaryButton, WM_SETFONT, reinterpret_cast<WPARAM>(bodyFont), TRUE);
		SetFocus(primaryButton);

		RECT rect = { 0, 0, contentWidth + (kResultDialogHorizontalMargin * 2), top + kButtonHeight + 18 };
		const LONG style = GetWindowLongW(hwnd, GWL_STYLE);
		const LONG exStyle = GetWindowLongW(hwnd, GWL_EXSTYLE);
		AdjustWindowRectEx(&rect, style, FALSE, exStyle);
		SetWindowPos(hwnd, nullptr, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		return 0;
	}

	case WM_COMMAND:
		if (LOWORD(wParam) == IdChoiceDialogPrimary)
		{
			if (data)
			{
				data->result = IDYES;
			}
			DestroyWindow(hwnd);
			return 0;
		}

		if (LOWORD(wParam) == IdChoiceDialogSecondary)
		{
			if (data)
			{
				data->result = IDNO;
			}
			DestroyWindow(hwnd);
			return 0;
		}
		break;

	case WM_CLOSE:
		if (data)
		{
			data->result = IDNO;
		}
		DestroyWindow(hwnd);
		return 0;

	case WM_NCDESTROY:
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, 0);
		return 0;

	case WM_CTLCOLORSTATIC:
		SetBkMode(reinterpret_cast<HDC>(wParam), TRANSPARENT);
		SetTextColor(reinterpret_cast<HDC>(wParam), kColorTextPrimary);
		if (g_app.backgroundBrush)
		{
			return reinterpret_cast<INT_PTR>(g_app.backgroundBrush);
		}
		break;

	case WM_DRAWITEM:
		if (wParam == IdChoiceDialogPrimary || wParam == IdChoiceDialogSecondary)
		{
			DrawButton(reinterpret_cast<const DRAWITEMSTRUCT*>(lParam));
			return TRUE;
		}
		break;

	case WM_ERASEBKGND:
		if (g_app.backgroundBrush)
		{
			RECT clientRect = {};
			GetClientRect(hwnd, &clientRect);
			FillRect(reinterpret_cast<HDC>(wParam), &clientRect, g_app.backgroundBrush);
			return TRUE;
		}
		break;
	}

	return DefWindowProcW(hwnd, message, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int commandShow)
{
	const wchar_t className[] = L"TestCryptoUiWindowClass";
	const DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

	INITCOMMONCONTROLSEX commonControls = {};
	commonControls.dwSize = sizeof(commonControls);
	commonControls.dwICC = ICC_PROGRESS_CLASS;
	InitCommonControlsEx(&commonControls);

	WNDCLASSEXW windowClass = {};
	windowClass.cbSize = sizeof(windowClass);
	windowClass.lpfnWndProc = MainWindowProc;
	windowClass.hInstance = instance;
	windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	windowClass.hbrBackground = nullptr;
	windowClass.lpszClassName = className;

	RegisterClassExW(&windowClass);

	RECT windowRect = { 0, 0, kClientWidth, kClientHeight };
	AdjustWindowRectEx(&windowRect, windowStyle, FALSE, 0);

	HWND hwnd = CreateWindowExW(
		0,
		className,
		L"Test Crypto UI",
		windowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr,
		nullptr,
		instance,
		nullptr);

	if (!hwnd)
	{
		return 0;
	}

	ApplyWindowTheme(hwnd);
	ShowWindow(hwnd, commandShow);
	UpdateWindow(hwnd);

	MSG msg = {};
	while (GetMessageW(&msg, nullptr, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return static_cast<int>(msg.wParam);
}
