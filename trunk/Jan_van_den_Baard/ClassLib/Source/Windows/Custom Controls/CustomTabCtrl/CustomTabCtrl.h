#ifndef __CUSTOMTABCTRL_H__
#define __CUSTOMTABCTRL_H__

/////////////////////////////////////////////////////////////////////////////
// CCustomTabCtrl - A base class to help implement
//   Tab Controls with different appearances
//
// Original work by Bjarke Viksoe (bjarke@viksoe.dk)
// Revised version by Daniel Bowen (dbowen@es.com).
// Copyright (c) 2001-2002 Bjarke Viksoe.
// Copyright (c) 2002-2004 Daniel Bowen.
//
// ClassLib version by Jan van den Baard
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. 
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//
// History (Date/Author/Description):
// ----------------------------------
//
// 2004/12/25: Jan van den Baard
// - First implementation for the ClassLib library.
// - Adjusted class names to fit the names of the classes in ClassLib. This 
//   basically means the "C" prefixes at the start of the class names are 
//   replaced by "Cls".
// - Adjusted indentation etc. a bit more to my style, This is not a snear
//   to the style of the original authors. It is just my way to force me to
//   read all the code so that I can understand it better.
//
// 2004/12/26: Jan van den Baard
// - Dragging will no longer be initiated when there is only one tab present.
//

#include "../../../standard.h"
#include "../../../strings/string.h"
#include "../../Window.h"
#include "../../Common Controls/imagelist.h"
#include "../../Common Controls/tooltip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
#define THIS_FILE __FILE__
#endif

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lParam)	((int)(short)LOWORD(lParam))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lParam)	((int)(short)HIWORD(lParam))
#endif

// Window styles:
// NOTE: "CTCS" stands for "Custom tab control style"
#define CTCS_SCROLL              0x0001   // TCS_SCROLLOPPOSITE
#define CTCS_BOTTOM              0x0002   // TCS_BOTTOM
//#define CTCS_RIGHT               0x0002   // TCS_RIGHT
//#define CTCS_MULTISELECT         0x0004   // TCS_MULTISELECT
#define CTCS_CLOSEBUTTON         0x0008   // TCS_FLATBUTTONS
//#define CTCS_FORCEICONLEFT       0x0010   // TCS_FORCEICONLEFT
//#define CTCS_FORCELABELLEFT      0x0020   // TCS_FORCELABELLEFT
#define CTCS_HOTTRACK            0x0040   // TCS_HOTTRACK
//#define CTCS_VERTICAL            0x0080   // TCS_VERTICAL
//#define CTCS_TABS                0x0000   // TCS_TABS
#define CTCS_FLATEDGE            0x0100   // TCS_BUTTONS
//#define CTCS_SINGLELINE          0x0000   // TCS_SINGLELINE
//#define CTCS_MULTILINE           0x0200   // TCS_MULTILINE
//#define CTCS_RIGHTJUSTIFY        0x0000   // TCS_RIGHTJUSTIFY
#define CTCS_DRAGREARRANGE       0x0400   // TCS_FIXEDWIDTH
//#define CTCS_OLEDRAGDROP         0x0800   // TCS_RAGGEDRIGHT
//#define CTCS_FOCUSONBUTTONDOWN   0x1000   // TCS_FOCUSONBUTTONDOWN
#define CTCS_BOLDSELECTEDTAB     0x2000   // TCS_OWNERDRAWFIXED
#define CTCS_TOOLTIPS            0x4000   // TCS_TOOLTIPS
//#define CTCS_FOCUSNEVER          0x8000   // TCS_FOCUSNEVER

// Notifications:

#define CTCN_FIRST              (0U-550U)           // TCN_FIRST
#define CTCN_LAST               (0U-580U)           // TCN_LAST

#define CTCN_SELCHANGE          (TCN_FIRST - 1)     // TCN_SELCHANGE
#define CTCN_SELCHANGING        (TCN_FIRST - 2)     // TCN_SELCHANGING
//#define CTCN_GETOBJECT          (TCN_FIRST - 3)     // TCN_GETOBJECT
//#define CTCN_FOCUSCHANGE        (TCN_FIRST - 4)     // TCN_FOCUSCHANGE
//#define CTCN_INITIALIZE         (TCN_FIRST - 10)    // obsolete for now
#define CTCN_INSERTITEM         (TCN_FIRST - 11)
#define CTCN_DELETEITEM         (TCN_FIRST - 12)
#define CTCN_MOVEITEM           (TCN_FIRST - 13)
#define CTCN_SWAPITEMPOSITIONS  (TCN_FIRST - 14)
#define CTCN_CLOSE              (TCN_FIRST - 15)
#define CTCN_BEGINITEMDRAG      (TCN_FIRST - 21)
#define CTCN_ACCEPTITEMDRAG     (TCN_FIRST - 22)
#define CTCN_CANCELITEMDRAG     (TCN_FIRST - 23)
#define CTCN_MCLICK             (TCN_FIRST - 24)
#define CTCN_MDBLCLK            (TCN_FIRST - 25)

// Hit Test codes
#define CTCHT_NOWHERE            0x0001             // TCHT_NOWHERE
#define CTCHT_ONITEMICON         0x0002             // TCHT_ONITEMICON
#define CTCHT_ONITEMLABEL        0x0004             // TCHT_ONITEMLABEL
#define CTCHT_ONITEM             (CTCHT_ONITEMICON | CTCHT_ONITEMLABEL)
#define CTCHT_ONCLOSEBTN         0x0010
#define CTCHT_ONSCROLLRIGHTBTN   0x0020
#define CTCHT_ONSCROLLLEFTBTN    0x0040

// Find Item flags
#define CTFI_NONE                0x0000
#define CTFI_RECT                0x0001
#define CTFI_IMAGE               0x0002
#define CTFI_TEXT                0x0004
#define CTFI_TOOLTIP             0x0008
#define CTFI_TABVIEW             0x0010
#define CTFI_HIGHLIGHTED         0x0020
#define CTFI_CANCLOSE            0x0040
#define CTFI_LAST                CTFI_CANCLOSE
#define CTFI_ALL                 0xFFFF

// Number of milliseconds for scroll repeat
#ifndef CTCSR_NONE
#define CTCSR_NONE           0
#endif
#ifndef CTCSR_SLOW
#define CTCSR_SLOW           100
#endif
#ifndef CTCSR_NORMAL
#define CTCSR_NORMAL         25
#endif
#ifndef CTCSR_FAST
#define CTCSR_FAST           10
#endif

// Drag and drop related constant
#ifndef CTCD_SCROLLZONEWIDTH
#define CTCD_SCROLLZONEWIDTH 20
#endif

// Structures
typedef struct tagNMCTCITEM
{
	NMHDR   hdr;
	int     iItem;  // Item Index
	POINT   pt;     // Client Coordinates
} NMCTCITEM, *LPNMCTCITEM;

typedef struct tagNMCTC2ITEMS
{
	NMHDR   hdr;
	int     iItem1;  // First Item Index
	int     iItem2;  // Second Item Index
	POINT   pt;      // Client Coordinates
} NMCTC2ITEMS, *LPNMCTC2ITEMS;

typedef struct tagCTCHITTESTINFO
{
	POINT pt;        // Client Coordinates of point to test
	UINT flags;
} CTCHITTESTINFO, *LPCTCHITTESTINFO;

typedef struct tagNMCTCCUSTOMDRAW
{
	NMCUSTOMDRAW nmcd;
	HFONT hFontInactive;
	HFONT hFontSelected;
	HBRUSH hBrushBackground;
	COLORREF clrTextInactive;
	COLORREF clrTextSelected;
	COLORREF clrSelectedTab;
	COLORREF clrBtnFace;
	COLORREF clrBtnShadow;
	COLORREF clrBtnHighlight;
	COLORREF clrBtnText;
	COLORREF clrHighlight;
	COLORREF clrHighlightHotTrack;
	COLORREF clrHighlightText;
} NMCTCCUSTOMDRAW, FAR * LPNMCTCCUSTOMDRAW;

typedef struct tagCTCSETTINGS
{
	signed char iPadding;
	signed char iMargin;
	signed char iSelMargin;
	signed char iIndent;
} CTCSETTINGS;

// Tab Item classes
class ClsCustomTabItem
{
	// Constructors/Destructors
public:
	ClsCustomTabItem() :
		m_nImage( -1 ),
		m_bHighlighted( false ),
		m_bCanClose( true )
	{
		::SetRectEmpty( &m_rcItem );
	}

	ClsCustomTabItem( const ClsCustomTabItem& rhs )
	{
		*this = rhs;
	}

	virtual ~ClsCustomTabItem()
	{
	}

	const ClsCustomTabItem& operator=( const ClsCustomTabItem& rhs )
	{
		if( &rhs != this )
		{
		      m_rcItem        = rhs.m_rcItem;
		      m_nImage        = rhs.m_nImage;
		      m_sText         = rhs.m_sText;
		      m_sToolTip      = rhs.m_sToolTip;
		      m_bHighlighted  = rhs.m_bHighlighted;
		      m_bCanClose     = rhs.m_bCanClose;
		}
		return *this;
	}

	// Accessors
	RECT GetRect() const				{ return m_rcItem; }
	LPCRECT GetRectRef() const			{ return &m_rcItem; }
	bool SetRect( RECT rcItem )			{ m_rcItem = rcItem; return true; }
	int GetImageIndex() const			{ return m_nImage; }
	bool SetImageIndex( int nImage = -1 )		{ m_nImage = nImage; return true; }
	ClsString GetText() const			{ return m_sText; }
	LPCTSTR GetTextRef() const			{ return ( LPCTSTR )m_sText; }
	bool SetText( LPCTSTR sNewText )		{ m_sText = sNewText ? sNewText : _T( "" ); return true; }
	ClsString GetToolTip() const			{ return m_sToolTip; }
	LPCTSTR GetToolTipRef() const			{ return ( LPCTSTR )m_sToolTip; }
	bool SetToolTip( LPCTSTR sNewText )		{ m_sToolTip = sNewText ? sNewText : _T( "" ); return true; }
	bool IsHighlighted() const			{ return m_bHighlighted; }
	bool SetHighlighted( bool bHighlighted )	{ m_bHighlighted = bHighlighted; return true; }
	bool CanClose() const				{ return m_bCanClose; }
	bool SetCanClose( bool bCanClose )		{ m_bCanClose = bCanClose; return true;	}

	// Methods:
	bool UsingImage() const				{ return ( m_nImage >= 0 ); }
	bool UsingText() const				{ return ( m_sText.GetStringLength() > 0 ); }
	bool UsingToolTip() const			{ return ( m_sToolTip.GetStringLength() > 0 ); }
	BOOL InflateRect( int dx, int dy )		{ return ::InflateRect( &m_rcItem, dx, dy ); }

	bool MatchItem( ClsCustomTabItem* pItem, DWORD eFlags ) const	
	{ 
		bool bMatch = true;
		if( bMatch && ( eFlags & CTFI_RECT ) == CTFI_RECT )
		{
			bMatch = ( TRUE == ::EqualRect( &m_rcItem, &pItem->m_rcItem )) ? true : false;
		}

		if( bMatch && ( eFlags & CTFI_IMAGE ) == CTFI_IMAGE )
		{
			bMatch = ( m_nImage == pItem->m_nImage )  ? true : false;
		}

		if( bMatch && ( eFlags & CTFI_TEXT ) == CTFI_TEXT )
		{
			bMatch = ( m_sText == pItem->m_sText )  ? true : false;
		}

		if( bMatch && ( eFlags & CTFI_TOOLTIP ) == CTFI_TOOLTIP )
		{
			bMatch = ( m_sToolTip == pItem->m_sToolTip )  ? true : false;
		}

		if( bMatch && ( eFlags & CTFI_HIGHLIGHTED ) == CTFI_HIGHLIGHTED )
		{
			bMatch = ( m_bHighlighted == pItem->m_bHighlighted )  ? true : false;
		}

		if( bMatch && ( eFlags & CTFI_CANCLOSE ) == CTFI_CANCLOSE )
		{
			bMatch = ( m_bCanClose == pItem->m_bCanClose )  ? true : false;
		}

		if( bMatch )
		{
			*pItem = *this;
		}

		return bMatch;
	}

	// Member variables
protected:
	RECT		m_rcItem;
	int		m_nImage;
	ClsString	m_sText;
	ClsString	m_sToolTip;
	bool		m_bHighlighted;
	bool		m_bCanClose;
};

// Derived Tab Item class that supports an HWND identifying a "tab view"
class ClsTabViewTabItem : public ClsCustomTabItem
{
protected:
	typedef ClsCustomTabItem baseClass;
	// Constructors/Destructors
public:
	ClsTabViewTabItem() : m_hWndTabView( NULL ) {;}
	ClsTabViewTabItem( const ClsTabViewTabItem& rhs ) { *this = rhs; }
	virtual ~ClsTabViewTabItem() {;}

        const ClsTabViewTabItem& operator=( const ClsTabViewTabItem& rhs )
	{
		if( &rhs != this )
		{
			m_rcItem        = rhs.m_rcItem;
			m_nImage        = rhs.m_nImage;
			m_sText         = rhs.m_sText;
			m_sToolTip      = rhs.m_sToolTip;
			m_bHighlighted  = rhs.m_bHighlighted;
			m_bCanClose     = rhs.m_bCanClose;
			m_hWndTabView   = rhs.m_hWndTabView;
		}
		return *this;
	}

	// Accessors
	HWND GetTabView() const				{ return m_hWndTabView;	}
	bool SetTabView( HWND hWnd = NULL )		{ m_hWndTabView = hWnd;	return true; }

	// Methods:
	bool UsingTabView() const			{ return ( m_hWndTabView != NULL ); }

	bool MatchItem( ClsTabViewTabItem* pItem, DWORD eFlags ) const
	{
		bool bMatch = true;
		if( eFlags == CTFI_TABVIEW )
		{
			// Make the common case a little faster
			// (searching only for a match to the "tab view" HWND)
			bMatch = ( m_hWndTabView == pItem->m_hWndTabView )  ? true : false;
		}
		else
		{
			// Do an extensive comparison
			bMatch = baseClass::MatchItem( pItem, eFlags );
			if( bMatch && ( eFlags & CTFI_TABVIEW ) == CTFI_TABVIEW )
			{
				bMatch = ( m_hWndTabView == pItem->m_hWndTabView )  ? true : false;
			}
		}

		if( bMatch )
		{
			*pItem = *this;
		}

		return bMatch;
	}

	// Member variables (in addition to ClsCustomTabItem ones)
protected:
	HWND		m_hWndTabView;
};

template < class TItem >
class ClsCustomTabCtrl : public ClsWindow
{
	_NO_COPY( ClsCustomTabCtrl );
protected:
	typedef ClsWindow	baseClass;
	//typedef CWindowImpl< T, TBase, TWinTraits > baseClass;
	//typedef COffscreenDrawRect< T > offscreenDrawClass;

