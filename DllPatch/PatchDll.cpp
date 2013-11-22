#include "stdafx.h"
#include "PatchDll.h"
#include <shellapi.h>
#include <tchar.h>
#include <tlhelp32.h>

#pragma data_seg ( "shared" )
HHOOK g_hHook = NULL ;
PatchCallback g_pCallback = NULL;
LPVOID g_pParam = NULL;
HMODULE g_hDll = NULL;
HWND g_hWndNotify = NULL;
#pragma data_seg ()
#pragma comment ( linker, "/section:shared,rws" )

DWORD GetMainThreadID( DWORD dwPID )
{
    DWORD dwThreadID = 0;
    THREADENTRY32 te32 = {sizeof( te32 )};
    HANDLE hThreadSnap = CreateToolhelp32Snapshot( TH32CS_SNAPTHREAD, 0 );
    if ( Thread32First( hThreadSnap, &te32 ) )
    {
        do
        {
            if ( dwPID == te32.th32OwnerProcessID )
            {
                dwThreadID = te32.th32ThreadID;
                break;
            }
        }
        while ( Thread32Next( hThreadSnap, &te32 ) );
    }
    return dwThreadID;
}

BOOL InstallHook( HWND hWnd, DWORD dwPID, BOOL bGlobalHook )
{
    g_hWndNotify = hWnd;
    
    if ( NULL == g_hHook )
    {
        DWORD dwThreadID = bGlobalHook ? 0 : GetMainThreadID( dwPID );
        g_hHook =::SetWindowsHookEx( WH_KEYBOARD, KeyboardProc, g_hDll, dwThreadID );
    }
    return ( NULL != g_hHook );
}

void UnInstallHook()
{
    if ( NULL != g_hHook )
    {
        ::UnhookWindowsHookEx( g_hHook );
        g_hHook = NULL;
    }
}

LRESULT CALLBACK KeyboardProc( int code, WPARAM wParam, LPARAM lParam )
{
    LRESULT lRet =::CallNextHookEx( g_hHook, code, wParam, lParam );
    if ( code == HC_ACTION )
    {
        DWORD dwVK = ( DWORD )wParam;
        
        if ( (VK_SPACE == dwVK) && (lParam & 0x80000000) )
        {
            SendMessage( g_hWndNotify, WM_MYMESSAGE, ( WPARAM )g_pCallback, ( LPARAM )g_pParam );
        }
    }
    return lRet;
}

void SetCallback( PatchCallback pCallback, LPVOID lpParam )
{
    g_pCallback = pCallback;
    g_pParam = lpParam;
}
