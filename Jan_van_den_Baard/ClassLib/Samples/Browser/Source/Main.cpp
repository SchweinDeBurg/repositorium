#include "stdafx.h"

#include "../../../Source/all.h"
#include "Resource.h"

#ifdef _DEBUG 
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// A simple dialog class.
class CrcDlg : public ClsDialog
{
	_NO_COPY( CrcDlg );
public:
	// No-op constructor and destructor.
	CrcDlg() {;}
	virtual ~CrcDlg() {;}

protected:
	// WM_COMMAND handler.
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
	{
		// The checkbox?
		if ( nCtrlID == IDC_SHOWFILES )
		{
			m_Browser.ShowFiles() = m_Check.IsChecked();
			return 0;
		}
		return ClsDialog::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
	}

	// WM_INITDIALOG handler.
	virtual LRESULT OnInitDialog( PROPSHEETPAGE *psp )
	{
		// Attach FileBrowser control.
		m_Browser.Attach( GetDlgItemHandle( IDC_FILE ));
		m_Browser.LoadingTextColor() = RGB( 255, 0, 0 );
		m_Browser.ShowFiles() = FALSE;

		m_Check.Attach( GetDlgItemHandle( IDC_SHOWFILES ));
		m_Check.SetCheck( BST_UNCHECKED );

		return ClsDialog::OnInitDialog( psp );
	}
	// Data.
	ClsFileDirBrowser	m_Browser;
	ClsCheckBox		m_Check;
};

// Entry point.
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nShowCmd )
{
	int		rc = 0;

	// Setup app.
	if ( ClsGetApp()->Setup( hInstance, lpCmdLine, nShowCmd ))
	{
		// Check OS version.
		if ( ClsGetApp()->GetPlatformID() != VER_PLATFORM_WIN32_WINDOWS &&
		     ClsGetApp()->GetPlatformID() != VER_PLATFORM_WIN32_NT )
		{
			MessageBox( NULL, _T( "Windows 95/98/ME/NT/2000/XP required!" ), _T( "\"ClsCrc\" Sample" ), MB_OK );
			return NULL;
		}

		// Create and open the dialog.
		CrcDlg	dlg;
		dlg.DoModal( IDD_CRC );
	}
	return rc;
}
