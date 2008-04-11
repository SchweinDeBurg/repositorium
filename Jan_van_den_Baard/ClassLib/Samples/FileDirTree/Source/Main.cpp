//
// FileDirTree sample.
//
// Shows the usage of the ClsFileDirTree class.
//

#include "stdafx.h"

#include "../../../Source/all.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ClsFileDirTree derived class which opens up a descriptive
// dialog which shows the user the selected drive is empty.
class EmptyFileTree : public ClsFileDirTree
{
	_NO_COPY( EmptyFileTree );
public:
	// No-op constructor and destructor.
	EmptyFileTree() {;}
	virtual ~EmptyFileTree() {;}

protected:
	// Show a dialog when the user tried to open a
	// drive which is empty.
	virtual void OnDriveEmpty( ClsString strPath )
	{
		// Get the drive display name.
		SHFILEINFO sfi;
		SHGetFileInfo( strPath, 0, &sfi, sizeof( sfi ), SHGFI_DISPLAYNAME );

		// Format the output.
		ClsString str;
		str.Format( _T( "%s is empty." ), sfi.szDisplayName );

		// Popup dialog.
		MessageBox( str, _T( "\"ClsFileDirTree\" Sample" ), MB_ICONWARNING | MB_OK );
	}
};

// Dialog which contains a ClsFileDirTree control, 
// a checkbox, a infobar control and a button in a 
// layout engine.
class FileDirTreeDlg : public ClsSizeDialog
{
	_NO_COPY( FileDirTreeDlg );
public:
	// No-op constructor and destructor.
	FileDirTreeDlg() {;}
	virtual ~FileDirTreeDlg() {;}

protected:
	// Window procedure override.
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		// Make sure the dialog does not get resized
		// smaller than it should.
		if ( uMsg == WM_GETMINMAXINFO )
		{
			LPMINMAXINFO pmmi = ( LPMINMAXINFO )lParam;
			pmmi->ptMinTrackSize.x = sz_master.CX();
			pmmi->ptMinTrackSize.y = sz_master.CY();
			return 0;
		}
		// Pass the rest on to the base class.
		return ClsSizeDialog::WindowProc( uMsg, wParam, lParam );
	}

	// WM_NOTIFY handler.
	virtual LRESULT OnNotify( LPNMHDR pNMHDR )
	{
		// Is this the correct notification from the tree?
		if ( pNMHDR->idFrom == IDC_TREE && pNMHDR->code == TVN_SELCHANGED )
		{
			// Get the current selected path.
			ClsString s;
			files.GetSelectedPath( s );

			// Set it in the static control.
			path.SetWindowText( s );
			return 0;
		}

		// Pass to the base class.
		return ClsSizeDialog::OnNotify( pNMHDR );
	}

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

			case	IDC_SHOWFILES:
				// Switch the ShowFiles option on or off depending
				// on the checkbox state.
				files.ShowFiles(( BOOL )( IsDlgButtonChecked( IDC_SHOWFILES ) == BST_CHECKED ));
				
				// Focus the tree.
				files.SetFocus();
				return 0;
		}
		// Pass to the base class.
		return ClsSizeDialog::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
	}

	// WM_SIZE handler.
	virtual LRESULT OnSize( UINT nSizeType, int nWidth, int nHeight )
	{
		// Re-layout the master layout engine.
		master.Relayout();

		// Pass to the base class.
		return ClsSizeDialog::OnSize( nSizeType, nWidth, nHeight );
	}

	// WM_INITDIALOG handler.
	virtual LRESULT OnInitDialog( PROPSHEETPAGE *psp )
	{
		// Setup icon.
		SendMessage( WM_SETICON, ICON_SMALL, ( WPARAM )::LoadIcon( NULL, IDI_APPLICATION ));
		
		// Attach (subclass) the treeview control so that it
		// becomes a ClsFileDirTree control.
		files.Attach( GetDlgItemHandle( IDC_TREE ));
		files.SetupTree();
		files.LoadingText() = _T( "Loading. Please wait..." );
		files.LoadingTextColor() = RGB( 255, 0, 0 );

		// Attach (subclass) the other controls.
		check.Attach( GetDlgItemHandle( IDC_SHOWFILES ));
		button.Attach( GetDlgItemHandle( IDC_OK ));
		path.Attach( GetDlgItemHandle( IDC_PATH ));
		button.SetHotFrame();
		
		// We display a path in the info bar.
		path.CompactAsPath() = TRUE;
		path.TipClippedCaption();

		// Check the checkbox.
		CheckDlgButton( IDC_SHOWFILES, BST_CHECKED );
		
		// Create the layout engine controls. For a more
		// comprehensive demonstration of the layout engine
		// and it's possibilities look at the layout engine
		// samples.
		master.Create( *this, LAYOUT_Horizontal, FALSE, 
				      LAYOUT_Master,	 TRUE,
				      LAYOUT_Spacing,	 6,
				      Offsets( 6, 6, 6, 6 ),
				      TAG_END );
		slave.Create( *this,  Offsets( 0, 0, 0, 0 ),
				      TAG_END );
		
		// Add two spacing members with the button in the
		// middle to the horizontal sub-group.
		slave.AddSpacingMember();
	 	slave.AddMember( &button, NULL, ATTR_UseControlSize, TRUE, ATTR_FixMinWidth, TRUE, TAG_END );
		slave.AddSpacingMember();

		// Add the controls to the master layout engine.
		master.AddMember( &path, NULL, ATTR_FixMinHeight, TRUE, TAG_END );
		master.AddMember( &files, NULL, ATTR_UseControlSize, TRUE, TAG_END );
		master.AddMember( &check, NULL, ATTR_FixMinSize, TRUE, TAG_END );
		master.AddMember( &slave, NULL, ATTR_FixMinHeight, TRUE, TAG_END );

		// Check for errors.
		if ( master.Error())
		{
			EndDialog( FALSE );
			return FALSE;
		}
		
		// Get the master layout engine it's minimum size.
		if ( master.OnGetMinSize( sz_master ) == FALSE )
		{
			EndDialog( 0 );
			return FALSE;
		}

		// Add frames to the minimum size.
		sz_master.CY() += ( ::GetSystemMetrics( SM_CYFRAME ) * 2 ) + ::GetSystemMetrics( SM_CYCAPTION );
		sz_master.CX() += ::GetSystemMetrics( SM_CXFRAME ) * 2;

		// Layout the master layout engine.
		master.Relayout();
 
		// Pass to the base class.
		return ClsSizeDialog::OnInitDialog( psp );
	} 

	// Data.
	EmptyFileTree		files;
	ClsCheckBox		check;
	ClsFlatButton		button;
	ClsInfoBar		path;
	ClsLayoutEngine		master, slave;
	ClsSize			sz_master;
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
			MessageBox( NULL, _T( "Windows 95/98/ME/NT/2000 required!" ), _T( "\"ClsFileDirTree\" Sample" ), MB_OK );
			return NULL;
		}
		
		// Create and open the dialog.
		FileDirTreeDlg	dlg;
		dlg.DoModal( IDD_FILEDIRTREE );
	}
	return rc;
}