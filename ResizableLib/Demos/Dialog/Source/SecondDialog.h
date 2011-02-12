#if !defined(AFX_SECONDDIALOG_H__627E96C2_3D23_11D4_9C0C_A29A4CB0AA4A__INCLUDED_)
#define AFX_SECONDDIALOG_H__627E96C2_3D23_11D4_9C0C_A29A4CB0AA4A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SecondDialog.h : header file
//
#include "ResizableDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CSecondDialog dialog

class CSecondDialog : public CResizableDialog
{
// Construction
public:
	CSecondDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSecondDialog)
	enum { IDD = IDD_SECOND_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSecondDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CToolBar m_wndToolBar;
	CBrush m_brushBack;

	// Generated message map functions
	//{{AFX_MSG(CSecondDialog)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	afx_msg void OnButton3();
	afx_msg void OnButton4();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SECONDDIALOG_H__627E96C2_3D23_11D4_9C0C_A29A4CB0AA4A__INCLUDED_)
