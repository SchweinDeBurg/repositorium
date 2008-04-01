#ifndef _TREEVIEW_H_
#define _TREEVIEW_H_
//
// treeview.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "commoncontrol.h"

// A wrapper for the Treeview common control.
class ClsTreeView : public ClsCommonControl
{
	_NO_COPY_PARAM( ClsTreeView, ClsCommonControl );
public:
	// Constructor.
	ClsTreeView() : ClsCommonControl( ICC_WIN95_CLASSES )
	{;}

	// Destructor.
	virtual ~ClsTreeView()
	{;}

	// Create a treeview control.
	inline BOOL Create( ClsWindow *parent, ClsRect& rBounds, UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | TVS_HASLINES )
	{
		_ASSERT( m_hWnd == NULL ); // May not be valid.

		// Create the control.
		return ClsCommonControl::Create( 0, WC_TREEVIEW, NULL, dwStyle, rBounds, parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	// Create a drag image.
	inline HIMAGELIST CreateDragImage( HTREEITEM hItem ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_CreateDragImage( m_hWnd, hItem );
	}

	// Delete all items.
	inline BOOL DeleteAllItems()
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_DeleteAllItems( m_hWnd );
	}

	// Delete a single item.
	inline BOOL DeleteItem( HTREEITEM hItem )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_DeleteItem( m_hWnd, hItem );
	}

