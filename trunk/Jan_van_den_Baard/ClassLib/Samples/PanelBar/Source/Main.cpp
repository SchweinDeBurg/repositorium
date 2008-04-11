//
//	PanelBar sample.
//
//	Shows the usage of the ClsPanel, ClsPanelBar
//	and Cls(XP)OutlookBar classes.
//

#include "stdafx.h"

#include "../../../Source/all.h"
#include "Resource.h"

#ifdef _DEBUG  
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

class PanelDlg : public ClsDialog
{
	_NO_COPY( PanelDlg );
public:
	// No-op constructor and destructor.
	PanelDlg() : m_bInserted( false ), m_bAdded( false ) {;} 
	virtual ~PanelDlg() {;}
 
protected:

	// WM_COMMAND handler.
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
	{
		// Is this an ID coming from the outlook
		// bar control?
		if ( hWndCtrl == m_panOutlook )
		{
			ClsString title;

			// Is there a text in the edit control?
			// If so set it to the clicked button.
			if ( m_Edit.GetWindowTextLength())
			{
				// Set the button title.
				m_Edit.GetWindowText( title );
				m_panOutlook.SetOutlookButtonLabel( nCtrlID, title );
				m_Edit.SetWindowText( _T( "" ));
			}
			else
			{
				// Tell which button was clicked.
				title.Format( _T( "You have clicked on button '%s' (ID=%ld)!" ), m_panOutlook.GetOutlookButtonLabel( nCtrlID ), nCtrlID );
				MessageBox( title, _T( "Demo" ), MB_ICONINFORMATION | MB_OK );
			}

		}
		else
		{
			switch ( nCtrlID )
			{
				case	IDC_SMALL:
					// Toggle small images.
					m_panOutlook.UseSmallImages( ( m_Small.GetCheck() == BST_CHECKED ) ? TRUE : FALSE, TRUE );
					break;

				case	IDC_XP:
					// Toggle office XP style.
					m_panOutlook.OldStyle() = ( m_XP.GetCheck() == BST_CHECKED ) ? FALSE : TRUE;
					m_panOutlook.Invalidate();
					break;

				case	IDC_DISABLE:
					// Toggle enabled state second outlook button.
					m_panOutlook.EnableOutlookButton( 11, ( m_Disable.GetCheck() == BST_CHECKED ) ? FALSE : TRUE );
					break;

				case	IDC_INSERT:
					// Panel already inserted?
					if ( m_bInserted )
					{
						// Yes. Remove it.
						m_Bar.RemovePanel( &m_panInsert );
						m_Insert.SetWindowText( _T( "Insert Panel..." ));
					}
					else
					{
						// No. Insert it.
						m_Bar.InsertPanel( &m_panInsert, &m_panOutlook );
						m_Insert.SetWindowText( _T( "Remove Inserted Panel..." ));
					}
					// Toggle inserted flag.
					m_bInserted = ! m_bInserted;
					break;

				case	IDC_ADD:
					// Panel already added?
					if ( m_bAdded )
					{
						// Yes. Remove it.
						m_Bar.RemovePanel( &m_panEdit );
						m_Add.SetWindowText( _T( "Add Panel..." ));
					}
					else
					{ 
						// No. Add it.
						m_Bar.InsertPanel( &m_panEdit, &m_panTree );
						m_Add.SetWindowText( _T( "Remove Added Panel..." ));
					}
					// Toggle added flag.
					m_bAdded = ! m_bAdded;
					break;
			}
		}
		// Pass to the base class.
		return ClsDialog::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
	}

