#ifndef _LISTVIEW_H_
#define _LISTVIEW_H_
//
// listview.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "commoncontrol.h"
#include "../../gdi/getdc.h"
#include "../../gdi/selector.h"

// A wrapper for the Listview common control.
class ClsListView : public ClsCommonControl
{
	_NO_COPY_PARAM( ClsListView, ClsCommonControl );
public:
	// Constructor.
	ClsListView() : ClsCommonControl( ICC_WIN95_CLASSES )
	{;}

	// Destructor.
	virtual ~ClsListView()
	{;}

	// Create a listview control.
	inline BOOL Create( ClsWindow *parent, ClsRect& rBounds, UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS )
	{
		_ASSERT( m_hWnd == NULL ); // May not be valid.

		// Create the control.
		return ClsCommonControl::Create( 0, WC_LISTVIEW, NULL, dwStyle, rBounds, parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	// Approximate the view rectangle.
	inline DWORD ApproximateViewRect( int cx, int cy, int iCount = -1 )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Approximate...
		return ListView_ApproximateViewRect( m_hWnd, cx, cy, iCount );
	}

	// Arrange the items.
	inline BOOL Arrange( UINT nCode = LVA_DEFAULT )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Arrange the items.
		return ListView_Arrange( m_hWnd, nCode );
	}

