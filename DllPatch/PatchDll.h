#pragma once

#ifndef _USRDLL
#define FUNC_EXPORTS extern "C" __declspec(dllimport)
#else
#define FUNC_EXPORTS extern "C" __declspec(dllexport)
#endif

const int WM_MYMESSAGE = WM_USER + 100;

FUNC_EXPORTS BOOL InstallHook( HWND hWnd, DWORD dwPID, BOOL bGlobalHook = FALSE );

FUNC_EXPORTS void UnInstallHook();

typedef HRESULT( *PatchCallback )( LPVOID lpParam );
FUNC_EXPORTS void SetCallback( PatchCallback pCallback, LPVOID lpParam );

static LRESULT CALLBACK KeyboardProc( int code, WPARAM wParam, LPARAM lParam );

