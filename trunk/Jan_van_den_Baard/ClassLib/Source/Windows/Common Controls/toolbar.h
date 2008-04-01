#ifndef _TOOLBAR_H_
#define _TOOLBAR_H_
//
// toolbar.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "commoncontrol.h"

// A wrapper for the Toolbar common control.
class ClsToolbar : public ClsCommonControl
{
	_NO_COPY_PARAM( ClsToolbar, ClsCommonControl );
public:
	// Constructor.
	ClsToolbar() : ClsCommonControl( ICC_WIN95_CLASSES )
	{;}

	// Destructor.
	virtual ~ClsToolbar()
	{;}

	// Create a toolbar control.
	inline BOOL Create( ClsWindow *parent, ClsRect& rBounds, UINT nID, DWORD dwStyle = WS_CHILD | WS_BORDER | WS_VISIBLE | CCS_TOP | TBSTYLE_TOOLTIPS | TBSTYLE_WRAPABLE )
	{
		_ASSERT( m_hWnd == NULL ); // May not be valid.

		// Create the control.
		return ClsCommonControl::Create( 0, TOOLBARCLASSNAME, NULL, dwStyle, rBounds, parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	// Add a bitmaps.
	inline int AddBitmap( int nBitmaps, LPTBADDBITMAP pBitmaps )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )SendMessage( TB_ADDBITMAP, nBitmaps, ( LPARAM )pBitmaps );
	}

