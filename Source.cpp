#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include <stdio.h>
#include <chrono>
#include <ctime>

TCHAR szClassName[] = TEXT("Window");

INT64 GetUnixTime(int nYear, int nMonth, int nDay)
{
	SYSTEMTIME systime = { 0 };
	systime.wYear = nYear;
	systime.wMonth = nMonth;
	systime.wDay = nDay;
	FILETIME filetime = { 0 };
	SystemTimeToFileTime(&systime, &filetime);
	INT64 result;
	result = filetime.dwHighDateTime;
	result <<= 32;
	result += filetime.dwLowDateTime;
	result -= 116444736000000000L;
	result /= 10000000L;
	return result;
}

INT64 RataDieDay(int nYear, int nMonth, int nDay)
{
	if (nMonth < 3) nYear--, nMonth += 12;
	return 365L * nYear + nYear / 4L - nYear / 100L + nYear / 400L + (153L * nMonth - 457L) / 5L + nDay - 306L;
}

SYSTEMTIME StringToSystemTime(LPCTSTR lpszString)
{
	SYSTEMTIME systemtime = { 0 };
	swscanf_s(lpszString, TEXT("%hd/%hd/%hd"),
		&systemtime.wYear,
		&systemtime.wMonth,
		&systemtime.wDay);
	return systemtime;
}

LPTSTR SystemTimeToString(const LPSYSTEMTIME lpSystemTime)
{
	LPTSTR lpszString = (LPTSTR)GlobalAlloc(0, sizeof(TCHAR)*(4 + 1 + 2 + 1 + 2 + 1));
	wsprintf(lpszString, TEXT("%04d/%02d/%02d"), lpSystemTime->wYear, lpSystemTime->wMonth, lpSystemTime->wDay);
	return lpszString;
}

std::chrono::system_clock::time_point makeTimePoint(int year, int mon, int day)
{
	struct std::tm t = { 0 };
	t.tm_mday = day;
	t.tm_mon = mon - 1;
	t.tm_year = year - 1900;
	t.tm_isdst = -1;
	__int64 tt = std::mktime(&t);
	if (tt == -1)
	{
		throw "no valid system time";
	}
	return std::chrono::system_clock::from_time_t(tt);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit1;
	static HWND hEdit2;
	static HWND hEdit3;
	static HWND hEdit4;
	static HWND hEdit5;
	static HWND hButton;
	switch (msg)
	{
	case WM_CREATE:
		hEdit1 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("1601/01/01"), WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit2 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("2016/05/11"), WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit3 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_READONLY, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit4 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_READONLY, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hEdit5 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), 0, WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_READONLY, 0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton = CreateWindow(TEXT("BUTTON"), TEXT("•ÏŠ·"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)IDOK, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		break;
	case WM_SIZE:
		MoveWindow(hEdit1, 10, 10, 256, 32, TRUE);
		MoveWindow(hEdit2, 10, 50, 256, 32, TRUE);
		MoveWindow(hEdit3, 10, 90, 256, 32, TRUE);
		MoveWindow(hEdit4, 10, 130, 256, 32, TRUE);
		MoveWindow(hEdit5, 10, 170, 256, 32, TRUE);
		MoveWindow(hButton, 10, 210, 256, 32, TRUE);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			TCHAR szText1[256];
			TCHAR szText2[256];
			TCHAR szText3[256];
			TCHAR szText4[256];
			TCHAR szText5[256];
			GetWindowText(hEdit1, szText1, 256);
			GetWindowText(hEdit2, szText2, 256);
			const SYSTEMTIME systemtime1 = StringToSystemTime(szText1);
			const SYSTEMTIME systemtime2 = StringToSystemTime(szText2);

			INT64 d = GetUnixTime(systemtime2.wYear, systemtime2.wMonth, systemtime2.wDay) - GetUnixTime(systemtime1.wYear, systemtime1.wMonth, systemtime1.wDay);
			d /= (60 * 60 * 24);
			wsprintf(szText3, TEXT("%I64d “ú"), d);
			SetWindowText(hEdit3, szText3);

			d = RataDieDay(systemtime2.wYear, systemtime2.wMonth, systemtime2.wDay) - RataDieDay(systemtime1.wYear, systemtime1.wMonth, systemtime1.wDay);
			wsprintf(szText4, TEXT("%I64d “ú"), d);
			SetWindowText(hEdit4, szText4);

			try
			{
				typedef std::chrono::duration<int, std::ratio<3600 * 24>> Days;
				auto diff = makeTimePoint(systemtime2.wYear, systemtime2.wMonth, systemtime2.wDay) - makeTimePoint(systemtime1.wYear, systemtime1.wMonth, systemtime1.wDay);
				Days days = std::chrono::duration_cast<Days>(diff);
				wsprintf(szText5, TEXT("%d “ú"), days.count());
				SetWindowText(hEdit5, szText5);
			}
			catch (...)
			{
				SetWindowText(hEdit5, 0);
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("Window"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
