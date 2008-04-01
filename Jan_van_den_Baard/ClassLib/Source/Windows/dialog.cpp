//
// dialog.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "dialog.h"
#include "../tools/xpcolors.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Extended dialog template structure. Used by the dynamic dialog
// creation methods.
typedef struct
{
	WORD			dlgVer;		// Version (1).
	WORD			signature;	// 0xFFFF.
	DWORD			helpID;		// Send with WM_HELP.
	DWORD			exStyle;	// Extended style.
	DWORD			style;		// Style.
	WORD			cDlgItems;	// # of items (0).
	short			x, y, cx, cy;	// Left, top, width & height.
	short			sMenu;		// 0
	WCHAR			sClass[ 1 ];	// Class name or 0
	//
	//	Followed by title and font specific data.
	//
} DLGTEMP, FAR *LPDLGTEMP;

// Constructor.
ClsDialog::ClsDialog()
{
	// Not modal by default.
	m_bIsModal = FALSE;
}

// Destructor. Does nothing.
ClsDialog::~ClsDialog()
{
}

// Open a modal dialog.
int ClsDialog::DoModal( ClsWindow& cwParent, LPCTSTR pszDialogName )
{
	_ASSERT_VALID( pszDialogName ); // Must be valid.

	// Save the window which has focus.
	HWND hFocus = ::GetFocus();

	// Open the dialog.
	int nResult = ModalDialog( &cwParent, pszDialogName );

	// Restore focus.
	if ( hFocus ) ::SetFocus( hFocus );

	// Return the result.
	return nResult;
}

// Open a modal dialog.
int ClsDialog::DoModal( LPCTSTR pszDialogName )
{
	_ASSERT_VALID( pszDialogName ); // Must be valid.

	// Save the window which has focus.
	HWND hFocus = ::GetFocus();

	// Open the dialog.
	int nResult = ModalDialog( NULL, pszDialogName );

	// Restore focus.
	if ( hFocus ) ::SetFocus( hFocus );

	// Return the result.
	return nResult;
}

// Open a modal dialog.
int ClsDialog::DoModal( int nDialogID )
{
	// Do it.
	return DoModal( MAKEINTRESOURCE( nDialogID ));
}

// Open a modal dialog.
int ClsDialog::DoModal( ClsWindow& cwParent, int nDialogID )
{
	// Do it.
	return DoModal( cwParent, MAKEINTRESOURCE( nDialogID ));
}

// Open a modal dialog.
int ClsDialog::DoModal( LPCREATEDIALOG pCreateDialog )
{
	_ASSERT_VALID( pCreateDialog ); // Must be valid.

	// Save the window which has focus.
	HWND hFocus = ::GetFocus();

	// Open the dialog.
	int nResult = ModalDialog( NULL, NULL, pCreateDialog );

	// Restore focus.
	if ( hFocus ) ::SetFocus( hFocus );

	// Return the result.
	return nResult;
}

// Open a modal dialog.
int ClsDialog::DoModal( ClsWindow& cwParent, LPCREATEDIALOG pCreateDialog )
{
	_ASSERT_VALID( pCreateDialog ); // Must be valid.

	// Save the window which has focus.
	HWND hFocus = ::GetFocus();

	// Open the dialog.
	int nResult = ModalDialog( &cwParent, NULL, pCreateDialog );

	// Restore focus.
	if ( hFocus ) ::SetFocus( hFocus );

	// Return the result.
	return nResult;
}

// Create a non-modal dialog.
BOOL ClsDialog::Create( ClsWindow& cwParent, LPCTSTR pszDialogName )
{
	_ASSERT_VALID( pszDialogName ); // Must be valid.

	// Create the dialog and return the result.
	if ( ::CreateDialogParam( ClsGetInstanceHandle(), pszDialogName, cwParent.GetSafeHWND(), ( DLGPROC )ClsWindow::StaticWindowProc, ( LPARAM )this ))
	{
		// Set us to non-modal.
		if ( ! m_bKeepModal ) m_bIsModal = FALSE;
		return TRUE;
	}
	return FALSE;
}

// Open a non-modal dialog.
BOOL ClsDialog::Create( LPCTSTR pszDialogName )
{
	_ASSERT_VALID( pszDialogName ); // Must be valid.

	// Create the dialog and return the result.
	if ( ::CreateDialogParam( ClsGetInstanceHandle(), pszDialogName, NULL, ( DLGPROC )ClsWindow::StaticWindowProc, ( LPARAM )this ))
	{
		// Set us to non-modal.
		if ( ! m_bKeepModal ) m_bIsModal = FALSE;
		return TRUE;
	}
	return FALSE;
}

