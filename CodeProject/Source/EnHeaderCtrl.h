//_ **********************************************************
//_ 
//_ Name: EnHeaderCtrl.h 
//_ Purpose: 
//_ Created: 15 September 1998 
//_ Author: D.R.Godson
//_ Modified By: 
//_ 
//_ Copyright (c) 1998 Brilliant Digital Entertainment Inc. 
//_ 
//_ **********************************************************

// EnHeaderCtrl.h : header file
//
#if !defined ( ENHEADERCTRL_H )
#define ENHEADERCTRL_H 

/////////////////////////////////////////////////////////////////////////////
// CEnHeaderCtrl window

class CEnHeaderCtrl : public CHeaderCtrl
{
// Construction
public:
	CEnHeaderCtrl();
	void EnableTracking(BOOL bAllow) { m_bAllowTracking = bAllow; }
	int AddColumn(int nCol, int nWidth, CString sTitle = _T(""), int nFormat = HDF_LEFT, UINT uIDBitmap = 0);
	BOOL SetColumnWidth(int nCol, int nWidth);
	int GetColumnWidth(int nCol);

// Attributes
private:
	BOOL m_bAllowTracking;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnHeaderCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEnHeaderCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEnHeaderCtrl)
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	afx_msg void OnBeginTrackHeader(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
};

#endif
/////////////////////////////////////////////////////////////////////////////
////