	enum StateBits
	{
		// Flags
		// bits			= 0x000000ff
		ectcMouseInWindow		= 0x00000001,
		ectcOverflowLeft		= 0x00000002,
		ectcOverflowRight		= 0x00000004,
		//ectcOverflowBottom        = 0x00000002,  // alias for vertical mode
		//ectcOverflowTop           = 0x00000004,  // alias for vertical mode
		ectcEnableRedraw		= 0x00000008,
		ectcDraggingItem		= 0x00000010,
		//ectcFlag20                = 0x00000020,
		//ectcFlag40                = 0x00000040,
		//ectcFlag80                = 0x00000080,

		// Scroll
		// bits			= 0x0000ff00
		ectcScrollDeltaMask		= 0x00003f00,    //0011 1111
		ectcScrollDeltaShift		= 8,

		// We have to publicly expose these:
		ectcScrollRepeat		= 0x0000c000,    //1100 0000
		//ectcScrollRepeat_None     = 0x00000000,
		//ectcScrollRepeat_Slow     = 0x00004000,    //0100 0000
		//ectcScrollRepeat_Normal   = 0x00008000,    //1000 0000
		//ectcScrollRepeat_Fast     = 0x0000c000,    //1100 0000

		// Hot Tracking
		// bits			= 0x000f0000
		ectcHotTrack			= 0x000f0000,
		ectcHotTrack_CloseButton	= 0x00010000,
		ectcHotTrack_ScrollRight	= 0x00020000,
		ectcHotTrack_ScrollLeft		= 0x00030000,
		ectcHotTrack_TabItem		= 0x00040000,

		// Mouse Down
		// bits			= 0x00f00000
		ectcMouseDown			= 0x00f00000,
		ectcMouseDownL_CloseButton	= 0x00100000,
		ectcMouseDownL_ScrollRight	= 0x00200000,
		ectcMouseDownL_ScrollLeft	= 0x00300000,
		ectcMouseDownL_TabItem		= 0x00400000,

		ectcMouseDownR_CloseButton	= 0x00900000,
		ectcMouseDownR_ScrollRight	= 0x00a00000,
		ectcMouseDownR_ScrollLeft	= 0x00b00000,
		ectcMouseDownR_TabItem		= 0x00c00000,

		// Mouse Over
		// bits			= 0x0f000000
		ectcMouseOver			= 0x0f000000,
		ectcMouseOver_CloseButton	= 0x01000000,
		ectcMouseOver_ScrollRight	= 0x02000000,
		ectcMouseOver_ScrollLeft	= 0x03000000,
		ectcMouseOver_TabItem		= 0x04000000,
	};

	enum ButtonToolTipIDs
	{
		ectcToolTip_Close		= 0xFFFFFFF0,
		ectcToolTip_ScrollRight		= 0xFFFFFFF1,
		ectcToolTip_ScrollLeft		= 0xFFFFFFF2,
	};

	enum TimerIDs
	{
		ectcTimer_ScrollLeft		= 0x00000010,
		ectcTimer_ScrollRight		= 0x00000020,
	};

	// Public enumerations
public:
	enum ScrollRepeat
	{
		ectcScrollRepeat_None		= 0x00000000,
		ectcScrollRepeat_Slow		= 0x00004000,
		ectcScrollRepeat_Normal		= 0x00008000,
		ectcScrollRepeat_Fast		= 0x0000c000,
	};

	// Constructors
	ClsCustomTabCtrl() :
		m_iCurSel( -1 ),
		m_iHotItem( -1 ),
		m_dwState( 0 ),
		m_iDragItem( -1 ),
		m_iDragItemOriginal( -1 ),
		m_hCursorMove( NULL ),
		m_hCursorNoDrop( NULL ),
		m_iScrollOffset( 0 )
	{
		::ZeroMemory( &m_settings, sizeof( CTCSETTINGS ));
		::ZeroMemory( &m_ptDragOrigin, sizeof( POINT ));

		::SetRectEmpty( &m_rcTabItemArea );
		::SetRectEmpty( &m_rcCloseButton );
		::SetRectEmpty( &m_rcScrollLeft  );
		::SetRectEmpty( &m_rcScrollRight );

		m_dwState |= (( 40 << ectcScrollDeltaShift ) & ectcScrollDeltaMask );
		m_dwState |= ectcScrollRepeat_Normal;
		m_dwState |= ectcEnableRedraw;
	}
	virtual ~ClsCustomTabCtrl() {}

	// Implementation
protected:

	void InitializeTooltips( void )
	{
		_ASSERT( ! m_tooltip.GetSafeHWND());
		if( !m_tooltip.GetSafeHWND())
		{
			// Be sure InitCommonControlsEx is called before this,
			// with one of the flags that includes the tooltip control
			m_tooltip.Create( this, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP /* | TTS_BALLOON */ );
			if( m_tooltip.GetSafeHWND())
			{
				m_tooltip.ModifyExStyle( 0, WS_EX_TOOLWINDOW );
				m_tooltip.SetWindowPos( HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE );

				m_tooltip.SetDelayTime( ::GetDoubleClickTime(), TTDT_INITIAL );
				m_tooltip.SetDelayTime( ::GetDoubleClickTime() * 20, TTDT_AUTOPOP );
				m_tooltip.SetDelayTime( ::GetDoubleClickTime() / 5, TTDT_RESHOW );
			}
		}
	}

	void ActivateTooltips( BOOL bActivate = TRUE )
	{
		_ASSERT( m_tooltip.GetSafeHWND());
		if( m_tooltip.GetSafeHWND())
		{
			m_tooltip.Activate( bActivate );
		}
	}

	void ClearCurrentHotTracking( bool bRedrawEffectedArea = true )
	{
		switch( m_dwState & ectcHotTrack )
		{
			case ectcHotTrack_CloseButton:
				m_dwState &= ~ectcHotTrack;
				if( bRedrawEffectedArea )
				{
					this->InvalidateRect( &m_rcCloseButton );
				}
				break;

			case ectcHotTrack_ScrollRight:
				m_dwState &= ~ectcHotTrack;
				if( bRedrawEffectedArea )
				{
					this->InvalidateRect( &m_rcScrollRight );
				}
				break;

			case ectcHotTrack_ScrollLeft:
				m_dwState &= ~ectcHotTrack;
				if( bRedrawEffectedArea )
				{
					this->InvalidateRect( &m_rcScrollLeft );
				}
				break;

			case ectcHotTrack_TabItem:
				m_dwState &= ~ectcHotTrack;
				m_iHotItem = -1;

				if( bRedrawEffectedArea )
				{
					// In case the derived class actually changes the width
					//  of a hot tracked tab, invalidate the whole tab window
					this->Invalidate();
				}
				break;

			default:
				m_dwState &= ~ectcHotTrack;
				m_iHotItem = -1;
				if( bRedrawEffectedArea )
				{
					this->Invalidate();
				}
				break;
		}
	}

	void ClearCurrentMouseDownTracking( bool bRedrawEffectedArea = true )
	{
		switch( m_dwState & ectcMouseDown )
		{
			case ectcMouseDownL_CloseButton:
			case ectcMouseDownR_CloseButton:
				m_dwState &= ~ectcMouseDown;
				if( bRedrawEffectedArea )
				{
					this->InvalidateRect( &m_rcCloseButton );
				}
				break;

			case ectcMouseDownL_ScrollRight:
			case ectcMouseDownR_ScrollRight:
				m_dwState &= ~ectcMouseDown;
				if( bRedrawEffectedArea )
				{
					this->InvalidateRect( &m_rcScrollRight );
				}
				break;

			case ectcMouseDownL_ScrollLeft:
			case ectcMouseDownR_ScrollLeft:
				m_dwState &= ~ectcMouseDown;
				if( bRedrawEffectedArea )
				{
					this->InvalidateRect( &m_rcScrollLeft );
				}
				break;

			case ectcMouseDownL_TabItem:
			case ectcMouseDownR_TabItem:
				m_dwState &= ~ectcMouseDown;

				if( bRedrawEffectedArea )
				{
					// In case the derived class actually changes the width
					//  of a hot tracked tab, invalidate the whole tab window
					this->Invalidate();

					//if(m_iActionItem >= 0 && m_iActionItem < m_Items.GetCount())
					//{
					//	RECT rcItemDP;
					//	this->GetItemRect(m_iActionItem, &rcItemDP);
					//	this->InvalidateRect(rcItemDP);
					//}
				}
				break;

			default:
				m_dwState &= ~ectcMouseDown;
				if( bRedrawEffectedArea )
				{
					this->Invalidate();
				}
				break;
		}
	}

	void ClearCurrentMouseOverTracking( bool bRedrawEffectedArea = true )
	{
		switch( m_dwState & ectcMouseOver )
		{
			case ectcMouseOver_CloseButton:
				m_dwState &= ~ectcMouseOver;
				if( bRedrawEffectedArea )
				{
					this->InvalidateRect( &m_rcCloseButton );
				}
				break;
		
			case ectcMouseOver_ScrollRight:
				m_dwState &= ~ectcMouseOver;
				if( bRedrawEffectedArea )
				{
					this->InvalidateRect( &m_rcScrollRight );
				}
				break;

			case ectcMouseOver_ScrollLeft:
				m_dwState &= ~ectcMouseOver;
				if( bRedrawEffectedArea )
				{
					this->InvalidateRect( &m_rcScrollLeft );
				}
				break;

			case ectcMouseOver_TabItem:
				m_dwState &= ~ectcMouseOver;
				if( bRedrawEffectedArea )
				{
					// In case the derived class actually changes the width
					//  of a hot tracked tab, invalidate the whole tab window
					this->Invalidate();

					//if(m_iActionItem >= 0 && m_iActionItem < m_Items.GetCount())
					//{
					//	RECT rcItemDP = {0};
					//	this->GetItemRect(m_iActionItem, &rcItemDP);
					//	this->InvalidateRect(rcItemDP);
					//}
				}
				break;

			default:
				m_dwState &= ~ectcMouseOver;
				if( bRedrawEffectedArea )
				{
					this->Invalidate();
				}
				break;
		}
	}

	// Drag and drop methods (overrideable)
	// In this base class implementation, we mimic the drag rearrange
	// of MDI tabs in VS.NET (except, we add scroll support)
	virtual void AcceptItemDrag( bool bRedrawEffectedArea = true )
	{
		NMCTC2ITEMS nmh = {{ m_hWnd, this->GetDlgCtrlID(), CTCN_ACCEPTITEMDRAG }, m_iDragItemOriginal, m_iDragItem, { -1, -1 }};
		::SendMessage( GetParentHandle(), WM_NOTIFY, nmh.hdr.idFrom, ( LPARAM )&nmh );

		// In this implementation, the tab is moved as they drag.
		this->StopItemDrag( bRedrawEffectedArea );
	}

	virtual void CancelItemDrag( bool bRedrawEffectedArea = true )
	{
		// In this implementation, the tab is moved as they drag.
		// To cancel the drag, we move the item back to its original place.
		if( m_iDragItemOriginal >= 0 &&
		    m_iDragItem >= 0 &&
		    m_iDragItemOriginal != m_iDragItem )
		{
			this->MoveItem( m_iDragItem, m_iDragItemOriginal, true, false );
			this->EnsureVisible( m_iDragItemOriginal );
		}

		NMCTCITEM nmh = {{ m_hWnd, this->GetDlgCtrlID(), CTCN_CANCELITEMDRAG }, m_iDragItemOriginal, { -1, -1 }};
		::SendMessage( GetParentHandle(), WM_NOTIFY, nmh.hdr.idFrom, ( LPARAM )&nmh );

		this->StopItemDrag( bRedrawEffectedArea );
	}

	virtual void StopItemDrag( bool bRedrawEffectedArea = true )
	{
		if( ectcDraggingItem == ( m_dwState & ectcDraggingItem ))
		{
			m_dwState &= ~ectcDraggingItem;

			// Restore the default cursor
			::SetCursor(( HCURSOR )( UINT_PTR )::GetClassLongPtr( m_hWnd, GCLP_HCURSOR ));

			if( m_hCursorMove != NULL )
			{
				::DestroyCursor( m_hCursorMove );
				m_hCursorMove = NULL;
			}
			if( m_hCursorNoDrop != NULL )
			{
				::DestroyCursor( m_hCursorNoDrop );
				m_hCursorNoDrop = NULL;
			}

			m_iDragItem = -1;
			m_iDragItemOriginal = -1;
			::ZeroMemory( &m_ptDragOrigin, sizeof( POINT ));

			if( bRedrawEffectedArea )
			{
				this->Invalidate();
			}
		}
	}

	virtual void BeginItemDrag( int index, POINT ptDragOrigin )
	{
		if( index >= 0 )
		{
			DWORD dwStyle = this->GetStyle();

			if( CTCS_DRAGREARRANGE == ( dwStyle & CTCS_DRAGREARRANGE ))
			{
				NMCTCITEM nmh = {{ m_hWnd, this->GetDlgCtrlID(), CTCN_BEGINITEMDRAG }, index, { ptDragOrigin.x, ptDragOrigin.y }};
				if( FALSE != ::SendMessage( GetParentHandle(), WM_NOTIFY, nmh.hdr.idFrom, ( LPARAM )&nmh ))
				{
					// Returning non-zero prevents our default handling.
					// We've possibly already set a couple things that we
					// need to cleanup, so call StopItemDrag
					this->StopItemDrag( false );
				}
				else
				{
					// returning FALSE let's us do our default handling

					// Mark the beginning of a drag operation.
					// We should have already done SetCapture, but just
					// in case, we'll set it again.
					this->SetCapture();

					// Set focus so that we get an ESC key press
					this->SetFocus();

					// This call to DoDragDrop is just to ensure a dependency on OLE32.dll.
					// In the future, if we support true OLE drag and drop,
					// we'll really use DoDragDrop.
					::DoDragDrop( NULL, NULL, 0, 0 );

					// To save on resources, we'll load the drag cursor
					// only when we need it, and destroy it when the drag is done
					HMODULE hOle32 = ::GetModuleHandle( _T( "OLE32.dll" ));
					if( hOle32 != NULL )
					{
						// Cursor ID identified using resource editor in Visual Studio
						int dragCursor = 2;
						int noDropCursor = 1;
						m_hCursorMove = ::LoadCursor( hOle32, MAKEINTRESOURCE( dragCursor ));
						m_hCursorNoDrop = ::LoadCursor( hOle32, MAKEINTRESOURCE( noDropCursor ));
					}

					m_dwState |= ectcDraggingItem;

					m_iDragItem = index;
					m_iDragItemOriginal = index;
					m_ptDragOrigin = ptDragOrigin;
				}
			}
		}
	}