// Open a non-modal dialog.
BOOL ClsDialog::Create( ClsWindow& cwParent, LPCREATEDIALOG pCreateDialog )
{
	_ASSERT_VALID( pCreateDialog ); // Must be valid.

	// Create template.
	HGLOBAL hTemplate = CreateDialogTemplate( pCreateDialog );
	if ( hTemplate )
	{
		// Create the dialog...
		m_bIsDialog = TRUE;
		BOOL bRC = ( BOOL )( CreateDialogIndirectParam( ClsGetInstanceHandle(), ( LPDLGTEMPLATE )hTemplate, cwParent.GetSafeHWND(), ( DLGPROC )ClsWindow::StaticWindowProc, ( LPARAM )this ) ? TRUE : FALSE );

		// Free the template.
		::GlobalFree( hTemplate );
		return bRC;
	}
	return FALSE;
}

// Open a non-modal dialog.
BOOL ClsDialog::Create( LPCREATEDIALOG pCreateDialog )
{
	_ASSERT_VALID( pCreateDialog ); // Must be valid.

	// Create template.
	HGLOBAL hTemplate = CreateDialogTemplate( pCreateDialog );
	if ( hTemplate )
	{
		// Create the dialog...
		m_bIsDialog = TRUE;
		BOOL bRC = ( BOOL )( CreateDialogIndirectParam( ClsGetInstanceHandle(), ( LPDLGTEMPLATE )hTemplate, NULL, ( DLGPROC )ClsWindow::StaticWindowProc, ( LPARAM )this ) ? TRUE : FALSE );

		// Free the template.
		::GlobalFree( hTemplate );
		return bRC;
	}
	return FALSE;
}

// Breaks off the modal dialog box message pump
BOOL ClsDialog::EndDialog( int nResult )
{
	_ASSERT( m_bIsModal == TRUE ); // Must be a modal dialog.

	// Stop the dialog.
	if ( GetSafeHWND() && m_bContinueModal )
	{
		// Store the result code.
		m_nResult = nResult;

		// Break the modal loop.
		m_bContinueModal = FALSE; 
		return TRUE;
	}
	return FALSE;
}

// OnOK message handler.
BOOL ClsDialog::OnOK()
{
	// Return TRUE by default.
	return TRUE;
}

// OnCancel message handler.
BOOL ClsDialog::OnCancel()
{
	// Return TRUE by default.
	return TRUE;
}

// OnInitDialog message handler.
LRESULT ClsDialog::OnInitDialog( PROPSHEETPAGE *pPropSheetPage )
{
	// If we are modal we return TRUE to set the
	// keyboard focus.
	return m_bIsModal;
}

// Called when the font has been distributed.
void ClsDialog::OnFontDistributed()
{
}

// Base class override. This will call the OnInitDialog() message
// handler when a WM_INITDIALOG message is received.
LRESULT ClsDialog::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// Is it a WM_INITDIALOG message?
	if ( uMsg == WM_INITDIALOG )
	{
		// Call overidable.
		LRESULT rc = OnInitDialog(( PROPSHEETPAGE * )lParam );

		// Distribute the dialog font to the
		// child windows.
		EnumChildWindows( GetSafeHWND(), DistributeDialogFont, SendMessage( WM_GETFONT )); // Yes call the handler.

		// Call overidable.
		OnFontDistributed();
		return rc;
	}
	// Call the base class.
	return ClsWindow::WindowProc( uMsg, wParam, lParam );
}

// EnumChildWindows() routine which will set the dialog font on all of it's
// children.
BOOL CALLBACK ClsDialog::DistributeDialogFont( HWND hWndChild, LPARAM lParam )
{
	// Valid font?
	if ( lParam == 0 )
		return FALSE;

	// Get the current
	HFONT hFont = ( HFONT )::SendMessage( hWndChild, WM_GETFONT, 0, 0 );

	// Is it already set?
	if ( hFont != NULL && hFont != ( HFONT )lParam )
		return TRUE;

	// No font set?
	if ( hFont == NULL ) 
		::SendMessage( hWndChild, WM_SETFONT, lParam, FALSE );

	return TRUE;
}

