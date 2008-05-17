// CJToolBarCtrl.h : header file
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
 * $Revision: 8 $
 * $Archive: /CodeJockey/Include/CJToolBarCtrl.h $
 *
 * $History: CJToolBarCtrl.h $
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 7/31/99    Time: 4:27p
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 10:00p
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
 * User: Kirk Stowell Date: 7/18/99    Time: 10:10p
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

#ifndef __CJTOOLBARCTRL_H__
#define __CJTOOLBARCTRL_H__

// The CCJToolBarCtrl class provides the functionality of the Windows toolbar 
// common control. This control (and therefore the CCJToolBarCtrl class) is available 
// only to programs running under Windows 95 and Windows NT version 3.51 and later.
class _CJX_EXT_CLASS CCJToolBarCtrl : public CToolBarCtrl
{
// Construction
public:
  CCJToolBarCtrl();
  BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
public:
	DWORD GetButtonSize() const;
	CImageList* GetDisabledImageList() const;
	CImageList* GetHotImageList() const;
	CImageList* GetImageList() const;
	DWORD GetStyle() const;
	int GetMaxTextRows() const;
	BOOL IsButtonHighlighted(int nID) const;
	BOOL SetButtonWidth(int cxMin, int cxMax);
	CImageList* SetDisabledImageList(CImageList* pImageList);
	CImageList* SetHotImageList(CImageList* pImageList);
	CImageList* SetImageList(CImageList* pImageList);
	HRESULT GetDropTarget(IDropTarget** ppDropTarget) const;;
	BOOL SetIndent(int iIndent);
	BOOL SetMaxTextRows(int iMaxRows);
	void SetStyle(DWORD dwStyle);
	BOOL GetButtonInfo(int nID, TBBUTTONINFO* ptbbi) const;
	BOOL SetButtonInfo(int nID, TBBUTTONINFO* ptbbi);
	DWORD SetDrawTextFlags(DWORD dwMask, DWORD dwDTFlags);
	BOOL GetAnchorHighlight() const;
	BOOL SetAnchorHighlight(BOOL fAnchor = TRUE);
	int GetHotItem() const;
	int SetHotItem(int nHot);
	void GetInsertMark(TBINSERTMARK* ptbim) const;
	void SetInsertMark(TBINSERTMARK* ptbim);
	BOOL GetMaxSize(LPSIZE pSize) const;
	BOOL InsertMarkHitTest(LPPOINT ppt, LPTBINSERTMARK ptbim) const;
	DWORD GetExtendedStyle() const;
	DWORD SetExtendedStyle(DWORD dwExStyle);
	COLORREF GetInsertMarkColor() const;
	COLORREF SetInsertMarkColor(COLORREF clrNew);

// Operations
public:
	BOOL MapAccelerator(TCHAR chAccel, UINT* pIDBtn);
	BOOL MarkButton(int nID, BOOL bHighlight = TRUE);
	BOOL MoveButton(UINT nOldPos, UINT nNewPos);
	int HitTest(LPPOINT ppt) const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJToolBarCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCJToolBarCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCJToolBarCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// inline functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE DWORD CCJToolBarCtrl::GetButtonSize() const
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, TB_GETBUTTONSIZE, 0, 0L); }

_CJXLIB_INLINE CImageList* CCJToolBarCtrl::GetDisabledImageList() const
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, TB_GETDISABLEDIMAGELIST, 0, 0)); }

_CJXLIB_INLINE CImageList* CCJToolBarCtrl::GetHotImageList() const
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, TB_GETHOTIMAGELIST, 0, 0)); }

_CJXLIB_INLINE CImageList* CCJToolBarCtrl::GetImageList() const
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, TB_GETIMAGELIST, 0, 0)); }

_CJXLIB_INLINE DWORD CCJToolBarCtrl::GetStyle() const
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, TB_GETSTYLE, 0, 0L); }

_CJXLIB_INLINE int CCJToolBarCtrl::GetMaxTextRows() const
	{ ASSERT(::IsWindow(m_hWnd)); return (INT) ::SendMessage(m_hWnd, TB_GETTEXTROWS, 0, 0L); }

_CJXLIB_INLINE BOOL CCJToolBarCtrl::IsButtonHighlighted(int nID) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_ISBUTTONHIGHLIGHTED, nID, 0); }

_CJXLIB_INLINE BOOL CCJToolBarCtrl::SetButtonWidth(int cxMin, int cxMax)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_SETBUTTONWIDTH, 0, MAKELPARAM(cxMin, cxMax)); }

_CJXLIB_INLINE CImageList* CCJToolBarCtrl::SetDisabledImageList(CImageList* pImageList)
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, TB_SETDISABLEDIMAGELIST, 0, (LPARAM)pImageList->GetSafeHandle())); }