	// Add buttons.
	inline BOOL AddButtons( int nButtons, LPTBBUTTON pButtons )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_ADDBUTTONS, nButtons, ( LPARAM )pButtons );
	}

	// Add a string.
	inline int AddString( UINT nIDString, HINSTANCE hInst = NULL )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )SendMessage( TB_ADDSTRING, ( WPARAM )( hInst == NULL ? ClsGetResourceHandle() : hInst ), MAKELONG( nIDString, 0 ));
	}

	// Add a string.
	inline int AddString( LPSTR pszString )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )SendMessage( TB_ADDSTRING, NULL, ( LPARAM )pszString );
	}

	// Auto-size the toolbar.
	inline void AutoSize()
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TB_AUTOSIZE );
	}

	// Get the number of buttons.
	inline int ButtonCount() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )::SendMessage( m_hWnd, TB_BUTTONCOUNT, 0, 0 );
	}

	// Set the size of the button struct.
	inline void ButtonStructSize( int cb )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TB_BUTTONSTRUCTSIZE, cb );
	}

	// Change a button bitmap.
	inline BOOL ChangeBitmap( UINT idButton, UINT idBitmap )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_CHANGEBITMAP, idButton, MAKELPARAM( idBitmap, 0 ));
	}

	// Check a button.
	inline BOOL CheckButton( UINT idButton, BOOL bCheck = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_CHECKBUTTON, idButton, MAKELONG( bCheck, 0 ));
	}

	// Convert command ID to index.
	inline int CommandToIndex( UINT idButton ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )::SendMessage( m_hWnd, TB_COMMANDTOINDEX, idButton, 0 );
	}

	// Display the Customize dialog.
	inline void Customize()
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TB_CUSTOMIZE );
	}

	// Delete a button.
	inline BOOL DeleteButton( int iButton )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_DELETEBUTTON, iButton );
	}

	// Enable or disable a button.
	inline BOOL EnableButton( UINT idButton, BOOL bEnable = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_ENABLEBUTTON, idButton, MAKELONG( bEnable, 0 ));
	}

	// Get anchor highlighting flag.
	inline BOOL GetAnchorHighlight() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, TB_GETANCHORHIGHLIGHT, 0, 0 );
	}

	// Get bitmap index of a button.
	inline int GetBitmap( UINT idButton ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )::SendMessage( m_hWnd, TB_GETBITMAP, idButton, 0 );
	}

	// Get bitmap flags.
	inline DWORD GetBitmapFlags() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( DWORD )::SendMessage( m_hWnd, TB_GETBITMAPFLAGS, 0, 0 );
	}

	// Get button information.
	inline BOOL GetButton( int iButton, TBBUTTON& button ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, TB_GETBUTTON, iButton, ( LPARAM )&button );
	}

	// Get extended button information.
	inline int GetButtonInfo( UINT idButton, TBBUTTONINFO& button ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )::SendMessage( m_hWnd, TB_GETBUTTONINFO, idButton, ( LPARAM )&button );
	}

	// Get button size.
	inline DWORD GetButtonSize() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( DWORD )::SendMessage( m_hWnd, TB_GETBUTTONSIZE, 0, 0 );
	}

	// Get button text.
	inline UINT GetButtonText( UINT idButton, LPSTR pszString ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( UINT )::SendMessage( m_hWnd, TB_GETBUTTONTEXT, idButton, ( LPARAM )pszString );
	}

	// Get color scheme.
	inline BOOL GetColorScheme( COLORSCHEME& cscheme ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, TB_GETCOLORSCHEME, 0, ( LPARAM )&cscheme );
	}

	// Get the disabled image list.
	inline HIMAGELIST GetDisabledImageList() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( HIMAGELIST )::SendMessage( m_hWnd, TB_GETDISABLEDIMAGELIST, 0, 0 );
	}

	// Get the extended style bits.
	inline DWORD GetExtendedStyle() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( DWORD )::SendMessage( m_hWnd, TB_GETEXTENDEDSTYLE, 0, 0 );
	}

	// Get the hot image list.
	inline HIMAGELIST GetHotImageList() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( HIMAGELIST )::SendMessage( m_hWnd, TB_GETHOTIMAGELIST, 0, 0 );
	}

	// Get the hot item.
	inline int GetHotItem() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )::SendMessage( m_hWnd, TB_GETHOTITEM, 0, 0 );
	}

	// Get the image list.
	inline HIMAGELIST GetImageList() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( HIMAGELIST )::SendMessage( m_hWnd, TB_GETIMAGELIST, 0, 0 );
	}

	// Get the insertion mark.
	inline BOOL GetInsertMark( TBINSERTMARK& mark ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return  ( BOOL )::SendMessage( m_hWnd, TB_GETINSERTMARK, 0, ( LPARAM )&mark );
	}

	// Get the insert mark color.
	inline COLORREF GetInsertMarkColor() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( COLORREF )::SendMessage( m_hWnd, TB_GETINSERTMARKCOLOR, 0, 0 );
	}

	// Get button bounds.
	inline BOOL GetItemRect( int iButton, LPRECT pRect ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, TB_GETITEMRECT, iButton, ( LPARAM )pRect );
	}

	// Get the size of all visible buttons.
	inline BOOL GetMaxSize( LPSIZE pSize ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, TB_GETMAXSIZE, 0, ( LPARAM )pSize );
	}

	// Get the drop target.
	inline HRESULT GetObject( REFIID iid, LPVOID *ppvObject )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( HRESULT )::SendMessage( m_hWnd, TB_GETOBJECT, ( WPARAM )&iid, ( LPARAM )ppvObject );
	}

	// Get button padding.
	inline DWORD GetPadding() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( DWORD )::SendMessage( m_hWnd, TB_GETPADDING, 0, 0 );
	}

	// Get button bounds.
	inline BOOL GetRect( UINT idButton, LPRECT pRect ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, TB_GETRECT, idButton, ( LPARAM )pRect );
	}

	// Get the number of rows.
	inline int GetRows() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )::SendMessage( m_hWnd, TB_GETROWS, 0, 0 );
	}

	// Get the button state.
	inline DWORD GetState( UINT idButton ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( DWORD )::SendMessage( m_hWnd, TB_GETSTATE, idButton, 0 );
	}

	// Get the control styles.
	inline DWORD GetStyle() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( DWORD )::SendMessage( m_hWnd, TB_GETSTYLE, 0, 0 );
	}

	// Get the number of text rows that can be displayed.
	inline int GetTextRows() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )::SendMessage( m_hWnd, TB_GETTEXTROWS, 0, 0 );
	}

	// Get the tooltip control.
	inline HWND GetToolTips() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( HWND )::SendMessage( m_hWnd, TB_GETTOOLTIPS, 0, 0 );
	}

	// Get the unicode flag.
	inline BOOL GetUnicodeFormat() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, TB_GETUNICODEFORMAT, 0, 0 );
	}

	// Show or hide a button.
	inline BOOL HideButton( UINT idButton, BOOL bHide = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_HIDEBUTTON, idButton, MAKELONG( bHide, 0 ));
	}

	// Hittest the control.
	inline int HitTest( LPPOINT pPoint ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )::SendMessage( m_hWnd, TB_HITTEST, 0, ( LPARAM )pPoint );
	}

	// Indeterminate a button.
	inline BOOL Indeterminate( UINT idButton, BOOL bIndeter = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_INDETERMINATE, idButton, MAKELONG( bIndeter, 0 ));
	}

	// Insert a button.
	inline BOOL InsertButton( int iButton, TBBUTTON& button )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_INSERTBUTTON, iButton, ( LPARAM )&button );
	}

	// Get insertion poiint.
	inline BOOL InsertMarkHitTest( LPPOINT pPoint, LPTBINSERTMARK pInsertMark ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, TB_INSERTMARKHITTEST, ( WPARAM )pPoint, ( LPARAM )pInsertMark );
	}

	// Is the button checked?
	inline BOOL IsButtonChecked( UINT idButton ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, TB_ISBUTTONCHECKED, idButton, 0 );
	}

	// Is the button enabled?
	inline BOOL IsButtonEnabled( UINT idButton ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, TB_ISBUTTONENABLED, idButton, 0 );
	}

	// Is the button hidden?
	inline BOOL IsButtonHidden( UINT idButton ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, TB_ISBUTTONHIDDEN, idButton, 0 );
	}

	// Is the button highlighted.
	inline BOOL IsButtonHighlighted( UINT idButton ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, TB_ISBUTTONHIGHLIGHTED, idButton, 0 );
	}

	// Is the button indeterminate.
	inline BOOL IsButtonIndeterminate( UINT idButton ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, TB_ISBUTTONINDETERMINATE, idButton, 0 );
	}

	// Is the button pressed?
	inline BOOL IsButtonPressed( UINT idButton ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, TB_ISBUTTONPRESSED, idButton, 0 );
	}

	// Load images.
	inline int LoadImages( UINT nBitmapID, HINSTANCE hInst = NULL )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )SendMessage( TB_LOADIMAGES, nBitmapID, ( LPARAM )( hInst == NULL ? ClsGetResourceHandle() : hInst ));
	}

	// Map accelerator character.
	inline BOOL MapAccelerator( TCHAR chAccel, LPUINT pIDButton ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, TB_MAPACCELERATOR, chAccel, ( LPARAM )pIDButton );
	}

	// Highlight a button.
	inline BOOL MarkButton( UINT idButton, BOOL bMark = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_MARKBUTTON, idButton, MAKELONG( bMark, 0 ));
	}

	// Move a button.
	inline BOOL MoveButton( UINT nOldPos, UINT nNewPos )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_MOVEBUTTON, nOldPos, nNewPos );
	}

	// Press a button.
	inline BOOL PressButton( UINT idButton, BOOL bPress )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_PRESSBUTTON, idButton, MAKELONG( bPress, 0 ));
	}

	// Replace existing bitmap.
	inline BOOL ReplaceBitmap( TBREPLACEBITMAP& rpbitmap )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_REPLACEBITMAP, 0, ( LPARAM )&rpbitmap );
	}

	// Save or restore toolbar state.
	inline void SaveRestore( BOOL bSave, TBSAVEPARAMS& saveparams )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TB_SAVERESTORE, bSave, ( LPARAM )&saveparams );
	}

	// Set anchor highlighting flag.
	inline BOOL SetAnchorHighlight( BOOL bHighlight )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_SETANCHORHIGHLIGHT, bHighlight );
	}

	// Set the bitmap size.
	inline BOOL SetBitmapSize( int cx, int cy )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_SETBITMAPSIZE, 0, MAKELONG( cx, cy ));
	}

	// Set button info.
	inline BOOL SetButtonInfo( UINT idButton, TBBUTTONINFO& button )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_SETBUTTONINFO, idButton, ( LPARAM )&button );
	}

	// Set button size.
	inline BOOL SetButtonSize( int cx, int cy )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_SETBUTTONSIZE, 0, MAKELONG( cx, cy ));
	}

	// Set minimum and maximum button width.
	inline BOOL SetButtonWidth( int cxMin, int cxMax )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_SETBUTTONWIDTH, 0, MAKELONG( cxMin, cxMax ));
	}

	// Set button identifier.
	inline BOOL SetCmdID( int iButton, UINT idButton )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_SETCMDID, iButton, idButton );
	}

	// Set the color scheme.
	inline void SetColorScheme( COLORSCHEME& cscheme )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TB_SETCOLORSCHEME, 0, ( LPARAM )&cscheme );
	}

	// Set the disabled image list.
	inline HIMAGELIST SetDisabledImageList( HIMAGELIST hImageList )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( HIMAGELIST )SendMessage( TB_SETDISABLEDIMAGELIST, 0, ( LPARAM )hImageList );
	}

	// Set the DrawText() flags.
	inline DWORD SetDrawTextFlags( DWORD dwMask, DWORD dwFlags )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( DWORD )SendMessage( TB_SETDRAWTEXTFLAGS, dwMask, dwFlags );
	}

	// Set the extended style bits.
	inline DWORD SetExtendedStyle( DWORD dwExStyle )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( DWORD )SendMessage( TB_SETEXTENDEDSTYLE, 0, dwExStyle );
	}

	// Set the hot image list.
	inline HIMAGELIST SetHotImageList( HIMAGELIST hImageList )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( HIMAGELIST )SendMessage( TB_SETHOTIMAGELIST, 0, ( LPARAM )hImageList );
	}

	// Set the hot item.
	inline int SetHotItem( int iItem )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )SendMessage( TB_SETHOTITEM, iItem );
	}

	// Set the imagelist.
	inline HIMAGELIST SetImageList( HIMAGELIST hImageList )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( HIMAGELIST )SendMessage( TB_SETIMAGELIST, 0, ( LPARAM )hImageList );
	}

	// Set the firts button indention.
	inline BOOL SetIndent( int iIndent )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_SETINDENT, iIndent );
	}

	// Set insertion mark.
	inline void SetInsertMark( TBINSERTMARK& mark )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TB_SETINSERTMARK, 0, ( LPARAM )&mark );
	}

	// Set the insert mark color.
	inline COLORREF SetInsertMarkColor( COLORREF crColor )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( COLORREF )SendMessage( TB_SETINSERTMARKCOLOR, 0, crColor );
	}

	// Set the maximum text rows.
	inline BOOL SetMaxTextRows( int iRows )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_SETMAXTEXTROWS, iRows );
	}

	// Set button padding.
	inline DWORD SetPadding( int cx, int cy )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( DWORD )SendMessage( TB_SETPADDING, 0, MAKELONG( cx, cy ));
	}

	// Set notification parent.
	inline HWND SetNotifyParent( HWND hWndParent )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( HWND )SendMessage( TB_SETPARENT, ( WPARAM )hWndParent );
	}

	// Set the number of rows in the toolbar.
	inline void SetRows( int nRows, BOOL bLarger, LPRECT pRect )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TB_SETROWS, MAKEWPARAM( nRows, bLarger ), ( LPARAM )pRect );
	}

	// Set button state.
	inline BOOL SetState( UINT idButton, WORD wFlags )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_SETSTATE, idButton, MAKELONG( wFlags, 0 ));
	}

	// Set the toolbar style.
	inline void SetStyle( DWORD dwStyle )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TB_SETSTYLE, 0, dwStyle );
	}

	// Set the tooltip control.
	inline void SetToolTips( HWND hWndTooltip )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TB_SETTOOLTIPS, ( WPARAM )hWndTooltip );
	}

	// Set the unicode flag.
	inline BOOL SetUnicodeFormat( BOOL bUnicode )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TB_SETUNICODEFORMAT, bUnicode );
	}