	// Update the drag operation.
	//  ptCursor should be in client coordinates
	virtual void ContinueItemDrag( POINT ptCursor )
	{
		RECT rcClient = { 0 };
		::GetClientRect( m_hWnd, &rcClient );
		if( ::PtInRect( &rcClient, ptCursor ))
		{
			::SetCursor( m_hCursorMove );
		}
		else
		{
			::SetCursor( m_hCursorNoDrop );
		}

		CTCHITTESTINFO tchti = { 0 };
		tchti.pt = ptCursor;
		int index = this->HitTest( &tchti );
		if(( index != m_iDragItem ) && ( index >= 0 ))
		{
			RECT rcItem = { 0 };
			this->GetItemRect( index, &rcItem );

			int itemMiddlePointX = rcItem.left + (( rcItem.right - rcItem.left ) / 2 );

			if(( m_iDragItem < index ) && ( ptCursor.x > itemMiddlePointX ))
			{
				// They're dragging an item from left to right,
				// and have dragged it over the half way mark to the right
				this->MoveItem( m_iDragItem, index, true, false );
				m_iDragItem = index;
			}
			else if(( m_iDragItem > index ) && ( ptCursor.x < itemMiddlePointX ))
			{
				// They're dragging an item from right to left,
				// and have dragged it over the half way mark to the left
				this->MoveItem( m_iDragItem, index, true, false );
				m_iDragItem = index;
			}
		}

		// Now scroll if necessary
		DWORD dwStyle = this->GetStyle();
		if( CTCS_SCROLL == ( dwStyle & CTCS_SCROLL ))
		{
			RECT rcLeftScrollZone = { rcClient.left, rcClient.top, ( m_rcTabItemArea.left + CTCD_SCROLLZONEWIDTH ), rcClient.bottom };
			RECT rcRightScrollZone = { ( m_rcTabItemArea.right - CTCD_SCROLLZONEWIDTH ), rcClient.top, rcClient.right, rcClient.bottom };

			if( ::PtInRect( &rcLeftScrollZone, ptCursor ) &&
			  ( ectcOverflowLeft == ( m_dwState & ectcOverflowLeft )))
			{
				this->ScrollLeft( true );
				this->SetTimer( ectcTimer_ScrollLeft, CTCSR_SLOW );
			}
			else if( ::PtInRect( &rcRightScrollZone, ptCursor ) &&
			       ( ectcOverflowRight == ( m_dwState & ectcOverflowRight )))
			{
				this->ScrollRight( true );
				this->SetTimer( ectcTimer_ScrollRight, CTCSR_SLOW );
			}
		}
	}

	// Message Handling

	BEGIN_MSG_MAP_HEADER( ClsWindow )
		MESSAGE_RANGE_HANDLER(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
		START_MAP()
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
		MESSAGE_HANDLER(WM_MOUSELEAVE, OnMouseLeave)
		MESSAGE_HANDLER(WM_CAPTURECHANGED, OnCaptureChanged)
		MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
		MESSAGE_HANDLER(WM_LBUTTONUP, OnLButtonUp)
		MESSAGE_HANDLER(WM_LBUTTONDBLCLK, OnLButtonDoubleClick)
		MESSAGE_HANDLER(WM_RBUTTONDOWN, OnRButtonDown)
		MESSAGE_HANDLER(WM_RBUTTONUP, OnRButtonUp)
		MESSAGE_HANDLER(WM_RBUTTONDBLCLK, OnRButtonDoubleClick)
		MESSAGE_HANDLER(WM_MBUTTONDOWN, OnMButtonDown)
		MESSAGE_HANDLER(WM_MBUTTONUP, OnMButtonUp)
		MESSAGE_HANDLER(WM_MBUTTONDBLCLK, OnMButtonDoubleClick)
		MESSAGE_HANDLER(WM_SETTINGCHANGE, OnSettingChange)
		MESSAGE_HANDLER(WM_SYSCOLORCHANGE, OnSettingChange)
		MESSAGE_HANDLER(WM_GETDLGCODE, OnGetDlgCode)
		MESSAGE_HANDLER(WM_KEYDOWN, OnKeyDown)
		MESSAGE_HANDLER(WM_GETFONT, OnGetFont)
		MESSAGE_HANDLER(WM_SETFONT, OnSetFont)
		MESSAGE_HANDLER(WM_STYLECHANGED, OnStyleChanged)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_SETREDRAW, OnSetRedraw)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
	END_MSG_MAP()

	BEGIN_NOTIFICATION_MAP( ClsWindow )
		NOTIFICATION_HANDLER(TTN_GETDISPINFO, OnNotify)
	END_NOTIFICATION_MAP()

	virtual LRESULT OnNotify( int, LPNMHDR pHdr, BOOL&  )
	{
		if ( pHdr->code == TTN_GETDISPINFO )
		{
			LPNMTTDISPINFO pToolTipInfo = ( LPNMTTDISPINFO )pHdr;
			if( pToolTipInfo )
			{
				// The way we implement tooltips for tab items
				// is to have as many "tools" as there are tabs.
				// The relationship of tool ID => tab index is:
				// tool ID = tab index + 1	(to avoid 0 as an ID)
				//
				// We supply the RECT elsewhere and the text here
				UINT id = ( UINT )pToolTipInfo->hdr.idFrom;
				if( id > 0 && id <= ( UINT )m_Items.GetSize())
				{
					TItem* pItem = m_Items[ id - 1 ];
					_ASSERT_VALID( pItem );
					if( pItem )
					{
						if( pItem->UsingToolTip())
						{
							pToolTipInfo->lpszText = const_cast< LPTSTR >( pItem->GetToolTipRef());
						}
						else if( pItem->UsingText())
						{
							pToolTipInfo->lpszText = const_cast< LPTSTR >( pItem->GetTextRef());
						}
					}
				}
			}
		}
		return 0;
	}

	virtual LRESULT OnPaint( UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
	{
		if( wParam != NULL )
		{
			ClsBufferDC memdc( ClsDC::FromHandle(( HDC )wParam ));
			RECT rcClip;
			memdc.GetClipBox( &rcClip );
			this->DoPaint( memdc, rcClip );
		}
		else
		{
			ClsPaintDC dc( this );
			ClsBufferDC memdc( &dc, &(( LPPAINTSTRUCT )dc )->rcPaint, TRUE );
			this->DoPaint( memdc, (( LPPAINTSTRUCT )dc )->rcPaint );
		}
		return 0;
	}

	virtual LRESULT OnEraseBackground( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
	{
		return 1; // handled; no need to erase background; do it in DoPaint();
	}

	virtual LRESULT OnCreate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/ )
	{
		LRESULT lRes = baseClass::WindowProc( uMsg, wParam, lParam );
		if( lRes == -1 )
		{
			return -1;
		}

		this->Initialize();
		return lRes;
	}

	virtual LRESULT OnDestroy( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
	{
		this->Uninitialize();
		bHandled = FALSE;
		return 0;
	}

	virtual LRESULT OnSize( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
	{
		this->UpdateLayout();
		this->Invalidate();
		bHandled = FALSE;
		return 0;
	}

	virtual LRESULT OnSettingChange( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
	{
		this->UpdateLayout();
		this->Invalidate();
		return 0;
	}

	virtual LRESULT OnGetDlgCode( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/ )
	{
		return baseClass::WindowProc( uMsg, wParam, lParam ) | DLGC_WANTARROWS;
	}

	virtual LRESULT OnMouseMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		bHandled = FALSE;
		if( m_tooltip.GetSafeHWND())
		{
			MSG msg = { m_hWnd, uMsg, wParam, lParam };
			m_tooltip.RelayEvent( &msg );
		}
		return 1;
	}

	virtual LRESULT OnMouseMove( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled )
	{
		bHandled = FALSE;

		DWORD dwStyle = this->GetStyle();
		POINT ptCursor = { GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) };

		if( ectcMouseInWindow != ( m_dwState & ectcMouseInWindow ))
		{
			TRACKMOUSEEVENT tme = { 0 };
			tme.cbSize = sizeof(tme);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = m_hWnd;
			if( _TrackMouseEvent( &tme ))
			{
				m_dwState |= ectcMouseInWindow;

				this->UpdateLayout();
				this->Invalidate();

				// "OnMouseEnter"
				//...
			}
		}

		if(( m_iDragItem >= 0 ) &&
		   ( ectcMouseInWindow == ( m_dwState & ectcMouseInWindow )) &&
		   ( ectcDraggingItem != ( m_dwState & ectcDraggingItem )) &&
		   ( ectcMouseDownL_TabItem == ( m_dwState & ectcMouseDown )) &&
		   ( m_ptDragOrigin.x != ptCursor.x ) &&
		   ( m_Items.GetSize() > 1 ))
		{
			this->BeginItemDrag( m_iDragItem, m_ptDragOrigin );
		}
		else if( ectcDraggingItem == ( m_dwState & ectcDraggingItem ))
		{
			this->ContinueItemDrag( ptCursor );
		}
		else if( ectcMouseInWindow == ( m_dwState & ectcMouseInWindow ))
		{
			// hit test
			if( ::PtInRect( &m_rcCloseButton, ptCursor ))
			{
				if( ectcMouseOver_CloseButton != ( m_dwState & ectcMouseOver ))
				{
					this->ClearCurrentMouseOverTracking( true );
					m_dwState |= ectcMouseOver_CloseButton;

					if( ectcMouseDownL_CloseButton == ( m_dwState & ectcMouseDown ))
					{
						this->InvalidateRect( &m_rcCloseButton );
					}
				}
				else if( 0 == ( m_dwState & ectcMouseDown ) &&
					 ectcHotTrack_CloseButton != ( m_dwState & ectcHotTrack ))
				{
					this->ClearCurrentHotTracking( true );
					m_dwState |= ectcHotTrack_CloseButton;
					this->InvalidateRect( &m_rcCloseButton );
				}
			}
			else
			{
				if( ectcMouseOver_CloseButton == ( m_dwState & ectcMouseOver ))
				{
					this->ClearCurrentMouseOverTracking( true );
				}
				if( ectcHotTrack_CloseButton == ( m_dwState & ectcHotTrack ))
				{
					this->ClearCurrentHotTracking( true );
				}
			}

			if( ::PtInRect( &m_rcScrollRight, ptCursor ))
			{
				if( ectcMouseOver_ScrollRight != ( m_dwState & ectcMouseOver ))
				{
					this->ClearCurrentMouseOverTracking( true );
					m_dwState |= ectcMouseOver_ScrollRight;

					if( ectcMouseDownL_ScrollRight == ( m_dwState & ectcMouseDown ))
					{
						this->InvalidateRect( &m_rcScrollRight );
					}
				}
				else if( 0 == ( m_dwState & ectcMouseDown ) &&
					 ectcHotTrack_ScrollRight != ( m_dwState & ectcHotTrack ))
				{
					this->ClearCurrentHotTracking( true );
					m_dwState |= ectcHotTrack_ScrollRight;
					this->InvalidateRect( &m_rcScrollRight );
				}
			}
			else
			{
				if( ectcMouseOver_ScrollRight == ( m_dwState & ectcMouseOver ))
				{
					this->ClearCurrentMouseOverTracking( true );
				}
				if( ectcHotTrack_ScrollRight == ( m_dwState & ectcHotTrack ))
				{
					this->ClearCurrentHotTracking( true );
				}
			}

			if( ::PtInRect( &m_rcScrollLeft, ptCursor ))
			{
				if( ectcMouseOver_ScrollLeft != ( m_dwState & ectcMouseOver ))
				{
					this->ClearCurrentMouseOverTracking( true );
					m_dwState |= ectcMouseOver_ScrollLeft;

					if( ectcMouseDownL_ScrollLeft == ( m_dwState & ectcMouseDown ))
					{
						this->InvalidateRect( &m_rcScrollLeft );
					}
				}
				else if( 0 == ( m_dwState & ectcMouseDown ) &&
					 ectcHotTrack_ScrollLeft != ( m_dwState & ectcHotTrack ))
				{
					this->ClearCurrentHotTracking( true );
					m_dwState |= ectcHotTrack_ScrollLeft;
					this->InvalidateRect( &m_rcScrollLeft );
				}
			}
			else
			{
				if( ectcMouseOver_ScrollLeft == ( m_dwState & ectcMouseOver ))
				{
					this->ClearCurrentMouseOverTracking( true );
				}
				if( ectcHotTrack_ScrollLeft == ( m_dwState & ectcHotTrack ))
				{
					this->ClearCurrentHotTracking( true );
				}
			}

			if( ::PtInRect( &m_rcTabItemArea, ptCursor ))
			{
				if( ectcMouseOver_TabItem != ( m_dwState & ectcMouseOver ))
				{
					this->ClearCurrentMouseOverTracking( true );
					m_dwState |= ectcMouseOver_TabItem;

					// Not needed for simple hot tracking:
					//if(ectcMouseDownL_TabItem == (m_dwState & ectcMouseDown))
					//{
					//	this->InvalidateRect(&m_rcTabItemArea);
					//}
				}
				else if( CTCS_HOTTRACK == ( dwStyle & CTCS_HOTTRACK ))
					// && ectcHotTrack_TabItem != (m_dwState & ectcHotTrack))
				{
					this->ClearCurrentHotTracking( true );

					CTCHITTESTINFO tchti = { 0 };
					tchti.pt = ptCursor;
					int nIndex = this->HitTest( &tchti );
					if( nIndex >= 0 )
					{
						m_iHotItem = nIndex;

						m_dwState |= ectcHotTrack_TabItem;
						RECT rcItem = { 0 };
						this->GetItemRect( nIndex, &rcItem );
						this->InvalidateRect( &rcItem );
					}
				}
			}
			else
			{
				if( ectcMouseOver_TabItem == ( m_dwState & ectcMouseOver ))
				{
					this->ClearCurrentMouseOverTracking( true );
				}
				if( CTCS_HOTTRACK == ( dwStyle & CTCS_HOTTRACK ) &&
				    ectcHotTrack_TabItem == ( m_dwState & ectcHotTrack ))
				{
					this->ClearCurrentHotTracking( true );
				}
			}
		}

		return 1;
	}

	virtual LRESULT OnMouseLeave( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
	{
		bHandled = FALSE;

		m_dwState &= ~ectcMouseInWindow;
		this->ClearCurrentHotTracking( false );
		this->ClearCurrentMouseOverTracking( false );

		this->UpdateLayout();
		this->Invalidate();

		return 0;
	}

	virtual LRESULT OnCaptureChanged( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
	{
		bHandled = FALSE;

		this->ClearCurrentMouseDownTracking( false );
		if( ectcDraggingItem == ( m_dwState & ectcDraggingItem ))
		{
			this->CancelItemDrag(false);
		}

		return 0;
	}

	virtual LRESULT OnLButtonDown( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/ )
	{
		POINT ptCursor = { GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) };
		if( ::PtInRect( &m_rcCloseButton, ptCursor ))
		{
			m_dwState |= ( ectcMouseDownL_CloseButton | ectcMouseOver_CloseButton );
			this->InvalidateRect( &m_rcCloseButton );
			this->SetCapture();
		}
		else if( ::PtInRect( &m_rcScrollRight, ptCursor ))
		{
			m_dwState |= ( ectcMouseDownL_ScrollRight | ectcMouseOver_ScrollRight );
			if( ectcOverflowRight == ( m_dwState & ectcOverflowRight ))
			{
				int nScrollSpeed = 0;
				switch( m_dwState & ectcScrollRepeat )
				{
					case ectcScrollRepeat_Slow:
						nScrollSpeed = CTCSR_SLOW;
						break;

					case ectcScrollRepeat_Normal:
						nScrollSpeed = CTCSR_NORMAL;
						break;

					case ectcScrollRepeat_Fast:
						nScrollSpeed = CTCSR_FAST;
						break;

					case ectcScrollRepeat_None:
					default:
						nScrollSpeed = CTCSR_NONE;
						break;
				}

				this->ScrollRight( true );
				this->SetTimer( ectcTimer_ScrollRight, nScrollSpeed );
			}
			this->SetCapture();
		}
		else if( ::PtInRect( &m_rcScrollLeft, ptCursor ))
		{
			m_dwState |= ( ectcMouseDownL_ScrollLeft | ectcMouseOver_ScrollLeft );
			if( ectcOverflowLeft == ( m_dwState & ectcOverflowLeft ))
			{
				int nScrollSpeed = 0;
				switch( m_dwState & ectcScrollRepeat )
				{
					case ectcScrollRepeat_Slow:
						nScrollSpeed = CTCSR_SLOW;
						break;

					case ectcScrollRepeat_Normal:
						nScrollSpeed = CTCSR_NORMAL;
						break;

					case ectcScrollRepeat_Fast:
						nScrollSpeed = CTCSR_FAST;
						break;

					case ectcScrollRepeat_None:
					default:
						nScrollSpeed = CTCSR_NONE;
						break;
				}

				this->ScrollLeft( true );
				this->SetTimer( ectcTimer_ScrollLeft, nScrollSpeed );
			}
			this->SetCapture();
		}
		else
		{
			// Search for a tab
			CTCHITTESTINFO tchti = { 0 };
			tchti.pt = ptCursor;
			int nIndex = this->HitTest( &tchti );

			NMCTCITEM nmh = {{ m_hWnd, this->GetDlgCtrlID(), NM_CLICK }, nIndex, { ptCursor.x, ptCursor.y }};
			if( FALSE == ::SendMessage( GetParentHandle(), WM_NOTIFY, nmh.hdr.idFrom, ( LPARAM )&nmh ))
			{
				// returning FALSE let's us do our default handling
				if( nIndex >= 0 )
				{
					// NOTE: If they click on a tab, only grab the focus
					//  if a drag operation is started.
					//this->SetFocus();
					this->SetCurSel( nIndex );

					m_iDragItem = nIndex;
					m_ptDragOrigin = ptCursor;

					m_dwState |= ectcMouseDownL_TabItem;

					// This could be a drag operation.  We'll start the actual drag
					// operation in OnMouseMove if the mouse moves while the left mouse
					// button is still pressed.  OnLButtonUp will ReleaseCapture.
					this->SetCapture();
				}
			}
		}

		return 0;
	}

	virtual LRESULT OnLButtonUp( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/ )
	{
		if( m_hWnd == ::GetCapture())
		{
			POINT ptCursor = { GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) };

			if( ectcDraggingItem == ( m_dwState & ectcDraggingItem ))
			{
				this->AcceptItemDrag();
			}

			// Before we release the capture, remember what the state was
			// (in WM_CAPTURECHANGED we ClearCurrentMouseDownTracking)
			DWORD dwState = m_dwState;

			::ReleaseCapture();

			if( ectcMouseDownL_CloseButton == ( dwState & ectcMouseDown ) &&
			    ectcMouseOver_CloseButton == ( dwState & ectcMouseOver ))
			{
				// Close Button
				NMCTCITEM nmh = {{ m_hWnd, this->GetDlgCtrlID(), CTCN_CLOSE }, m_iCurSel, { ptCursor.x, ptCursor.y }};
				::SendMessage( GetParentHandle(), WM_NOTIFY, nmh.hdr.idFrom, ( LPARAM )&nmh );
			}

			this->UpdateLayout();
			this->Invalidate();
		}
		return 0;
	}

	virtual LRESULT OnLButtonDoubleClick( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/ )
	{
		POINT ptCursor = { GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) };

		// Search for a tab
		CTCHITTESTINFO tchti = { 0 };
		tchti.pt = ptCursor;
		int nIndex = this->HitTest( &tchti );

		// returning TRUE tells us not to do our default handling
		NMCTCITEM nmh = {{ m_hWnd, this->GetDlgCtrlID(), NM_DBLCLK }, nIndex, { ptCursor.x, ptCursor.y }};
		if( FALSE == ::SendMessage( GetParentHandle(), WM_NOTIFY, nmh.hdr.idFrom, ( LPARAM )&nmh ))
		{
			// returning FALSE let's us do our default handling
			if( nIndex >= 0 )
			{
				//this->SetFocus();
				//this->SetCurSel( nIndex );
			}
		}

		return 0;
	}

	virtual LRESULT OnRButtonDown( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/ )
	{
		POINT ptCursor = { GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) };

		// Search for a tab
		CTCHITTESTINFO tchti = { 0 };
		tchti.pt = ptCursor;
		int nIndex = this->HitTest( &tchti );

		// returning TRUE tells us not to do our default handling
		NMCTCITEM nmh = {{ m_hWnd, this->GetDlgCtrlID(), NM_RCLICK }, nIndex, { ptCursor.x, ptCursor.y }};
		if( FALSE == ::SendMessage( GetParentHandle(), WM_NOTIFY, nmh.hdr.idFrom, ( LPARAM)&nmh ))
		{
			// returning FALSE let's us do our default handling
			if( nIndex >= 0 )
			{
				// NOTE: If they click on a tab, only grab the focus
				//  if a drag operation is started.
				//this->SetFocus();
				this->SetCurSel( nIndex );
			}
		}

		return 0;
	}

