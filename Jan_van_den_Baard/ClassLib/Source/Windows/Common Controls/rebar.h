#ifndef _REBAR_H_
#define _REBAR_H_
//
// rebar.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "commoncontrol.h"

// A wrapper for the Rebar common control.
class ClsRebar : public ClsCommonControl
{
	_NO_COPY_PARAM( ClsRebar, ClsCommonControl );
public:
	// Constructor.
	ClsRebar() : ClsCommonControl( ICC_COOL_CLASSES )
	{;}

	// Destructor.
	virtual ~ClsRebar()
	{;}

	// Create a rebar control.
	inline BOOL Create( ClsWindow *parent, ClsRect& rBounds, UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS | RBS_VARHEIGHT | RBS_TOOLTIPS | RBS_BANDBORDERS )
	{
		_ASSERT( m_hWnd == NULL ); // May not be valid.

		// Create the control.
		return ClsCommonControl::Create( 0, REBARCLASSNAME, NULL, dwStyle, rBounds, parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	// Put the control in DnD mode.
	inline void BeginDrag( UINT nBand, DWORD dwPos )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( RB_BEGINDRAG, nBand, dwPos );
	}

	// Delete a band.
	inline BOOL DeleteBand( UINT nBand )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( RB_DELETEBAND, nBand );
	}

	// Update drag position.
	inline void DragMove( DWORD dwPos )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( RB_DRAGMOVE, 0, dwPos );
	}

	// End DnD mode.
	inline void EndDrag()
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( RB_ENDDRAG );
	}

	// Get the band borders.
	inline void GetBandBorders( UINT nBand, LPRECT pRect ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		::SendMessage( m_hWnd, RB_GETBANDBORDERS, nBand, ( LPARAM )pRect );
	}

	// Get the numbers of bands.
	inline UINT GetBandCount() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( UINT )::SendMessage( m_hWnd, RB_GETBANDCOUNT, 0, 0 );
	}

	// Get band info.
	inline BOOL GetBandInfo( UINT nBand, REBARBANDINFO& binfo ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, RB_GETBANDINFO, nBand, ( LPARAM )&binfo );
	}

	// Get the height of the control.
	inline UINT GetBarHeight() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( UINT )::SendMessage( m_hWnd, RB_GETBARHEIGHT, 0, 0 );
	}

	// Get the control info.
	inline BOOL GetBarInfo( REBARINFO& rbinfo ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, RB_GETBARINFO, 0, ( LPARAM )&rbinfo );
	}

	// Get background color.
	inline COLORREF GetBkColor() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( COLORREF )::SendMessage( m_hWnd, RB_GETBKCOLOR, 0, 0 );
	}

	// Get color scheme
	inline BOOL GetColorScheme( COLORSCHEME& cscheme ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, RB_GETCOLORSCHEME, 0, ( LPARAM )&cscheme );
	}

	// Get the drop target.
	inline void GetDropTarget( IDropTarget **ppDropTarget ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		::SendMessage( m_hWnd, RB_GETDROPTARGET, 0, 0 );
	}

	// Get the palette.
	inline HPALETTE GetPalette() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( HPALETTE )::SendMessage( m_hWnd, RB_GETPALETTE, 0, 0 );
	}

	// Get band bounds.
	inline BOOL GetRect( UINT nBand, LPRECT pRect ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, RB_GETRECT, nBand, ( LPARAM )pRect );
	}

	// Get the number of rows.
	inline UINT GetRowCount() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( UINT )::SendMessage( m_hWnd, RB_GETROWCOUNT, 0, 0 );
	}

	// Get the height of the row.
	inline UINT GetRowHeight( UINT nRow ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( UINT )::SendMessage( m_hWnd, RB_GETROWHEIGHT, nRow, 0 );
	}

	// Get the text color.
	inline COLORREF GetTextColor() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( COLORREF )::SendMessage( m_hWnd, RB_GETTEXTCOLOR, 0, 0 );
	}

	// Get the tooltip control.
	inline HWND GetToolTips() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( HWND )::SendMessage( m_hWnd, RB_GETTOOLTIPS, 0, 0 );
	}

	// Get the unicode flag.
	inline BOOL GetUnicodeFormat() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, RB_GETUNICODEFORMAT, 0, 0 );
	}

	// Hittest the rebar.
	inline int HitTest( RBHITTESTINFO& htinfo ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )::SendMessage( m_hWnd, RB_HITTEST, 0, ( LPARAM )&htinfo );
	}

	// Convert ID to index.
	inline int IDToIndex( UINT nID ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )::SendMessage( m_hWnd, RB_IDTOINDEX, nID, 0 );
	}

	// Insert a band.
	inline BOOL InsertBand( UINT nIndex, REBARBANDINFO& bandinfo )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( RB_INSERTBAND, nIndex, ( LPARAM )&bandinfo );
	}

	// Maximize a band.
	inline void MaximizeBand( UINT nBand, BOOL bIdeal = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( RB_MAXIMIZEBAND, nBand, bIdeal );
	}

	// Minimize a band.
	inline void MinimizeBand( UINT nBand )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( RB_MINIMIZEBAND, nBand );
	}

	// Move a band.
	inline BOOL MoveBand( UINT nFrom, UINT nTo )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( RB_MOVEBAND, nFrom, nTo );
	}

	// Set band info.
	inline BOOL SetBandInfo( UINT nBand, REBARBANDINFO& rbinfo )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( RB_SETBANDINFO, nBand, ( LPARAM )&rbinfo );
	}

	// Set bar info.
	inline BOOL SetBarInfo( REBARINFO& rinfo )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( RB_SETBARINFO, 0, ( LPARAM )&rinfo );
	}

	// Set background color.
	inline COLORREF SetBkColor( COLORREF crColor )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( COLORREF )SendMessage( RB_SETBKCOLOR, 0, crColor );
	}

	// Set color scheme.
	inline void SetColorScheme( COLORSCHEME& cscheme )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( RB_SETCOLORSCHEME, 0, ( LPARAM )&cscheme );
	}

	// Set the palette.
	inline HPALETTE SetPalette( HPALETTE hPalette )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( HPALETTE )SendMessage( RB_SETPALETTE, 0, ( LPARAM )hPalette );
	}

	// Set the parent window.
	inline HWND SetNotifyParent( HWND hWndParent )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( HWND )SendMessage( RB_SETPARENT, ( WPARAM )hWndParent );
	}

	// Set the text color.
	inline COLORREF SetTextColor( COLORREF crColor )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( COLORREF )SendMessage( RB_SETTEXTCOLOR, 0, crColor );
	}

	// Set the tooltip control.
	inline void SetToolTips( HWND hWndTooltip )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( RB_SETTOOLTIPS, ( WPARAM )hWndTooltip );
	}

	// Set the unicode format.
	inline BOOL SetUnicodeFormat( BOOL bFormat )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( RB_SETUNICODEFORMAT, bFormat );
	}

	// Show or hide a band.
	inline BOOL ShowBand( UINT nBand, BOOL bShow = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( RB_SHOWBAND, nBand, bShow );
	}

	// Size the rebar.
	inline BOOL SizeToRect( LPRECT pRect )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( RB_SIZETORECT, 0, ( LPARAM )pRect );
	}