	// Edit an item.
	inline HWND EditLabel( HTREEITEM hItem )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_EditLabel( m_hWnd, hItem );
	}

	// End editing the label.
	inline BOOL EndEditLabelNow( BOOL bCancel )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_EndEditLabelNow( m_hWnd, bCancel );
	}

	// Make sure an item is visible.
	inline BOOL EnsureVisible( HTREEITEM hItem )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_EnsureVisible( m_hWnd, hItem );
	}

	// Expand or collapse an item.
	inline BOOL Expand( HTREEITEM hItem, UINT nFlags )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_Expand( m_hWnd, hItem, nFlags );
	}

	// Get background color.
	inline COLORREF GetBkColor() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetBkColor( m_hWnd );
	}

	// Get the item it's first child.
	inline HTREEITEM GetChild( HTREEITEM hItem ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetChild( m_hWnd, hItem );
	}

	// Get the number of items.
	inline UINT GetCount() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetCount( m_hWnd );
	}

	// Get the DnD target.
	inline HTREEITEM GetDropHilight() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetDropHilight( m_hWnd );
	}

	// Get the label edit control handle.
	inline HWND GetEditControl() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetEditControl( m_hWnd );
	}

	// Get the first visible item.
	inline HTREEITEM GetFirstVisible() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetFirstVisible( m_hWnd );
	}

	// Get the specific image list.
	inline HIMAGELIST GetImageList( int iImage ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetImageList( m_hWnd, iImage );
	}

	// Get the parent->child indentation.
	inline UINT GetIndent() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetIndent( m_hWnd );
	}

	// Get the insertion mark color.
	inline COLORREF GetInsertMarkColor() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetInsertMarkColor( m_hWnd );
	}

	// Get the incremental search string.
	inline UINT GetISearchString( LPTSTR pszString ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetISearchString( m_hWnd, pszString );
	}

	// Get item info.
	inline BOOL GetItem( TVITEM& item ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetItem( m_hWnd, &item );
	}

	// Get the height of the items.
	inline int GetItemHeight() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetItemHeight( m_hWnd );
	}

	// Get the item bounds.
	inline BOOL GetItemRect( HTREEITEM hItem, LPRECT pRect, BOOL bOnlyLabel = FALSE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetItemRect( m_hWnd, hItem, pRect, bOnlyLabel );
	}

	// Get the last visible item.
	inline HTREEITEM GetLastVisible() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetLastVisible( m_hWnd );
	}

	// Get the next item.
	inline HTREEITEM GetNextItem( HTREEITEM hItem, UINT nFlags ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetNextItem( m_hWnd, hItem, nFlags );
	}

	// Get the next sibling.
	inline HTREEITEM GetNextSibling( HTREEITEM hItem ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetNextSibling( m_hWnd, hItem );
	}

	// Get the next visible item.
	inline HTREEITEM GetNextVisible( HTREEITEM hItem ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetNextVisible( m_hWnd, hItem );
	}

	// Get the parent.
	inline HTREEITEM GetParentItem( HTREEITEM hItem ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetParent( m_hWnd, hItem );
	}

	// Get the previous sibling.
	inline HTREEITEM GetPrevSibling( HTREEITEM hItem ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetPrevSibling( m_hWnd, hItem );
	}

	// Get the previous visible item.
	inline HTREEITEM GetPrevVisible( HTREEITEM hItem ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetPrevVisible( m_hWnd, hItem );
	}

	// Get the root item.
	inline HTREEITEM GetRoot() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetRoot( m_hWnd );
	}

	// Get the maximum scroll time.
	inline UINT GetScrollTime() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetScrollTime( m_hWnd );
	}

	// Get the currently selected item.
	inline HTREEITEM GetSelection() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetSelection( m_hWnd );
	}

	// Get the text color.
	inline COLORREF GetTextColor() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetTextColor( m_hWnd );
	}

	// Get the tooltip control.
	inline HWND GetToolTips() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetToolTips( m_hWnd );
	}

	// Get unicode flag.
	inline BOOL GetUnicodeFormat() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetUnicodeFormat( m_hWnd );
	}

	// Get number of visible items.
	inline UINT GetVisibleCount() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_GetVisibleCount( m_hWnd );
	}

	// Hit test the control.
	inline HTREEITEM HitTest( TVHITTESTINFO& htinfo ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_HitTest( m_hWnd, &htinfo );
	}

	// Insert an item.
	inline HTREEITEM InsertItem( TVINSERTSTRUCT& istruct )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_InsertItem( m_hWnd, &istruct );
	}

	// Select an item.
	inline BOOL Select( HTREEITEM hItem, UINT nFlags )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_Select( m_hWnd, hItem, nFlags );
	}

	// Redraw item to a DnD target.
	inline BOOL SelectDropTarget( HTREEITEM hItem )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_SelectDropTarget( m_hWnd, hItem );
	}

	// Select the item.
	inline BOOL SelectItem( HTREEITEM hItem )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_SelectItem( m_hWnd, hItem );
	}

	// Setup background color.
	inline COLORREF SetBkColor( COLORREF crColor )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_SetBkColor( m_hWnd, crColor );
	}

	// Select the item and make it the first visible item.
	inline BOOL SelectSetFirstVisible( HTREEITEM hItem )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_SelectSetFirstVisible( m_hWnd, hItem );
	}

	// Set the specific image list.
	inline HIMAGELIST SetImageList( HIMAGELIST hImageList, int iImage )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_SetImageList( m_hWnd, hImageList, iImage );
	}

	// Set parent->child indention.
	inline void SetIndent( int iIndent )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		TreeView_SetIndent( m_hWnd, iIndent );
	}

	// Set insertion mark.
	inline BOOL SetInsertMark( HTREEITEM hItem, BOOL bAfter )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_SetInsertMark( m_hWnd, hItem, bAfter );
	}

	// Set insertion mark color.
	inline COLORREF SetInsertMarkColor( COLORREF crColor )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_SetInsertMarkColor( m_hWnd, crColor );
	}

	// Set item info.
	inline BOOL SetItem( TVITEM& item )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )TreeView_SetItem( m_hWnd, &item );
	}

	// Set item height.
	inline int SetItemHeight( int cy )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_SetItemHeight( m_hWnd, cy );
	}

	// Set the maximum scroll time.
	inline UINT SetScrollTime( UINT nScrollTime )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_SetScrollTime( m_hWnd, nScrollTime );
	}

	// Set the text color.
	inline COLORREF SetTextColor( COLORREF crColor )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_SetTextColor( m_hWnd, crColor );
	}

	// Set the tooltip control.
	inline HWND SetToolTips( HWND hWndTooltip )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_SetToolTips( m_hWnd, hWndTooltip );
	}

	// Set the unicode flag.
	inline BOOL SetUnicodeFormat( BOOL bUnicode )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_SetUnicodeFormat( m_hWnd, bUnicode );
	}

	// Sort children.
	inline BOOL SortChildren( HTREEITEM hItem, BOOL bRecurse = FALSE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_SortChildren( m_hWnd, hItem, bRecurse );
	}

	// Sort children with callback.
	inline BOOL SortChildrenCB( TVSORTCB& sort, BOOL bRecurse = FALSE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return TreeView_SortChildrenCB( m_hWnd, &sort, bRecurse );
	}

	// Obtain the item data.
	inline LPARAM GetItemData( HTREEITEM hItem ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		TVITEM tvi;
		tvi.mask  = TVIF_PARAM;
		tvi.hItem = hItem;
		if ( GetItem( tvi ))
			return tvi.lParam;
		return 0;
	}
