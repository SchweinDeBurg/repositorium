//
// fontdialog.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../../stdafx.h"

#include "fontdialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Popup the font dialog.
BOOL ClsFontDialog::DoModal( ClsWindow *parent, LOGFONT *pLogFont /* = NULL */, DWORD dwFlags /* = CF_BOTH | CF_EFFECTS */, int x /* = CW_USEDEFAULT */, int y /* = CW_USEDEFAULT */ )
{
	// Setup location.
	m_ptPos.X() = x;
	m_ptPos.Y() = y;

	// Did we get a LOGFONT? If not we use the internal one.
	LOGFONT		lf;
	if ( pLogFont ) lf = *pLogFont;
	else            lf = m_LogFont;

	// Setup structure.
	CHOOSEFONT cf  = { 0 };
	cf.lStructSize = sizeof( cf );
	cf.hwndOwner   = parent ? parent->GetSafeHWND() : NULL;
	cf.lpLogFont   = &lf;
	cf.Flags       = dwFlags | CF_ENABLEHOOK | CF_INITTOLOGFONTSTRUCT;
	cf.rgbColors   = m_crColor;
	cf.lCustData   = ( DWORD )this;
	cf.lpfnHook    = HookProc;
	
	// Call the PreShowDialog() overidable.
	if ( PreShowDialog( &cf ))
	{
		// Open the dialog.
		if ( ChooseFont( &cf ))
		{
			// Copy the LOGFONT.
			m_LogFont = *cf.lpLogFont;
			if ( pLogFont ) *pLogFont = *cf.lpLogFont;

			// Save the color.
			m_crColor = cf.rgbColors;
			return TRUE;
		}
	}
	return FALSE;
}

// Hook procedure. This will attach the dialog
// to the object.
UINT CALLBACK ClsFontDialog::HookProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// The lParam parameter of the WM_INITDIALOG message holds a pointer
	// to the CHOOSEFONT structure. This structure holds in the "lCustData"
	// field the pointer to the object we need to get attached to.
	if ( uMsg == WM_INITDIALOG )
	{
		_ASSERT_VALID( lParam ); // Must be valid.
		ClsFontDialog *pFontDialog = ( ClsFontDialog * )(( LPCHOOSEFONT )lParam )->lCustData;
		_ASSERT_VALID( pFontDialog ); // Must be valid.

		// Attach us to the object.
		pFontDialog->Attach( hWnd );

		// Re-position dialog if necessary.
		pFontDialog->RePositionDialog( pFontDialog );
	}
	return 0;
}