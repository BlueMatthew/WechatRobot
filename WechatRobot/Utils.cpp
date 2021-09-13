#include "stdafx.h"
#include "Utils.h"
#include <psapi.h>

void Utils::mouseClick(int x, int y)
{
	INPUT *buffer = new INPUT[3];
	buffer->type = INPUT_MOUSE;
	buffer->mi.dx = x;
	buffer->mi.dy = y;
	buffer->mi.mouseData = 0;
	buffer->mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE);
	buffer->mi.time = 0;
	buffer->mi.dwExtraInfo = 0;

	(buffer + 1)->type = INPUT_MOUSE;
	(buffer + 1)->mi.dx = x;
	(buffer + 1)->mi.dy = y;
	(buffer + 1)->mi.mouseData = 0;
	(buffer + 1)->mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	(buffer + 1)->mi.time = 0;
	(buffer + 1)->mi.dwExtraInfo = 0;

	(buffer + 2)->type = INPUT_MOUSE;
	(buffer + 2)->mi.dx = x;
	(buffer + 2)->mi.dy = y;
	(buffer + 2)->mi.mouseData = 0;
	(buffer + 2)->mi.dwFlags = MOUSEEVENTF_LEFTUP;
	(buffer + 2)->mi.time = 0;
	(buffer + 2)->mi.dwExtraInfo = 0;

	SendInput(3, buffer, sizeof(INPUT));
	delete[] buffer;
}


DWORD findWechatProcessId()
{
	// Get the list of process identifiers.
	TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
	DWORD aProcesses[1024], cbNeeded, cProcesses;
	unsigned int i;

	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		return 1;
	}


	// Calculate how many process identifiers were returned.

	cProcesses = cbNeeded / sizeof(DWORD);

	// Print the name and process identifier for each process.

	for (i = 0; i < cProcesses; i++)
	{
		if (aProcesses[i] != 0)
		{
			// Get a handle to the process.

			HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
				PROCESS_VM_READ,
				FALSE, processID);

			// Get the process name.

			if (NULL != hProcess)
			{
				HMODULE hMod;
				DWORD cbNeeded;

				if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
					&cbNeeded))
				{
					GetModuleBaseName(hProcess, hMod, szProcessName,
						sizeof(szProcessName) / sizeof(TCHAR));
				}
			}

			// Print the process name and identifier.

			_tprintf(TEXT("%s  (PID: %u)\n"), szProcessName, processID);

			// Release the handle to the process.

			CloseHandle(hProcess);

			// PrintProcessNameAndID(aProcesses[i]);
		}
	}

	return 0;
}

bool Utils::copyimage(const wchar_t* filename)
{
	bool result = false;
	Gdiplus::Bitmap *gdibmp = Gdiplus::Bitmap::FromFile(filename);
	if (gdibmp)
	{
		HBITMAP hbitmap;
		gdibmp->GetHBITMAP(0, &hbitmap);
		if (OpenClipboard(NULL))
		{
			EmptyClipboard();
			DIBSECTION ds;
			if (GetObject(hbitmap, sizeof(DIBSECTION), &ds))
			{
				HDC hdc = GetDC(HWND_DESKTOP);
				//create compatible bitmap (get DDB from DIB)
				HBITMAP hbitmap_ddb = CreateDIBitmap(hdc, &ds.dsBmih, CBM_INIT,
					ds.dsBm.bmBits, (BITMAPINFO*)&ds.dsBmih, DIB_RGB_COLORS);
				ReleaseDC(HWND_DESKTOP, hdc);
				SetClipboardData(CF_BITMAP, hbitmap_ddb);
				DeleteObject(hbitmap_ddb);
				result = true;
			}
			CloseClipboard();
		}

		//cleanup:
		DeleteObject(hbitmap);
		delete gdibmp;
	}
	return result;
}


//Visual Studio specific

int Utils::sendKeystrokes(HWND hWnd, const TCHAR ch, DWORD optionKey)
{
	INPUT *keystroke;
	UINT i, character_count, keystrokes_to_send, keystrokes_sent;

	//Bring the Notepad window to the front.
	if (!SetForegroundWindow(hWnd))
		return 0;

	//Fill in the array of keystrokes to send.
	character_count = 1;
	keystrokes_to_send = character_count * 2;
	keystroke = new INPUT[4];

	keystroke[0].type = INPUT_KEYBOARD;
	keystroke[0].ki.wVk = 0;
	keystroke[0].ki.wScan = optionKey;
	keystroke[0].ki.dwFlags = KEYEVENTF_UNICODE;
	keystroke[0].ki.time = 0;
	keystroke[0].ki.dwExtraInfo = GetMessageExtraInfo();

	keystroke[1].type = INPUT_KEYBOARD;
	keystroke[1].ki.wVk = 0;
	keystroke[1].ki.wScan = ch;
	keystroke[1].ki.dwFlags = KEYEVENTF_UNICODE;
	keystroke[1].ki.time = 0;
	keystroke[1].ki.dwExtraInfo = GetMessageExtraInfo();

	keystroke[2].type = INPUT_KEYBOARD;
	keystroke[2].ki.wVk = 0;
	keystroke[2].ki.wScan = ch;
	keystroke[2].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
	keystroke[2].ki.time = 0;
	keystroke[2].ki.dwExtraInfo = GetMessageExtraInfo();

	keystroke[3].type = INPUT_KEYBOARD;
	keystroke[3].ki.wVk = 0;
	keystroke[3].ki.wScan = optionKey;
	keystroke[3].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
	keystroke[3].ki.time = 0;
	keystroke[3].ki.dwExtraInfo = GetMessageExtraInfo();

	//Send the keystrokes.
	keystrokes_sent = SendInput((UINT)keystrokes_to_send, keystroke, sizeof(*keystroke));
	delete[] keystroke;

	return keystrokes_sent == keystrokes_to_send;
}