protected:
	// Overidables.
	virtual LRESULT OnBeginDrag( LPNMTREEVIEW pTreeView, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnBeginLabelEdit( LPNMTVDISPINFO pDispInfo, BOOL& bNotifyParent )
	{ return FALSE; }

	virtual LRESULT OnBeginRDrag( LPNMTREEVIEW pTreeView, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnDeleteItem( LPNMTREEVIEW pTreeView, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnEndLabelEdit( LPNMTVDISPINFO pDispInfo, BOOL& bNotifyParent )
	{ return pDispInfo->item.pszText ? TRUE : FALSE; }

	virtual LRESULT OnGetDispInfo( LPNMTVDISPINFO pDispInfo, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnGetInfoTip( LPNMTVGETINFOTIP pInfoTip, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnItemExpanded( LPNMTREEVIEW pTreeView, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnItemExpanding( LPNMTREEVIEW pTreeView, BOOL& bNotifyParent )
	{ return FALSE; }

	virtual LRESULT OnKeyDown( LPNMTVKEYDOWN pKeyDown, BOOL& bNotifyParent )
	{ return FALSE; }

	virtual LRESULT OnSelChanged( LPNMTREEVIEW pTreeView, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnSelChanging( LPNMTREEVIEW pTreeView, BOOL& bNotifyParent )
	{ return FALSE; }

	virtual LRESULT OnSetDispInfo( LPNMTVDISPINFO pDispInfo, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnSingleExpand( LPNMTREEVIEW pTreeView, BOOL& bNotifyParent )
	{ return 0; }

	// Reflected notification handler which will call
	// the virtual notification handlers.
	virtual LRESULT OnReflectedNotify( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{
		// Evaluate notification code.
		switch ( pNMHDR->code )
		{
			case	TVN_BEGINDRAG:	    return OnBeginDrag(( LPNMTREEVIEW )pNMHDR, bNotifyParent );
			case	TVN_BEGINLABELEDIT: return OnBeginLabelEdit(( LPNMTVDISPINFO )pNMHDR, bNotifyParent );
			case	TVN_BEGINRDRAG:	    return OnBeginRDrag(( LPNMTREEVIEW )pNMHDR, bNotifyParent );
			case	TVN_DELETEITEM:     return OnDeleteItem(( LPNMTREEVIEW )pNMHDR, bNotifyParent );
			case	TVN_ENDLABELEDIT:   return OnEndLabelEdit(( LPNMTVDISPINFO )pNMHDR, bNotifyParent );
			case	TVN_GETDISPINFO:    return OnGetDispInfo(( LPNMTVDISPINFO )pNMHDR, bNotifyParent );
			case	TVN_GETINFOTIP:     return OnGetInfoTip(( LPNMTVGETINFOTIP )pNMHDR, bNotifyParent );
			case	TVN_ITEMEXPANDED:   return OnItemExpanded(( LPNMTREEVIEW )pNMHDR, bNotifyParent );
			case	TVN_ITEMEXPANDING:  return OnItemExpanding(( LPNMTREEVIEW )pNMHDR, bNotifyParent );
			case	TVN_KEYDOWN:        return OnKeyDown(( LPNMTVKEYDOWN )pNMHDR, bNotifyParent );
			case	TVN_SELCHANGED:     return OnSelChanged(( LPNMTREEVIEW )pNMHDR, bNotifyParent );
			case	TVN_SELCHANGING:    return OnSelChanging(( LPNMTREEVIEW )pNMHDR, bNotifyParent );
			case	TVN_SETDISPINFO:    return OnSetDispInfo(( LPNMTVDISPINFO )pNMHDR, bNotifyParent );
			case	TVN_SINGLEEXPAND:   return OnSingleExpand(( LPNMTREEVIEW )pNMHDR, bNotifyParent );
		}
		// Call the base class.
		return ClsCommonControl::OnReflectedNotify( pNMHDR, bNotifyParent );
	}

	// The treeview seems to have trouble setting a normal arrow mouse
	// pointer... at least under Windows ME.
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		// The right message?
		if ( uMsg == WM_SETCURSOR )
			SetCursor( LoadCursor( NULL, IDC_ARROW ));

		// Let the base handle the rest...
		return ClsCommonControl::WindowProc( uMsg, wParam, lParam );
	}
};

#endif // _TREEVIEW_H_