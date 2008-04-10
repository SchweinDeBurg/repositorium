//
// Colorpicker sample.
//
// Shows the usage of the ClsColorPicker class.
// 

#include "stdafx.h"

#include "../../../Source/all.h"
#include "Resource.h"
 
#ifdef _DEBUG  
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
 
class ColorPickerDlg : public ClsDialog
{
	_NO_COPY( ColorPickerDlg ); 
public:
	// No-op constructor and destructor.
	ColorPickerDlg() {;}
	virtual ~ColorPickerDlg() {;}

protected:
	virtual LRESULT OnInitDialog( PROPSHEETPAGE *p )
	{
		// Attach (subclass) and setup the colorpickers.
		picker1.Attach( GetDlgItemHandle( IDC_COLOR ));
		picker1.SetHotFrame();
		picker1.ShowToolTip();
		picker1.Color() = RGB( 255, 0, 0 );
		
		picker2.Attach( GetDlgItemHandle( IDC_COLOR2 ));
		picker2.ShowToolTip();
		picker2.DropDownArrow() = TRUE;
		picker2.SetXPStyle( TRUE, TRUE );
		picker2.Color() = RGB( 0, 255, 0 );

		picker3.Attach( GetDlgItemHandle( IDC_COLOR3 ));
		picker3.ShowToolTip();
		picker3.SetHotTrack( FALSE );
		picker3.Color() = RGB( 0, 0, 255 );
		
		// Attach the buttons.
		about.Attach( GetDlgItemHandle( IDC_ABOUT ));
		about.SetHotFrame();
		
		ok.Attach( GetDlgItemHandle( IDOK ));
		ok.SetHotFrame();
		return ClsDialog::OnInitDialog( p );
	}

	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
	{
		if ( nCtrlID == IDC_ABOUT )
		{
			// Show the about box.
			ClsMessageBox::MsgBox( *this,
					_T( "*&OK" ),
					_T( "The code for the \"" ) ISEQ_ITALIC _T( "ColourPopup" ) ISEQ_NORMAL _T( "\" is a version of the\n" )
					_T( "excellent original MFC code by Chris Maunder\n" )
					_T( "(" ) ISEQ_BOLD _T( "www.codeproject.com" ) ISEQ_NORMAL _T( ")\ntweaked to run with this class library ." ),
					_T( "\"ClsColorPicker\" Sample" ), ClsMessageBox::MBF_ICONINFORMATION );
			return 0;
		}
		return ClsDialog::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
	}
	
	ClsColorPicker picker1, picker2, picker3;
	ClsFlatButton about, ok;
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
			MessageBox( NULL, _T( "Windows 95/98/ME/NT/2000 required!" ), _T( "\"ClsColorPicker\" Sample" ), MB_OK );
			return NULL;
		}
		
		// Create and open the dialog.
		ColorPickerDlg	dlg;
		dlg.DoModal( IDD_COLORPICKER );
	}
	return rc;
}
