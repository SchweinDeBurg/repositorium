//
// colordialog.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../../stdafx.h"

#include "colordialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Popup the color dialog.
BOOL ClsColorDialog::DoModal( ClsWindow *parent, COLORREF crColor, DWORD dwFlags /* = CC_FULLOPEN */, int x /* = CW_USEDEFAULT */, int y /* = CW_USEDEFAULT */ )
{
	// Setup location.
	m_ptPos.X() = x;
	m_ptPos.Y() = y;

	// Setup structure.
	CHOOSECOLOR	cc = { 0 };
	cc.lStructSize	= sizeof( cc );
	cc.hwndOwner	= parent ? parent->GetSafeHWND() : NULL;
	cc.rgbResult	= crColor;
	cc.lpCustColors	= GetCustomColorTable();
	cc.lpfnHook	= HookProc;
	cc.lCustData    = ( DWORD )this;
	cc.Flags	= dwFlags | CC_RGBINIT | CC_ENABLEHOOK;

	// Call the PreShowDialog() overridable.
	if ( PreShowDialog( &cc ))
	{
		// Open the dialog box.
		if ( ChooseColor( &cc ))
		{
			// Store the color and return TRUE.
			m_crColor = cc.rgbResult;
			return TRUE;
		}
	}
	return FALSE;
}

// Hook procedure. This will attach the dialog
// to the object.
UINT CALLBACK ClsColorDialog::HookProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// The lParam parameter of the WM_INITDIALOG message holds a pointer
	// to the CHOOSECOLOR structure. This structure holds in the "lCustData"
	// field the pointer to the object we need to get attached to.
	if ( uMsg == WM_INITDIALOG )
	{
		_ASSERT_VALID( lParam ); // Must be valid.
		ClsColorDialog *pColorDialog = ( ClsColorDialog * )(( LPCHOOSECOLOR )lParam )->lCustData;
		_ASSERT_VALID( pColorDialog ); // Must be valid.

		// Attach us to the object.
		pColorDialog->Attach( hWnd );

		// Re-position if necessary.
		pColorDialog->RePositionDialog( pColorDialog );
	}
	return 0;
}
