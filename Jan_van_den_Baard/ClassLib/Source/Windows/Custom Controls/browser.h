#ifndef _BROWSER_H_
#define _BROWSER_H_
//
// browser.h
//
// (C) Copyright 2001 Jan van den Baard.
//     All Rights Reserved.
//

#include "../controls/edit.h"
#include "../controls/button.h"
#include "../../coords/rect.h"
#include "../../strings/string.h"

// A ClsEdit derived class which enables the user
// to enter text and push a selection button for
// dialog selection.
class ClsBrowser : public ClsEdit
{
	_NO_COPY( ClsBrowser );
public:
	// Constructor.
	ClsBrowser();

	// Destructor.
	virtual ~ClsBrowser();

	// Implementation.
	BOOL Attach( HWND hWnd, BOOL bDestroy = FALSE );
	void GetBrowserRect( LPRECT lpRect )
	{ if ( lpRect ) *lpRect = GetButtonRect(); }

protected:
	// Overidables.
	virtual void OnBrowserClicked() = 0;
	virtual void OnFileDropped( HDROP hDrop ) = 0;
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void GetButtonSize( ClsDC *pDC, ClsSize& sButtonSize );
	virtual void RenderButton( ClsDC *pDC, ClsRect rcClip );

	// Helpers.
	ClsRect GetButtonRect();
	BOOL CursorOnButton();
	void InternalRenderButton( ClsDC *pDC );

	// Data.
	BOOL	  m_bHasCapture;
	BOOL	  m_bDown;
	BOOL	  m_bRectValid;
	BOOL	  m_bClicked;
	int	  m_nButtonWidth;
	ClsRect	  m_ButtonRect;
	WCHAR	  m_szTheme[ 64 ];
	HANDLE	  m_hTheme;
};

#endif // _BROWSER_H_