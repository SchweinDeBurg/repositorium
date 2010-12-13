/* ==========================================================================
    File :          RRECEnToolBar.h

    Class :         CRRECToolBar

    Author :        Johan Rosengren, Abstrakt Mekanik AB
                    Iain Clarke

    Date :          2004-05-07

    Purpose :       This class encapsulates a toolbar that can be used with
                    "CRulerRichEditCtrl". The class is derived from "CEnToolBar",
                    and manages a formatting toolbar

    Description :   A "CEnToolBar"-derived class. Reads a toolbar resource
                    with the ID "TOOLBAR_CONTROL" and adds combo controls for
                    font name and -size, as well as a color picker at the
                    positions "FONT_NAME_POS", "FONT_SIZE_POS" and
                    "FONT_COLOR_POS" respectively.

    Usage :         Created by the rich edit mini-editor.

    Note :          Very heavily modified by .dan.g.

   ========================================================================*/

#if !defined(AFX_RRECEnToolBar_H__E0596D7D_418C_49B9_AC57_2F0BF93053C9__INCLUDED_)
#define AFX_RRECEnToolBar_H__E0596D7D_418C_49B9_AC57_2F0BF93053C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RRECEnToolBar.h : header file
//

#include "../../../CodeProject/Source/FontComboBox.h"
#include "../../../CodeProject/Source/SizeComboBox.h"

#include "../../../CodeProject/Source/EnToolBar.h"

/////////////////////////////////////////////////////////////////////////////
// CRRECEnToolBar window

class CRRECToolBar : public CEnToolBar
{
// Construction
public:
	CRRECToolBar();
	BOOL Create(CWnd* parent);

	BOOL SetButtonState(int nID, UINT nState);
	BOOL IsButtonChecked(int nID) const;
	BOOL CheckButton(int nID, BOOL bChecked);
	operator CToolBarCtrl& ()
	{
		return GetToolBarCtrl();
	}

	BOOL GetDroppedState() const;

// Attributes
public:

// Operations
public:

	BOOL SetFontName(const CString& font);
	BOOL SetFontSize(int size);
	void SetFontColor(COLORREF color, BOOL bForeground);
	COLORREF GetFontColor(BOOL bForeground, BOOL bTranslateDefault = FALSE);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRRECEnToolBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRRECToolBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRRECEnToolBar)
	afx_msg void OnSelchangeFont();
	afx_msg void OnSelchangeSize();
	afx_msg LRESULT OnColorButton(WPARAM color, LPARAM nCtrlID);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam);
	afx_msg void OnColorDropDown(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

	LRESULT OnItemPostPaint(LPNMTBCUSTOMDRAW lpNMCustomDraw);

private:

	CFontComboBox   m_font;
	CSizeComboBox   m_size;
	COLORREF        m_crText, m_crBack;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RRECEnToolBar_H__E0596D7D_418C_49B9_AC57_2F0BF93053C9__INCLUDED_)
