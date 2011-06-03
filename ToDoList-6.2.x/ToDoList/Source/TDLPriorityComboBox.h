// Copyright (C) 2003-2005 AbstractSpoon Software.
//
// This license applies to everything in the ToDoList package, except where
// otherwise noted.
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software in a
// product, an acknowledgment in the product documentation would be appreciated
// but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - added AbstractSpoon Software copyright notice and license information
// - adjusted #include's paths
// - reformatted using Artistic Style 2.02 with the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-cases
//      --max-instatement-indent=2
//      --style=allman
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
//*****************************************************************************

#if !defined(AFX_TDLPRIORITYCOMBOBOX_H__94985E3D_C2CA_44C0_B3AD_E55C110AEE45__INCLUDED_)
#define AFX_TDLPRIORITYCOMBOBOX_H__94985E3D_C2CA_44C0_B3AD_E55C110AEE45__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TDLPriorityComboBox.h : header file
//

#include "../../../CodeProject/Source/ColorComboBox.h"

/////////////////////////////////////////////////////////////////////////////
// CTDLPriorityComboBox window

// replacement DDX routine
void AFXAPI DDX_CBPriority(CDataExchange* pDX, int nIDC, int& nPriority);

class CTDLPriorityComboBox : public CColorComboBox
{
// Construction
public:
	CTDLPriorityComboBox();

	BOOL SetColors(const CDWordArray& aColors); // must have 11 elements
	void SetReverseOrder(BOOL bReverse = TRUE);

	int IncrementPriority(int nAmount);
	int GetPriority() const; // -2 -> 10
	void SetPriority(int nPriority); // -2 -> 10

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTDLPriorityComboBox)
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

protected:
	BOOL m_bReverse;
	CDWordArray m_aColors;

// Implementation
public:
	virtual ~CTDLPriorityComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTDLPriorityComboBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	void BuildCombo();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TDLPRIORITYCOMBOBOX_H__94985E3D_C2CA_44C0_B3AD_E55C110AEE45__INCLUDED_)
