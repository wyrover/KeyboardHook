// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "..\DllPatch\PatchDll.h"
#pragma comment(lib,"DllPatch.lib")
#include <atlstr.h>


class CMainDlg : public CDialogImpl<CMainDlg>
{
private:
    static HRESULT MyCallback( LPVOID lPParam )
    {
        CMainDlg* pThis = ( CMainDlg* )lPParam;
        if ( NULL != pThis )
        {
            ShellExecute( NULL, _T( "open" ), _T( "calc.exe" ), NULL, NULL, SW_SHOW );
        }
        
        return S_OK;
    }
public:
    enum { IDD = IDD_MAINDLG };
    
    BEGIN_MSG_MAP( CMainDlg )
    MESSAGE_HANDLER( WM_INITDIALOG, OnInitDialog )
    COMMAND_ID_HANDLER( ID_APP_ABOUT, OnAppAbout )
    COMMAND_ID_HANDLER( IDOK, OnOK )
    COMMAND_ID_HANDLER( IDCANCEL, OnCancel )
    MESSAGE_HANDLER( WM_MYMESSAGE, OnMyMsg )
    COMMAND_ID_HANDLER( IDC_BUTTON1, OnBtnInstallHook )
    COMMAND_ID_HANDLER( IDC_BUTTON2, OnBtnUnInstallHook )
    END_MSG_MAP()
    
    DWORD GetExplorerPID()
    {
        HWND hProgman  = 	 GetShellWindow();
        if ( hProgman == NULL )
            hProgman = FindWindow( 0, _T( "Progman Manager" ) );
        DWORD dwProcessId = 0;
        GetWindowThreadProcessId( hProgman, &dwProcessId );
        return dwProcessId;
    }
    
    LRESULT OnMyMsg( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/ )
    {
        PatchCallback callBackFunc = ( PatchCallback )wParam;
        callBackFunc( ( LPVOID )lParam );
        return 0;
    }
    
    BOOL IsGlobalHook()
    {
        CButton btnRadio1 = GetDlgItem( IDC_RADIO1 );
        CButton btnRadio2 = GetDlgItem( IDC_RADIO2 );
        if ( BST_CHECKED == btnRadio1.GetCheck() )
        {
            return TRUE;
        }
        else if ( BST_CHECKED == btnRadio2.GetCheck() )
        {
            return FALSE;
        }
        return FALSE;
    }
    
    LRESULT OnInitDialog( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
    {
        // center the dialog on the screen
        CenterWindow();
        
        // set icons
        HICON hIcon = AtlLoadIconImage( IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics( SM_CXICON ), ::GetSystemMetrics( SM_CYICON ) );
        SetIcon( hIcon, TRUE );
        HICON hIconSmall = AtlLoadIconImage( IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics( SM_CXSMICON ), ::GetSystemMetrics( SM_CYSMICON ) );
        SetIcon( hIconSmall, FALSE );
        
        CButton btnRadio1 = GetDlgItem( IDC_RADIO1 );
        btnRadio1.SetCheck( BST_CHECKED );
        return TRUE;
    }
    
    LRESULT OnBtnInstallHook( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        UnInstallHook();
        BOOL bGlobalHook = IsGlobalHook();
        if ( InstallHook( m_hWnd, GetExplorerPID() , bGlobalHook ) )
        {
            SetCallback( MyCallback, this );
        }
        return 0;
    }
    
    LRESULT OnBtnUnInstallHook( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        UnInstallHook();
        return 0;
    }
    
    LRESULT OnAppAbout( WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        CSimpleDialog<IDD_ABOUTBOX, FALSE> dlg;
        dlg.DoModal();
        return 0;
    }
    
    LRESULT OnOK( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        // TODO: Add validation code
        EndDialog( wID );
        return 0;
    }
    
    LRESULT OnCancel( WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/ )
    {
        UnInstallHook();
        EndDialog( wID );
        return 0;
    }
};