	virtual LRESULT OnRButtonUp( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
	{
		bHandled = FALSE;
		return 0;
	}

	virtual LRESULT OnRButtonDoubleClick( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/ )
	{
		POINT ptCursor = { GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) };

		// Search for a tab
		CTCHITTESTINFO tchti = { 0 };
		tchti.pt = ptCursor;
		int nIndex = this->HitTest( &tchti );

		// returning TRUE tells us not to do our default handling
		NMCTCITEM nmh = {{ m_hWnd, this->GetDlgCtrlID(), NM_RDBLCLK }, nIndex, { ptCursor.x, ptCursor.y }};
		if( FALSE == ::SendMessage( GetParentHandle(), WM_NOTIFY, nmh.hdr.idFrom, ( LPARAM )&nmh ))
		{
			// returning FALSE let's us do our default handling
			if( nIndex >= 0 )
			{
				//this->SetFocus();
				//this->SetCurSel( nIndex );
			}
		}

		return 0;
	}

	virtual LRESULT OnMButtonDown( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/ )
	{
		POINT ptCursor = { GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) };

		// Search for a tab
		CTCHITTESTINFO tchti = { 0 };
		tchti.pt = ptCursor;
		int nIndex = this->HitTest( &tchti );

		// returning TRUE tells us not to do our default handling
		NMCTCITEM nmh = {{ m_hWnd, this->GetDlgCtrlID(), CTCN_MCLICK }, nIndex, { ptCursor.x, ptCursor.y }};
		if( FALSE == ::SendMessage( GetParentHandle(), WM_NOTIFY, nmh.hdr.idFrom, ( LPARAM )&nmh ))
		{
			// returning FALSE let's us do our default handling
			if( nIndex!=-1 )
			{
				//this->SetFocus();
				//this->SetCurSel( nIndex );
			}
		}