protected:
	// Overidables.
	virtual LRESULT OnAutoSize( LPNMRBAUTOSIZE pAutoSize, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnBeginDrag( LPNMREBAR pRebar, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnChildSize( LPNMREBARCHILDSIZE pChildSize, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnDeletedBand( LPNMREBAR pRebar, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnDeletingBand( LPNMREBAR pRebar, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnEndDrag( LPNMREBAR pRebar, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnGetObject( LPNMOBJECTNOTIFY pObject, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnHeightChange( LPNMREBAR pRebar, BOOL& bNotifyParent )
	{ return 0; }

	virtual LRESULT OnLayoutChanged( LPNMREBAR pRebar, BOOL& bNotifyParent )
	{ return 0; }

	// Reflected notification handler which will call
	// the virtual notification handlers.
	virtual LRESULT OnReflectedNotify( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{
		// Evaluate notification code.
		switch ( pNMHDR->code )
		{
			case	RBN_AUTOSIZE:      return OnAutoSize(( LPNMRBAUTOSIZE )pNMHDR, bNotifyParent );
			case	RBN_BEGINDRAG:     return OnBeginDrag(( LPNMREBAR )pNMHDR, bNotifyParent );
			case	RBN_CHILDSIZE:     return OnChildSize(( LPNMREBARCHILDSIZE )pNMHDR, bNotifyParent );
			case	RBN_DELETEDBAND:   return OnDeletedBand(( LPNMREBAR )pNMHDR, bNotifyParent );
			case	RBN_DELETINGBAND:  return OnDeletingBand(( LPNMREBAR )pNMHDR, bNotifyParent );
			case	RBN_ENDDRAG:	   return OnEndDrag(( LPNMREBAR )pNMHDR, bNotifyParent );
			case	RBN_GETOBJECT:	   return OnGetObject(( LPNMOBJECTNOTIFY )pNMHDR, bNotifyParent );
			case	RBN_HEIGHTCHANGE:  return OnHeightChange(( LPNMREBAR )pNMHDR, bNotifyParent );
			case	RBN_LAYOUTCHANGED: return OnLayoutChanged(( LPNMREBAR )pNMHDR, bNotifyParent );
		}
		// Call the base class.
		return ClsCommonControl::OnReflectedNotify( pNMHDR, bNotifyParent );
	}
};

#endif // _REBAR_H_