#pragma once

const int WM_MYMESSAGE = WM_USER + 100;

BOOL InstallHook( HWND hWnd, DWORD dwPID, BOOL bGlobalHook = FALSE );

void UnInstallHook();

typedef HRESULT( *PatchCallback )( LPVOID lpParam );
void SetCallback( PatchCallback pCallback, LPVOID lpParam );

static LRESULT CALLBACK LowKeyboardProc( int code, WPARAM wParam, LPARAM lParam );

