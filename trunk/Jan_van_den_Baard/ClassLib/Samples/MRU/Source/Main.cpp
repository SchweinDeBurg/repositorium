//
// MRU sample.
//
// Shows the usage of the ClsMRU/ClsFlatButton classes.
//

#include "stdafx.h"

#include "../../../Source/all.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Dialog which contains a few buttons to show the
// ClsMRU class usage.
class MRUDlg : public ClsDialog
{
	_NO_COPY( MRUDlg );
public:
	// No-op constructor and destructor.
	MRUDlg() : fromrem( FALSE ) {;}
	virtual ~MRUDlg() {;}

protected:
	// WM_COMMAND handler.
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
	{
		// Was a menu from the MRU list selected?
		if ( hWndCtrl == NULL && mru.IsMRUSelection( nCtrlID ))
		{
			// Display the path of the selection or
			// an error message.
			ClsString str;
			if ( mru.GetMRUEntryPath( nCtrlID, str ))
			{
				// Show dialog.
				MessageBox( str, fromrem ? _T( "MRU Remove..." ) : _T( "MRU Selection..." ));
				
				// Was the selection made from the remove button?
				if ( fromrem )
				{
					mru.RemoveMRUEntry( str );
					fromrem = FALSE;
				}
			}
			else					  
				MessageBox( _T( "Problems obtaining MRU selection" ));
		}
		else
		{
			// Evaluate control ID.
			switch ( nCtrlID )
			{
				case	IDC_ABOUT:
				{
					ClsMessageBox::MsgBox( *this, 
							       _T( "*&OK" ),
							       _T( "The " ) ISEQ_BOLD _T( "ClsMRU" ) ISEQ_NORMAL _T( " class enables you to keep track\n" )
							       _T( "of a so called " )
							       ISEQ_TEXTRGB( _T( "255,0,0" ) ) _T( "M" ) ISEQ_TEXTSYSCOLOR( IG_COLOR_BTNTEXT ) _T( "ost " )
							       ISEQ_TEXTRGB( _T( "255,0,0" ) ) _T( "R" ) ISEQ_TEXTSYSCOLOR( IG_COLOR_BTNTEXT ) _T( "ecently " )
							       ISEQ_TEXTRGB( _T( "255,0,0" ) ) _T( "U" ) ISEQ_TEXTSYSCOLOR( IG_COLOR_BTNTEXT ) _T( "sed list. The class\n" )
							       _T( "will track the contents in a menu which you\n" )
							       _T( "can show at any given moment.\n\nThe contents can be stored and retrieved from\n" )
							       _T( "the system registry." ),
							       _T( "ClsMRU sample..." ), ClsMessageBox::MBF_HOTBUTTONS );
					break;
				}

				case	IDC_FILEICONS:
					mru.UseIcons( icons.GetCheck() == BST_CHECKED ? TRUE : FALSE );
					break;

				case	IDC_OFFICE_XP:
				{
					// Get selection.
					BOOL bXPStyle = ( xp.GetCheck() == BST_CHECKED ) ? TRUE : FALSE;

					// Change buttons.
					add.SetXPStyle( bXPStyle, TRUE ); 
					rem.SetXPStyle( bXPStyle, TRUE );
					show.SetXPStyle( bXPStyle, TRUE ); 
					save.SetXPStyle( bXPStyle, TRUE );
					about.SetXPStyle( bXPStyle, TRUE );

					// And the mru menu.
					mru.XPStyle( bXPStyle );
					break;
				}
					
				case	IDOK:
					// Close the dialog.
					EndDialog( 0 );
					return 0;

				case	IDC_ADD:
				{
					// Open a file dialog.
					ClsFileDialog fd;
					ClsRect rc;
					add.GetWindowRect( rc );
					if ( fd.DoModal( this, NULL, NULL, TRUE, OFN_ALLOWMULTISELECT | OFN_ENABLESIZING, rc.Left(), rc.Bottom()))
					{
						// Get all the selected names.
						ClsString file;
						int index;
						if (( index = fd.GetName( 0, file )) > 0 )
						{
							do
							{
								// Make sure a backslash is added before
								// the file name if necessary.
								if ( fd.Dir()[ fd.Dir().GetStringLength() - 1 ] != _T( '\\' ))
									file.Insert( 0, _T( '\\' ));

								// Add the path to the MRU list.
								mru.AddMRUEntry( fd.Dir() + file );
							} while (( index = fd.GetName( index, file )) > 0 );
						}
					}
					break;
				}

				case	IDC_REM:
				{
					// Display the popup menu under the button.
					ClsRect	rc;
					rem.GetWindowRect( rc );
					fromrem = TRUE;
					mru.GetMenuObject()->SetParentRect( rc );
					TrackPopupMenu( mru, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, max( rc.Left(), 0 ), rc.Bottom(), NULL, *this, NULL );
					break;
				}

	 			case	IDC_SHOW:
				{
					// Display the popup menu under the button.
					ClsRect	rc;
					show.GetWindowRect( rc );
					fromrem = FALSE;
					mru.GetMenuObject()->SetParentRect( rc );
					TrackPopupMenu( mru, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, max( rc.Left(), 0 ), rc.Bottom(), NULL, *this, NULL );
					break;
				}

				case	IDC_SAVE:
					// Write the current MRU contents to the registry.
					if ( ClsMessageBox::MsgBox( *this, 
							 _T( "&Yes|*&No" ),
							 ISEQ_TEXTRGB( _T( "255,0,0" )) ISEQ_BOLD _T( "Warning:" ) ISEQ_NORMAL ISEQ_TEXTSYSCOLOR( IG_COLOR_BTNTEXT )
							 _T( "\n\nThis option will save the information in the\n" )
							 _T( "MRU menu to the system registry under the key:\n\nHKEY_CURRENT_USER\\Software\\ClassLibSamples\n\nDo you want to continue?" ), _T( "ClsMRU sample" ), ClsMessageBox::MBF_ICONWARNING | ClsMessageBox::MBF_HOTBUTTONS ) == 1 )
					{
						if ( mru.SaveMRUList())
							ClsMessageBox::MsgBox( *this, _T( "*&OK" ), _T( "MRU stored in the registry succesfully." ), _T( "ClsMRU sample" ), ClsMessageBox::MBF_ICONINFORMATION );
					}
					break;
					
			}
		}
		// Pass to the base class.
		return ClsDialog::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
	}

