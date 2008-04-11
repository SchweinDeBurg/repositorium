#include "stdafx.h"

#include "../../../Source/all.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class XPDialog : public ClsDialog
{
	_NO_COPY( XPDialog );
public:
	// No-op constructor/destructor.
	XPDialog() {;}
	virtual ~XPDialog() {;}

protected:
	// WM_COMMAND message handler.
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlId, HWND hWndCtrl )
	{
		switch ( nCtrlId )
		{
			case	ID_IMAGE:
			case	ID_SUBITEMS_CHECKABLE:
			{
				// Invert the checked status of the selected ite,
				ClsMenuItemInfo mi;
				mi.fMask = MIIM_STATE;
				m_Menu.GetItemInfo( nCtrlId, &mi);
				m_Menu.CheckItem( nCtrlId, mi.fState & MFS_CHECKED ? MFS_UNCHECKED : MFS_CHECKED );

				if ( nCtrlId == ID_SUBITEMS_CHECKABLE )
					m_Menu.DrawMenuShadow( mi.fState & MFS_CHECKED ? FALSE : TRUE );
				break;
			}

			case	ID_SUBITEMS_SUBITEM1:
			case	ID_SUBITEMS_SUBITEM2:
				// Setup the new rendering style of the menu and
				// update the radiochecks.
				m_Menu.OldStyle() = nCtrlId == ID_SUBITEMS_SUBITEM1 ? TRUE : FALSE;
				m_Menu.CheckRadioItem( ID_SUBITEMS_SUBITEM1, ID_SUBITEMS_SUBITEM2, nCtrlId );

				// Should be done after changing the menu.
				DrawMenuBar();
				break;

			case	ID_EXIT:
				EndDialog( 0 );
				break;

			default:
			{
				// Show the selected item.
				if ( hWndCtrl == NULL && nCtrlId != IDOK && nCtrlId != IDCANCEL )
				{
					ClsString str;
					m_Menu.GetItemText( nCtrlId, str );
					MessageBox( str, NULL, MB_OK );
				}
				break;
			}
		}
		return ClsDialog::OnCommand( nNotifyCode, nCtrlId, hWndCtrl );
	}

	virtual LRESULT OnInitDialog( LPPROPSHEETPAGE psp )
	{
		// Bitmap indexes and item ID's.
		struct StrItemBitmap sbm[] =
		{
			0,	ID_NEW,
			1,	ID_OPEN,
			2,	ID_SAVE,
			3,	ID_SAVEALL,
			4,	ID_PRINT,
			5,	ID_CUT,
			6,	ID_COPY,
			7,	ID_PASTE,
			8,	ID_DELETE,
			9,	ID_EXIT,
			10,	ID_UNDO,
			11,	ID_REDO,
			12,	ID_IMAGE,
			SBM_END,SBM_END
		};

		// Load up the bitmap and menus. Should really do some
		// error checking here...
		m_Images.LoadBitmap( MAKEINTRESOURCE( IDB_BITMAP ), 16, 0, RGB( 255, 0, 255 ));
		m_Menu.Load( MAKEINTRESOURCE( IDR_MENU1 ));
		m_Menu.SetImageList( m_Images );
		m_Menu.SetItemBitmap( sbm );
		SetMenu( m_Menu );

		// Make sure the MFT_RADIOCHECK flags are set on all items...
		m_Menu.CheckRadioItem( ID_SUBITEMS_SUBITEM1, ID_SUBITEMS_SUBITEM2, ID_SUBITEMS_SUBITEM1 );
		m_Menu.CheckRadioItem( ID_SUBITEMS_SUBITEM1, ID_SUBITEMS_SUBITEM2, ID_SUBITEMS_SUBITEM2 );
		return ClsDialog::OnInitDialog( psp );
	}

	ClsXPMenu	m_Menu;
	ClsImageList	m_Images;
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
			MessageBox( NULL, _T( "Windows 98/ME/NT/2000/XP required!" ), _T( "ClsXPMenu Sample" ), MB_OK );
			return NULL;
		}

		// Popup the dialog.
		XPDialog xpd;
		xpd.DoModal( IDD_XPMENU );
	}
	return rc;
}
