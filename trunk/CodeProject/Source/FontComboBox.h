/* ==========================================================================
    File :          FontComboBox.h

    Class :         CFontComboBox

    Author :        Johan Rosengren, Abstrakt Mekanik AB
                    Iain Clarke

    Date :          2005-05-06

    Purpose :       CFontComboBox is derived from "CComboBox" and is an
                    autosizing no-frills combobox for display of the fonts
                    installed in the system..

    Description :   Simpel derived class with members to fill the box, to
                    autosize the dropdown and select an entry by name.

    Usage :         Create as any combobox, and call "FillCombo" to fill
                    the control with the names of the fonts installed in
                    the system. Call "SelectFontName" to select a font by
                    name.

   ========================================================================*/

#if !defined(AFX_FONTCOMBOBOX_H__B88A8EAC_D643_444C_B9B0_87CE8DC81E89__INCLUDED_)
#define AFX_FONTCOMBOBOX_H__B88A8EAC_D643_444C_B9B0_87CE8DC81E89__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FontComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFontComboBox window

class CFontComboBox : public CComboBox
{
// Construction
public:
	CFontComboBox();

// Attributes
public:
	void DDX(CDataExchange* pDX, CString& font);

// Operations
public:
	void FillCombo();
	void SetMaxWidth(int maxWidth);
	int GetMaxWidth() const;
	BOOL SelectFontName(const CString& font);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFontComboBox)
public:
	virtual void PreSubclassWindow()
	{
		FillCombo();
	}
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFontComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFontComboBox)
	afx_msg void OnDropdown();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	int m_maxWidth;

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTCOMBOBOX_H__B88A8EAC_D643_444C_B9B0_87CE8DC81E89__INCLUDED_)