protected:
	// Overidables.
	virtual LRESULT OnBeginAdjust( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnBeginDrag( LPNMTOOLBAR pToolbar, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnCustHelp( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnDeletingButton( LPNMTOOLBAR pToolbar, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnDragOut( LPNMTOOLBAR pToolbar, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnDropDown( LPNMTOOLBAR pToolbar, BOOL& bNotifyParent )
	{ return TBDDRET_DEFAULT; }

	virtual LRESULT OnEndAdjust( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnEndDrag( LPNMTOOLBAR pToolbar, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnGetButtonInfo( LPNMTOOLBAR pToolbar, BOOL& bNotifyParent )
	{ return FALSE; }

	virtual LRESULT OnGetDispInfo( LPNMTBDISPINFO pDispInfo, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnGetInfoTip( LPNMTBGETINFOTIP pInfoTip, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnGetObject( LPNMOBJECTNOTIFY pObjectNotify, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnHotItemChange( LPNMTBHOTITEM pHotItem, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnQueryDelete( LPNMTOOLBAR pToolbar, BOOL& bNotifyParent )
	{ return TRUE; }

	virtual LRESULT OnQueryInsert( LPNMTOOLBAR pToolbar, BOOL& bNotifyParent )
	{ return TRUE; }

	virtual LRESULT OnReset( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnToolbarChange( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{ return 0; }

	// Reflected notification handler which will call
	// the virtual notification handlers.
	virtual LRESULT OnReflectedNotify( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{
		// Evaluate notification code.
		switch ( pNMHDR->code )
		{
			case	TBN_BEGINADJUST:    return OnBeginAdjust( pNMHDR, bNotifyParent );
			case	TBN_BEGINDRAG:	    return OnBeginDrag(( LPNMTOOLBAR )pNMHDR, bNotifyParent );
			case	TBN_CUSTHELP:	    return OnCustHelp( pNMHDR, bNotifyParent );
			case	TBN_DELETINGBUTTON: return OnDeletingButton(( LPNMTOOLBAR )pNMHDR, bNotifyParent );
			case	TBN_DRAGOUT:	    return OnDragOut(( LPNMTOOLBAR )pNMHDR, bNotifyParent );
			case	TBN_DROPDOWN:	    return OnDropDown(( LPNMTOOLBAR )pNMHDR, bNotifyParent );
			case	TBN_ENDADJUST:	    return OnEndAdjust( pNMHDR, bNotifyParent );
			case	TBN_ENDDRAG:	    return OnEndDrag(( LPNMTOOLBAR )pNMHDR, bNotifyParent );
			case	TBN_GETBUTTONINFO:  return OnGetButtonInfo(( LPNMTOOLBAR )pNMHDR, bNotifyParent );
			case	TBN_GETDISPINFO:    return OnGetDispInfo(( LPNMTBDISPINFO )pNMHDR, bNotifyParent );
			case	TBN_GETINFOTIP:     return OnGetInfoTip(( LPNMTBGETINFOTIP )pNMHDR, bNotifyParent );
			case	TBN_GETOBJECT:      return OnGetObject(( LPNMOBJECTNOTIFY )pNMHDR, bNotifyParent );
			case	TBN_HOTITEMCHANGE:  return OnHotItemChange(( LPNMTBHOTITEM )pNMHDR, bNotifyParent );
			case	TBN_QUERYDELETE:    return OnQueryDelete(( LPNMTOOLBAR )pNMHDR, bNotifyParent );
			case	TBN_QUERYINSERT:    return OnQueryInsert(( LPNMTOOLBAR )pNMHDR, bNotifyParent );
			case	TBN_RESET:	    return OnReset( pNMHDR, bNotifyParent );
			case	TBN_TOOLBARCHANGE:  return OnToolbarChange( pNMHDR, bNotifyParent );
		}
		// Call the base class.
		return ClsCommonControl::OnReflectedNotify( pNMHDR, bNotifyParent );
	}
};

#endif // _TOOLBAR_H_