	// WM_INITDIALOG handler.
	virtual LRESULT OnInitDialog( PROPSHEETPAGE *psp )
	{
		img.LoadBitmap( MAKEINTRESOURCE( IDB_BITMAP1 ), 16, 0, RGB( 255, 0, 255 ));

		// Setup icon.
		SendMessage( WM_SETICON, ICON_SMALL, ( WPARAM )::LoadIcon( NULL, IDI_APPLICATION ));

		// Setup buttons.
		add.Attach(  GetDlgItemHandle( IDC_ADD  )); 
		add.SetHotFrame(); 
		add.SetXPStyle(); 
		add.SetImageList( img ); 
		add.SetImageIndex( ClsFlatButton::FIIF_NORMAL, 0 );
		add.SetImageIndex( ClsFlatButton::FIIF_HOT, 0 );
		
		rem.Attach(  GetDlgItemHandle( IDC_REM  )); 
		rem.SetHotFrame(); 
		rem.SetXPStyle(); 
		rem.SetImageList( img );
		rem.SetImageIndex( ClsFlatButton::FIIF_NORMAL, 1 );
		rem.SetImageIndex( ClsFlatButton::FIIF_HOT, 1 );
		rem.ShowXPDefault();

		show.Attach( GetDlgItemHandle( IDC_SHOW )); 
		show.SetHotFrame(); 
		show.SetXPStyle(); 
		show.SetImageList( img );
		show.SetImageIndex( ClsFlatButton::FIIF_NORMAL, 2 );
		show.SetImageIndex( ClsFlatButton::FIIF_HOT, 2 );
		show.ShowXPDefault();

		save.Attach( GetDlgItemHandle( IDC_SAVE )); 
		save.SetHotFrame(); 
		save.SetXPStyle(); 
		save.SetImageList( img );
		save.SetImageIndex( ClsFlatButton::FIIF_NORMAL, 3 );
		save.SetImageIndex( ClsFlatButton::FIIF_HOT, 3 );
		
		about.Attach( GetDlgItemHandle( IDC_ABOUT )); 
		about.SetHotFrame();
		about.SetXPStyle( TRUE );

		xp.Attach( GetDlgItemHandle( IDC_OFFICE_XP )); xp.SetCheck();
		icons.Attach( GetDlgItemHandle( IDC_FILEICONS )); icons.SetCheck();
		ok.Attach(   GetDlgItemHandle( IDOK     )); ok.SetHotFrame();

		// Setup the MRU list.
		if ( mru.SetupMRU( HKEY_CURRENT_USER, _T( "Software\\ClassLibSamples" )) == FALSE )
			EndDialog( 0 );

		// We want XP style menus.
		mru.XPStyle( TRUE );
		mru.UseIcons( TRUE );

		// Pass to the base class.
		return ClsDialog::OnInitDialog( psp );
	} 

	// Data.
	ClsXPMenu	hMenu;
	ClsMRU		mru;
	ClsFlatButton	add, rem, show, save, ok, about;
	ClsCheckBox	xp, icons;
	ClsImageList	img;
	BOOL		fromrem;
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
			MessageBox( NULL, _T( "Windows 95/98/ME/NT/2000 required!" ), _T( "\"ClsMRU\" Sample" ), MB_OK );
			return NULL;
		}
		
		// Create and open the dialog.
		MRUDlg	dlg;
		dlg.DoModal( IDD_MRU );
	}
	return rc;
}
