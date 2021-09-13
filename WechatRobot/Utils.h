#pragma once
class Utils
{
	static void mouseClick(int x, int y);

	static bool copyimage(const wchar_t* filename);

	static int SendKeystrokes(HWND hWnd, const TCHAR ch, DWORD optionKey);
};

