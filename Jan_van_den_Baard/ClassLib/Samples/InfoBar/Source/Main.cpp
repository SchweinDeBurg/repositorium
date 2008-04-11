//
// InfoBar sample.
//
// Shows the usage of the ClsInfoBar class.
//

#include "stdafx.h"

#include "../../../Source/all.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Dialog which contains a few ClsInfoBar controls.
class InfoBarDlg : public ClsDialog
{
	_NO_COPY( InfoBarDlg );
public:
	// No-op constructor and destructor.
	InfoBarDlg() {;}
	virtual ~InfoBarDlg() {;}

protected:
	// WM_COMMAND handler.
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
	{
		// Evaluate control ID.
		switch ( nCtrlID )
		{
			case	IDC_OK:
				// Close the dialog.
				EndDialog( 0 );
				return 0;
		}
		// Pass to the base class.
		return ClsDialog::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
	}

	// WM_INITDIALOG handler.
	virtual LRESULT OnInitDialog( PROPSHEETPAGE *psp )
	{
		// Setup icon.
		SendMessage( WM_SETICON, ICON_SMALL, ( WPARAM )::LoadIcon( NULL, IDI_APPLICATION ));
		
		// Attach (subclass) the controls.
		left.Attach( GetDlgItemHandle( IDC_VERTTOP ));
		left.SetIcon( MAKEINTRESOURCE( IDI_TOP ));
		left.Gradient() = FALSE;
		left.Vertical() = TRUE;
		left.BackgroundColor() = ::GetSysColor( COLOR_ACTIVECAPTION );

		top.Attach( GetDlgItemHandle( IDC_HORIZLEFT ));
		top.SetIcon( MAKEINTRESOURCE( IDI_RIGHT ));

		right.Attach( GetDlgItemHandle( IDC_VERTBOTTOM ));
		right.SetIcon( MAKEINTRESOURCE( IDI_BOTTOM ));
		right.IconRight() = FALSE;
		right.Gradient() = TRUE;
		right.Vertical() = TRUE;
		right.BackgroundColor() = ::GetSysColor( COLOR_ACTIVECAPTION );
		right.BackgroundGradient() = ::GetSysColor( COLOR_GRADIENTACTIVECAPTION );

		bottom.Attach( GetDlgItemHandle( IDC_HORIZRIGHT ));
		bottom.SetIcon( MAKEINTRESOURCE( IDI_LEFT ));
		bottom.IconRight() = FALSE;
		bottom.Gradient() = TRUE;

		desc.Attach( GetDlgItemHandle( IDC_DESC ));
		desc.SetWindowText( ISEQ_CENTER _T( "A couple of \"" ) ISEQ_BOLD _T( "ClsInfoBar" ) ISEQ_NORMAL _T( "\" controls\n\n" )
						_T( "BTW. This descriptive text is shown in a\n" )
						_T( "\"" ISEQ_BOLD _T( "ClsInfoControl" ) ISEQ_NORMAL _T( "\" control.\n\n" ) )
						_T( "By using command sequences in the text this\ncontrol can show text styles like:\n\n" )
						ISEQ_BOLD _T( "Bold" ) ISEQ_NORMAL _T( "," ) 
						ISEQ_ITALIC _T( "Italic" ) ISEQ_NORMAL _T( "," )
						ISEQ_UNDERLINE _T( "Underline" ) ISEQ_NORMAL _T( ", and " )
						ISEQ_BOLD ISEQ_ITALIC ISEQ_UNDERLINE _T( "Combinations" ) ISEQ_NORMAL
						_T( "\n\n" )
						_T( "Also text in " )
						ISEQ_BACKRGB( _T( "255,255,0" ) )
						ISEQ_TEXTRGB( _T( "255,0,0C" ) )
						ISEQ_TEXTRGB( _T( "0,155,0o" ) )
						ISEQ_TEXTRGB( _T( "0,0,255l" ) )
						ISEQ_TEXTRGB( _T( "0,155,0o" ) )
						ISEQ_TEXTRGB( _T( "255,0,0r" ) )
						ISEQ_TEXTRGB( _T( "0,155,0s" ) )
						ISEQ_TEXTSYSCOLOR( IG_COLOR_BTNTEXT )
						ISEQ_BACKSYSCOLOR( IG_COLOR_BTNFACE )
						_T( " is possible...\n\n" )
						_T( "Besides that text can also be\n\n" )
						ISEQ_LEFT _T( "left aligned\n" )
						ISEQ_RIGHT _T( "right aligned\n" )
						ISEQ_CENTER _T( "or centered" ));
		
		button.Attach( GetDlgItemHandle( IDC_OK ));
		button.SetHotFrame();
		
		// Pass to the base class.
		return ClsDialog::OnInitDialog( psp );
	} 

	// Data.
	ClsFlatButton		button;
	ClsInfoBar		left, top, right, bottom;
	ClsInfoControl		desc;
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
			MessageBox( NULL, _T( "Windows 95/98/ME/NT/2000 required!" ), _T( "\"ClsInfoBar\" Sample" ), MB_OK );
			return NULL;
		}
		
		// Create and open the dialog.
		InfoBarDlg	dlg;
		dlg.DoModal( IDD_INFOBAR );
	}
	return rc;
}
