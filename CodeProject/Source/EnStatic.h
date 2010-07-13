#if !defined(AFX_ENSTATIC_H__A52ABD1A_3708_4DA2_AB5E_B632CB7C78A7__INCLUDED_)
#define AFX_ENSTATIC_H__A52ABD1A_3708_4DA2_AB5E_B632CB7C78A7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EnStatic.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEnStatic window

class CEnStatic : public CStatic
{
// Construction
public:
	CEnStatic(BOOL bEnableGripper = FALSE);

	void EnableGripper(BOOL bEnable = TRUE);

// Attributes
protected:
	BOOL m_bGripper;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CEnStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CEnStatic)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	//}}AFX_MSG
#if _MSC_VER >= 1400
	afx_msg LRESULT OnNcHitTest(CPoint point);
#else
	afx_msg UINT OnNcHitTest(CPoint point);
#endif

	DECLARE_MESSAGE_MAP()

	BOOL IsShowingGripper();
	CRect GetGripperRect();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ENSTATIC_H__A52ABD1A_3708_4DA2_AB5E_B632CB7C78A7__INCLUDED_)
