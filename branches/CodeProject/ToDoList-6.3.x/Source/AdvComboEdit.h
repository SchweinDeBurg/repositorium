/////////////////////////////////////////////////////////////////////////////
// AdvComboEdit.h : header file
//
// CAdvComboBox Control
// Version: 2.1
// Date: September 2002
// Author: Mathias Tunared
// Email: Mathias@inorbit.com
// Copyright (c) 2002. All Rights Reserved.
//
// This code, in compiled form or as source code, may be redistributed
// unmodified PROVIDING it is not sold for profit without the authors
// written consent, and providing that this notice and the authors name
// and all copyright notices remains intact.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef ADVCOMBOEDIT_H
#define ADVCOMBOEDIT_H

/////////////////////////////////////////////////////////////////////////////
// CAdvComboEdit window

class CAdvComboEdit : public CEdit
{
// Construction
public:
	CAdvComboEdit();
	virtual ~CAdvComboEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdvComboEdit)
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	//{{AFX_MSG(CAdvComboEdit)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //ADVCOMBOEDIT_H
