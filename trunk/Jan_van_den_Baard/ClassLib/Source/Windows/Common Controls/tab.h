#ifndef _TAB_H_
#define _TAB_H_
//
// tab.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "commoncontrol.h"

// A wrapper for the Tab common control.
class ClsTab : public ClsCommonControl
{
	_NO_COPY_PARAM( ClsTab, ClsCommonControl );
public:
	// Constructor.
	ClsTab() : ClsCommonControl( ICC_WIN95_CLASSES )
	{;}

	// Destructor.
	virtual ~ClsTab()
	{;}

	// Create a tab control.
	inline BOOL Create( ClsWindow *parent, const ClsRect& rBounds, UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS )
	{
		_ASSERT( m_hWnd == NULL ); // May not be valid.

		// Create the control.
		return ClsCommonControl::Create( 0, WC_TABCONTROL, NULL, dwStyle, rBounds, parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	// Adjust control bounds.
	inline void AdjustRect( BOOL fLarger, LPRECT pRect )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		TabCtrl_AdjustRect( m_hWnd, fLarger, pRect );
	}

	// Delete all items.
	inline BOOL DeleteAllItems()
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_DeleteAllItems( m_hWnd );
	}

	// Delete specific item.
	inline BOOL DeleteItem( int iItem )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_DeleteItem( m_hWnd, iItem );
	}

	// Deselect all items.
	inline void DeselectAll( BOOL fExcludeFocus = FALSE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		TabCtrl_DeselectAll( m_hWnd, fExcludeFocus );
	}

	// Return focused item.
	inline int GetCurFocus() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_GetCurFocus( m_hWnd );
	}

	// Return selected item.
	inline int GetCurSel() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_GetCurSel( m_hWnd );
	}

	// Return extended style.
	inline DWORD GetExtendedStyle() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_GetExtendedStyle( m_hWnd );
	}

	// Get the image list.
	inline HIMAGELIST GetImageList() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_GetImageList( m_hWnd );
	}

	// Get item information.
	inline BOOL GetItem( int iItem, TCITEM& item ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_GetItem( m_hWnd, iItem, &item );
	}

	// Get the number of items.
	inline int GetItemCount() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_GetItemCount( m_hWnd );
	}

	// Get the item rectangle.
	inline BOOL GetItemRect( int iItem, LPRECT pRect ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_GetItemRect( m_hWnd, iItem, pRect );
	}

	// Get number of rows.
	inline int GetRowCount() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_GetRowCount( m_hWnd );
	}

	// Get the tooltip control.
	inline HWND GetToolTips() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_GetToolTips( m_hWnd );
	}

	// Get the unicode flag.
	inline BOOL GetUnicodeFormat() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_GetUnicodeFormat( m_hWnd );
	}

	// Higlight an item.
	inline BOOL HighlightItem( int iItem, WORD fHighlight )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_HighlightItem( m_hWnd, iItem, fHighlight );
	}

	// Hittest the control.
	inline int HitTest( TCHITTESTINFO& htinfo ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_HitTest( m_hWnd, &htinfo );
	}

	// Insert an item.
	inline int InsertItem( int iItem, const TCITEM& item )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_InsertItem( m_hWnd, iItem, &item );
	}

	// Remove an image.
	inline void RemoveImage( int iImage )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		TabCtrl_RemoveImage( m_hWnd, iImage );
	}

	// Set current focus.
	inline void SetCurFocus( int iItem )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		TabCtrl_SetCurFocus( m_hWnd, iItem );
	}

	// Set current selection.
	inline int SetCurSel( int iItem )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_SetCurSel( m_hWnd, iItem );
	}

	// Set the extended style.
	inline DWORD SetExtendedStyle( DWORD dwExStyle )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_SetExtendedStyle( m_hWnd, dwExStyle );
	}

	// Set the image list.
	inline HIMAGELIST SetImageList( HIMAGELIST hImageList )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_SetImageList( m_hWnd, hImageList );
	}

	// Set item info.
	inline BOOL SetItem( int iItem, const TCITEM& item )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_SetItem( m_hWnd, iItem, &item );
	}

	// Set extra bytes per item reserved for application.
	inline BOOL SetItemExtra( int cb )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_SetItemExtra( m_hWnd, cb );
	}
	
	// Set the item size.
	inline DWORD SetItemSize( int cx, int cy )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_SetItemSize( m_hWnd, cx, cy );
	}

	// Set minimal tab width.
	inline int SetMinTabWidth( int cx )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_SetMinTabWidth( m_hWnd, cx );
	}

	// Set tab icon padding.
	inline void SetPadding( int cx, int cy )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		TabCtrl_SetPadding( m_hWnd, cx, cy );
	}

	// Set the tooltips window.
	inline void SetToolTips( HWND hWndToolTips )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		TabCtrl_SetToolTips( m_hWnd, hWndToolTips );
	}

	// Set the unicode flag.
	inline BOOL SetUnicodeFormat( BOOL bUnicode )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TabCtrl_SetUnicodeFormat( m_hWnd, bUnicode );
	}
protected:
	// Overidables.
	virtual LRESULT OnGetObject( LPNMOBJECTNOTIFY pObjectNotify, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnKeyDown( NMTCKEYDOWN *pKeyDown, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnSelChange( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnSelChanging( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	// Reflected notification handler which will call
	// the virtual notification handlers.
	virtual LRESULT OnReflectedNotify( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{
		// Evaluate notification code.
		switch ( pNMHDR->code )
		{
			case	TCN_GETOBJECT:   return OnGetObject(( LPNMOBJECTNOTIFY )pNMHDR, bNotifyParent );
			case	TCN_KEYDOWN:     return OnKeyDown(( NMTCKEYDOWN * )pNMHDR, bNotifyParent );
			case	TCN_SELCHANGE:   return OnSelChange( pNMHDR, bNotifyParent );
			case	TCN_SELCHANGING: return OnSelChanging( pNMHDR, bNotifyParent );
		}
		// Call the base class.
		return ClsCommonControl::OnReflectedNotify( pNMHDR, bNotifyParent );
	}
};

#endif // _TAB_H_