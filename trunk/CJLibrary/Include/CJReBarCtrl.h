// CJReBarCtrl.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
// Copied from MFC, Visual C++ 6, to extend functionality to Visual C++ 5 
// users, with some ideas taken from Luis Barreira's article 'Classes for new IE4 controls'
// http://www.codeguru.com/controls/ie4_controls_classes.shtml
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 8/31/99 1:11a $
 * $Revision: 7 $
 * $Archive: /CodeJockey/Include/CJReBarCtrl.h $
 *
 * $History: CJReBarCtrl.h $
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 7/31/99    Time: 4:27p
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:13p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 7/14/99    Time: 10:29p
 * Created in $/CodeJockey/Include
 * Copied from MFC v6 and techno preview for v5. Added to extend
 * functionality to Visual C++ 5.0 users.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJREBARCTRL_H__
#define __CJREBARCTRL_H__

/////////////////////////////////////////////////////////////////////////////

  class CImageList;
  class CToolTipCtrl;

// The CCJReBarCtrl class encapsulates the functionality of a rebar control, 
// which is a container for a child window. The application in which the rebar 
// control resides assigns the child window contained by the rebar control to the 
// rebar band. The child window is usually another common control.
class _CJX_EXT_CLASS CCJReBarCtrl : public CWnd
{
  DECLARE_DYNAMIC(CCJReBarCtrl)

// Construction
public:
  CCJReBarCtrl();
  BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
  
// Attributes
public:
  UINT GetBandCount() const;
  BOOL GetBandInfo(UINT uBand, REBARBANDINFO* prbbi) const;
  UINT GetBarHeight() const;
  BOOL GetBarInfo(REBARINFO* prbi) const;
  COLORREF GetBkColor() const;
  IDropTarget* GetDropTarget() const;
  BOOL GetRect(UINT uBand, LPRECT prc) const;
  UINT GetRowCount() const;
  UINT GetRowHeight(UINT uRow) const;
  COLORREF GetTextColor() const;
  CToolTipCtrl* GetToolTips() const;
  int IDToIndex(UINT uBandID) const;
  BOOL SetBandInfo(UINT uBand, REBARBANDINFO* prbbi);
  BOOL SetBarInfo(REBARINFO* prbi);
  COLORREF SetBkColor(COLORREF clr);
  CWnd* SetOwner(CWnd* pWnd);
  COLORREF SetTextColor(COLORREF clr);
  void SetToolTips(CToolTipCtrl* pToolTip);
  CImageList* GetImageList() const;
  BOOL SetImageList(CImageList* pImageList);
  void GetBandBorders(UINT uBand, LPRECT prc) const;
  CPalette* GetPalette() const;
  CPalette* SetPalette(HPALETTE hPal);
  BOOL GetColorScheme(COLORSCHEME* lpcs);
  void SetColorScheme(const COLORSCHEME* lpcs);
  
// Operations
public:
  void BeginDrag(UINT uBand, DWORD dwPos = (DWORD)-1);
  BOOL DeleteBand(UINT uBand);
  void DragMove(DWORD dwPos = (DWORD)-1);
  void EndDrag();
  int HitTest(RBHITTESTINFO* prbht);
  BOOL InsertBand(UINT uIndex, REBARBANDINFO* prbbi);
  void MaximizeBand(UINT uBand);
  void MinimizeBand(UINT uBand);
  BOOL ShowBand(UINT uBand, BOOL fShow = TRUE);
  BOOL SizeToRect(CRect& rect);
  BOOL MoveBand(UINT uFrom, UINT uTo);
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE UINT CCJReBarCtrl::GetBandCount() const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, RB_GETBANDCOUNT, 0, 0L); }

_CJXLIB_INLINE BOOL CCJReBarCtrl::GetBandInfo(UINT uBand, REBARBANDINFO* prbbi) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, RB_GETBANDINFO, uBand, (LPARAM)prbbi); }

_CJXLIB_INLINE UINT CCJReBarCtrl::GetBarHeight() const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, RB_GETBARHEIGHT, 0, 0L); }

_CJXLIB_INLINE BOOL CCJReBarCtrl::GetBarInfo(REBARINFO* prbi) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, RB_GETBARINFO, 0, (LPARAM)prbi); }

_CJXLIB_INLINE COLORREF CCJReBarCtrl::GetBkColor() const
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, RB_GETBKCOLOR, 0, 0L); }

_CJXLIB_INLINE IDropTarget* CCJReBarCtrl::GetDropTarget() const
	{ ASSERT(::IsWindow(m_hWnd)); IDropTarget* pdt; ::SendMessage(m_hWnd, RB_GETDROPTARGET, 0, (LPARAM)&pdt); return pdt; }

_CJXLIB_INLINE BOOL CCJReBarCtrl::GetRect(UINT uBand, LPRECT prc) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, RB_GETRECT, uBand, (LPARAM)prc); }

_CJXLIB_INLINE UINT CCJReBarCtrl::GetRowCount() const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, RB_GETROWCOUNT, 0, 0L); }