// Base class override. This WM_COMMAND message handler will call OnOK()
// when the ID is IDOK and OnCancel() when the ID is IDCANCEL.
LRESULT ClsDialog::OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
{
	// Look of the IDOK and IDCANCEL.
	switch ( nCtrlID )
	{
		case	IDOK:
			// Call the virtual handler.
			if ( OnOK() && m_bIsModal )
				// Call end dialog when the dialog is
				// modal and OnOK() returns TRUE.
				EndDialog( IDOK );
			return 0;

		case	IDCANCEL:
			// Call the virtual handler.
			if ( OnCancel() && m_bIsModal )
				// Call end dialog when the dialog is
				// modal and OnCancel() returns TRUE.
				EndDialog( IDCANCEL );
			return 0;
	}
	// Call the base class handler.
	return ClsWindow::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
}

// Construct a dialog template.
HGLOBAL ClsDialog::CreateDialogTemplate( LPCREATEDIALOG pCreateDialog )
{
	INT		nchar = 0, fchar = 0;

	// Compute the length (in wide chars) of the title 
	// as a wide-char string.
	if ( pCreateDialog->pszTitle )
#ifndef _UNICODE
		nchar = 1 + MultiByteToWideChar( CP_ACP, 0, pCreateDialog->pszTitle, -1, NULL, 0 );
#else
		nchar = 1 + wcslen( pCreateDialog->pszTitle );
#endif

	// Compute the length (in wide chars) of the font
	// face name.
	if ( pCreateDialog->pFontData )
#ifndef _UNICODE
		fchar = 1 + MultiByteToWideChar( CP_ACP, 0, &pCreateDialog->pFontData->szFacename[ 0 ], -1, NULL, 0 );
#else
		fchar = 1 + wcslen( &pCreateDialog->pFontData->szFacename[ 0 ] );
#endif

	// Allocate enough memory for the dialog
	// template and title string.
	HGLOBAL hgbl = ::GlobalAlloc( GMEM_ZEROINIT, (( sizeof( DLGTEMP ) + (( nchar + fchar + 3 ) * sizeof( WCHAR ))) + ( 3 * sizeof( short ))));

	// OK?
	if ( ! hgbl )
		return NULL;

	// Lock the memory.
	LPDLGTEMP lpdt = ( LPDLGTEMP )GlobalLock( hgbl );

	// Setup the template.
	lpdt->dlgVer	= 1;
	lpdt->signature = 0xFFFF;
	lpdt->cDlgItems	= 0;
	lpdt->x		= ( short )pCreateDialog->nLeft;
	lpdt->y		= ( short )pCreateDialog->nTop;
	lpdt->cx	= ( short )pCreateDialog->nWidth;
	lpdt->cy	= ( short )pCreateDialog->nHeight;
	lpdt->style	= pCreateDialog->dwStyle;
	lpdt->exStyle	= pCreateDialog->dwExStyle;
	lpdt->sMenu	= 0;

	// Get a pointer to the memory located directly
	// after the template.
	LPWORD lpw = ( LPWORD )&lpdt->sClass[ 0 ];

	// No class.
	*lpw++ = 0;

	// Title?
	if ( pCreateDialog->pszTitle )
		// Convert the dialog title.
#ifndef _UNICODE
		lpw += MultiByteToWideChar( CP_ACP, 0, pCreateDialog->pszTitle, -1, ( LPWSTR )lpw, nchar  );
#else
	{
		wcscpy(( LPWSTR )lpw, pCreateDialog->pszTitle );
		lpw += 1 + wcslen( pCreateDialog->pszTitle );
	}
#endif
	else
		// No title.
		*lpw++ = 0;

	// Font data?
	if ( pCreateDialog->pFontData ) 
	{
		// Setup font data.
		*lpw++ = pCreateDialog->pFontData->sPointSize;
		*lpw++ = pCreateDialog->pFontData->sWeight;
		*lpw++ = ( short )pCreateDialog->pFontData->bItalic;

		// Make sure we use the DS_SETFONT style.
		lpdt->style |= DS_SETFONT;

		// Convert the face name.
#ifndef _UNICODE
		MultiByteToWideChar( CP_ACP, 0, &pCreateDialog->pFontData->szFacename[ 0 ], -1, ( LPWSTR )lpw, fchar );
#else
		wcscpy(( LPWSTR )lpw, &pCreateDialog->pFontData->szFacename[ 0 ] );
#endif
	}
	else
		// Make sure we do not use the DS_SETFONT style.
		lpdt->style &= ~DS_SETFONT;


	// Unlock the handle.
	GlobalUnlock( hgbl );
	return hgbl;
}