		return 0;
	}

	virtual LRESULT OnMButtonUp( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled )
	{
		bHandled = FALSE;
		return 0;
	}

	virtual LRESULT OnMButtonDoubleClick( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/ )
	{
		POINT ptCursor = { GET_X_LPARAM( lParam ), GET_Y_LPARAM( lParam ) };

		// Search for a tab
		CTCHITTESTINFO tchti = { 0 };
		tchti.pt = ptCursor;
		int nIndex = this->HitTest( &tchti );

		// returning TRUE tells us not to do our default handling
		NMCTCITEM nmh = {{ m_hWnd, this->GetDlgCtrlID(), CTCN_MDBLCLK }, nIndex, { ptCursor.x, ptCursor.y }};
		if( FALSE == ::SendMessage( GetParentHandle(), WM_NOTIFY, nmh.hdr.idFrom, ( LPARAM )&nmh ))
		{
			// returning FALSE let's us do our default handling
			if( nIndex != -1 )
			{
				//this->SetFocus();
				//this->SetCurSel( nIndex );
			}
		}

		return 0;
	}

	virtual LRESULT OnKeyDown( UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled )
	{
		switch( wParam )
		{
			case VK_LEFT:
				if( m_iCurSel > 0 )
				{
					this->SetCurSel( m_iCurSel - 1 );
				}
				return 0;

			case VK_RIGHT:
				if( m_iCurSel < ( int )m_Items.GetSize() - 1 )
				{
					this->SetCurSel( m_iCurSel + 1 );
				}
				return 0;

			case VK_ESCAPE:
				if( ectcDraggingItem == ( m_dwState & ectcDraggingItem ))
				{
					this->CancelItemDrag( true );
					return 0;
				}
				break;
		}
		bHandled = FALSE;
		return 0;
	}

	virtual LRESULT OnGetFont( UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/ )
	{
		// DDB: 2002/04/22
		// The code was doing GetFont and SetFont, but wasn't actually
		//  properly dealing with implementing it if the window
		//  was not a subclassed static control.
		return ( LRESULT )( HFONT )m_font;
	}

	virtual LRESULT OnSetFont( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/ )
	{
		// DDB: 2002/04/22
		// The code was doing GetFont and SetFont, but wasn't actually
		//  properly dealing with implementing it if the window
		//  was not a subclassed static control.
		//
		// Also, we're managing the lifetime of our font
		//  (i.e., we're going to DeleteObject it in our destructor),
		//  so if someone calls SetFont, keep a copy of the
		//  font instead of just "pointing" to it
		LOGFONT lfCopy = { 0 };
		::GetObject(( HFONT )wParam, sizeof( LOGFONT ), &lfCopy );

		if( m_font.IsValid()) m_font.DeleteObject();

		m_font.CreateFontIndirect( &lfCopy );

		if( LOWORD( lParam ))
		{
			this->Invalidate();
		}

		return 0;
	}

	virtual LRESULT OnStyleChanged( UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
	{
		if( wParam == GWL_STYLE )
		{
			LPSTYLESTRUCT pStyles = ( LPSTYLESTRUCT )lParam;
			if( pStyles )
			{
				// Tooltips
				if(((( pStyles->styleOld ) & CTCS_TOOLTIPS ) != CTCS_TOOLTIPS ) &&
				   ((( pStyles->styleNew ) & CTCS_TOOLTIPS ) == CTCS_TOOLTIPS ))
				{
					this->ActivateTooltips(TRUE);
				}
				else if(((( pStyles->styleOld ) & CTCS_TOOLTIPS ) == CTCS_TOOLTIPS ) &&
					((( pStyles->styleNew ) & CTCS_TOOLTIPS ) != CTCS_TOOLTIPS ))
				{
					this->ActivateTooltips(FALSE);
				}

				// Scroll to fit
				if(((( pStyles->styleOld ) & CTCS_SCROLL ) != CTCS_SCROLL ) &&
				   ((( pStyles->styleNew ) & CTCS_SCROLL ) == CTCS_SCROLL ))
				{
					if( m_tooltip.GetSafeHWND())
					{
						TOOLINFO tool = { 0 };

						tool.cbSize	= sizeof( tool );
						tool.hinst	= ClsGetInstanceHandle();
						tool.hwnd	= m_hWnd;
						tool.lpszText	= _T( "Scroll Right" );
						tool.uFlags	= 0;
						::SetRect( &tool.rect, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT );
						tool.uId	= ( UINT )ectcToolTip_ScrollRight;
						m_tooltip.AddTool( tool );


						tool.lpszText	= _T( "Scroll Left" );
						::SetRect( &tool.rect, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT );
						tool.uId	= ( UINT )ectcToolTip_ScrollLeft;
						m_tooltip.AddTool( tool );
					}

					//this->UpdateLayout();
					//this->Invalidate();
				}
				else if(((( pStyles->styleOld ) & CTCS_SCROLL ) == CTCS_SCROLL ) &&
					((( pStyles->styleNew ) & CTCS_SCROLL ) != CTCS_SCROLL ))
				{
					if( m_tooltip.GetSafeHWND())
					{
						TOOLINFO tool = { 0 };

						tool.cbSize	= sizeof( tool );
						tool.hwnd	= m_hWnd;
						tool.uId	= ( UINT )ectcToolTip_ScrollRight;
						m_tooltip.DelTool( tool );
						tool.uId	= ( UINT )ectcToolTip_ScrollLeft;
						m_tooltip.DelTool( tool );
					}

					m_iScrollOffset = 0;
					//this->UpdateLayout();
					//this->Invalidate();
				}

				// Close Button
				if(((( pStyles->styleOld ) & CTCS_CLOSEBUTTON ) != CTCS_CLOSEBUTTON ) &&
				   ((( pStyles->styleNew ) & CTCS_CLOSEBUTTON ) == CTCS_CLOSEBUTTON ))
				{
					if( m_tooltip.GetSafeHWND())
					{
						TOOLINFO tool = { 0 };

						tool.cbSize	= sizeof( tool );
						tool.hinst	= ClsGetInstanceHandle();
						tool.hwnd	= m_hWnd;
						tool.lpszText	= _T( "Close" );
						tool.uFlags	= 0;
						::SetRect( &tool.rect, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT );
						tool.uId	= ( UINT )ectcToolTip_Close;
						m_tooltip.AddTool( tool );
					}

					//this->UpdateLayout();
					//this->Invalidate();
				}
				else if(((( pStyles->styleOld ) & CTCS_CLOSEBUTTON ) == CTCS_CLOSEBUTTON ) &&
					((( pStyles->styleNew ) & CTCS_CLOSEBUTTON ) != CTCS_CLOSEBUTTON ))
				{
					if(m_tooltip.GetSafeHWND())
					{
						TOOLINFO tool = { 0 };
						tool.cbSize	= sizeof( tool );
						tool.hwnd	= m_hWnd;
						tool.uId	= ( UINT )ectcToolTip_Close;
						m_tooltip.DelTool( tool );
					}

					//this->UpdateLayout();
					//this->Invalidate();
				}

				this->UpdateLayout();
				this->Invalidate();
			}
		}

		bHandled = FALSE;
		return 0;
	}

	virtual LRESULT OnTimer( UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled )
	{
		switch( wParam )
		{
			case ectcTimer_ScrollRight:
				if( ectcOverflowRight != ( m_dwState & ectcOverflowRight ))
				{
					this->KillTimer( ectcTimer_ScrollRight );
				}
				else // ectcOverflowRight == (m_dwState & ectcOverflowRight)
				{
					if( ectcDraggingItem == ( m_dwState & ectcDraggingItem ))
					{
						// We're scrolling because they're dragging a tab near the edge.
						// First kill the timer, then update the drag
						// (which might set the timer again)
						this->KillTimer( ectcTimer_ScrollRight );

						POINT ptCursor = { 0 };
						::GetCursorPos( &ptCursor );
						this->ScreenToClient( &ptCursor );
						this->ContinueItemDrag( ptCursor );
					}
					else if( ectcMouseDownL_ScrollRight == ( m_dwState & ectcMouseDown ) &&
						 ectcMouseOver_ScrollRight == ( m_dwState & ectcMouseOver ))
					{
						// We're scrolling because they're holding down the scroll button
						this->ScrollRight( true );

						if( ectcScrollRepeat_None == ( m_dwState & ectcScrollRepeat ))
						{
							this->KillTimer( ectcTimer_ScrollRight );
						}
					}
				}
				break;

			case ectcTimer_ScrollLeft:
				if( ectcOverflowLeft != ( m_dwState & ectcOverflowLeft ))
				{
					this->KillTimer( ectcTimer_ScrollLeft );
				}
				else // ectcOverflowLeft == (m_dwState & ectcOverflowLeft)
				{
					if( ectcDraggingItem == ( m_dwState & ectcDraggingItem ))
					{
						// We're scrolling because they're dragging a tab near the edge.
						// First kill the timer, then update the drag
						// (which might set the timer again)
						this->KillTimer( ectcTimer_ScrollLeft );

						POINT ptCursor = { 0 };
						::GetCursorPos( &ptCursor );
						this->ScreenToClient( &ptCursor );
						this->ContinueItemDrag( ptCursor );
					}
					else if( ectcMouseDownL_ScrollLeft == ( m_dwState & ectcMouseDown ) &&
						 ectcMouseOver_ScrollLeft == ( m_dwState & ectcMouseOver ))
					{
						// We're scrolling because they're holding down the scroll button
						this->ScrollLeft( true );

						if( ectcScrollRepeat_None == ( m_dwState & ectcScrollRepeat ))
						{
							this->KillTimer( ectcTimer_ScrollLeft );
						}
					}
				}
				break;

			default:
				bHandled = FALSE;
				break;
		}
		return 0;
	}

	virtual LRESULT OnSetRedraw( UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled )
	{
		// If someone sends us WM_SETREDRAW with FALSE, we can avoid
		// doing an update layout until they set it back to TRUE.
		if( wParam )
		{
			if( ectcEnableRedraw != ( m_dwState & ectcEnableRedraw ))
			{
				// Redrawing was turned off, but now its being
				// turned back on again
				m_dwState |= ectcEnableRedraw;

				this->UpdateLayout();

				// The caller will typically call InvalidateRect
				// or RedrawWindow after sending WM_SETREDRAW with TRUE,
				// so we won't do that here (but we will UpdateLayout,
				// so that we'll be ready to redraw)
			}
		}
		else
		{
			if( ectcEnableRedraw == ( m_dwState & ectcEnableRedraw ))
			{
				// Redrawing was turned on, but now its being turned off
				m_dwState &= ~ectcEnableRedraw;
			}
		}

		bHandled = FALSE;
		return 0;
	}

	// Overridables
	virtual void Initialize( void )
	{
		_ASSERT( GetSafeHWND());

		this->SendMessage( WM_SETTINGCHANGE, 0, 0 );
		this->InitializeTooltips();

		// NOTE: you can change the style at any time
		//  for a number of the cool tab control styles
		//  (tool tips, close button, scroll buttons, etc.)
		DWORD dwStyle = this->GetStyle();
		this->ActivateTooltips( CTCS_TOOLTIPS == ( dwStyle & CTCS_TOOLTIPS ));

		if( CTCS_SCROLL == ( dwStyle & CTCS_SCROLL ))
		{
			if( m_tooltip.GetSafeHWND())
			{
				TOOLINFO tool = { 0 };

				tool.cbSize	= sizeof( tool );
				tool.hinst	= ClsGetInstanceHandle();
				tool.hwnd	= m_hWnd;
				tool.lpszText	= _T( "Scroll Right" );
				::SetRect( &tool.rect, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT );
				tool.uFlags	= 0;
				tool.uId	= ( UINT )ectcToolTip_ScrollRight;
				m_tooltip.AddTool( tool );
				tool.lpszText	= _T( "Scroll Left" );
				::SetRect( &tool.rect, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT );
				tool.uId	= ( UINT )ectcToolTip_ScrollLeft;
				m_tooltip.AddTool( tool );
			}
		}

		if( CTCS_CLOSEBUTTON == ( dwStyle & CTCS_CLOSEBUTTON ))
		{
			if( m_tooltip.GetSafeHWND())
			{
				TOOLINFO tool = { 0 };

				tool.cbSize	= sizeof( tool );
				tool.hinst	= ClsGetInstanceHandle();
				tool.hwnd	= m_hWnd;
				tool.lpszText	= _T( "Close" );
				tool.uFlags	= 0;
				::SetRect( &tool.rect, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT );
				tool.uId	= ( UINT )ectcToolTip_Close;
				m_tooltip.AddTool( tool );
			}
		}
	}

	virtual void Uninitialize( void )
	{
		DWORD dwStyle = this->GetStyle();

		if( m_tooltip.GetSafeHWND())
		{
			if( CTCS_SCROLL == ( dwStyle & CTCS_SCROLL ))
			{
				TOOLINFO tool = { 0 };

				tool.cbSize	= sizeof( tool );
				tool.uId	= ( UINT )ectcToolTip_ScrollRight;
				m_tooltip.DelTool( tool );
				tool.uId	= ( UINT )ectcToolTip_ScrollLeft;
				m_tooltip.DelTool( tool );
			}

			if( CTCS_CLOSEBUTTON == ( dwStyle & CTCS_CLOSEBUTTON ))
			{
				TOOLINFO tool = { 0 };

				tool.cbSize	= sizeof( tool );
				tool.uId	= ( UINT )ectcToolTip_Close;
				m_tooltip.DelTool( tool );
			}
		}

		this->DeleteAllItems();

		if( m_tooltip.GetSafeHWND())
		{
			// Also sets the contained m_hWnd to NULL
			m_tooltip.Destroy();
		}
		else
		{
			m_tooltip.Detach();
		}
	}

	virtual TItem* CreateNewItem( void* pInitData = NULL )
	{
		pInitData; // avoid level 4 warning
		TItem* pNewItem = new TItem;
		return pNewItem;
	}

	virtual void DeleteItem( TItem* pOldItem )
	{
		delete pOldItem;
	}

	virtual void UpdateLayout( void )
	{
		if( ! m_hWnd ||
		    ! GetSafeHWND() ||
		  ( ectcEnableRedraw != ( m_dwState & ectcEnableRedraw )))
		{
			return;
		}

		::GetClientRect( m_hWnd, &m_rcTabItemArea );
		DWORD dwStyle = this->GetStyle();

		this->CalcSize_NonClient( &m_rcTabItemArea );

		if( CTCS_CLOSEBUTTON == ( dwStyle & CTCS_CLOSEBUTTON ))
		{
			if(( m_iCurSel >= 0 ) && (( size_t )m_iCurSel < ( size_t )m_Items.GetSize()))
			{
				TItem* pItem = m_Items[ m_iCurSel ];
				_ASSERT( pItem != NULL );
				if(( pItem != NULL ) && pItem->CanClose())
				{
					this->CalcSize_CloseButton( &m_rcTabItemArea );
				}
			}
		}

		if( CTCS_SCROLL == ( dwStyle & CTCS_SCROLL ))
		{
			this->CalcSize_ScrollButtons( &m_rcTabItemArea );
			this->UpdateLayout_ScrollToFit( m_rcTabItemArea );
			this->UpdateScrollOverflowStatus();
		}
		else
		{
			this->UpdateLayout_Default( m_rcTabItemArea );
		}

		this->UpdateTabItemTooltipRects();
	}

	virtual void CalcSize_NonClient( LPRECT prcTabItemArea )
	{
	}

	virtual void CalcSize_CloseButton( LPRECT prcTabItemArea )
	{
	}

	virtual void CalcSize_ScrollButtons( LPRECT prcTabItemArea )
	{
	}

	virtual void UpdateLayout_Default( RECT rcTabItemArea )
	{
		UpdateLayout_ScrollToFit( rcTabItemArea );
	}

	virtual void UpdateLayout_ScrollToFit( RECT rcTabItemArea )
	{
		ClsGetDC dc( this );
		ClsFont font;
		this->GetFont( font );
		HGDIOBJ hOldFont = dc.SelectObject( font );    

		int height = rcTabItemArea.bottom - rcTabItemArea.top;

		// Reposition tabs
		size_t nCount = m_Items.GetSize();
		int xpos = m_settings.iIndent;
		for( size_t i=0; i<nCount; ++i )
		{
			TItem* pItem = m_Items[ ( int )i ];
			_ASSERT( pItem != NULL );
			if( pItem )
			{
				RECT rc = { xpos, 0, xpos, height };
				if( pItem->UsingText())
				{
					RECT rcText = { 0 };
					ClsString sText = pItem->GetText();
					dc.DrawText( sText, sText.GetStringLength(), &rcText, DT_SINGLELINE | DT_CALCRECT );
					rc.right += ( rcText.right - rcText.left ) + ( m_settings.iPadding * 2 );
				}
				pItem->SetRect( rc );
				xpos += ( rc.right - rc.left ) + m_settings.iMargin;
			}
		}
		if(( m_iCurSel >= 0 ) && (( size_t )m_iCurSel < nCount ))
		{
			TItem* pItem = m_Items[ m_iCurSel ];
			_ASSERT( pItem != NULL );
			if( pItem )
			{
				pItem->InflateRect( m_settings.iSelMargin, 0 );
			}
		}

		dc.SelectObject( hOldFont );
	}

	virtual void UpdateTabItemTooltipRects( void )
	{
		// The way we implement tooltips for tab items
		// is to have as many "tools" as there are tabs.
		// The relationship of tool ID => tab index is:
		// tool ID = tab index + 1	(to avoid 0 as an ID)
		//
		// We supply the RECT here and text elsewhere.
		if( m_tooltip.GetSafeHWND())
		{
			size_t i = 0;
			size_t nCount = m_Items.GetSize();
			RECT rcIntersect = { 0 };
			for( i = 0; i < nCount; ++i )
			{
				RECT rcItemDP = { 0 };
				this->GetItemRect( i, &rcItemDP );
				::IntersectRect( &rcIntersect, &rcItemDP, &m_rcTabItemArea );

				// NOTE: Even if IntersectRect determines the rectangles
				// don't intersect at all, we still need
				// to update the tool rect, or we'll get the wrong
				// tooltip in some cases.
				TOOLINFO tool = { 0 };
				tool.hwnd = m_hWnd;
				tool.uId =  i + 1;
				tool.rect = rcIntersect;
				m_tooltip.NewToolRect( tool );
			}
		}
	}

	virtual void ScrollLeft( bool bForceRedraw = true )
	{
		m_iScrollOffset += (( m_dwState & ectcScrollDeltaMask ) >> ectcScrollDeltaShift );

		this->UpdateLayout();
		this->UpdateScrollOverflowStatus();
		this->UpdateTabItemTooltipRects();
		if( bForceRedraw )
		{
			this->Invalidate();
			// If something a little more forceful is needed:
			//::RedrawWindow(m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}
	}

	virtual void ScrollRight( bool bForceRedraw = true )
	{
		m_iScrollOffset -= (( m_dwState & ectcScrollDeltaMask ) >> ectcScrollDeltaShift );

		this->UpdateLayout();
		this->UpdateScrollOverflowStatus();
		this->UpdateTabItemTooltipRects();
		if( bForceRedraw )
		{
			this->Invalidate();
			// If something a little more forceful is needed:
			//::RedrawWindow(m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}
	}

	virtual void UpdateScrollOverflowStatus( void )
	{
		// Check for overflow left
		if( m_iScrollOffset >= 0 )
		{
			m_iScrollOffset = 0;
			m_dwState &= ~ectcOverflowLeft;
		}
		else
		{
			m_dwState |= ectcOverflowLeft;
		}

		// Check for overflow right
		m_dwState &= ~ectcOverflowRight;

		size_t nCount = m_Items.GetSize();
		if( nCount > 0 )
		{
			// Check last item
			RECT rcItemDP = { 0 };
			this->GetItemRect( nCount - 1, &rcItemDP );
			if( rcItemDP.right > m_rcTabItemArea.right )
			{
				m_dwState |= ectcOverflowRight;
			}
		}
	}

	virtual void DoPaint( ClsDC dc, RECT &rcClip )
	{
		// Save current DC selections
		int nSave = dc.SaveDC();
		_ASSERT( nSave != 0 );

		DWORD dwStyle = this->GetStyle();

		// Make sure we don't paint outside client area (possible with paint dc)
		RECT rcClient = { 0 };
		::GetClientRect( m_hWnd, &rcClient );
		dc.IntersectClipRect( &rcClient );

		// Prepare DC
		ClsFont font;
		this->GetFont( font );
		dc.SelectObject( font );

		LRESULT lResCustom;
		NMCTCCUSTOMDRAW nmc = { 0 };
		LPNMCUSTOMDRAW pnmcd= &( nmc.nmcd );
		pnmcd->hdr.hwndFrom = m_hWnd;
		pnmcd->hdr.idFrom = this->GetDlgCtrlID();
		pnmcd->hdr.code = NM_CUSTOMDRAW;
		pnmcd->hdc = dc;
		pnmcd->uItemState = 0;

		this->InitializeDrawStruct( &nmc );

		pnmcd->dwDrawStage = CDDS_PREPAINT;
		lResCustom = ::SendMessage( GetParentHandle(), WM_NOTIFY, pnmcd->hdr.idFrom, ( LPARAM )&nmc );

		if( CDRF_SKIPDEFAULT != ( lResCustom & CDRF_SKIPDEFAULT ))
		{
			this->DoPrePaint( rcClient, &nmc );
		}

		if( lResCustom == CDRF_DODEFAULT || CDRF_NOTIFYITEMDRAW == ( lResCustom & CDRF_NOTIFYITEMDRAW ))
		{
			POINT ptPreviousViewport = { 0 };
			HRGN hRgnClip = NULL;

			bool bScrollStyle = ( CTCS_SCROLL == ( dwStyle & CTCS_SCROLL ));
			bool bHotTrackStyle = ( CTCS_HOTTRACK == ( dwStyle & CTCS_HOTTRACK ));

			if( bScrollStyle )
			{
				// Remember clip region before we modify it
				//  to only include the tab item area
				// NOTE: GetClipRgn expects an already created
				//  region, so we create one with an empty rectangle.
				hRgnClip = ::CreateRectRgn( 0,0,0,0 );
				::GetClipRgn( dc, hRgnClip );

				dc.IntersectClipRect( &m_rcTabItemArea );

				ClsPoint pt = dc.GetViewportOrg(); 
				ptPreviousViewport = pt;
				dc.SetViewportOrg( m_iScrollOffset, 0 );
			}

			size_t nCount = m_Items.GetSize();
			// Draw the list items, except the selected one. It is drawn last
			// so it can cover the tabs below it.
			RECT rcIntersect = { 0 };
			for( size_t i = 0; i < nCount; ++i )
			{
				if(( int )i != m_iCurSel )
				{
					TItem* pItem = m_Items[ ( int )i ];
					_ASSERT( pItem != NULL );
					if( pItem )
					{
						RECT rcItemLP = { 0 }, rcItemDP = { 0 };
						rcItemLP = pItem->GetRect();

						::CopyRect( &rcItemDP, &rcItemLP );
						::OffsetRect( &rcItemDP, m_iScrollOffset, 0 );

						if( ::IntersectRect( &rcIntersect, &rcItemDP, &rcClip ))
						{
							pnmcd->dwItemSpec = i;
							pnmcd->uItemState = 0;
							if( bHotTrackStyle && (( DWORD )m_iHotItem == i ))
							{
								pnmcd->uItemState |= CDIS_HOT;
							}
							if( pItem->IsHighlighted())
							{
								pnmcd->uItemState |= CDIS_MARKED;
							}
							pnmcd->rc = rcItemLP;
							this->ProcessItem( lResCustom, &nmc );
						}
					}
				}
			}
			if( m_iCurSel >= 0 && ( size_t )m_iCurSel < nCount )
			{
				TItem* pItem = m_Items[ m_iCurSel ];
				_ASSERT( pItem != NULL );
				if( pItem )
				{
					RECT rcItemLP = { 0 }, rcItemDP = { 0 };
					rcItemLP = pItem->GetRect();

					::CopyRect( &rcItemDP, &rcItemLP );
					::OffsetRect( &rcItemDP, m_iScrollOffset, 0 );

					if( ::IntersectRect( &rcIntersect, &rcItemDP, &rcClip ))
					{
						pnmcd->dwItemSpec = m_iCurSel;
						pnmcd->uItemState = CDIS_SELECTED;
						if( bHotTrackStyle && ( m_iHotItem == m_iCurSel ))
						{
							pnmcd->uItemState |= CDIS_HOT;
						}
						if( pItem->IsHighlighted())
						{
							pnmcd->uItemState |= CDIS_MARKED;
						}
						pnmcd->rc = rcItemLP;
						this->ProcessItem( lResCustom, &nmc );
					}
				}
			}
			pnmcd->uItemState = 0;
			pnmcd->dwItemSpec = 0;

			if( bScrollStyle )
			{
				dc.SetViewportOrg( ptPreviousViewport );

				ClsRgn rgn;
				rgn.Attach( hRgnClip );
				dc.SelectClipRgn( &rgn );
			}
		}

		if( CDRF_SKIPDEFAULT != ( lResCustom & CDRF_SKIPDEFAULT ))
		{
			this->DoPostPaint( rcClient, &nmc );
		}

		if( CDRF_NOTIFYPOSTPAINT == ( lResCustom & CDRF_NOTIFYPOSTPAINT ))
		{
			pnmcd->dwDrawStage = CDDS_POSTPAINT;
			pnmcd->dwItemSpec = 0;
			pnmcd->uItemState = 0;
			pnmcd->rc = rcClient;
			::SendMessage( GetParentHandle(), WM_NOTIFY, pnmcd->hdr.idFrom, ( LPARAM )&nmc );
		}

		dc.RestoreDC( nSave );
	}

	virtual void ProcessItem( LRESULT lResCustom, LPNMCTCCUSTOMDRAW lpNMCustomDraw )
	{
		LRESULT lResItem = CDRF_DODEFAULT;
		if( CDRF_NOTIFYITEMDRAW == (lResCustom & CDRF_NOTIFYITEMDRAW) )
		{
			lpNMCustomDraw->nmcd.dwDrawStage = CDDS_ITEMPREPAINT;
			lResItem = ::SendMessage( GetParentHandle(), WM_NOTIFY, lpNMCustomDraw->nmcd.hdr.idFrom, ( LPARAM )lpNMCustomDraw );
		}
		if( CDRF_SKIPDEFAULT != ( lResCustom & CDRF_SKIPDEFAULT ))
		{
			// Do default item-drawing
			this->DoItemPaint( lpNMCustomDraw );
		}
		if( CDRF_NOTIFYITEMDRAW == ( lResCustom & CDRF_NOTIFYITEMDRAW ) &&
		    CDRF_NOTIFYPOSTPAINT == ( lResItem & CDRF_NOTIFYPOSTPAINT ))
		{
			lpNMCustomDraw->nmcd.dwDrawStage = CDDS_ITEMPOSTPAINT;
			::SendMessage( GetParentHandle(), WM_NOTIFY, lpNMCustomDraw->nmcd.hdr.idFrom, ( LPARAM )lpNMCustomDraw );
		}
	}

	virtual void InitializeDrawStruct( LPNMCTCCUSTOMDRAW lpNMCustomDraw )
	{
//		DWORD dwStyle = this->GetStyle();

		lpNMCustomDraw->hFontInactive = m_font;
		lpNMCustomDraw->hFontSelected = m_fontSel;
		lpNMCustomDraw->hBrushBackground = ::GetSysColorBrush( COLOR_BTNFACE );
		lpNMCustomDraw->clrTextSelected = ::GetSysColor( COLOR_BTNTEXT );
		lpNMCustomDraw->clrTextInactive = ::GetSysColor( COLOR_BTNTEXT );
		lpNMCustomDraw->clrSelectedTab = ::GetSysColor( COLOR_BTNFACE );
		lpNMCustomDraw->clrBtnFace = ::GetSysColor( COLOR_BTNFACE );
		lpNMCustomDraw->clrBtnShadow = ::GetSysColor( COLOR_BTNSHADOW );
		lpNMCustomDraw->clrBtnHighlight = ::GetSysColor( COLOR_BTNHIGHLIGHT );
		lpNMCustomDraw->clrBtnText = ::GetSysColor( COLOR_BTNTEXT );
		lpNMCustomDraw->clrHighlight = ::GetSysColor( COLOR_HIGHLIGHT );
#if WINVER >= 0x0500 || _WIN32_WINNT >= 0x0500
		lpNMCustomDraw->clrHighlightHotTrack = ::GetSysColor( COLOR_HOTLIGHT );
#else
		lpNMCustomDraw->clrHighlightHotTrack = ::GetSysColor( COLOR_HIGHLIGHT );
#endif
		lpNMCustomDraw->clrHighlightText = ::GetSysColor( COLOR_HIGHLIGHTTEXT );
	}

	virtual void DoPrePaint( RECT rcClient, LPNMCTCCUSTOMDRAW lpNMCustomDraw )
	{
		// "Erase Background"
		// NOTE: Your derived class might be able to do a
		//  better job of erasing only the necessary area
		//  (using the clip box, etc.)
		ClsDC dc( lpNMCustomDraw->nmcd.hdc );

		HGDIOBJ hOldBrush = dc.SelectObject( lpNMCustomDraw->hBrushBackground );
		dc.PatBlt( rcClient.left, rcClient.top, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, PATCOPY );
		dc.SelectObject( hOldBrush );

		dc.SetTextColor( lpNMCustomDraw->clrTextInactive );
		dc.SetBkMode( TRANSPARENT );
	}

	virtual void DoItemPaint( LPNMCTCCUSTOMDRAW /*lpNMCustomDraw*/ )
	{
	}

	virtual void DoPostPaint( RECT /*rcClient*/, LPNMCTCCUSTOMDRAW /*lpNMCustomDraw*/ )
	{
		// In your derived verion, paint the scroll buttons if CTCS_SCROLL
		// is set and the close button if CTCS_CLOSEBUTTON is set
		// (if you want to support these)
	}

	// Operations
public:
	BOOL SubclassWindow( HWND hWnd )
	{
		_ASSERT( m_hWnd == NULL );
		_ASSERT( ::GetSafeHWND( hWnd ));
		BOOL bRet = baseClass::Attach( hWnd );
		if( bRet )
		{
			this->Initialize();
		}
		return bRet;
	}

	HWND UnsubclassWindow( BOOL bForce = FALSE )
	{
		bForce;
		this->Uninitialize();
		return baseClass::Detach();
	}

	ClsImageList SetImageList( HIMAGELIST hImageList )
	{
		ClsImageList imageListOld = m_imageList;
		m_imageList = m_imageList.Attach( hImageList );
		return imageListOld;
	}

	ClsImageList& GetImageList() const
	{
		return m_imageList;
	}

	ClsTooltip GetTooltips() const
	{
		return m_tooltip;
	}

	//void SetTooltips(HWND hWndToolTip)
	//{
	//	ATLASSERT(::GetSafeHWND(m_hWnd));
	//	::SendMessage(m_hWnd, TCM_SETTOOLTIPS, (WPARAM)hWndToolTip, 0L);
	//}

	bool SetScrollDelta( UINT nDelta )
	{
		if( nDelta > ( ectcScrollDeltaMask >> ectcScrollDeltaShift ))
		{
			return false;
		}
		m_dwState |= (( nDelta << ectcScrollDeltaShift ) & ectcScrollDeltaMask );

		return true;
	}

	UINT GetScrollDelta( void )
	{
		return (( m_dwState & ectcScrollDeltaMask ) >> ectcScrollDeltaShift );
	}

	bool SetScrollRepeat( ScrollRepeat ectcNewScrollRepeat = ectcScrollRepeat_Normal )
	{
		m_dwState &= ~ectcScrollRepeat;
		m_dwState |= ( ectcNewScrollRepeat & ectcScrollRepeat );

		return true;
	}

	ScrollRepeat GetScrollRepeat( void )
	{
		return ( m_dwState & ectcScrollRepeat );
	}

	int InsertItem( int nItem, LPCTSTR sText = NULL, int nImage = -1, LPCTSTR sToolTip = NULL, bool bSelectItem = false )
	{
		TItem* pItem = this->CreateNewItem();
		if( pItem )
		{
			pItem->SetText( sText );
			pItem->SetImageIndex( nImage );
			pItem->SetToolTip( sToolTip );

			return InsertItem( nItem, pItem, bSelectItem );
		}
		return -1;
	}

	int InsertItem( int nItem, TItem* pItem, bool bSelectItem = false )
	{
		_ASSERT( GetSafeHWND());
		_ASSERT( pItem );
		_ASSERT( nItem >= 0 && nItem <= ( int )m_Items.GetSize());
		if( ! GetSafeHWND() || pItem == NULL )
		{
			return -1;
		}

		size_t nOldCount = ( size_t )m_Items.GetSize();

		if( nItem < 0 || nItem > (int)nOldCount )
		{
			nItem = (int)nOldCount;
		}

		m_Items.InsertAt( &pItem, nItem, 1 );

		size_t nNewCount = m_Items.GetSize();

		// Send notification
		NMCTCITEM nmh = {{ m_hWnd, this->GetDlgCtrlID(), CTCN_INSERTITEM }, nItem, { -1, -1 }};
		::SendMessage( GetParentHandle(), WM_NOTIFY, nmh.hdr.idFrom, ( LPARAM )&nmh );
		// Select if first tab
		if( nNewCount == 1 )
		{
			this->SetCurSel( 0 );
		}
		else if( bSelectItem )
		{
			this->SetCurSel( nItem );
		}

		// The way we implement tooltips is to have as many
		// "tools" as there are tabs.  The relationship of
		// tool ID => tab index is:
		// tool ID = tab index + 1	(to avoid 0 as an ID)
		//
		// We supply the RECT and text elsewhere.
		if( m_tooltip.GetSafeHWND())
		{
			TOOLINFO tool = { 0 };
			tool.cbSize	= sizeof( tool );
			tool.hinst	= ClsGetInstanceHandle();
			tool.hwnd	= m_hWnd;
			tool.lpszText	= LPSTR_TEXTCALLBACK;
			tool.uFlags	= 0;
			::SetRect( &tool.rect, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT );
			tool.uId	= nNewCount;
			m_tooltip.AddTool( tool );
		}

		this->UpdateLayout();
		this->Invalidate();
		return nItem;
	}

	BOOL MoveItem( size_t nFromIndex, size_t nToIndex, bool bUpdateSelection = true, bool bNotify = true )
	{
		_ASSERT( GetSafeHWND());
		_ASSERT( nFromIndex < ( size_t )m_Items.GetSize());
		_ASSERT( nToIndex < ( size_t )m_Items.GetSize());

		if( ! GetSafeHWND() || nFromIndex >= ( size_t )m_Items.GetSize() || nToIndex >= ( size_t )m_Items.GetSize())
		{
			return FALSE;
		}

		TItem *pFromItem = m_Items[ ( int )nFromIndex ];
		_ASSERT( pFromItem != NULL );
		m_Items.RemoveAt(( int )nFromIndex, 1 );
		m_Items.InsertAt( &pFromItem, ( int )nToIndex, 1 );

		// The number of items is staying the same, so m_iCurSel
		// won't be invalid whether it gets updated or not
		if( bUpdateSelection )
		{
			if( nFromIndex == ( size_t )m_iCurSel )
			{
				this->SetCurSel(( int )nToIndex );
			}
		}

		if( bNotify )
		{
			NMCTC2ITEMS nmh = {{ m_hWnd, this->GetDlgCtrlID(), CTCN_MOVEITEM }, ( int )nFromIndex, ( int )nToIndex, { -1, -1 }};
			::SendMessage( GetParentHandle(), WM_NOTIFY, nmh.hdr.idFrom, ( LPARAM )&nmh );
		}

		return TRUE;
	}

	BOOL SwapItemPositions( size_t nFromIndex, size_t nToIndex, bool bUpdateSelection = true, bool bNotify = true )
	{
		_ASSERT( ::GetSafeHWND(m_hWnd));
		_ASSERT( nFromIndex < m_Items.GetSize());
		_ASSERT( nToIndex < m_Items.GetSize());

		if( ! ::GetSafeHWND( m_hWnd ) || nFromIndex >= m_Items.GetSize() || nToIndex >= m_Items.GetSize())
		{
			return FALSE;
		}

		TItem* pFromItem = m_Items[ nFromIndex ];
		TItem* pToItem = m_Items[ nToIndex ];
		_ASSERT( pFromItem != NULL );
		_ASSERT( pToItem != NULL );
		m_Items[ nFromIndex ] = pToItem;
		m_Items[ nToIndex ] = pFromItem;

		// The number of items is staying the same, so m_iCurSel
		// won't be invalid whether it gets updated or not
		if( bUpdateSelection )
		{
			if( nFromIndex == ( size_t )m_iCurSel )
			{
				this->SetCurSel( nToIndex );
			}
			else if( nToIndex == ( size_t )m_iCurSel )
			{
				this->SetCurSel( nFromIndex );
			}
		}

		if( bNotify )
		{
			NMCTC2ITEMS nmh = {{ m_hWnd, this->GetDlgCtrlID(), CTCN_SWAPITEMPOSITIONS }, nFromIndex, nToIndex, { -1, -1 }};
			::SendMessage( GetParentHandle(), WM_NOTIFY, nmh.hdr.idFrom, ( LPARAM )&nmh );
		}

		return TRUE;
	}

	BOOL DeleteItem( size_t nItem, bool bUpdateSelection = true, bool bNotify = true )
	{
		_ASSERT( GetSafeHWND());
		size_t nOldCount = m_Items.GetSize();
		if( nItem >= nOldCount )
		{
			return FALSE;
		}

		// Send notification
		if( bNotify )
		{
			// Returning TRUE tells us not to delete the item
			NMCTCITEM nmh = {{ m_hWnd, this->GetDlgCtrlID(), CTCN_DELETEITEM }, ( int )nItem, { -1,-1 }};
			if( TRUE == ::SendMessage( GetParentHandle(), WM_NOTIFY, nmh.hdr.idFrom, ( LPARAM )&nmh ))
			{
				// Cancel the attempt
				return FALSE;
			}
			else
			{
				// Just in case handler of the notification
				// changed the count somehow, get it again
				nOldCount = m_Items.GetSize();
				if( nItem >= nOldCount )
				{
					return FALSE;
				}
			}
		}

		int nPostDeleteSelection = -1;

		// The number of items is changing, so m_iCurSel
		// might be invalid if we don't change it.
		if( nOldCount <= 1 )
		{
			// There's only one item left,
			// and its being deleted.
			m_iCurSel = -1;
		}
		else if( bUpdateSelection )
		{
			if(( int )nItem < m_iCurSel )
			{
				// The item being removed is before the current selection.
				// We still want the same item to be selected, but
				// the index needs to be adjusted to account for the missing item
				m_iCurSel--;
				this->EnsureVisible( m_iCurSel );
				// NOTE: We don't call SetCurSel because we don't want
				//  any of the notifications to go to the parent - because
				//  the selection didn't change, the index just had to be adjusted.
			}
			else if(( int )nItem == m_iCurSel )
			{
				// If the item to be deleted is currently selected,
				//  select something else.
				// NOTE: We've already handled the "we're deleting
				//  the only remaining item" case.
				if( nItem >= ( nOldCount - 1 ))
				{
					// The selected item was the last item,
					// and there will still be at least one
					// item after this deletion occurs.
					// Select the item that will be the
					// new last item.
					// We need to do this before the actual
					// deletion so that m_iCurSel
					// will still be valid during the SetCurSel
					// call and the notification handlers of
					// CTCN_SELCHANGING and CTCN_SELCHANGE
					this->SetCurSel( m_iCurSel - 1 );
				}
				else
				{
					// The selected item was NOT the last item,
					// and there will still be at least one
					// item after this deletion occurs.
					// Force a selection of the item that will
					// have the same index as the selected item being
					// deleted, but do it after the actual deletion.
					//pT->SetCurSel(m_iCurSel);
					nPostDeleteSelection = m_iCurSel;
				}
			}
		}
		else
		{
			if((( int )nItem == m_iCurSel ) && ( nItem >= ( nOldCount - 1 )))
			{
				// If bUpdateSelection is false,
				// the item being deleted is selected,
				// and the item being deleted is the last
				// item, we need to clear the current selection
				// (setting m_iCurSel to -1)
				// so that our call to UpdateLayout
				// and our paint message handling don't
				// crash and burn with an invalid selected
				// item index. Its likely that the
				// caller is going to SetCurSel right
				// after this call to DeleteItem.
				m_iCurSel = -1;
			}
		}

		// Remove tooltip and item

		// The way we implement tooltips is to have as many
		// "tools" as there are tabs.  The relationship of
		// tool ID => tab index is:
		// tool ID = tab index + 1	(to avoid 0 as an ID)
		//
		// We supply the RECT and text elsewhere.
		if( m_tooltip.GetSafeHWND())
		{
			TOOLINFO tool = { 0 };
			tool.cbSize	= sizeof( tool );
			tool.uId	= m_Items.GetSize();
			m_tooltip.DelTool( tool );
		}

		TItem* pItem = m_Items[ ( int )nItem ];
		_ASSERT( pItem != NULL );
		m_Items.RemoveAt(( int ) nItem, 1 );

		this->DeleteItem( pItem );

		if( nPostDeleteSelection >= 0 )
		{
			this->SetCurSel( nPostDeleteSelection );
		}

		// Repaint
		this->UpdateLayout();
		this->Invalidate();
		return TRUE;
	}

	BOOL DeleteAllItems( bool bForceRedraw = false )
	{
		_ASSERT( GetSafeHWND());
		m_iCurSel = -1;

		this->SendMessage( WM_SETREDRAW, FALSE, 0 );

		while( GetItemCount() > 0 ) DeleteItem( 0U, false, true );

		this->SendMessage( WM_SETREDRAW, TRUE, 0 );

		// UpdateLayout will already have been called
		// when sending WM_SETREDRAW with TRUE after
		// having sent it with FALSE
		// (or in DeleteItem if we weren't doing WM_SETREDRAW stuff)
		if( bForceRedraw )
		{
			this->Invalidate();
			// If something a little more forceful is needed:
			//::RedrawWindow(m_hWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
		}

		return TRUE;
	}

	// NOTE: Now, instead of calling SetItem, call
	//   GetItem to get the pointer to the TItem, update
	//   what you want, then call UpdateLayout and Invalidate
	//   if appropriate.
	/*
	BOOL SetItem(int nItem, TItem* pItem)
	{
	ATLASSERT(::GetSafeHWND(m_hWnd));
	ATLASSERT(!::IsBadReadPtr(pItem,sizeof(TItem)));
	CHECK_ITEM(nItem);

	// Copy caller's data to the internal structure
	TItem* pItemT = m_Items[nItem];
	UINT mask = pItem->mask;
	if( mask & TCIF_TEXT )
	{
	pItemT->SetText(pItem->pszText);
	}
	// PBI: Added support for ImageList.
	if( mask & TCIF_IMAGE )
	{
	pItemT->iImage = pItem->iImage;
	pItemT->mask |= TCIF_IMAGE;
	}
	if( mask & TCIF_PARAM )
	{
	pItemT->lParam = pItem->lParam;
	pItemT->mask |= TCIF_PARAM;
	}

	// Repaint control
	T* pT = static_cast<T*>(this);
	pT->UpdateLayout();
	this->Invalidate();
	return TRUE;
	}
	*/

	TItem* GetItem( size_t nItem ) const
	{
		_ASSERT( nItem < ( size_t )m_Items.GetSize());
		if( nItem >= ( size_t )m_Items.GetSize())
		{
			return NULL;
		}

		return m_Items[ ( int )nItem ];
	}

	int SetCurSel( int nItem, bool bNotify = true )
	{
		_ASSERT( GetSafeHWND());

		// NEW (DDB):
		// Even if the newly requested selection index is
		// the same index as the previous selection index,
		// the item might be different (as in the case of
		// InsertItem inserting a new item where the old
		// selection used to be).  We also call EnsureVisible
		// and UpdateLayout in this method, which we will want
		// called even if it is the same item
		//
		// OLD:
		// // Selecting same tab?  If so, we won't go through all the notifications
		// if( (int)nItem==m_iCurSel ) return m_iCurSel;

		if( nItem >= ( int )m_Items.GetSize())
		{
			nItem = m_iCurSel;
		}

		int iOldSel = m_iCurSel;
		// Send notification
		NMCTC2ITEMS nmh = {{ m_hWnd, this->GetDlgCtrlID(), CTCN_SELCHANGING }, iOldSel, nItem, { -1, -1 }};
		if( bNotify )
		{
			if( TRUE == ::SendMessage( GetParentHandle(), WM_NOTIFY, nmh.hdr.idFrom, ( LPARAM )&nmh ))
			{
				// Cancel the attempt
				return -1;
			}
		}

		// Change tab
		m_iCurSel = nItem;

		if( m_iCurSel >= 0 )
		{
			this->EnsureVisible( m_iCurSel );
		}

		// Recalc new layout and redraw
		this->UpdateLayout();
		this->Invalidate();

		if( bNotify )
		{
			// Send notification
			nmh.hdr.code = CTCN_SELCHANGE;
			::SendMessage( GetParentHandle(), WM_NOTIFY, nmh.hdr.idFrom, ( LPARAM )&nmh );
		}
		return iOldSel;
	}

	int GetCurSel() const
	{
		return m_iCurSel;
	}

	int GetItemCount() const
	{
		return (int)m_Items.GetSize();
	}

	int HitTest( LPCTCHITTESTINFO pHitTestInfo ) const
	{
		_ASSERT( ! ::IsBadWritePtr( pHitTestInfo,sizeof( CTCHITTESTINFO )));
		pHitTestInfo->flags = CTCHT_NOWHERE;
		if( ::PtInRect( &m_rcTabItemArea, pHitTestInfo->pt ))
		{
			// TODO: Do a smarter search.  Currently,
			//  the tabs are always going to be left to right.
			//  Use this knowledge to do a better spacial search.

			RECT rcItemDP = { 0 };
			size_t nCount = m_Items.GetSize();
			for( size_t i = 0; i < nCount; ++i )
			{
				// NOTE: GetItemRect accounts for any scroll
				this->GetItemRect( i, &rcItemDP );

				if( ::PtInRect( &rcItemDP, pHitTestInfo->pt ))
				{
					// TODO: check for ONITEMLABEL, ONITEMICON
					pHitTestInfo->flags = CTCHT_ONITEM;
					return ( int )i;
				}
			}
		}
		else
		{
			if( ::PtInRect( &m_rcCloseButton, pHitTestInfo->pt ))
			{
				pHitTestInfo->flags = CTCHT_ONCLOSEBTN;
			}
			else if( ::PtInRect( &m_rcScrollRight, pHitTestInfo->pt ))
			{
				pHitTestInfo->flags = CTCHT_ONSCROLLRIGHTBTN;
			}
			else if( ::PtInRect( &m_rcScrollLeft, pHitTestInfo->pt ))
			{
				pHitTestInfo->flags = CTCHT_ONSCROLLLEFTBTN;
			}
		}
		return -1;
	}

	bool EnsureVisible( int nItem, bool bPartialOK = false, bool bRecalcAndRedraw = false )
	{
		bool bAdjusted = false;

		// Adjust scroll offset so that item is visible
		if( 0 != ( m_dwState & ( ectcOverflowLeft|ectcOverflowRight )))
		{
			if( nItem < 0 || nItem >= ( int )m_Items.GetSize())
			{
				return false;
			}

			// TODO: Depend on some system metric for this value
			int nScrollToViewPadding = 20;

			RECT rcItemDP = { 0 };
			this->GetItemRect( nItem, &rcItemDP );
			if( rcItemDP.left < m_rcTabItemArea.left )
			{
				if( ! bPartialOK || ( rcItemDP.right < m_rcTabItemArea.left ))
				{
					m_iScrollOffset += ( m_rcTabItemArea.left - rcItemDP.left ) + nScrollToViewPadding;
					bAdjusted = true;
				}
			}
			else if( rcItemDP.right > m_rcTabItemArea.right )
			{
				if( ! bPartialOK || ( rcItemDP.left > m_rcTabItemArea.right ))
				{
					m_iScrollOffset -= ( rcItemDP.right - m_rcTabItemArea.right ) + nScrollToViewPadding;
					bAdjusted = true;
				}
			}

			// Note: UpdateLayout should call UpdateScrollOverflowStatus which
			//  will catch m_iScrollOffset being scrolled too far either direction
		}

		if( bAdjusted && bRecalcAndRedraw )
		{
			if( bRecalcAndRedraw )
			{
				this->UpdateLayout();
				this->Invalidate();
			}
			else
			{
				this->UpdateScrollOverflowStatus();
				this->UpdateTabItemTooltipRects();
			}
		}

		return true;
	}

	int GetRowCount() const
	{
		return 1;
	}

	DWORD SetItemSize( size_t nItem, int cx, int cy )
	{
		_ASSERT( ::GetSafeHWND( m_hWnd ));
		_ASSERT( nItem < m_Items.GetSize());
		if( nItem >= m_Items.GetSize())
		{
			return 0;
		}

		// TODO: Review this method.  It seems that all the tabs
		//  after the one being set would have the wrong RECT.
		//  (unless the caller is iterating through all of the items)

		TItem* pItem = m_Items[ nItem ];
		_ASSERT( pItem != NULL );

		RECT rcOld = pItem->GetRect();
		RECT rcNew = { rcOld.left, rcOld.top, rcOld.left + cx, rcOld.top cy };
		pItem->SetRect( rcNew );
		this->UpdateLayout();
		this->Invalidate();
		return MAKELONG( rcOld.right - rcOld.left, rcOld.bottom - rcOld.top );
	}

	BOOL GetItemRect( size_t nItem, RECT *prcItem ) const
	{
		_ASSERT( prcItem );
		if( prcItem == NULL ) return FALSE;
		if( nItem >= ( size_t )m_Items.GetSize())
		{
			::SetRectEmpty( prcItem );
			return FALSE;
		}
		TItem* pItem = m_Items[ ( int )nItem ];
		_ASSERT( pItem != NULL );
		if( pItem )
		{
			*prcItem = pItem->GetRect();
		}

		// Adjust for any scroll, so that the caller
		// gets the RECT in device coordinates
		// instead of logical coordinates
		::OffsetRect( prcItem, m_iScrollOffset, 0 );

		return TRUE;
	}

	BOOL HighlightItem( size_t nItem, bool bHighlight = true )
	{
		_ASSERT( nItem < m_Items.GetSize());
		if( nItem >= m_Items.GetSize())
		{
			return FALSE;
		}

		TItem* pItem = m_Items[ nItem ];
		_ASSERT( pItem != NULL );
		if( pItem )
		{
			bool bCurrentHighlight = pItem->IsHighlighted();
			if( bCurrentHighlight != bHighlight )
			{
				pItem->SetHighlighted( bHighlight );

				RECT rcItem = { 0 };
				this->GetItemRect( nItem, &rcItem );
				this->InvalidateRect( &rcItem );
			}
		}
		return TRUE;
	}

	void SetPadding( int iPadding ) 
	{ 
		m_iPadding = iPadding; 
		this->UpdateLayout();
		this->Invalidate();
	};

	// FindItem:    Find the next tab item matching the search criteria
	//              The functions is meant to mimic how
	//              CListViewCtrl::FindItem and LVM_FINDITEM work,
	//              since there are no comparable messages or functions
	//              for a tab control
	//
	//  eFlags should specify a mask of things to check for,
	//   and have the corresponding fields set in pFindItem.
	//   For example, set the flags to eCustomTabItem_TabView and set the
	//   tab view on pFindItem to search for a tab with a particular HWND.
	//  If nStart is -1, the search begins from the beginning.
	//   If nStart is not -1, the search begins with the item
	//   just after nStart (like with LVM_FINDITEM).
	//  If a matching item is found, its index is returned.
	//   Otherwise -1 is returned.
	int FindItem( TItem* pFindItem, DWORD eFlags, int nStart = -1 ) const
	{
		if( nStart < 0 )
		{
			nStart = -1;
		}

		// Find the next item matching the criteria specified
		size_t nCount = m_Items.GetSize();
		for( size_t i = ( nStart + 1 ); i < nCount; ++i )
		{
			if( m_Items[ i ]->MatchItem( pFindItem, eFlags ))
			{
				return i;
			}
		}

		return -1;
	}

	// Member variables
protected:
	int				m_iCurSel;
	int				m_iHotItem;
	CTCSETTINGS			m_settings;
	ClsSimpleArray< TItem* >	m_Items;
	ClsFont				m_font;
	ClsFont				m_fontSel;
	ClsImageList			m_imageList;
	ClsTooltip			m_tooltip;

	RECT				m_rcTabItemArea;
	RECT				m_rcScrollLeft;
	RECT				m_rcScrollRight;
	RECT				m_rcCloseButton;

	int				m_iDragItem;
	int				m_iDragItemOriginal;
	POINT				m_ptDragOrigin;
	HCURSOR				m_hCursorMove;
	HCURSOR				m_hCursorNoDrop;

	int				m_iScrollOffset;

	// Flags, internal state, etc.
	//
	//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
	//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
	//  +-------+-------+-------+-------+---+-----------+---------------+
	//  |  FUT  |  MO   |  MD   |  HT   |SR |    SD     |     FLAGS     |
	//  +-------+-------+-------+-------+---+-----------+---------------+
	//
	//  FLAGS - boolean flags
	//  SD - Scroll delta.  The number of pixels to move in a single scroll.
	//       Valid values are 0-63 (the value is bit shifted to/from position).
	//  SR - Scroll repeat speed.  Valid values are no-repeat,
	//       slow repeat, normal repeat and fast repeat
	//  HT - Current hot tracked item (if its a tab, then m_iHotItem is the hot tab item) 
	//  MD - Item under mouse when mouse button down message was sent
	//       but before mouse button up message is sent
	//  MO - Item current under mouse cursor
	//  FUT - Not used at this time, but reserved for the future.
	DWORD				m_dwState;
};

template < class TItem = ClsCustomTabItem, bool t_bDotNetAdjust = false >
class ClsCloseScrollTabCtrl : public ClsCustomTabCtrl< TItem >
{
	_NO_COPY( ClsCloseScrollTabCtrl );
protected:
	typedef ClsCustomTabCtrl< TItem > baseClass;

public:
	ClsCloseScrollTabCtrl() : m_bDotNetAdjust( t_bDotNetAdjust ) {;}
	virtual ~ClsCloseScrollTabCtrl() {;}

	virtual void DoPostPaint( RECT /*rcClient*/, LPNMCTCCUSTOMDRAW lpNMCustomDraw )
	{
		DWORD dwStyle = this->GetStyle();

		if( 0 == ( dwStyle & ( CTCS_CLOSEBUTTON | CTCS_SCROLL )))
		{
			return;
		}

		// Close Button
		if( CTCS_CLOSEBUTTON == ( dwStyle & CTCS_CLOSEBUTTON ))
		{
			if(( m_iCurSel >= 0 ) && (( size_t )m_iCurSel < ( size_t )m_Items.GetSize()))
			{
				TItem* pItem = m_Items[ m_iCurSel ];
				_ASSERT( pItem != NULL );
				if(( pItem != NULL ) && pItem->CanClose())
				{
					this->DrawCloseButton( lpNMCustomDraw );
				}
			}
		}

		// Scroll Buttons
		if( CTCS_SCROLL == ( dwStyle & CTCS_SCROLL ))
		{
			this->DrawScrollButtons( lpNMCustomDraw );
		}
	}

	virtual void DrawCloseButton( LPNMCTCCUSTOMDRAW lpNMCustomDraw )
	{
		ClsDC dc( lpNMCustomDraw->nmcd.hdc );

		ClsPen penButtons;
		penButtons.CreatePen( PS_SOLID, 1, lpNMCustomDraw->clrTextInactive );
		ClsBrush brushArrow;
		brushArrow.CreateSolidBrush( lpNMCustomDraw->clrTextInactive );

		HGDIOBJ penOld = dc.SelectObject( penButtons );
		HGDIOBJ brushOld = dc.SelectObject( brushArrow );

		RECT rcX = m_rcCloseButton;

		if( ectcMouseDownL_CloseButton == ( m_dwState & ectcMouseDown ))
		{
			if( ectcMouseOver_CloseButton == ( m_dwState & ectcMouseOver ))
			{
				::OffsetRect( &rcX, 1, 1 );
			}
		}

		const int sp = 4;

		dc.MoveTo( rcX.left + sp + -1, rcX.top + sp );
		dc.LineTo( rcX.right - sp -1, rcX.bottom - sp );
		dc.MoveTo( rcX.left + sp, rcX.top + sp );
		dc.LineTo( rcX.right - sp, rcX.bottom - sp );

		dc.MoveTo( rcX.left + sp -1, rcX.bottom - sp -1 );
		dc.LineTo( rcX.right - sp -1, rcX.top + sp -1 );
		dc.MoveTo( rcX.left + sp, rcX.bottom - sp -1 );
		dc.LineTo( rcX.right - sp, rcX.top + sp -1 );

		if( ectcMouseDownL_CloseButton == ( m_dwState & ectcMouseDown ))
		{
			if( ectcMouseOver_CloseButton == ( m_dwState & ectcMouseOver ))
			{
				dc.DrawEdge( &m_rcCloseButton, BDR_SUNKENOUTER, BF_RECT );
			}
		}
		else if( ectcHotTrack_CloseButton == ( m_dwState & ectcHotTrack ))
		{
			dc.DrawEdge( &m_rcCloseButton, BDR_RAISEDINNER, BF_RECT );
		}

		dc.SelectObject( brushOld );
		dc.SelectObject( penOld );
	}

	virtual void DrawScrollButtons( LPNMCTCCUSTOMDRAW lpNMCustomDraw )
	{
		ClsDC dc( lpNMCustomDraw->nmcd.hdc );

		ClsPen penButtons;
		penButtons.CreatePen( PS_SOLID, 1, lpNMCustomDraw->clrTextInactive );
		ClsBrush brushArrow;
		brushArrow.CreateSolidBrush( lpNMCustomDraw->clrTextInactive );

		HGDIOBJ penOld = dc.SelectObject( penButtons );
		HGDIOBJ brushOld = dc.SelectObject( brushArrow );

		RECT rcArrowRight = m_rcScrollRight;
		RECT rcArrowLeft = m_rcScrollLeft;

		if( ectcMouseDownL_ScrollRight == ( m_dwState & ectcMouseDown ))
		{
			if( ectcMouseOver_ScrollRight == ( m_dwState & ectcMouseOver ))
			{
				if( ectcOverflowRight == ( m_dwState & ectcOverflowRight ))
				{
					::OffsetRect( &rcArrowRight, 1, 1 );
				}
			}
		}

		if( ectcMouseDownL_ScrollLeft == ( m_dwState & ectcMouseDown ))
		{
			if( ectcMouseOver_ScrollLeft == ( m_dwState & ectcMouseOver ))
			{
				if( ectcOverflowLeft == ( m_dwState & ectcOverflowLeft ))
				{
					::OffsetRect( &rcArrowLeft, 1, 1 );
				}
			}
		}

		const int spRight = 5;
		const int spLeft = 6;

		POINT ptsArrowRight[] = {
			{ rcArrowRight.left + spRight, rcArrowRight.top + spRight - 2 },
			{ rcArrowRight.left + spRight, rcArrowRight.bottom - spRight + 1 },
			{ rcArrowRight.right - spRight - 1, ( rcArrowRight.bottom + m_rcScrollRight.top ) / 2 },
			{ rcArrowRight.left + spRight, rcArrowRight.top + spRight - 2 }
		};

		if( ectcOverflowRight != ( m_dwState & ectcOverflowRight ))
		{
			dc.Polyline( ptsArrowRight, 4 );
		}
		else
		{
			dc.Polygon( ptsArrowRight, 4 );

			if( ectcMouseDownL_ScrollRight == ( m_dwState & ectcMouseDown ))
			{
				if( ectcMouseOver_ScrollRight == ( m_dwState & ectcMouseOver ))
				{
					dc.DrawEdge( &m_rcScrollRight, BDR_SUNKENOUTER, BF_RECT );
				}
			}
			else if( ectcHotTrack_ScrollRight == ( m_dwState & ectcHotTrack ))
			{
				dc.DrawEdge( &m_rcScrollRight, BDR_RAISEDINNER, BF_RECT );
			}
		}

		POINT ptsArrowLeft[] = {
			{ rcArrowLeft.right - spLeft, rcArrowLeft.top + spLeft - 3 },
			{ rcArrowLeft.right - spLeft, rcArrowLeft.bottom - spLeft + 2 },
			{ rcArrowLeft.left + spLeft - 1, ( rcArrowLeft.bottom + m_rcScrollLeft.top ) / 2 },
			{ rcArrowLeft.right - spLeft, rcArrowLeft.top + spLeft - 3 }
		};

		if( ectcOverflowLeft != ( m_dwState & ectcOverflowLeft ))
		{
			dc.Polyline( ptsArrowLeft, 4 );
		}
		else
		{
			dc.Polygon( ptsArrowLeft, 4 );

			if( ectcMouseDownL_ScrollLeft == ( m_dwState & ectcMouseDown ))
			{
				if( ectcMouseOver_ScrollLeft == ( m_dwState & ectcMouseOver))
				{
					dc.DrawEdge( &m_rcScrollLeft, BDR_SUNKENOUTER, BF_RECT );
				}
			}
			else if( ectcHotTrack_ScrollLeft == ( m_dwState & ectcHotTrack ))
			{
				dc.DrawEdge( &m_rcScrollLeft, BDR_RAISEDINNER, BF_RECT );
			}
		}

		dc.SelectObject( brushOld );
		dc.SelectObject( penOld );
	}

	virtual void CalcSize_CloseButton( LPRECT prcTabItemArea )
	{
		//int nButtonSizeX = ::GetSystemMetrics(SM_CXSMSIZE);
		//int nButtonSizeY = ::GetSystemMetrics(SM_CYSMSIZE);
		// NOTE: After several tests, VS.Net does NOT depend on
		//  any system metric for the button size, so neither will we.
		int nButtonSizeX = 15;
		int nButtonSizeY = 15;

		if(( prcTabItemArea->right - prcTabItemArea->left ) < nButtonSizeX )
		{
			::SetRectEmpty( &m_rcCloseButton );
			return;
		}

		m_rcCloseButton = *prcTabItemArea;

		DWORD dwStyle = this->GetStyle();

		if ( CTCS_BOTTOM == ( dwStyle & CTCS_BOTTOM ))
		{
			m_rcCloseButton.top += 3;
			m_rcCloseButton.right -= 3;
		}
		else
		{
			m_rcCloseButton.top += 1;
			m_rcCloseButton.bottom -= 2;
			m_rcCloseButton.right -= 2;
		}
		m_rcCloseButton.top = ( m_rcCloseButton.bottom + m_rcCloseButton.top - nButtonSizeY ) / 2;
		m_rcCloseButton.bottom = m_rcCloseButton.top + nButtonSizeY;

		m_rcCloseButton.left = m_rcCloseButton.right - ( nButtonSizeX );

		if ( m_bDotNetAdjust == false && ( CTCS_BOTTOM == ( dwStyle & CTCS_BOTTOM )))
		{
			::OffsetRect( &m_rcCloseButton, 0, -1 );
		}

		if( m_tooltip.GetSafeHWND())
		{
			TOOLINFO tool = { 0 };
			tool.cbSize = sizeof( tool );
			tool.hwnd = m_hWnd;
			tool.uId = ( UINT )ectcToolTip_Close;
			tool.rect = m_rcCloseButton;
			m_tooltip.NewToolRect( tool );
		}

		// Adjust the tab area
		prcTabItemArea->right = m_rcCloseButton.left;
	}

	virtual void CalcSize_ScrollButtons( LPRECT prcTabItemArea )
	{
		//int nButtonSizeX = ::GetSystemMetrics(SM_CXSMSIZE);
		//int nButtonSizeY = ::GetSystemMetrics(SM_CYSMSIZE);
		// NOTE: After several tests, VS.Net does NOT depend on
		//  any system metric for the button size, so neither will we.
		int nButtonSizeX = 15;
		int nButtonSizeY = 15;

		if(( prcTabItemArea->right - prcTabItemArea->left ) < nButtonSizeX )
		{
			::SetRectEmpty( &m_rcScrollRight );
			::SetRectEmpty( &m_rcScrollLeft );
			return;
		}

		RECT rcScroll = *prcTabItemArea;

		DWORD dwStyle = this->GetStyle();

		if ( CTCS_BOTTOM == ( dwStyle & CTCS_BOTTOM ))
		{
			rcScroll.top += 3;
			if( 0 == ( dwStyle & CTCS_CLOSEBUTTON ))
			{
				rcScroll.right -= 3;
			}
		}
		else
		{
			rcScroll.top += 1;
			rcScroll.bottom -= 2;
			if( 0 == ( dwStyle & CTCS_CLOSEBUTTON ))
			{
				rcScroll.right -= 2;
			}
		}
		rcScroll.top = ( rcScroll.bottom + rcScroll.top - nButtonSizeY ) / 2;
		rcScroll.bottom = rcScroll.top + nButtonSizeY;

		m_rcScrollRight = rcScroll;
		m_rcScrollLeft = rcScroll;

		m_rcScrollRight.left = m_rcScrollRight.right - nButtonSizeX;

		m_rcScrollLeft.right = m_rcScrollRight.left;
		m_rcScrollLeft.left = m_rcScrollLeft.right - nButtonSizeX;

		if ( m_bDotNetAdjust == false && ( CTCS_BOTTOM == ( dwStyle & CTCS_BOTTOM )))
		{
			::OffsetRect( &m_rcScrollLeft, 0, -1 );
			::OffsetRect( &m_rcScrollRight, 0, -1 );
		}

		if( m_tooltip.GetSafeHWND())
		{
			TOOLINFO tool = { 0 };
			tool.cbSize = sizeof( tool );
			tool.hwnd = m_hWnd;
			tool.uId = (UINT)ectcToolTip_ScrollRight;
			tool.rect = m_rcScrollRight;
			m_tooltip.NewToolRect( tool );
			tool.uId = (UINT)ectcToolTip_ScrollLeft;
			tool.rect = m_rcScrollLeft;
			m_tooltip.NewToolRect( tool );
		}

		// Adjust the tab area
		prcTabItemArea->right = m_rcScrollLeft.left;
	}
protected:
	bool	m_bDotNetAdjust;
};

#endif // __CUSTOMTABCTRL_H__