_CJXLIB_INLINE UINT CCJReBarCtrl::GetRowHeight(UINT uRow) const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, RB_GETROWHEIGHT, uRow, 0L); }

_CJXLIB_INLINE COLORREF CCJReBarCtrl::GetTextColor() const
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, RB_GETTEXTCOLOR, 0, 0L); }

_CJXLIB_INLINE CToolTipCtrl* CCJReBarCtrl::GetToolTips() const
	{ ASSERT(::IsWindow(m_hWnd)); return (CToolTipCtrl*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, RB_GETTOOLTIPS, 0, 0L)); }

_CJXLIB_INLINE int CCJReBarCtrl::IDToIndex(UINT uBandID) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, RB_IDTOINDEX, uBandID, 0L); }

_CJXLIB_INLINE BOOL CCJReBarCtrl::SetBandInfo(UINT uBand, REBARBANDINFO* prbbi)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, RB_SETBANDINFO, uBand, (LPARAM)prbbi); }

_CJXLIB_INLINE BOOL CCJReBarCtrl::SetBarInfo(REBARINFO* prbi)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, RB_SETBARINFO, 0, (LPARAM)prbi); }

_CJXLIB_INLINE COLORREF CCJReBarCtrl::SetBkColor(COLORREF clr)
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, RB_SETBKCOLOR, 0, (LPARAM)clr); }

_CJXLIB_INLINE CWnd* CCJReBarCtrl::SetOwner(CWnd* pWnd)
	{ ASSERT(::IsWindow(m_hWnd)); return CWnd::FromHandle((HWND)::SendMessage(m_hWnd, RB_SETPARENT, (WPARAM)pWnd->m_hWnd, 0L)); }

_CJXLIB_INLINE COLORREF CCJReBarCtrl::SetTextColor(COLORREF clr)
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, RB_SETTEXTCOLOR, 0, (LPARAM)clr); }

_CJXLIB_INLINE void CCJReBarCtrl::SetToolTips(CToolTipCtrl* pToolTip)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, RB_SETTOOLTIPS, (WPARAM)pToolTip->m_hWnd, 0L); }

_CJXLIB_INLINE void CCJReBarCtrl::GetBandBorders(UINT uBand, LPRECT prc) const
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, RB_GETBANDBORDERS, uBand, (LPARAM)prc); }

_CJXLIB_INLINE CPalette* CCJReBarCtrl::GetPalette() const
	{ ASSERT(::IsWindow(m_hWnd)); return CPalette::FromHandle((HPALETTE)::SendMessage(m_hWnd, RB_GETPALETTE, 0, 0L)); }

_CJXLIB_INLINE CPalette* CCJReBarCtrl::SetPalette(HPALETTE hPal)
	{ ASSERT(::IsWindow(m_hWnd)); return CPalette::FromHandle((HPALETTE)::SendMessage(m_hWnd, RB_SETPALETTE, 0, (LPARAM)hPal)); }

_CJXLIB_INLINE void CCJReBarCtrl::BeginDrag(UINT uBand, DWORD dwPos)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, RB_BEGINDRAG, uBand, dwPos); }

_CJXLIB_INLINE BOOL CCJReBarCtrl::DeleteBand(UINT uBand)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, RB_DELETEBAND, uBand, 0L); }

_CJXLIB_INLINE void CCJReBarCtrl::DragMove(DWORD dwPos)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, RB_DRAGMOVE, 0, dwPos); }

_CJXLIB_INLINE void CCJReBarCtrl::EndDrag()
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, RB_ENDDRAG, 0, 0L); }

_CJXLIB_INLINE int CCJReBarCtrl::HitTest(RBHITTESTINFO* prbht)
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, RB_HITTEST, 0, (LPARAM)prbht); }

_CJXLIB_INLINE BOOL CCJReBarCtrl::InsertBand(UINT uIndex, REBARBANDINFO* prbbi)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, RB_INSERTBAND, uIndex, (LPARAM)prbbi); }

_CJXLIB_INLINE void CCJReBarCtrl::MaximizeBand(UINT uBand)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, RB_MAXIMIZEBAND, uBand, 0L); }

_CJXLIB_INLINE void CCJReBarCtrl::MinimizeBand(UINT uBand)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, RB_MINIMIZEBAND, uBand, 0L); }

_CJXLIB_INLINE BOOL CCJReBarCtrl::ShowBand(UINT uBand, BOOL fShow)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, RB_SHOWBAND, uBand, fShow); }

_CJXLIB_INLINE BOOL CCJReBarCtrl::SizeToRect(CRect& rect)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, RB_SIZETORECT, 0, (LPARAM)&rect); }

_CJXLIB_INLINE BOOL CCJReBarCtrl::MoveBand(UINT uFrom, UINT uTo)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, RB_MOVEBAND, uFrom, uTo); }

#endif //__CJREBARCTRL_H__

/////////////////////////////////////////////////////////////////////////////

