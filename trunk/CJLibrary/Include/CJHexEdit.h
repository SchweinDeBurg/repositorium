// CJHexEdit.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
// Based on the article "HexEdit Control" by Andreas Saurwein [saurwein@uniwares.com]
// http://www.codeguru.com/editctrl/hexeditctrl.shtml
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/30/99 9:20p $
 * $Revision: 1 $
 * $Archive: /CodeJock/Include/CJHexEdit.h $
 *
 * $History: CJHexEdit.h $
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 9:20p
 * Created in $/CodeJock/Include
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJHEXEDIT_H__
#define __CJHEXEDIT_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// CCJHexEdit is a CEdit derived class which allows editing in hex
// display format.
class _CJX_EXT_CLASS CCJHexEdit : public CEdit
{
	DECLARE_DYNAMIC(CCJHexEdit)

// Construction
public:
	CCJHexEdit();

// Attributes
public:

	enum EDITMODE{ EDIT_NONE, EDIT_ASCII, EDIT_HIGH, EDIT_LOW } ;

	LPBYTE		m_pData;			// pointer to data
	int			m_length;			// length of data
	int			m_topindex;			// offset of first visible byte on screen
	int			m_currentAddress;	// address under cursor
	EDITMODE	m_currentMode;		// current editing mode: address/hex/ascii
	int			m_selStart;			// start address of selection
	int			m_selEnd;			// end address of selection
	int			m_bpr;				// byte per row 
	int			m_lpp;				// lines per page
	BOOL		m_bShowAddress;
	BOOL		m_bShowAscii;
	BOOL		m_bShowHex;
	BOOL		m_bAddressIsWide;
	BOOL		m_bNoAddressChange;	// internally used
	BOOL		m_bHalfPage;
	CFont		m_Font;
	int			m_lineHeight;
	int			m_nullWidth;
	BOOL		m_bUpdate;
	int			m_offHex;
	int			m_offAscii;
	int			m_offAddress;
	CPoint		m_editPos;

// Operations
public:

	virtual int		GetData(LPBYTE p, int len);
	virtual void	SetData(LPBYTE p, int len);
	virtual CSize	GetSel(void);
	virtual void	SetSel(int s, int e);
	virtual void	SetBPR(int bpr);
	virtual void	SetOptions(BOOL a, BOOL h, BOOL c, BOOL w);

protected:

	virtual void	ScrollIntoView(int p);
	virtual void	RepositionCaret(int p);
	virtual void	Move(int x, int y);
	virtual BOOL	IsSelected(void);
	virtual void	UpdateScrollbars(void);
	virtual void	CreateEditCaret(void);
	virtual void	CreateAddressCaret(void);
	virtual CPoint	CalcPos(int x, int y);
	virtual void	SelInsert(int s, int l);
	virtual void	SelDelete(int s, int e);
	virtual void	NormalizeSel(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJHexEdit)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCJHexEdit();

// Generated message map functions
protected:
	//{{AFX_MSG(CCJHexEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg UINT OnGetDlgCode();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditClear();
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnEditSelectAll();
	afx_msg void OnEditUndo();
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE UINT CCJHexEdit::OnGetDlgCode() 
	{ return DLGC_WANTALLKEYS; }

_CJXLIB_INLINE void CCJHexEdit::SetOptions(BOOL a, BOOL h, BOOL c, BOOL w)
	{ m_bShowHex = h; m_bShowAscii = c; m_bShowAddress = a; m_bAddressIsWide = w; m_bUpdate = TRUE; }

_CJXLIB_INLINE void CCJHexEdit::SetBPR(int bpr)
	{ m_bpr = bpr; m_bUpdate = TRUE; }

_CJXLIB_INLINE BOOL CCJHexEdit::IsSelected()
	{ return m_selStart != 0xffffffff; }

_CJXLIB_INLINE CSize CCJHexEdit::GetSel()
	{ return CSize(m_selStart, m_selEnd); }

_CJXLIB_INLINE int CCJHexEdit::GetData(LPBYTE p, int len)
	{ memcpy(p, m_pData, min(len, m_length)); return m_length; }

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJHEXEDIT_H__

