#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#include "res.h"

#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>
#include <shellapi.h>
#include <strsafe.h>

#define NID_MSG WM_APP       + 0x2200
#define IDM_EXIT WM_USER     + 0x2210

#define MSG_BOX( text )      MessageBox(NULL, _T( text ), _T("Keep on Top"), MB_OK);
#define PRINT( text )        std::wcout << text << std::endl;

static             TCHAR szWindowClass[] = _T("DesktopApp");
static             TCHAR szTitle[] = _T("Keep on Top");

HINSTANCE          hInst;
NOTIFYICONDATA     nid;
// HICON              hIcon = static_cast<HICON>(LoadImage(NULL, TEXT("notifyicon_w.ico"), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_SHARED | LR_DEFAULTSIZE | LR_LOADFROMFILE));
LRESULT            CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HWND               hWnd;
HWND               AhWnd;
TCHAR              wnd_class[256];

VOID CALLBACK WinEventProcCallback ( HWINEVENTHOOK hWinEventHook, DWORD dwEvent, HWND hwnd, LONG idObject, LONG idChild, DWORD dwEventThread, DWORD dwmsEventTime)
{
   if (dwEvent == EVENT_SYSTEM_FOREGROUND){

      GetClassName(GetForegroundWindow(), wnd_class, sizeof(wnd_class));

      bool tray = _tcscmp(wnd_class, _T("Shell_TrayWnd")) == 0;
      bool overflow = _tcscmp(wnd_class, _T("NotifyIconOverflowWindow")) == 0;

      if (!( tray || overflow ))
      {
         AhWnd = GetForegroundWindow();
      }

      PRINT ( AhWnd )

   }
}

int CALLBACK WinMain(
   _In_ HINSTANCE hInstance,
   _In_opt_ HINSTANCE hPrevInstance,
   _In_ LPSTR     lpCmdLine,
   _In_ int       nCmdShow
)
{
   //Create App
   WNDCLASSEX wcex;
   wcex.cbSize = sizeof(WNDCLASSEX);
   wcex.style          = CS_HREDRAW | CS_VREDRAW;
   wcex.lpfnWndProc    = WndProc;
   wcex.cbClsExtra     = 0;
   wcex.cbWndExtra     = 0;
   wcex.hInstance      = hInstance;
   // wcex.hIcon          = hIcon;
   wcex.hIcon        = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PIN));
   wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
   wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
   wcex.lpszMenuName   = NULL;
   wcex.lpszClassName  = szWindowClass;
   // wcex.hIconSm        = hIcon;
   wcex.hIconSm        = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PIN));

   if (!RegisterClassEx(&wcex))
   {
      MSG_BOX("Call to RegisterClassEx failed!")
      return 1;
   }

   hInst = hInstance;

   hWnd = CreateWindow(
      szWindowClass,
      szTitle,
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT,
      500, 100,
      NULL,
      NULL,
      hInstance,
      NULL
   );

   if (!hWnd)
   {
      MSG_BOX("Call to CreateWindow failed!")
      return 1;
   }

   //Tray Icon
   ZeroMemory(&nid, sizeof(NOTIFYICONDATA));
   nid.cbSize = sizeof(nid);
   nid.hWnd = hWnd;
   nid.uID = 100;
   nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
   nid.uCallbackMessage = NID_MSG;
   // nid.hIcon = hIcon;
   nid.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PIN));
   StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), TEXT("Keep on Top"));

   Shell_NotifyIcon(NIM_ADD, &nid);

   HWINEVENTHOOK hEvent = SetWinEventHook(EVENT_SYSTEM_FOREGROUND , EVENT_SYSTEM_FOREGROUND ,NULL, WinEventProcCallback, 0, 0, WINEVENT_OUTOFCONTEXT | WINEVENT_SKIPOWNPROCESS);

   // Message loop
   MSG msg;
   while (GetMessage(&msg, NULL, 0, 0))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
   return (int) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)
   {
   case WM_DESTROY:

      Shell_NotifyIcon(NIM_DELETE, &nid);
      PostQuitMessage(0);
      break;

   case NID_MSG:

      // Icon Right-Click Menu
      if(LOWORD(lParam) == WM_RBUTTONDOWN){
         POINT lpClickPoint;
         GetCursorPos(&lpClickPoint);
         HMENU hPopMenu = CreatePopupMenu();

         InsertMenu(hPopMenu, 0xFFFFFFFF, MF_BYPOSITION | MF_STRING, IDM_EXIT, _T("EXIT"));

         SetForegroundWindow(hWnd);
         TrackPopupMenu(hPopMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, lpClickPoint.x, lpClickPoint.y, 0, hWnd, NULL);
      }

      // Toggle Pin
      if(LOWORD(lParam) == WM_LBUTTONDOWN){
         if (!(GetWindowLongPtr(AhWnd, GWL_EXSTYLE) & WS_EX_TOPMOST))
	   	{
	   		SetWindowPos(AhWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	   	}
	   	else
	   	{
	   		SetWindowPos(AhWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	   	}
      }
      break;

   case WM_SYSCOMMAND:
   case WM_COMMAND:{
   	int wmId = wParam & 0xFFF0;
      switch (wmId){
         case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
      }
   }
      break;

   default:
      return DefWindowProc(hWnd, message, wParam, lParam);
      break;
   }

   return 0;
}
