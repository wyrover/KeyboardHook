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

BOOL g_bDbClickSpace = FALSE;
int g_dbClickSpaceCNT = 0;
DWORD g_dwFirstClickSpace = 0;
DWORD g_dwSecondClickSpace = 0;

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
        
        const int DBCLICKTIMEOUT = 500;
        
        if ( ( VK_SPACE == dwVK )  && ( lParam & 0x80000000 ) )	// 按下并抬起了空格键
        {
            static BOOL bFirst = TRUE;
        LABEL:
            if ( bFirst )
            {
                g_dbClickSpaceCNT = 1;
                g_dwFirstClickSpace = GetTickCount();
                bFirst = FALSE;
            }
            else
            {
                g_dbClickSpaceCNT++;
                g_dwSecondClickSpace = GetTickCount();
                
                if ( ( g_dwSecondClickSpace - g_dwFirstClickSpace >= DBCLICKTIMEOUT ) )
                {
                    g_dwFirstClickSpace = GetTickCount();
                    bFirst = TRUE;
                    goto LABEL;
                }
                if ( g_dbClickSpaceCNT == 2 )
                {
                    SendMessage( g_hWndNotify, WM_MYMESSAGE, ( WPARAM )g_pCallback, ( LPARAM )g_pParam );
                }
                bFirst = TRUE;
            }
        }
    }
    return lRet;
}

void SetCallback( PatchCallback pCallback, LPVOID lpParam )
{
    g_pCallback = pCallback;
    g_pParam = lpParam;
}
