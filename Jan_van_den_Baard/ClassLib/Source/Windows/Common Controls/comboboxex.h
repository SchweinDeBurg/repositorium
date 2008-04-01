#ifndef _COMBOBOXEX_H_
#define _COMBOBOXEX_H_
//
// comboboxex.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "commoncontrol.h"

// Forward declarations.
class ClsComboBox;
class ClsEdit;

// A wrapper for the ComboBoxEx common control.
class ClsComboBoxEx : public ClsCommonControl
{
	_NO_COPY_PARAM( ClsComboBoxEx, ClsCommonControl );
public:
	// Constructor.
	ClsComboBoxEx() : ClsCommonControl( ICC_USEREX_CLASSES )
	{;}

	// Destructor.
	virtual ~ClsComboBoxEx()
	{;}

	// Create a ComboBoxEx control.
	inline BOOL Create( ClsWindow *parent, ClsRect& rBounds, UINT nID, DWORD dwStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | CBS_DROPDOWN )
	{
		_ASSERT( m_hWnd == NULL ); // May not be valid.

		// Create the control.
		return ClsCommonControl::Create( 0, WC_COMBOBOXEX, NULL, dwStyle, rBounds, parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	// Delete an item.
	inline int DeleteItem( int nIndex )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Delete the item.
		return ( int )SendMessage( CBEM_DELETEITEM, nIndex );
	}

	// Get the combobox control.
	inline ClsComboBox *GetComboControl() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the control.
		return ( ClsComboBox * )ClsWindow::FromHandle(( HWND )::SendMessage( m_hWnd, CBEM_GETCOMBOCONTROL, 0, 0 ));
	}

	// Get the edit control.
	inline ClsEdit *GetEditControl() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the control.
		return ( ClsEdit * )ClsWindow::FromHandle(( HWND )::SendMessage( m_hWnd, CBEM_GETEDITCONTROL, 0, 0 ));
	}

	// Get the extended style bits.
	inline DWORD GetExtendedStyle() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the extended style bits.
		return ( DWORD )::SendMessage( m_hWnd, CBEM_GETEXTENDEDSTYLE, 0, 0 );
	}

	// Get the image list.
	inline HIMAGELIST GetImageList() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the image list.
		return ( HIMAGELIST )::SendMessage( m_hWnd, CBEM_GETIMAGELIST, 0, 0 );
	}

	// Get item information.
	inline BOOL GetItem( COMBOBOXEXITEM& item ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the item info.
		return ( BOOL )::SendMessage( m_hWnd, CBEM_GETITEM, 0, ( LPARAM )&item );
	}

	// Get the unicode flag.
	inline BOOL GetUnicodeFormat() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the unicode flag.
		return ( BOOL )::SendMessage( m_hWnd, CBEM_GETUNICODEFORMAT, 0, 0 );
	}

	// Edit contents changed?
	inline BOOL HasEditChanged() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the changed flag.
		return ( BOOL )::SendMessage( m_hWnd, CBEM_HASEDITCHANGED, 0, 0 );
	}

	// Insert a new item.
	inline int InsertItem( const COMBOBOXEXITEM& item )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Insert the item
		return ( int )SendMessage( CBEM_INSERTITEM, 0, ( LPARAM )&item );
	}

	// Set the extended style.
	inline DWORD SetExtendedStyle( DWORD dwExStyle, DWORD dwMask = 0 )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the extended style.
		return ( DWORD )SendMessage( CBEM_SETEXTENDEDSTYLE, dwMask, dwExStyle );
	}

	// Set the image list.
	inline HIMAGELIST SetImageList( HIMAGELIST hImageList )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the imagelist.
		return ( HIMAGELIST )SendMessage( CBEM_SETIMAGELIST, 0, ( LPARAM )hImageList );
	}

	// Set item info.
	inline BOOL SetItem( const COMBOBOXEXITEM& item )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the item info.
		return ( BOOL )SendMessage( CBEM_SETITEM, 0, ( LPARAM )&item );
	}

	// Set the unicode format.
	inline BOOL SetUnicodeFormat( BOOL bFormat )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the unicode flag.
		return ( BOOL )SendMessage( CBEM_SETUNICODEFORMAT, bFormat );
	}

protected:
	// Overidables.
	virtual LRESULT OnBeginEdit( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 1; }

	virtual LRESULT OnDeleteItem( PNMCOMBOBOXEX pComboBoxEx, BOOL& bNotifyParent )
	{ return 1; }

	virtual LRESULT OnDragBegin( LPNMCBEDRAGBEGIN pDragBegin, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnEndEdit( PNMCBEENDEDIT pEndEdit, BOOL& bNotifyParent )
	{ return FALSE; }

	virtual LRESULT OnGetDispInfo( PNMCOMBOBOXEX pComboBoxEx, BOOL& bNotifyParent )
	{ return 1; }

	virtual LRESULT OnInsertItem( PNMCOMBOBOXEX pComboBoxEx, BOOL& bNotifyParent )
	{ return 1; }

	// Reflected notification handler which will call
	// the virtual notification handlers.
	virtual LRESULT OnReflectedNotify( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{
		// Evaluate notification code.
		switch ( pNMHDR->code )
		{
			case	CBEN_BEGINEDIT:		return OnBeginEdit( pNMHDR, bNotifyParent );
			case	CBEN_DELETEITEM:	return OnDeleteItem(( PNMCOMBOBOXEX )pNMHDR, bNotifyParent );
			case	CBEN_DRAGBEGIN:		return OnDragBegin(( LPNMCBEDRAGBEGIN )pNMHDR, bNotifyParent );
			case	CBEN_ENDEDIT:		return OnEndEdit(( PNMCBEENDEDIT )pNMHDR, bNotifyParent );
			case	CBEN_GETDISPINFO:	return OnGetDispInfo(( PNMCOMBOBOXEX )pNMHDR, bNotifyParent );
			case	CBEN_INSERTITEM:	return OnInsertItem(( PNMCOMBOBOXEX )pNMHDR, bNotifyParent );
		}
		// Call the base class.
		return ClsCommonControl::OnReflectedNotify( pNMHDR, bNotifyParent );
	}
};

#endif // _COMBOBOXEX_H