// The modal message pump. The system modal dialog box behaviour is
// simulated so that we can keep calling the PreTranslateMessage()
// overidables for those messages directed at windows in our global
// window list.
//
// This routine simulates the system modal dialog box message pump with
// the exception that it will not send WM_ENTERIDLE messages to the parent
// window wether the DS_NOIDLEMSG style is set or not.
#include "../menus/menu.h"
#include "../menus/bitmapmenu.h"

// Import the global window list and the object lookup
// routine from "window.cpp".
extern ClsLinkedList<ClsWindow> global_window_list;
extern ClsWindow *ClsFindObjectByHandle( ClsLinkedList<ClsWindow>& list, HWND hWnd );

int ClsDialog::ModalDialog( ClsWindow *pParent, LPCTSTR pszDialogName, LPCREATEDIALOG pCreateDialog )
{
	// Preset enable flag and
	// result code.
	BOOL bMustEnable = FALSE;
	m_nResult     = -1;

	// Set the modal flag.
	m_bIsModal = m_bKeepModal = TRUE;

	// By default we continue processing messages
	// until this boolean is set to FALSE by the
	// EndModal() routine.
	m_bContinueModal = TRUE;

	// Now create the dialog.
	if ( pCreateDialog )
	{
		// Dynamic creation.
		if ( pParent ) Create( *pParent, pCreateDialog );
		else	       Create( pCreateDialog );
	}
	else
	{
		// Template creation.
		if ( pParent ) Create( *pParent, pszDialogName );
		else	       Create( pszDialogName );
	}

	// Did the dialog create OK?
	if ( GetSafeHWND() && m_bContinueModal )
	{
		// Make sure the dialog is visible.
		ShowWindow( SW_SHOWNORMAL );
		UpdateWindow();
		
		// Determine if the parent window is already disabled.
		// If it is not we disable it and set a flag telling us later
		// that we must enable it again.
		if ( pParent && pParent->IsWindowEnabled())
		{
			// Diable parent and set flag.
			pParent->EnableWindow( FALSE );
			bMustEnable = TRUE;
		}

		// Get messages.
		MSG msg;

		// Enter the modal message loop.
		while ( m_bContinueModal )
		{
			// Get the message and handle it.
			if ( ::GetMessage( &msg, NULL, 0, 0 ) >= 0 )
			{
				// Valid window?
				if ( msg.hwnd == NULL ) continue;

				// Is it a WM_QUIT message?
				if ( msg.message == WM_QUIT )
				{
					// Stop the modal loop and re-post
					// the message so that the main message
					// handler can process it.
					m_bContinueModal = FALSE;
					PostQuitMessage(( int )msg.wParam );
					break;
				}

				// Get the message window object. The message window may not
				// be located in the global window list since it might be a
				// child of the window located in the global window list.
				//
				// For example the messages from a edit control in a combobox
				// control. In this case, when the message window is not found,
				// we look up it's parent in the global window list.
				ClsWindow *pWindow = ClsFindObjectByHandle( global_window_list, msg.hwnd );
				if ( pWindow == NULL && IsChild( msg.hwnd )) pWindow = ClsFindObjectByHandle( global_window_list, ::GetParent( msg.hwnd ));

				// Call the PreTranslateMessage() function to pre-process
				// the message.
				BOOL bTranslate = FALSE;
				if ( pWindow ) bTranslate = pWindow->PreTranslateMessage( &msg );

				// Can we dispatch?
				if ( ! bTranslate )
				{
					// Message was not handled. Dispatch it.
					::TranslateMessage( &msg );
					::DispatchMessage( &msg );
				}

				// Delete temporary GDI, Window and (Bitmap)Menu objects.
				ClsGdiObject::DeleteTempObjects();
				ClsMenu::DeleteTempObjects();
				ClsBitmapMenu::DeleteTempObjects();
				ClsWindow::DeleteTempObjects();
			}
		}
		// Do we need to re-enable the parent?
		if ( bMustEnable )
		{
			// Enable it and set it to be the forground window.
			pParent->EnableWindow();
			pParent->SetForegroundWindow();
		}

		// Destroy the dialog.
		Destroy();
	}

	// No longer modal...
	m_bIsModal = m_bKeepModal = FALSE;

	// Return the dialog result code.
	return m_nResult;
}