_CJXLIB_INLINE CImageList* CCJToolBarCtrl::SetHotImageList(CImageList* pImageList)
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, TB_SETHOTIMAGELIST, 0, (LPARAM)pImageList->GetSafeHandle())); }

_CJXLIB_INLINE CImageList* CCJToolBarCtrl::SetImageList(CImageList* pImageList)
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, TB_SETIMAGELIST, 0, (LPARAM)pImageList->GetSafeHandle())); }

_CJXLIB_INLINE BOOL CCJToolBarCtrl::SetIndent(int iIndent)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_SETINDENT, iIndent, 0L); }

_CJXLIB_INLINE BOOL CCJToolBarCtrl::SetMaxTextRows(int iMaxRows)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_SETMAXTEXTROWS, iMaxRows, 0L); }

_CJXLIB_INLINE void CCJToolBarCtrl::SetStyle(DWORD dwStyle)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TB_SETSTYLE, 0, dwStyle); }

_CJXLIB_INLINE BOOL CCJToolBarCtrl::GetButtonInfo(int nID, TBBUTTONINFO* ptbbi) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_GETBUTTONINFO, nID, (LPARAM)ptbbi); }

_CJXLIB_INLINE BOOL CCJToolBarCtrl::SetButtonInfo(int nID, TBBUTTONINFO* ptbbi)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_SETBUTTONINFO, nID, (LPARAM)ptbbi); }

_CJXLIB_INLINE DWORD CCJToolBarCtrl::SetDrawTextFlags(DWORD dwMask, DWORD dwDTFlags)
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, TB_SETDRAWTEXTFLAGS, dwMask, dwDTFlags); }

_CJXLIB_INLINE BOOL CCJToolBarCtrl::GetAnchorHighlight() const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_GETANCHORHIGHLIGHT, 0, 0); }

_CJXLIB_INLINE BOOL CCJToolBarCtrl::SetAnchorHighlight(BOOL fAnchor)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_SETANCHORHIGHLIGHT, fAnchor, 0); }

_CJXLIB_INLINE int CCJToolBarCtrl::GetHotItem() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TB_GETHOTITEM, 0, 0); }

_CJXLIB_INLINE int CCJToolBarCtrl::SetHotItem(int nHot)
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TB_SETHOTITEM, nHot, 0); }

_CJXLIB_INLINE void CCJToolBarCtrl::GetInsertMark(TBINSERTMARK* ptbim) const
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TB_GETINSERTMARK, 0, (LPARAM)ptbim); }

_CJXLIB_INLINE void CCJToolBarCtrl::SetInsertMark(TBINSERTMARK* ptbim)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TB_SETINSERTMARK, 0, (LPARAM)ptbim); }

_CJXLIB_INLINE BOOL CCJToolBarCtrl::GetMaxSize(LPSIZE pSize) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_GETMAXSIZE, 0, (LPARAM)pSize); }

_CJXLIB_INLINE BOOL CCJToolBarCtrl::InsertMarkHitTest(LPPOINT ppt, LPTBINSERTMARK ptbim) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_INSERTMARKHITTEST, (WPARAM)ppt, (LPARAM)ptbim); }

_CJXLIB_INLINE DWORD CCJToolBarCtrl::GetExtendedStyle() const
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, TB_GETEXTENDEDSTYLE, 0, 0L); }

_CJXLIB_INLINE DWORD CCJToolBarCtrl::SetExtendedStyle(DWORD dwExStyle)
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, TB_SETEXTENDEDSTYLE, 0, dwExStyle); }

_CJXLIB_INLINE COLORREF CCJToolBarCtrl::GetInsertMarkColor() const
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, TB_GETINSERTMARKCOLOR, 0, 0); }

_CJXLIB_INLINE COLORREF CCJToolBarCtrl::SetInsertMarkColor(COLORREF clrNew)
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, TB_SETINSERTMARKCOLOR, 0, (LPARAM) clrNew); }

_CJXLIB_INLINE BOOL CCJToolBarCtrl::MapAccelerator(TCHAR chAccel, UINT* pIDBtn)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_MAPACCELERATOR, (WPARAM)chAccel, (LPARAM)pIDBtn); }

_CJXLIB_INLINE BOOL CCJToolBarCtrl::MarkButton(int nID, BOOL bHighlight)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_MARKBUTTON, nID, MAKELPARAM(bHighlight, 0)); }

_CJXLIB_INLINE BOOL CCJToolBarCtrl::MoveButton(UINT nOldPos, UINT nNewPos)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TB_MOVEBUTTON, nOldPos, nNewPos); }

_CJXLIB_INLINE int CCJToolBarCtrl::HitTest(LPPOINT ppt) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, TB_HITTEST, 0, (LPARAM)ppt); }

#endif //__CJTOOLBARCTRL_H__
