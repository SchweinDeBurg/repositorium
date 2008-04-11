//
// LayoutEngine sample.
//
// Shows the usage of the ClsLayoutEngine class.
//

#include "stdafx.h"

#include "../../../Source/all.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// A dialog class which will show some of the possibilities
// of the ClsLayoutEngine class.
class LayoutDlg : public ClsSizeDialog
{
	_NO_COPY( LayoutDlg );
public:
	LayoutDlg() {;}
	virtual ~LayoutDlg() { if ( m_hIcon ) ::DestroyIcon( m_hIcon ); }

protected:
	// Window procedure override.
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		if ( uMsg == WM_GETMINMAXINFO )
		{
			// Fill in the minimum size of the dialog.
			LPMINMAXINFO pmmi = ( LPMINMAXINFO )lParam;
			pmmi->ptMinTrackSize.x = m_MinSize.CX();
			pmmi->ptMinTrackSize.y = m_MinSize.CY();
			return 0;
		}
		// Call the base class.
		return ClsSizeDialog::WindowProc( uMsg, wParam, lParam );
	}

	// WM_SIZE message handler.
	virtual LRESULT OnSize( UINT nSizeType, int nWidth, int nHeight )
	{
		// Tell the master layout engine to layout it's
		// members.
		m_Master.Relayout();

		// Call the base class.
		return ClsSizeDialog::OnSize( nSizeType, nWidth, nHeight );
	}

	// WM_COMMAND message handler.
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
	{
		// The end?
		if ( nCtrlID == IDC_OK )
		{
			EndDialog( 0 );
			return TRUE;
		}

		// Call the base class.
		return ClsSizeDialog::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
	}
	
	// Helper function to add a "ClsInfoControl" object to the
	// specified "ClsLayoutEngine" control.
	void AddToGroup( ClsLayoutEngine *pGroup, ClsInfoControl *pControl, COLORREF crBkgnd, BOOL bFixed = FALSE, UINT nWeight = DEFAULT_WEIGHT )
	{
		// Add the member, tell it to center it's text vertically
		// and set the background color.
		pGroup->AddMember( pControl, NULL, ATTR_Weight, nWeight, ATTR_FixMinWidth, bFixed, TAG_END );
		pControl->VCenter() = TRUE;
		pControl->BackgroundColor() = crBkgnd;
	}

	// WM_INITDIALOG handler.
	virtual LRESULT OnInitDialog( PROPSHEETPAGE *psp )
	{
		// This style will prevent flickering when the dialog
		// is resized.
		ModifyStyle( 0, WS_CLIPCHILDREN );
		
		// We must create the layout engine controls here. This way the "ClsDialog"
		// class will distribute it's font to the layout engine controls aswell.
		m_LHorz.Create(     *this, Offsets( 0, 0, 0, 0 ), LAYOUT_Horizontal, TRUE, TAG_END );
		m_LVert.Create(     *this, Offsets( 0, 0, 0, 0 ), LAYOUT_Horizontal, FALSE, TAG_END );
		m_LGrh[ 0 ].Create( *this, Offsets( 0, 0, 0, 0 ), LAYOUT_Horizontal, TRUE, TAG_END );
		m_LGrh[ 1 ].Create( *this, Offsets( 0, 0, 0, 0 ), LAYOUT_Horizontal, TRUE, TAG_END );
		m_LGrh[ 2 ].Create( *this, Offsets( 0, 0, 0, 0 ), LAYOUT_Horizontal, TRUE, TAG_END );
		m_LGrv.Create(      *this, Offsets( 0, 0, 0, 0 ), LAYOUT_Horizontal, FALSE, TAG_END );
		m_HorVerGr.Create(  *this, Offsets( 0, 0, 0, 0 ), LAYOUT_Horizontal, TRUE, LAYOUT_FrameGroup, TRUE, LAYOUT_Title, _T( "Horizontal, Vertical && Grid:" ), TAG_END );
		m_LWeight.Create(   *this, Offsets( 0, 0, 0, 0 ), LAYOUT_Horizontal, TRUE, LAYOUT_FrameGroup, TRUE, LAYOUT_Title, _T( "Weight controlled:" ), TAG_END );
		m_FrFix.Create(     *this, Offsets( 0, 0, 0, 0 ), LAYOUT_Horizontal, TRUE, LAYOUT_FrameGroup, TRUE, LAYOUT_Title, _T( "Free && Fixed sizes:" ), TAG_END );
		m_HorzOK.Create(    *this, Offsets( 0, 0, 0, 0 ), LAYOUT_Horizontal, TRUE, TAG_END );
		m_Master.Create(    *this, Offsets( 6, 6, 6, 6 ), LAYOUT_Spacing, 6, LAYOUT_Horizontal, FALSE, LAYOUT_Master, TRUE, TAG_END );

		// Loadup and set icon.
		m_hIcon = ( HICON )::LoadImage( ClsGetApp()->GetResourceHandle(), MAKEINTRESOURCE( IDI_ICON ), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR );
		SetIcon( m_hIcon, FALSE );

		// Call the base class. ClsDialog will distribute the dialog
		// font to all of it's childdren.
		return ClsSizeDialog::OnInitDialog( psp );
	}

	// This overidable will be called when ClsDialog has finished
	// distributing the font to it's children.
	virtual void OnFontDistributed()
	{
		// Call the base class.
		ClsSizeDialog::OnFontDistributed();

		// Construct the horizontal group.
		m_Horz[ 0 ].Attach( GetDlgItemHandle( IDC_HORZ1 ));
		m_Horz[ 1 ].Attach( GetDlgItemHandle( IDC_HORZ2 ));
		m_Horz[ 2 ].Attach( GetDlgItemHandle( IDC_HORZ3 ));
		AddToGroup( &m_LHorz, &m_Horz[ 0 ], RGB( 255, 0, 0 ));
		AddToGroup( &m_LHorz, &m_Horz[ 1 ], RGB( 255, 0, 0 ));
		AddToGroup( &m_LHorz, &m_Horz[ 2 ], RGB( 255, 0, 0 ));

		// Construct the vertical group.
		m_Vert[ 0 ].Attach( GetDlgItemHandle( IDC_VERT1 ));
		m_Vert[ 1 ].Attach( GetDlgItemHandle( IDC_VERT2 ));
		m_Vert[ 2 ].Attach( GetDlgItemHandle( IDC_VERT3 ));
		AddToGroup( &m_LVert, &m_Vert[ 0 ], RGB( 255, 255, 255 ));
		AddToGroup( &m_LVert, &m_Vert[ 1 ], RGB( 255, 255, 255 ));
		AddToGroup( &m_LVert, &m_Vert[ 2 ], RGB( 255, 255, 255 ));

		// Construct the grid group(s).
		m_Grid[ 0 ].Attach( GetDlgItemHandle( IDC_GRID1 ));
		m_Grid[ 1 ].Attach( GetDlgItemHandle( IDC_GRID2 ));
		m_Grid[ 2 ].Attach( GetDlgItemHandle( IDC_GRID3 ));
		m_Grid[ 3 ].Attach( GetDlgItemHandle( IDC_GRID4 ));
		m_Grid[ 4 ].Attach( GetDlgItemHandle( IDC_GRID5 ));
		m_Grid[ 5 ].Attach( GetDlgItemHandle( IDC_GRID6 ));
		m_Grid[ 6 ].Attach( GetDlgItemHandle( IDC_GRID7 ));
		m_Grid[ 7 ].Attach( GetDlgItemHandle( IDC_GRID8 ));
		m_Grid[ 8 ].Attach( GetDlgItemHandle( IDC_GRID9 ));
		AddToGroup( &m_LGrh[ 0 ], &m_Grid[ 0 ], RGB( 0, 0, 255 ));
		AddToGroup( &m_LGrh[ 0 ], &m_Grid[ 1 ], RGB( 0, 0, 255 ));
		AddToGroup( &m_LGrh[ 0 ], &m_Grid[ 2 ], RGB( 0, 0, 255 ));
		AddToGroup( &m_LGrh[ 1 ], &m_Grid[ 3 ], RGB( 0, 0, 255 ));
		AddToGroup( &m_LGrh[ 1 ], &m_Grid[ 4 ], RGB( 0, 0, 255 ));
		AddToGroup( &m_LGrh[ 1 ], &m_Grid[ 5 ], RGB( 0, 0, 255 ));
		AddToGroup( &m_LGrh[ 2 ], &m_Grid[ 6 ], RGB( 0, 0, 255 ));
		AddToGroup( &m_LGrh[ 2 ], &m_Grid[ 7 ], RGB( 0, 0, 255 ));
		AddToGroup( &m_LGrh[ 2 ], &m_Grid[ 8 ], RGB( 0, 0, 255 ));
		m_LGrv.AddMember( &m_LGrh[ 0 ], NULL, TAG_END );
		m_LGrv.AddMember( &m_LGrh[ 1 ], NULL, TAG_END );
		m_LGrv.AddMember( &m_LGrh[ 2 ], NULL, TAG_END );

		// Add the horizontal, vertical and grid groups to it's parent
		// layout engine.
		m_HorVerGr.AddMember( &m_LHorz, NULL, TAG_END );
		m_HorVerGr.AddMember( &m_LVert, NULL, TAG_END );
		m_HorVerGr.AddMember( &m_LGrv, NULL, TAG_END );

		// Construct the Weight group.
		m_Weight[ 0 ].Attach( GetDlgItemHandle( IDC_25 ));
		m_Weight[ 1 ].Attach( GetDlgItemHandle( IDC_50 ));
		m_Weight[ 2 ].Attach( GetDlgItemHandle( IDC_75 ));
		m_Weight[ 3 ].Attach( GetDlgItemHandle( IDC_100 ));
		AddToGroup( &m_LWeight, &m_Weight[ 0 ], RGB( 0, 255, 0 ), FALSE, 25 );
		AddToGroup( &m_LWeight, &m_Weight[ 1 ], RGB( 0, 235, 0 ), FALSE, 50 );
		AddToGroup( &m_LWeight, &m_Weight[ 2 ], RGB( 0, 215, 0 ), FALSE, 75 );
		AddToGroup( &m_LWeight, &m_Weight[ 3 ], RGB( 0, 195, 0 ), FALSE, 100 );

		// Construct the Free & Fixed group.
		m_FreeFixed[ 0 ].Attach( GetDlgItemHandle( IDC_FREE1 ));
		m_FreeFixed[ 1 ].Attach( GetDlgItemHandle( IDC_FIXED1 ));
		m_FreeFixed[ 2 ].Attach( GetDlgItemHandle( IDC_FREE2 ));
		m_FreeFixed[ 3 ].Attach( GetDlgItemHandle( IDC_FIXED2 ));
		AddToGroup( &m_FrFix, &m_FreeFixed[ 0 ], RGB( 220, 220, 220 ));
		AddToGroup( &m_FrFix, &m_FreeFixed[ 1 ], RGB( 220, 220, 220 ), TRUE );
		AddToGroup( &m_FrFix, &m_FreeFixed[ 2 ], RGB( 220, 220, 220 ));
		AddToGroup( &m_FrFix, &m_FreeFixed[ 3 ], RGB( 220, 220, 220 ), TRUE );

		// Construct the Ok button group.
		m_Ok.Attach( GetDlgItemHandle( IDC_OK )); m_Ok.SetHotFrame();
		m_HorzOK.AddSpacingMember( 150 );
		m_HorzOK.AddMember( &m_Ok, NULL, ATTR_UseControlSize, TRUE, TAG_END );
		m_HorzOK.AddSpacingMember( 150 );

		// Add the sub-groups to the parent group.
		m_Master.AddMember( &m_HorVerGr, NULL, TAG_END );
		m_Master.AddMember( &m_LWeight, NULL, ATTR_FixMinHeight, TRUE, TAG_END );
		m_Master.AddMember( &m_FrFix, NULL, ATTR_FixMinHeight, TRUE, TAG_END );
		m_Master.AddMember( &m_HorzOK, NULL, ATTR_FixMinHeight, TRUE, TAG_END );
		
		// Was there an error in any of the above call's?
		if ( m_Master.Error())
		{
			EndDialog( 0 );
			return;
		}

		// Compute the minimum size of the master group.
		if ( m_Master.OnGetMinSize( m_MinSize ) == FALSE )
		{
			EndDialog( 0 );
			return;
		}

		// Add frame and caption sizes so that we know the minimum
		// size of the dialog.
		m_MinSize.CY() += ( ::GetSystemMetrics( SM_CYFRAME ) * 2 ) + ::GetSystemMetrics( SM_CYCAPTION );
		m_MinSize.CX() += ::GetSystemMetrics( SM_CXFRAME ) * 2;
		
		// Relayout the master layout engine control.
		m_Master.Relayout();
	}

	// Data.
	ClsLayoutEngine		m_Master;
	ClsLayoutEngine		m_HorVerGr, m_LWeight, m_FrFix;
	ClsLayoutEngine		m_LHorz, m_LVert, m_LGrh[ 3 ], m_LGrv, m_HorzOK;
	ClsInfoControl		m_Horz[ 3 ];
	ClsInfoControl		m_Vert[ 3 ];
	ClsInfoControl		m_Grid[ 9 ];
	ClsInfoControl		m_Weight[ 4 ], m_FreeFixed[ 4 ];
	ClsFlatButton		m_Ok;
	ClsSize			m_MinSize;
	ClsBitmapMenu		m_Menu;
	HICON			m_hIcon;
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
			MessageBox( NULL, _T( "Windows 98/ME/NT/2000/XP required!" ), _T( "\"ClsLayoutEngine\" Sample" ), MB_OK );
			return NULL;
		}
		// Build and show the dialog.
		LayoutDlg dlg;
		dlg.DoModal( IDD_LAYOUT );
	}
	return rc;
}
