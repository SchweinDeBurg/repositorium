#ifndef _HEADER_H_
#define _HEADER_H_
//
// header.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "commoncontrol.h"

// A wrapper for the Header common control.
class ClsHeader : public ClsCommonControl
{
	_NO_COPY_PARAM( ClsHeader, ClsCommonControl );
public:
	// Constructor.
	ClsHeader() : ClsCommonControl( ICC_WIN95_CLASSES )
	{;}

	// Destructor.
	virtual ~ClsHeader()
	{;}

	// Create a header control.
	inline BOOL Create( ClsWindow *parent, ClsRect& rBounds, UINT nID, DWORD dwStyle = WS_VISIBLE | WS_CHILD | HDS_BUTTONS | HDS_HORZ )
	{
		_ASSERT( m_hWnd == NULL ); // May not be valid.

		// Create the control.
		return ClsCommonControl::Create( 0, WC_HEADER, NULL, dwStyle, rBounds, parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	// Create a drag image.
	inline HIMAGELIST CreateDragImage( int nIndex ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the image list.
		return Header_CreateDragImage( m_hWnd, nIndex );
	}

	// Delete a header item.
	inline BOOL DeleteItem( int nIndex )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Delete the item.
		return Header_DeleteItem( m_hWnd, nIndex );
	}

	// Get the image list.
	inline HIMAGELIST GetImageList() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the image list.
		return Header_GetImageList( m_hWnd );
	}

	// Get an item.
	inline BOOL GetItem( HDITEM& item, int nIndex ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the item.
		return Header_GetItem( m_hWnd, nIndex, &item );
	}

	// Get the number of items.
	inline int GetItemCount() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Return the item count.
		return Header_GetItemCount( m_hWnd );
	}

	// Get the item bounds.
	inline BOOL GetItemRect( LPRECT pRect, int nIndex ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the bounds.
		return Header_GetItemRect( m_hWnd, nIndex, pRect );
	}

	// get the item order.
	inline BOOL GetOrderArray( int nCount, LPINT pArray ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the array/
		return Header_GetOrderArray( m_hWnd, nCount, pArray );
	}

	// Get the unicode format.
	inline BOOL GetUnicodeFormat() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the flag.
		return Header_GetUnicodeFormat( m_hWnd );
	}

	// Insert an item.
	inline int InsertItem( HDITEM& item, int nIndex )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Insert the item.
		return Header_InsertItem( m_hWnd, nIndex, &item );
	}

	// Layout the header.
	inline BOOL Layout( HDLAYOUT& layout )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Layout...
		return Header_Layout( m_hWnd, &layout );
	}

	// Order the items.
	inline int OrderToIndex( int iOrder )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Order the items.
		return Header_OrderToIndex( m_hWnd, iOrder );
	}

	// Set the hot divider.
	inline int SetHotDivider( BOOL bFlag, DWORD dwInputValue )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set it.
		return Header_SetHotDivider( m_hWnd, bFlag, dwInputValue );
	}

	// Set the image list.
	inline HIMAGELIST SetImageList( HIMAGELIST hImageList )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the image list.
		return Header_SetImageList( m_hWnd, hImageList );
	}

	// Set item info.
	inline BOOL SetItem( HDITEM& item, int nIndex )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the item info.
		return Header_SetItem( m_hWnd, nIndex, &item );
	}

	// Set the order array.
	inline BOOL SetOrderArray( int nCount, LPINT pArray )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the array.
		return Header_SetOrderArray( m_hWnd, nCount, pArray );
	}

	// Set the unicode flag.
	inline BOOL SetUnicodeFormat( BOOL bFormat )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the flag.
		return Header_SetUnicodeFormat( m_hWnd, bFormat );
	}

protected:
	// Overidables.
	virtual LRESULT OnBeginDrag( LPNMHEADER pHeader, BOOL& bNotifyParent )
	{ return FALSE; }

	virtual LRESULT OnBeginTrack( LPNMHEADER pHeader, BOOL& bNotifyParent )
	{ return FALSE; }

	virtual LRESULT OnDividerDblClick( LPNMHEADER pHeader, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnEndDrag( LPNMHEADER pHeader, BOOL& bNotifyParent )
	{ return FALSE; }

	virtual LRESULT OnEndTrack( LPNMHEADER pHeader, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnGetDispInfo( LPNMHDDISPINFO pDispInfo, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnItemChanged( LPNMHEADER pHeader, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnItemChanging( LPNMHEADER pHeader, BOOL& bNotifyParent )
	{ return FALSE; }

	virtual LRESULT OnItemClick( LPNMHEADER pHeader, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnItemDblClick( LPNMHEADER pHeader, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnTrack( LPNMHEADER pHeader, BOOL& bNotifyParent )
	{ return FALSE; }

	// Reflected notification handler which will call
	// the virtual notification handlers.
	virtual LRESULT OnReflectedNotify( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{
		// Evaluate notification code.
		switch ( pNMHDR->code )
		{
			case	HDN_BEGINDRAG:       return OnBeginDrag(( LPNMHEADER )pNMHDR, bNotifyParent );
			case	HDN_BEGINTRACK:	     return OnBeginTrack(( LPNMHEADER )pNMHDR, bNotifyParent );
			case	HDN_DIVIDERDBLCLICK: return OnDividerDblClick(( LPNMHEADER )pNMHDR, bNotifyParent );
			case	HDN_ENDDRAG:	     return OnEndDrag(( LPNMHEADER )pNMHDR, bNotifyParent );
			case	HDN_ENDTRACK:	     return OnEndTrack(( LPNMHEADER )pNMHDR, bNotifyParent );
			case	HDN_GETDISPINFO:     return OnGetDispInfo(( LPNMHDDISPINFO )pNMHDR, bNotifyParent );
			case	HDN_ITEMCHANGED:     return OnItemChanged(( LPNMHEADER )pNMHDR, bNotifyParent );
			case	HDN_ITEMCHANGING:    return OnItemChanging(( LPNMHEADER )pNMHDR, bNotifyParent );
			case	HDN_ITEMCLICK:       return OnItemClick(( LPNMHEADER )pNMHDR, bNotifyParent );
			case	HDN_ITEMDBLCLICK:    return OnItemDblClick(( LPNMHEADER )pNMHDR, bNotifyParent );
			case	HDN_TRACK:	     return OnTrack(( LPNMHEADER )pNMHDR, bNotifyParent );
		}
		// Call the base class.
		return ClsCommonControl::OnReflectedNotify( pNMHDR, bNotifyParent );
	}
};

#endif // _HEADER_H_