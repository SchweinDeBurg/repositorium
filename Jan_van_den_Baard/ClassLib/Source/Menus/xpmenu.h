#ifndef _XPMENU_H_
#define _XPMENU_H_
//
// xpmenu.h
//
// (C) Copyright 2002 Jan van den Baard.
//     All Rights Reserved.
//

#include "bitmapmenu.h"
#include "../tools/xpcolors.h"
#include "../gdi/dc.h"
#include "../gdi/bitmap.h"
#include "../coords/rect.h"

// Bitmap menu Office XP style. This class mimics the rendering style
// of the Office XP and VisualStudio .NET menus. It is not a carbon
// copy but it comes pretty close.
class ClsXPMenu : public ClsBitmapMenu
{
	// No copy constructor.
	_NO_COPY( ClsXPMenu );
public:
	// Constructor.
	ClsXPMenu() : m_bDrawOldStyle( FALSE ) {;}

	// Virtual destructor.
	virtual ~ClsXPMenu() {;}

	// Implementation.
	inline BOOL& OldStyle() { return ( BOOL& )m_bDrawOldStyle; }

protected:
	// helpers.
	void RenderItem( ClsDC *pDC, LPDRAWITEMSTRUCT pdis, ItemData *pData );
	void RefreshParentRectangle( ClsWindow *pWindow );

	// Overidables.
	virtual LRESULT OnReflectedMeasureItem( LPMEASUREITEMSTRUCT pmis, BOOL &bNotifyParent );
	virtual LRESULT OnReflectedDrawItem( LPDRAWITEMSTRUCT pdis, BOOL& bNotifyParent );
	virtual void OnReflectedUnInitMenuPopup( ClsWindow *pWindow, HMENU hPopup, LPARAM lParam );
	virtual void OnReflectedExitMenuLoop( ClsWindow *pWindow, BOOL bShortcut );
	virtual BOOL DoFrameRendering();
	virtual void PreDrawFrame( LPCRECT pRect, LPCRECT pRectScr );
	virtual void OnMeasureFrame( LPRECT pRect );
	virtual void OnChangeWindowPos( LPWINDOWPOS pWindowPos );
	virtual void OnDrawFrame( HDC hDC, LPCRECT pRect, LPCRECT pRectSrc ) ;
	void RenderRadioBullet( ClsDC *pDC, ClsRect& rcRect, COLORREF crColor, DWORD dwItemState );

	// Data.
	BOOL		m_bDrawOldStyle;
};

#endif // _XPMENU_H_