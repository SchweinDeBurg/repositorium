#ifndef _FONTDIALOG_H_
#define _FONTDIALOG_H_
//
// fontdialog.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "commondialog.h"

// A wrapper for the Font common dialog.
class ClsFontDialog : public ClsCommonDialog
{
	_NO_COPY( ClsFontDialog );
public:
	// Constructor. Initialize members.
	ClsFontDialog()
	{
		// Set font color to black.
		m_crColor = RGB( 0, 0, 0 );

		// Clear LOGFONT structure.
		memset(( void * )&m_LogFont, 0, sizeof( LOGFONT ));
	}

	// Destructor. Does nothing.
	virtual ~ClsFontDialog()
	{;}

	// Get the font color.
	inline COLORREF GetColor() const
	{ return m_crColor; }

	// Popup the font dialog.
	BOOL DoModal( ClsWindow *parent, LOGFONT *pLogFont = NULL, DWORD dwFlags = CF_BOTH | CF_EFFECTS, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT );

	// Operator overloads.
	operator COLORREF() const
	{ return m_crColor; }
	operator LOGFONT*()
	{ return &m_LogFont; }

protected:
	// Hook procedure. This will attach the dialog
	// to the object.
	static UINT CALLBACK HookProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Overidable. Called just before the dialog is opened.
	virtual BOOL PreShowDialog( LPCHOOSEFONT pChooseFont )
	{ return TRUE; }

	// Data.
	COLORREF		m_crColor;
	LOGFONT			m_LogFont;
};

#endif // _FONTDIALOG_H_