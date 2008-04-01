#ifndef _BITMAPMENU_H_
#define _BITMAPMENU_H_
//
// bitmapmenu.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "menu.h"
#include "../gdi/brush.h"
#include "../collectors/simplearray.h"
#include "../strings/string.h"
#include "../coords/rect.h"

// Just in case...
#ifndef ODS_NOACCEL
#define ODS_NOACCEL         0x0100
#endif

// An array of these can be used to pass 
// to the SetItemBitmap() function.
struct StrItemBitmap
{
	int		m_iBitmap;	// Bitmap index in the image list.
	WORD		m_wID;		// Item ID or index.
};

#define SBM_END	( WORD )-1	// Set in "m_wID" to terminate the array.

// Structure which is used to store the
// data required to render the items. A
// pointer to this structure is stored
// in the item data field.
class ClsBitmapMenu;
struct ItemData
{
	ClsBitmapMenu  *m_pOwner;
	BOOL		m_bSeparator;
	BOOL		m_bToplevel;
	int		m_iBitmap;
	ClsString	m_strItemName;
	UINT		m_wID;
};

// Forward declarations.
class ClsDC;

// A class providing ownerdraw routines for menus.
class ClsBitmapMenu : public ClsMenu
{
	// No copy constructor.
	_NO_COPY( ClsBitmapMenu );
public:
	// Constructor.
	ClsBitmapMenu();

	// Virtual destructor.
	virtual ~ClsBitmapMenu();

	// Interface.
	virtual BOOL Destroy();
	static void DeleteTempObjects();
	HIMAGELIST SetImageList( HIMAGELIST hImages );
	inline HIMAGELIST GetImageList() { return m_hImageList; }
	inline BOOL& IsPopupMenu() { return ( BOOL& )m_bIsPopup; }
	BOOL ConvertMenus();
	BOOL SetItemBitmap( UINT nItemID, int iBitmap, BOOL bByPos = FALSE );
	BOOL SetItemBitmap( StrItemBitmap *pItemBitmaps, BOOL bByPos = FALSE );
	BOOL GetItemText( UINT nItemID, ClsString& str, BOOL bByPos = FALSE );
	BOOL SetItemText( UINT nItemID, LPCTSTR pszName, BOOL bByPos = FALSE );
	virtual BOOL DeleteMenu( UINT uPosition, BOOL bByPos = FALSE );
	ClsBitmapMenu *GetSubMenu( int nPos );
	inline void IconFrame( BOOL bFrame = TRUE ) { m_bIconFrame = bFrame; }
	inline void DrawMenuShadow( BOOL bShadow = TRUE ) { m_bDrawShadow = bShadow; }

protected:
	// Overidables.
	virtual LRESULT OnReflectedMeasureItem( LPMEASUREITEMSTRUCT pmis, BOOL& bNotifyParent );
	virtual LRESULT OnReflectedDrawItem( LPDRAWITEMSTRUCT pdis, BOOL& bNotifyParent );
	virtual BOOL DoFrameRendering();
	virtual void OnChangeWindowPos( LPWINDOWPOS pWindowPos );
	virtual void OnMeasureFrame( LPRECT pRect );
	virtual void OnDrawFrame( HDC hDC, LPCRECT pRect, LPCRECT pRectSrc ) ;
	virtual void OnReflectedExitMenuLoop( ClsWindow *pWindow, BOOL bShortcut );
	virtual void OnReflectedUnInitMenuPopup( ClsWindow *pWindow, HMENU hPopup, LPARAM lParam );
	virtual void OnReflectedInitMenuPopup( ClsWindow *pWindow, HMENU hPopup, int nIndex, BOOL bWindowMenu );
	
	// Helpers.
	BOOL ConvertMenusHelper( HMENU hMenu = NULL );
	void SplitCaption( LPCTSTR pszCaption, ClsString& sLeft, ClsString& sRight );
	void RenderItemText( LPDRAWITEMSTRUCT pdis, ItemData *pData );
	void RenderRadioBullet( ClsDC *pDC, ClsRect& rcRect, COLORREF crColor );
	void RenderCheckmark( ClsDC *pDC, ClsRect& rcRect, COLORREF crFgColor, COLORREF crBgColor, DWORD dwItemState, LPMENUITEMINFO pmii );
	void DestroyItemData( HMENU hMenu = NULL );
	void RemoveFromArray( ItemData *pData );
	BOOL IsItemInUse( ItemData *pData, HMENU hMenu = NULL );
	void GarbageCollect();

	// Data.
	int				m_cxBitmap;
	int				m_cyBitmap;
	HIMAGELIST			m_hImageList;
	BOOL				m_bIsSubMenu;
	BOOL				m_bIconFrame;
	BOOL				m_bConverted;
	BOOL				m_bIsPopup;
	BOOL				m_bDrawShadow;
	ClsSimpleArray< ItemData *>	m_ItemArray;
	ItemData		        m_SeparatorData;
};

#endif // _BITMAPMENU_H_