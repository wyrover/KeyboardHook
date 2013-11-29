#include "stdafx.h"
#include "KeyboardHook.h"
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
        g_hHook =::SetWindowsHookEx( WH_KEYBOARD_LL, LowKeyboardProc, g_hDll, dwThreadID );
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

void SetCallback( PatchCallback pCallback, LPVOID lpParam )
{
    g_pCallback = pCallback;
    g_pParam = lpParam;
}

LRESULT CALLBACK LowKeyboardProc( int code, WPARAM wParam, LPARAM lParam )
{
    static BOOL bDbClicked = FALSE;
    static DWORD dwFirstClickTime = 0;
    static const DWORD DBCLICKTIME = 500;
    
    DWORD dwVKCode = ( DWORD )wParam;
    
    LRESULT lRet =::CallNextHookEx( g_hHook, code, wParam, lParam );
    if ( HC_ACTION != code )
    {
        return lRet;
    }
    
    KBDLLHOOKSTRUCT* pKbDllHookStruct = ( KBDLLHOOKSTRUCT* )( lParam );
    if ( NULL == pKbDllHookStruct )
    {
        return lRet;
    }
    
    if ( pKbDllHookStruct->flags & LLKHF_INJECTED )
    {
        return lRet;
    }
    
    if ( ( pKbDllHookStruct->vkCode != VK_LCONTROL ) && ( pKbDllHookStruct->vkCode != VK_RCONTROL ) )
    {
        dwFirstClickTime = 0;
        return lRet;
    }
    
    if ( wParam == WM_KEYUP )
    {
        if ( bDbClicked )
        {
            DWORD dwSecondClickTime = GetTickCount();
            DWORD dwIntervalTime = dwSecondClickTime - dwFirstClickTime;
            if ( dwIntervalTime < DBCLICKTIME )
            {
                SendMessage( g_hWndNotify, WM_MYMESSAGE, ( WPARAM )g_pCallback, ( LPARAM )g_pParam );
                dwFirstClickTime = 0;
                bDbClicked = FALSE;
                return lRet;
            }
            else
            {
                dwFirstClickTime = dwSecondClickTime;
                bDbClicked = FALSE;
            }
        }
        else
        {
            bDbClicked = FALSE;
        }
    }
    else
    {
        if ( dwVKCode == WM_KEYFIRST )
        {
            bDbClicked = TRUE;
        }
    }
    
    return lRet;
}
