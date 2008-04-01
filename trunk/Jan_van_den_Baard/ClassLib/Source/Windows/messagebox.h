#ifndef _MESSAGEBOX_H_
#define _MESSAGEBOX_H_
//
// messagebox.h
//
// (C) Copyright 2000-2001 Jan van den Baard.
//     All Rights Reserved.
//

#include "dialog.h"
#include "../layout/all.h"
#include "../gdi/all.h"
#include "../collectors/array.h"
#include "custom controls/infocontrol.h"
#include "custom controls/flatbutton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
#define THIS_FILE __FILE__
#endif

// A class which can be used to show message boxes simular
// to the ones the MessageBox() API does. This class does use
// the ClsInfoControl class to display the body text so font
// styles and text alignment is possible.
class ClsMessageBox : public ClsDialog
{
	_NO_COPY( ClsMessageBox );
public:
	// Flags.
	enum
	{
		MBF_ICONCUSTOM = 0x00000000,
		MBF_ICONEXCLAMATION = 0x00000001,
		MBF_ICONWARNING = 0x00000002,
		MBF_ICONINFORMATION = 0x00000004,
		MBF_ICONASTERISK = 0x00000004,
		MBF_ICONQUESTION = 0x00000008,
		MBF_ICONSTOP = 0x00000010,
		MBF_ICONERROR = 0x00000010,
		MBF_ICONHAND = 0x00000010,
		MBF_TOPMOST = 0x00000040,
		MBF_HOTBUTTONS = 0x00000080
	};

	// No-op constructor/destructor.
	ClsMessageBox() { m_hIcon = NULL; m_dwFlags = 0L; }
	virtual ~ClsMessageBox() {;}

	// Implementation.
	inline ClsString& Title() { return ( ClsString& )m_strTitle; }
	inline ClsString& Body() { return ( ClsString& )m_strBody; }
	inline ClsString& Buttons() { return ( ClsString& )m_strButtons; }
	inline DWORD& Flags() { return ( DWORD& )m_dwFlags; }
	inline HICON& Icon() { return ( HICON& )m_hIcon; }

	// Popup the message box.
	static int MsgBox( HWND hParent, LPCTSTR pszButtons, LPCTSTR pszBody, LPCTSTR pszTitle, DWORD dwFlags = MBF_ICONINFORMATION, LPFONTDATA lpFont = NULL )
	{
		// Setup parameters.
		ClsMessageBox mb;
		mb.Buttons() = pszButtons;
		mb.Body() = pszBody;
		mb.Title() = pszTitle;
		mb.Flags() = dwFlags;

		// Popup the dialog.
		return mb.MsgBox( hParent, lpFont );
	}