	// WM_INITDIALOG handler.
	virtual LRESULT OnInitDialog( PROPSHEETPAGE *psp )
	{
		ClsRect rcEmpty( -1, -1, -1, -1 );

		// Attach (subclass, wrap or what ever you want to call it..) the controls.
		m_Bar.Attach(      GetDlgItemHandle( IDC_BAR ));
		m_fileTree.Attach( GetDlgItemHandle( IDC_TREE ));
		m_Edit.Attach(	   GetDlgItemHandle( IDC_EDIT ));
		m_Insert.Attach(   GetDlgItemHandle( IDC_INSERT ));
		m_Add.Attach(      GetDlgItemHandle( IDC_ADD ));
		m_Small.Attach(    GetDlgItemHandle( IDC_SMALL ));
		m_XP.Attach(       GetDlgItemHandle( IDC_XP ));
		m_Disable.Attach(  GetDlgItemHandle( IDC_DISABLE ));

		// Setup the tree.
		m_fileTree.SetupTree();
		m_fileTree.LoadingText() = _T( "Loading. Please wait..." );
		m_fileTree.LoadingTextColor() = RGB( 0, 0, 255 );
		m_fileTree.DoubleBuffer() = TRUE; 

		// Create the outlook bar.
		m_panOutlook.Create( _T( "Cls(XP)OutlookBar..." ), rcEmpty ); 

		// Load up the images.
		HIMAGELIST imLarge = ImageList_LoadBitmap( ClsGetApp()->GetResourceHandle(), MAKEINTRESOURCE( IDB_BITMAP1 ), 32, 1, RGB( 255, 0, 255 ));
		HIMAGELIST imSmall = ImageList_LoadBitmap( ClsGetApp()->GetResourceHandle(), MAKEINTRESOURCE( IDB_BITMAP2 ), 16, 1, RGB( 255, 0, 255 ));

		// Setup the outlook bar.
		m_panOutlook.SetImageList( imLarge );
		m_panOutlook.SetImageList( imSmall, TRUE );
		m_panOutlook.InsertOutlookButton( _T( "Outlook Today" ), 0, 10 );
		m_panOutlook.InsertOutlookButton( _T( "Inbox" ), 1, 11 ); 
		m_panOutlook.InsertOutlookButton( _T( "Calendar" ), 2, 12 );
		m_panOutlook.InsertOutlookButton( _T( "Contacts" ), 3, 13 );
		m_panOutlook.InsertOutlookButton( _T( "Tasks" ), 4, 14 );
		m_panOutlook.InsertOutlookButton( _T( "Notes" ), 5, 15 );
		m_panOutlook.InsertOutlookButton( _T( "Deleted Items" ), 6, 16 );
		m_panOutlook.OldStyle() = TRUE;

		// Add the outlook bar panel.
		m_Bar.InsertPanel( &m_panOutlook );

		// Create the tree panel.
		m_panTree.Create( _T( "ClsFileDirTree" ), rcEmpty);
		m_panTree.SetPanelClient( &m_fileTree );

		// Add the tree panel.
		m_Bar.InsertPanel( &m_panTree );

		// Create the "Inserted" and "Added" panes.
		m_panInsert.Create( _T( "Inserted Panel" ), rcEmpty );
		m_panEdit.Create( _T( "Added Panel" ), rcEmpty );

		// Call the base class.
		return ClsDialog::OnInitDialog( psp );
	}

	// Data.
	ClsXPOutlookBar		m_panOutlook;
	ClsPanel		m_panTree, m_panInsert, m_panEdit;
	ClsFileDirTree		m_fileTree;
	ClsPanelBar		m_Bar; 
	ClsCheckBox		m_Small, m_XP, m_Disable;
	ClsEdit			m_Edit;
	ClsButton		m_Insert, m_Add;
	bool			m_bInserted, m_bAdded;
};

// Entry point.
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLm_Inserte, INT nShowCmd )
{
	int rc = 0;

	// Setup app.
	if ( ClsGetApp()->Setup( hInstance, lpCmdLm_Inserte, nShowCmd ))
	{
		// Check OS version.
		if ( ClsGetApp()->GetPlatformID() != VER_PLATFORM_WIN32_WINDOWS &&
		     ClsGetApp()->GetPlatformID() != VER_PLATFORM_WIN32_NT )
		{
			MessageBox( NULL, _T( "Wm_Insertdows 95/98/ME/NT/2000 required!" ), _T( "\"ClsPanelBar\" Sample" ), MB_OK );
			return NULL;
		}
		PanelDlg dlg;
		dlg.DoModal( IDD_PANELDLG );
	}
	return rc;
}
