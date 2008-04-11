//
// MultiMon sample.
//
// Shows the usage of the ClsMultiMon class.
//

#include "stdafx.h"

#include "../../../Source/all.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class Dlg : public ClsDialog
{
	_NO_COPY( Dlg );
public:
	// No-op constructor and destructor.
	Dlg() {;}
	virtual ~Dlg() {;}

protected:
	// WM_COMMAND handler.
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
	{
		// Evaluate control ID.
		switch ( nCtrlID )
		{
			case	IDOK:
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
		ClsMultiMon mm;
		ClsString s, a;

		// Get the number of display monitors on the system.
		int num = mm.NumDisplayMonitors();

		// Build string.
		s.Format( _T( "Multiple monitor supported by OS = %s\r\nNumber of display monitors = %ld\r\n\r\n" ), mm.OSMultiMonSupport() ? _T( "YES" ) : _T( "NO" ), num );

		// Iterate all monitors and add their monitor and work
		// area rectangles to the string.
		RECT mon, work;
		for ( int i = 0; i < num; i++ )
		{
			// Get rectangles.
			mm.GetMonitorRect( i, &mon );
			mm.GetMonitorRect( i, &work, TRUE );

			// Add monitor data to the string.
			a.Format( _T( "Monitor %ld rectangle : %ld,%ld,%ld,%ld (Workarea: %ld,%ld,%ld,%ld)\r\n" ), i, mon.left, mon.top, mon.right, mon.bottom, work.left, work.top, work.right, work.bottom );
			s += a;
		}

		// Show text in the edit control.
		SetDlgItemText( IDC_MONITORINFO, s );
		SendDlgItemMessage( IDC_MONITORINFO, EM_SETREADONLY, TRUE, 0 );

		// Base class.
		return ClsDialog::OnInitDialog( psp );
	} 
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
			MessageBox( NULL, _T( "Windows 95/98/ME/NT/2000 required!" ), _T( "\"ClsMultiMon\" Sample" ), MB_OK );
			return NULL;
		}
		
		// Create and open the dialog.
		Dlg dlg;
		dlg.DoModal( IDD_MULTIMON );
	}
	return rc;
}