	// Create a drag image.
	inline HIMAGELIST CreateDragImage( int iItem, LPPOINT pPoint )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the drag image.
		return ListView_CreateDragImage( m_hWnd, iItem, pPoint );
	}

	// Delete the contents.
	inline BOOL DeleteAllItems()
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Delete the items.
		return ListView_DeleteAllItems( m_hWnd );
	}

	// Delete a column.
	inline BOOL DeleteColumn( int nColumn )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Delete the column.
		return ListView_DeleteColumn( m_hWnd, nColumn );
	}

	// Delete an item.
	inline BOOL DeleteItem( int nItem )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Delete the item.
		return ListView_DeleteItem( m_hWnd, nItem );
	}

	// Edit a specific label.
	inline HWND EditLabel( int nItem )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Edit the label.
		return ListView_EditLabel( m_hWnd, nItem );
	}

	// Make sure an item is visible.
	inline BOOL EnsureVisible( int nItem, BOOL bPartialOK = FALSE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Make sure it's visible.
		return ListView_EnsureVisible( m_hWnd, nItem, bPartialOK );
	}

	// Find an item.
	inline int FindItem( int nStart, const LVFINDINFO& lvfi ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Find the item.
		return ListView_FindItem( m_hWnd, nStart, &lvfi );
	}

	// get the background color.
	inline COLORREF GetBkColor() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the color.
		return ListView_GetBkColor( m_hWnd );
	}

	// Get the background image info.
	inline BOOL GetBkImage( LVBKIMAGE& lvbki ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the image info.
		return ListView_GetBkImage( m_hWnd, &lvbki );
	}

	// Get the callback mask.
	inline UINT GetCallbackMask() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Return the callback mask.
		return ListView_GetCallbackMask( m_hWnd );
	}

	// Get the item checked state.
	inline BOOL GetCheckState( UINT nItem ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the checked state.
		return ListView_GetCheckState( m_hWnd, nItem );
	}

	// Get column info.
	inline BOOL GetColumn( int nColumn, LVCOLUMN& lvc ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the info.
		return ListView_GetColumn( m_hWnd, nColumn, &lvc );
	}

	// Get the columns order.
	inline BOOL GetColumnOrderArray( int iCount, LPINT pArray ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the order.
		return ListView_GetColumnOrderArray( m_hWnd, iCount, pArray );
	}

	// Get the column width.
	inline int GetColumnWidth( int nColumn ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the width.
		return ListView_GetColumnWidth( m_hWnd, nColumn );
	}

	// Get number of items per page.
	inline int GetCountPerPage() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the count.
		return ListView_GetCountPerPage( m_hWnd );
	}

	// Get the edit control.
	inline HWND GetEditControl() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the edit control.
		return ListView_GetEditControl( m_hWnd );
	}

	// Get the extended style.
	inline DWORD GetExtendedListViewStyle() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the extended style bits.
		return ListView_GetExtendedListViewStyle( m_hWnd );
	}

	// Get the header control.
	inline HWND GetHeader() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the header control.
		return ListView_GetHeader( m_hWnd );
	}

	// Get the hot cursor.
	inline HCURSOR GetHotCursor() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the cursor.
		return ListView_GetHotCursor( m_hWnd );
	}

	// Get the hot item.
	inline int GetHotItem() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the item.
		return ListView_GetHotItem( m_hWnd );
	}

	// Get the specific image list.
	inline HIMAGELIST GetImageList( int iImageList = LVSIL_NORMAL ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the image list.
		return ListView_GetImageList( m_hWnd, iImageList );
	}

	// Get the incremental search string.
	inline int GetISearchString( LPSTR pszString ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the string.
		return ListView_GetISearchString( m_hWnd, pszString );
	}

	// Get item info.
	inline BOOL GetItem( LVITEM& lvi ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the info.
		return ListView_GetItem( m_hWnd, &lvi );
	}

	// Get the number of items.
	inline int GetItemCount() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the count.
		return ListView_GetItemCount( m_hWnd );
	}

	// Get the item position.
	inline BOOL GetItemPosition( int nItem, LPPOINT pPoint ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the position.
		return ListView_GetItemPosition( m_hWnd, nItem, pPoint );
	}

	// Get the item bounds.
	inline BOOL GetItemRect( int nItem, LPRECT pRect, int nCode = LVIR_BOUNDS ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the bounds.
		return ListView_GetItemRect( m_hWnd, nItem, pRect, nCode );
	}

	// Get item spacing.
	inline DWORD GetItemSpacing( BOOL bSmall ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the spacing.
		return ListView_GetItemSpacing( m_hWnd, bSmall );
	}

	// Get item state.
	inline UINT GetItemState( int nItem, UINT nMask ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the item state.
		return ListView_GetItemState( m_hWnd, nItem, nMask );
	}

	// Get item text.
	inline void GetItemText( int nItem, int nSubItem, LPTSTR pszText, int nMax ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the item text.
		ListView_GetItemText( m_hWnd, nItem, nSubItem, pszText, nMax );
	}

	// Get item text.
	inline void GetItemText( int nItem, int nSubItem, ClsString& str ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// 4K chars is the maximum.
		TCHAR szBuffer[ 4096 ];

		// Get the item text.
		ListView_GetItemText( m_hWnd, nItem, nSubItem, szBuffer, 4096 );

		// Assign to the string object.
		str = szBuffer;
	}

	// Get the next item.
	inline int GetNextItem( int nStart, UINT nFlags = LVNI_ALL ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the next item.
		return ListView_GetNextItem( m_hWnd, nStart, nFlags );
	}

	// Get the number of work areas.
	inline BOOL GetNumberOfWorkAreas( UINT& nNumAreas ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the number of work areas.
		return ListView_GetNumberOfWorkAreas( m_hWnd, &nNumAreas );
	}

	// Get the view origin.
	inline BOOL GetOrigin( LPPOINT pPoint ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the origin.
		return ListView_GetOrigin( m_hWnd, pPoint );
	}

	// Get the number of selected items.
	inline UINT GetSelectedCount() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the count.
		return ListView_GetSelectedCount( m_hWnd );
	}

	// Get the selection mark.
	inline int GetSelectionMark() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Return the mark.
		return ListView_GetSelectionMark( m_hWnd );
	}

	// Get the string width.
	inline int GetStringWidth( LPSTR pszString ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Return the string width.
		return ListView_GetStringWidth( m_hWnd, pszString );
	}

	// Get the subitem rect.
	inline BOOL GetSubItemRect( int nItem, int nSubItem, int nCode, LPRECT pRect ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the rectangle.
		return ListView_GetSubItemRect( m_hWnd, nItem, nSubItem, nCode, pRect );
	}

	// Get the text background color.
	inline COLORREF GetTextBkColor() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the color.
		return ListView_GetTextBkColor( m_hWnd );
	}

	// Get the text color.
	inline COLORREF GetTextColor() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the color.
		return ListView_GetTextColor( m_hWnd );
	}

	// Get the tooltip control.
	inline HWND GetToolTips() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the control.
		return ListView_GetToolTips( m_hWnd );
	}

	// Get the listview top visible index.
	inline int GetTopIndex() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Return the index.
		return ListView_GetTopIndex( m_hWnd );
	}

	// Get the unicode flag.
	inline BOOL GetUnicodeFormat() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the flag.
		return ListView_GetUnicodeFormat( m_hWnd );
	}

	// Get the view bounds.
	inline BOOL GetViewRect( LPRECT pRect ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the bounds.
		return ListView_GetViewRect( m_hWnd, pRect );
	}

	// Get the work area bounds.
	inline void GetWorkAreas( int nRects, LPRECT pRectArray ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the bounds.
		ListView_GetWorkAreas( m_hWnd, nRects, pRectArray );
	}

	// Get the item at the specific position.
	inline int HitTest( LVHITTESTINFO& lvhti ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the item.
		return ListView_HitTest( m_hWnd, &lvhti );
	}

	// Insert a column.
	inline int InsertColumn( int nColumn, const LVCOLUMN& lvc )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Insert the column.
		return ListView_InsertColumn( m_hWnd, nColumn, &lvc );
	}

	// Insert a column.
	inline int InsertColumn( int nColumn, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1 )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.
		_ASSERT_VALID( lpszColumnHeading ); // Must be valid.

		// Build structure.
		LVCOLUMN	lvc = { 0 };
		ClsString	str;
		lvc.mask	= LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
		lvc.fmt		= nFormat;
		lvc.cx		= nWidth;
		lvc.iSubItem	= nSubItem;

		// If the hi-order word of "lpszColumnHeading" is 0
		// we load the string from the resources.
		if ( lpszColumnHeading && HIWORD( lpszColumnHeading ) == 0 )
		{
			// Load the string and set it up.
			str.LoadString( LOWORD( lpszColumnHeading ));
			lvc.pszText = str;
		}
		else
			// Use the input text.
			lvc.pszText = ( LPTSTR )lpszColumnHeading;

		// Width passed?
		if ( nWidth == -1 )
		{
			// Get control DC.
			ClsGetDC dc( this );

			// Get the header control.
			ClsWindow *pHeader = ClsWindow::FromHandle( GetHeader());
			if ( pHeader == NULL )
				return -1;

			// Obtain header font.
			ClsFont font;
			pHeader->GetFont( font );

			// Select font.
			ClsSelector sel( &dc, font );

			// Compute the column header width.
			lvc.cx = ( dc.GetTextExtent( lvc.pszText, ( int )_tcslen( lvc.pszText )).CX() + 3 * ::GetSystemMetrics( SM_CXFRAME ));
		}

		// Subitem OK?
		if ( nSubItem != -1 ) lvc.mask |= LVCF_SUBITEM;

		// Insert the item.
		return ListView_InsertColumn( m_hWnd, nColumn, &lvc );
	}

	// Insert an item.
	inline int InsertItem( LVITEM& item )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Insert the item.
		return ListView_InsertItem( m_hWnd, &item );
	}

	// Redraw items.
	inline BOOL RedrawItems( int nFirst, int nLast )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// redraw...
		return ListView_RedrawItems( m_hWnd, nFirst, nLast );
	}

	// Scroll the view.
	inline BOOL Scroll( int dx, int dy )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Scroll the view.
		return ListView_Scroll( m_hWnd, dx, dy );
	}

	// Set the background color.
	inline BOOL SetBkColor( COLORREF crColor )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the color.
		return ListView_SetBkColor( m_hWnd, crColor );
	}

	// Set the background image.
	inline BOOL SetBkImage( LVBKIMAGE& lvbi )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the image.
		return ListView_SetBkImage( m_hWnd, &lvbi );
	}

	// Set the callback mask.
	inline BOOL SetCallbackMask( UINT nMask )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the mask.
		return ListView_SetCallbackMask( m_hWnd, nMask );
	}

	// Set column info.
	inline BOOL SetColumn( int nColumn, LVCOLUMN& lvc )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the info.
		return ListView_SetColumn( m_hWnd, nColumn, &lvc );
	}

	// Set column order.
	inline BOOL SetColumnOrderArray( int nColumns, LPINT pArray )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the order.
		return ListView_SetColumnOrderArray( m_hWnd, nColumns, pArray );
	}

	// Set the column width.
	inline BOOL SetColumnWidth( int nColumn, int nWidth )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the width.
		return ListView_SetColumnWidth( m_hWnd, nColumn, nWidth );
	}

	// Set the extended listview style.
	inline void SetExtendedListViewStyle( DWORD dwExStyle )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the extended style.
		ListView_SetExtendedListViewStyle( m_hWnd, dwExStyle );
	}

	// Set the extended listview style.
	inline void SetExtendedListViewStyleEx( DWORD dwExMask, DWORD dwExStyle )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the extended style.
		ListView_SetExtendedListViewStyleEx( m_hWnd, dwExMask, dwExStyle );
	}

	// Set the hot cursor.
	inline HCURSOR SetHotCursor( HCURSOR hCursor )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the hot cursor.
		return ListView_SetHotCursor( m_hWnd, hCursor );
	}

	// Set the hot item.
	inline int SetHotItem( int nItem )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the hot item.
		return ListView_SetHotItem( m_hWnd, nItem );
	}

	// Set the hover time.
	inline DWORD SetHoverTime( DWORD dwHoverTime )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the hover time.
		return ListView_SetHoverTime( m_hWnd, dwHoverTime );
	}

	// Set the icon spacing.
	inline DWORD SetIconSpacing( int cx, int cy )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the spacing.
		return ListView_SetIconSpacing( m_hWnd, cx, cy );
	}

	// Set the specific image list.
	inline HIMAGELIST SetImageList( HIMAGELIST hImageList, int iImageList )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the image list.
		return ListView_SetImageList( m_hWnd, hImageList, iImageList );
	}

	// Set the item info.
	inline BOOL SetItem( const LVITEM& item )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the info.
		return ListView_SetItem( m_hWnd, &item );
	}

	// Set the number of items.
	inline void SetItemCount( int nItems )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the number of items.
		ListView_SetItemCount( m_hWnd, nItems );
	}

	// Set the number of items.
	inline void SetItemCountEx( int nItems, DWORD dwFlags )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the number of items.
		ListView_SetItemCountEx( m_hWnd, nItems, dwFlags );
	}

	// Set item position.
	inline BOOL SetItemPosition( int nItem, int x, int y )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set item position.
		return ListView_SetItemPosition( m_hWnd, nItem, x, y );
	}

	// Set item position.
	inline BOOL SetItemPosition( int nItem, ClsPoint& point )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set item position.
		return ListView_SetItemPosition( m_hWnd, nItem, point.X(), point.Y());
	}

	// Set item position.
	inline void SetItemPosition32( int nItem, int x, int y )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set item position.
		ListView_SetItemPosition32( m_hWnd, nItem, x, y );
	}

	// Set item position.
	inline void SetItemPosition32( int nItem, ClsPoint& point )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set item position.
		ListView_SetItemPosition32( m_hWnd, nItem, point.X(), point.Y());
	}

	// Set item state.
	inline void SetItemState( int nItem, UINT nState, UINT nMask )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the state.
		ListView_SetItemState( m_hWnd, nItem, nState, nMask );
	}

	// Set item text.
	inline void SetItemText( int nItem, int nSubItem, LPCTSTR pszText )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the item text.
		ListView_SetItemText( m_hWnd, nItem, nSubItem, ( LPTSTR )pszText );
	}

	// Set selection mark.
	inline int SetSelectionMark( int nItem )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the mark.
		return ListView_SetSelectionMark( m_hWnd, nItem );
	}

	// Set the text background color.
	inline BOOL SetTextBkColor( COLORREF crColor )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the text background color.
		return ListView_SetTextBkColor( m_hWnd, crColor );
	}

	// Set the text color.
	inline BOOL SetTextColor( COLORREF crColor )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the color.
		return ListView_SetTextColor( m_hWnd, crColor );
	}

	// Set the tooltips.
	inline HWND SetToolTips( HWND hWndToolTips )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the tooltips.
		return ListView_SetToolTips( m_hWnd, hWndToolTips );
	}

	// Set the unicode flag.
	inline BOOL SetUnicodeFormat( BOOL bUnicode )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the unicode flag.
		return ListView_SetUnicodeFormat( m_hWnd, bUnicode );
	}

	// Set the work areas.
	inline void SetWorkAreas( int nRects, LPRECT pRectArray )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the work areas.
		ListView_SetWorkAreas( m_hWnd, nRects, pRectArray );
	}

	// Sort the listview items.
	inline BOOL SortItems( PFNLVCOMPARE pfnCompare, LPARAM lParamSort )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Sort the items.
		return ListView_SortItems( m_hWnd, pfnCompare, lParamSort );
	}

	// Hittest a subitem.
	inline int SubItemHitTest( LVHITTESTINFO& lvhi )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Hittest...
		return ListView_SubItemHitTest( m_hWnd, &lvhi );
	}

	// Update.
	inline BOOL Update( int nItem )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Update the item.
		return ListView_Update( m_hWnd, nItem );
	}

	// Obtain the item data.
	inline LPARAM GetItemData( int nItem ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		LVITEM lvi;
		lvi.mask  = LVIF_PARAM;
		lvi.iItem = nItem;
		if ( GetItem( lvi ))
			return lvi.lParam;
		return 0;
	}