	// Popup the message box.
	int MsgBox( HWND hParent, LPFONTDATA lpFont = NULL )
	{
		_ASSERT( m_strButtons.GetStringLength()); // Buttons are required.

		// Was a font supplied?
		FONTDATA fdSystem;
		if ( lpFont == NULL )
		{
			// No. We will use the system MessageBox() font
			// in the dialog.
			NONCLIENTMETRICS	ncm = { sizeof( NONCLIENTMETRICS ), 0 };
			if ( ::SystemParametersInfo( SPI_GETNONCLIENTMETRICS, sizeof( ncm ), ( void * )&ncm, 0 ))
			{
				// Create a display device context.
				ClsDC dc;
				if ( dc.CreateDC( _T( "DISPLAY" ), NULL, NULL, NULL ))
				{
					// Construct the font information.
					fdSystem.bItalic    = ( short )ncm.lfMessageFont.lfItalic;
					fdSystem.sPointSize = ( short )MulDiv( -ncm.lfMessageFont.lfHeight, 72, ::GetDeviceCaps( dc, LOGPIXELSY ));
					fdSystem.sWeight    = ( short )ncm.lfMessageFont.lfWeight;
					_tcscpy( fdSystem.szFacename, ncm.lfMessageFont.lfFaceName );

					// Use this font...
					lpFont = &fdSystem;

					// Destroy the DC.
					dc.DeleteDC();
				}
			}
		}

		// Was there an icon supplied?
		HICON hIcon = m_hIcon;
		BOOL bLoaded = FALSE;
		if ( hIcon == NULL )
		{
			// Load up a system icon if necessary.
			LPCTSTR pszIcon = NULL;
			m_nSound = MB_OK;
			if (	  m_dwFlags & MBF_ICONEXCLAMATION ) { m_nSound = MB_ICONEXCLAMATION; pszIcon = IDI_EXCLAMATION; }
			else if ( m_dwFlags & MBF_ICONWARNING	  ) { m_nSound = MB_ICONEXCLAMATION; pszIcon = IDI_WARNING; }
			else if ( m_dwFlags & MBF_ICONINFORMATION ) { m_nSound = MB_ICONASTERISK; pszIcon = IDI_INFORMATION; }
			else if ( m_dwFlags & MBF_ICONQUESTION	  ) { m_nSound = MB_ICONQUESTION; pszIcon = IDI_QUESTION; }
			else if ( m_dwFlags & MBF_ICONSTOP	  ) { m_nSound = MB_ICONHAND; pszIcon = IDI_ERROR; }

			// Load it.
			if ( pszIcon )
			{
				bLoaded = TRUE;
				hIcon = m_hIcon = ::LoadIcon( NULL, pszIcon );
			}
		}

		// Preset the return code.
		int nResult = -1;

		// Build the CREATEDIALOG structure.
		CREATEDIALOG	cd = { 0 };
		cd.pszTitle  = m_strTitle;
		cd.pFontData = lpFont;
		cd.dwStyle   = DS_MODALFRAME | DS_CENTER | WS_POPUP | WS_CAPTION | WS_SYSMENU | ( m_dwFlags & MBF_TOPMOST ? DS_SYSMODAL : 0 );

		// Popup the dialog.
		if ( hParent ) nResult = DoModal( *ClsWindow::FromHandle( hParent ), &cd );
		else	       nResult = DoModal( &cd );

		// If we loaded a system icon to
		// display we destroy it here.
		if ( hIcon && bLoaded )
		{
			m_hIcon = NULL;
			::DestroyIcon( hIcon );
		}

		// Return result.
		return nResult;
	}

protected:
	// Canceling the dialog by closing it or
	// pressing the ESC key returns 0.
	virtual BOOL OnCancel()
	{
		EndDialog( 0 );
		return FALSE;
	}

	// Handle button clicks.
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
	{
		// Is the ID in range and was it a BN_CLICKED message?
		if ( nCtrlID >= 0 && nCtrlID < ( UINT )m_nMaxID && nNotifyCode == FBN_CLICKED )
			// Exit with the appropiate ID.
			EndDialog( nCtrlID == ( UINT )( m_nMaxID - 1 ) ? 0 : nCtrlID );

		// Call the base class.
		return ClsDialog::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
	}

	// Cleanup...
	virtual LRESULT OnDestroy()
	{
		// Delete all entries in the array.
		for ( int i = 0; i < m_Buttons.GetSize(); i++ )
			delete m_Buttons[ i ];

		// Pass to the base class.
		return ClsDialog::OnDestroy();
	}

	virtual LRESULT OnPaint( ClsDC *pDC )
	{
		// Force background color.
		m_Body.BackgroundColor() = ::GetSysColor( COLOR_BTNFACE );
		return ClsDialog::OnPaint( pDC );
	}

