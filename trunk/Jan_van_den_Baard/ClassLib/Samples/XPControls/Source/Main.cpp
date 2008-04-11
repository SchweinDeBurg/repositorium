#include "stdafx.h"

#include "../../../Source/all.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE 
static char THIS_FILE[] = __FILE__;
#endif  
 
class HotDlg : public ClsDialog
{
	_NO_COPY( HotDlg );
public:
	// No-op constructor and destructor.
	HotDlg() {;}
	virtual ~HotDlg() {;}

protected:
	// WM_COMMAND handler.
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
	{
		switch ( nCtrlID )
		{
			case	IDC_DISABLE:
			{
				BOOL bDisable = check.IsChecked();
				ok1.EnableWindow( !bDisable );
				ok2.EnableWindow( !bDisable );
				edit1.EnableWindow( !bDisable );
				edit2.EnableWindow( !bDisable );
				combo1.EnableWindow( !bDisable );
				combo2.EnableWindow( !bDisable );
				combo3.EnableWindow( !bDisable );
				combo4.EnableWindow( !bDisable );
				break;
			}

			case	IDC_OK_1:
			case	IDC_OK_2:
				EndDialog( 0L );
				break;
		}
		// Pass to the base class.
		return ClsDialog::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
	}

	// WM_INITDIALOG handler.
	virtual LRESULT OnInitDialog( PROPSHEETPAGE *psp )
	{
		check.Attach( GetDlgItemHandle( IDC_DISABLE ));
		ok1.Attach( GetDlgItemHandle( IDC_OK_1 ));
		ok2.Attach( GetDlgItemHandle( IDC_OK_2 ));
		edit1.Attach( GetDlgItemHandle( IDC_EDIT_1 ));
		edit2.Attach( GetDlgItemHandle( IDC_EDIT_2 ));
		combo1.Attach( GetDlgItemHandle( IDC_COMBO_1 ));
		combo2.Attach( GetDlgItemHandle( IDC_COMBO_2 ));
		combo3.Attach( GetDlgItemHandle( IDC_COMBO_3 ));
		combo4.Attach( GetDlgItemHandle( IDC_COMBO_4 ));

		ok1.SetHotFrame();
		ok1.SetHotTrack();
		ok1.SetXPStyle();
		ok2.SetHotFrame();
		ok2.SetHotTrack(); 

		edit1.SetXPStyle( FALSE );
		combo1.SetXPStyle( FALSE );
		combo2.SetXPStyle( FALSE );

		// Add some strings...
		LPCTSTR pszStr[] = { _T("auto"),_T("bool"),_T("break"),_T("case"),_T("catch"),_T("char"),_T("class"),_T("const"),_T("const_cast"),_T("continue"),NULL };
		int i = 0;

		while( pszStr[ i ] )
		{
			combo1.AddString( pszStr[ i ] );
			combo2.AddString( pszStr[ i ] );
			combo3.AddString( pszStr[ i ] );
			combo4.AddString( pszStr[ i++ ] );
		};
		combo1.DoCompletion();
		combo2.DoCompletion();

		// Base class.
		return ClsDialog::OnInitDialog( psp );
	}

	ClsFlatButton ok1, ok2;
	ClsHotEdit edit1, edit2;
	ClsHotComboBox combo1, combo2, combo3, combo4;
	ClsCheckBox check;
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
			MessageBox( NULL, _T( "Windows 95/98/ME/NT/2000 required!" ), _T( "\"XPControls\" Sample" ), MB_OK );
			return NULL;
		}
		
		// Create and open the dialog.
		HotDlg dlg;
		dlg.DoModal( IDD_XPC );
	}
	return rc;
}
