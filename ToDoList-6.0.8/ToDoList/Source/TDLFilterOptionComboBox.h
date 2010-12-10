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
//*****************************************************************************

#if !defined(AFX_FILTEROPTIONCOMBOBOX_H__9EE08B57_87B6_43D9_8CA7_F47A30CBF66B__INCLUDED_)
#define AFX_FILTEROPTIONCOMBOBOX_H__9EE08B57_87B6_43D9_8CA7_F47A30CBF66B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TDLFilterOptionComboBox.h : header file
//

#include "TDCStruct.h"
#include "../../../CodeProject/Source/EnCheckComboBox.h"
#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CFilterOptionComboBox window

class CTDLFilterOptionComboBox : public CEnCheckComboBox
{
// Construction
public:
	CTDLFilterOptionComboBox();

// Attributes
protected:
	DWORD m_dwOptions;
	CMap<CString, LPCTSTR, UINT, UINT&> m_mapItemFlag;

// Operations
public:
	void Initialize(const FTDCFILTER& filter, FTC_VIEW nView);
	void Initialize(DWORD dwFlags, DWORD dwOptions = 0);

	DWORD GetOptions() const;
	void SetOptions(DWORD dwOptions);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterOptionComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTDLFilterOptionComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFilterOptionComboBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	void OnCheckChange(int nIndex);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTEROPTIONCOMBOBOX_H__9EE08B57_87B6_43D9_8CA7_F47A30CBF66B__INCLUDED_)