	// Initialize dialog.
	virtual LRESULT OnInitDialog( PROPSHEETPAGE *pPropSheetPage )
	{
		// Create the layoutengine controls.
		m_Master.Create( *this, LAYOUT_Horizontal, FALSE, LAYOUT_Master, TRUE, TAG_END );
		m_HSlave1.Create( *this, LAYOUT_Horizontal, TRUE, LAYOUT_Spacing, 10, LAYOUT_LeftOffset, 4, LAYOUT_TopOffset, 4, LAYOUT_RightOffset, 0, LAYOUT_BottomOffset, 0, TAG_END );
		m_HSlave2.Create( *this, LAYOUT_Horizontal, TRUE, LAYOUT_LeftOffset, 4, LAYOUT_TopOffset, 4, LAYOUT_RightOffset, 0, LAYOUT_BottomOffset, 0, LAYOUT_EqualMinWidth, TRUE, LAYOUT_EqualMinHeight, TRUE, TAG_END );

		// Construct the buttons by parsing the input
		// button string.
		ClsString button;
		int nIndex;
		m_nMaxID = 1;

		// First we add a spacing member to the horizontal
		// layoutengine which will hold the buttons.
		m_HSlave2.AddSpacingMember( 75 );

		// Parse the button string.
		if (( nIndex = m_strButtons.GetDelimitedPart( _T( '|' ), 0, button )) > 0 )
		{
			// Preset the ID of the default button.
			int nDefID = 0;
			do
			{
				// Create a new pushbutton.
				ClsFlatButton *pButton = new ClsFlatButton;

				// Hot ot not?
				if ( m_dwFlags & MBF_HOTBUTTONS ) pButton->SetHotFrame();
				else				  pButton->SetHotTrack( FALSE );

				// Does this name have a * as the first
				// character? If so this will be the default
				// button.
				if ( button[ 0 ] == _T( '*' ))
				{
					// Note the button ID.
					nDefID = m_nMaxID;
					button = button.Right( button.GetStringLength() - 1 );
				}

				// Create the button and add it to the array and the
				// layoutengine.
				if ( pButton->Create( this, ClsRect( 0, 0, 0, 0 ), m_nMaxID, button ) == FALSE ) EndDialog( -1 );
				m_nMaxID++;
				if ( m_Buttons.Add( &pButton, 1 ) < 0 ) EndDialog( -1 );
				m_HSlave2.AddMember( pButton, NULL, TAG_END );
			}
			while (( nIndex = m_strButtons.GetDelimitedPart( _T( '|' ), nIndex, button )) > 0 );

			// Setup the default button ID and set focus to it.
			SendMessage( DM_SETDEFID, nDefID ? nDefID : m_nMaxID - 1 );
			::SetFocus( GetDlgItemHandle( nDefID ? nDefID : m_nMaxID - 1 ));
		}

		// Add a second spacing member.
		m_HSlave2.AddSpacingMember( 75 );

		// Do we have an icon?
		if ( m_hIcon )
		{
			// Create the icon static control.
			if ( m_Icon.Create( this, ClsRect( 0, 0, 0, 0 ), ( UINT )-1, NULL, WS_CHILD | WS_VISIBLE | SS_ICON ))
				// Setup the icon.
				m_Icon.SendMessage( STM_SETIMAGE, IMAGE_ICON, ( LPARAM )m_hIcon );
		}

		// Create the body ClsInfoControl.
		m_Body.Create( this, ClsRect( 0, 0, 0, 0 ), ( UINT )-1, m_strBody );

		// Add the icon and body controls to the first horizontal
		// layoyt engine.
		if ( m_hIcon ) m_HSlave1.AddMember( &m_Icon, NULL, ATTR_FixMinWidth, TRUE, ATTR_FixMinHeight, TRUE, TAG_END );
		m_HSlave1.AddMember( &m_Body, NULL, ATTR_FixMinWidth, TRUE, ATTR_FixMinHeight, TRUE, TAG_END );

		// Add both horizontal layoutengines to the
		// vertical master layoutengine.
		m_Master.AddMember( &m_HSlave1, NULL, ATTR_FixMinWidth, TRUE, ATTR_FixMinHeight, TRUE, TAG_END );
		m_Master.AddMember( &m_HSlave2, NULL, ATTR_FixMinHeight, TRUE, TAG_END );

		// Call the base class.
		ClsDialog::OnInitDialog( pPropSheetPage );

		// Get the dialog font and pass it onto the
		// controls in the dialog.
		ClsFont font;
		GetFont( font );
		m_Master.SetFont( &font );

		// Any errors?
		if ( m_Master.Error())
		{
			// Return error code.
			EndDialog( -1 );
			return FALSE;
		}

		// Obtain the master layoutengine control it's minimum
		// size.
		if ( m_Master.OnGetMinSize( m_Size ) == FALSE )
		{
			// return error code.
			EndDialog( -1 );
			return FALSE;
		}

		// Add the size of the dialog frames and caption.
		m_Size.CY() += ( ::GetSystemMetrics( SM_CYFRAME ) * 2 ) + ::GetSystemMetrics( SM_CYCAPTION );
		m_Size.CX() += ::GetSystemMetrics( SM_CXFRAME ) * 2;

		// Reposition and re-size the dialog.
		SetWindowPos( NULL, 0,
				    0,
				    m_Size.CX(),
				    m_Size.CY(),
				    SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE );

		// Center the dialog.
		CenterWindow( NULL );

		// Re-layout the layoutengine controls.
		m_Master.Relayout();

		// Play the sound.
		::MessageBeep( m_nSound );
		return FALSE;
	}

	// Data.
	ClsLayoutEngine			m_Master, m_HSlave1, m_HSlave2;
	ClsLStatic			m_Icon;
	ClsInfoControl			m_Body;
	ClsSimpleArray<ClsFlatButton*>	m_Buttons;
	ClsSize				m_Size;
	ClsString			m_strTitle;
	ClsString			m_strBody;
	ClsString			m_strButtons;
	DWORD				m_dwFlags;
	HICON				m_hIcon;
	UINT				m_nSound;
	int				m_nMaxID;
};
#endif // _MESSAGEBOX_H_