protected:
	// Overidables.
	virtual LRESULT OnBeginDrag( LPNMLISTVIEW pListView, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnBeginLabelEdit( NMLVDISPINFO *pDispInfo, BOOL& bNotifyParent )
	{ return FALSE; }

	virtual LRESULT OnBeginRDrag( LPNMLISTVIEW pListView, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnColumnClick( LPNMLISTVIEW pListView, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnDeleteAllItems( LPNMLISTVIEW pListView, BOOL& bNotifyParent )
	{ return FALSE; }

	virtual LRESULT OnDeleteItem( LPNMLISTVIEW pListView, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnEndLabelEdit( NMLVDISPINFO *pDispInfo, BOOL& bNotifyParent )
	{ return TRUE; }

	virtual LRESULT OnGetDispInfo( NMLVDISPINFO *pDispInfo, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnGetInfoTip( LPNMLVGETINFOTIP pGetInfoTip, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnInsertItem( LPNMLISTVIEW pListView, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnHotTrack( LPNMLISTVIEW pListView, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnItemActivate( LPNMITEMACTIVATE pItemActive, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnItemChanged( LPNMLISTVIEW pListView, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnItemChanging( LPNMLISTVIEW pListView, BOOL& bNotifyParent )
	{ return FALSE; }

	virtual LRESULT OnKeyDown( LPNMLVKEYDOWN pKeyDown, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnMarqueeBegin( LPNMLISTVIEW pListView, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnODCachehint( NMLVCACHEHINT *pCachehint, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnODFindItem( LPNMLVFINDITEM pFindItem, BOOL& bNotifyParent )
	{ return -1; }

	virtual LRESULT OnODStateChanged( LPNMLVODSTATECHANGE pStateChange, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnSetDispInfo( NMLVDISPINFO *pDispInfo, BOOL& bNotifyParent )
	{ return FALSE; }

	// Reflected notification handler which will call
	// the virtual notification handlers.
	virtual LRESULT OnReflectedNotify( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{
		// Evaluate notification code.
		switch ( pNMHDR->code )
		{
			case	LVN_BEGINDRAG:      return OnBeginDrag(( LPNMLISTVIEW )pNMHDR, bNotifyParent );
			case	LVN_BEGINLABELEDIT: return OnBeginLabelEdit(( NMLVDISPINFO * )pNMHDR, bNotifyParent );
			case	LVN_BEGINRDRAG:     return OnBeginRDrag(( LPNMLISTVIEW )pNMHDR, bNotifyParent );
			case	LVN_COLUMNCLICK:    return OnColumnClick(( LPNMLISTVIEW )pNMHDR, bNotifyParent );
			case	LVN_DELETEALLITEMS: return OnDeleteAllItems(( LPNMLISTVIEW )pNMHDR, bNotifyParent );
			case	LVN_DELETEITEM:     return OnDeleteItem(( LPNMLISTVIEW )pNMHDR, bNotifyParent );
			case	LVN_ENDLABELEDIT:   return OnEndLabelEdit(( NMLVDISPINFO * )pNMHDR, bNotifyParent );
			case	LVN_GETDISPINFO:    return OnGetDispInfo(( NMLVDISPINFO * )pNMHDR, bNotifyParent );
			case	LVN_GETINFOTIP:     return OnGetInfoTip(( LPNMLVGETINFOTIP )pNMHDR, bNotifyParent );
			case	LVN_HOTTRACK:	    return OnHotTrack(( LPNMLISTVIEW )pNMHDR, bNotifyParent );
			case	LVN_INSERTITEM:	    return OnInsertItem(( LPNMLISTVIEW )pNMHDR, bNotifyParent );
			case	LVN_ITEMACTIVATE:   return OnItemActivate(( LPNMITEMACTIVATE )pNMHDR, bNotifyParent );
			case	LVN_ITEMCHANGED:    return OnItemChanged(( LPNMLISTVIEW )pNMHDR, bNotifyParent );
			case	LVN_ITEMCHANGING:   return OnItemChanging(( LPNMLISTVIEW )pNMHDR, bNotifyParent );
			case	LVN_KEYDOWN:	    return OnKeyDown(( LPNMLVKEYDOWN )pNMHDR, bNotifyParent );
			case	LVN_MARQUEEBEGIN:   return OnMarqueeBegin(( LPNMLISTVIEW )pNMHDR, bNotifyParent );
			case	LVN_ODCACHEHINT:    return OnODCachehint(( NMLVCACHEHINT * )pNMHDR, bNotifyParent );
			case	LVN_ODFINDITEM:	    return OnODFindItem(( LPNMLVFINDITEM )pNMHDR, bNotifyParent );
			case	LVN_ODSTATECHANGED: return OnODStateChanged(( LPNMLVODSTATECHANGE )pNMHDR, bNotifyParent );
			case	LVN_SETDISPINFO:    return OnSetDispInfo(( NMLVDISPINFO * )pNMHDR, bNotifyParent );
		}
		// Call the base class.
		return ClsCommonControl::OnReflectedNotify( pNMHDR, bNotifyParent );
	}
};

#endif // _LISTVIEW_H_