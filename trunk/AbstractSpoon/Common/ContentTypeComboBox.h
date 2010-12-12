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
// - added AbstractSpoon Software copyright notice and licenese information
// - adjusted #include's paths
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-switches
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
//*****************************************************************************

#if !defined(AFX_CONTENTTYPECOMBOBOX_H__87AB439E_A5C6_4486_9985_0B5DC1D0F530__INCLUDED_)
#define AFX_CONTENTTYPECOMBOBOX_H__87AB439E_A5C6_4486_9985_0B5DC1D0F530__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ContentTypeComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CContentTypeComboBox window

class CContentMgr;
class CONTENTFORMAT;

class CContentTypeComboBox : public CComboBox
{
	// Construction
public:
	CContentTypeComboBox(const CContentMgr* pContentMgr);

	int GetSelectedFormat(CONTENTFORMAT& cf) const;
	int SetSelectedFormat(LPCTSTR szTypeID);
	void SetCurSel(int nSel);
	int GetCurSel() const;

	int GetCount() const;
	BOOL IsInitialized() const
	{
		return (m_pContentMgr != NULL);
	}

	// Attributes
protected:
	const CContentMgr* m_pContentMgr;
	int m_nInitSel;

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CContentTypeComboBox)
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CContentTypeComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CContentTypeComboBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	void FillCombo();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTENTTYPECOMBOBOX_H__87AB439E_A5C6_4486_9985_0B5DC1D0F530__INCLUDED_)
