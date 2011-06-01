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
// - merged with ToDoList version 6.1.2-6.2.2 sources
//*****************************************************************************

#if !defined(AFX_TDCOLUMNLISTBOX_H__C5D9EFCE_7B59_498F_88E4_890286A344A5__INCLUDED_)
#define AFX_TDCOLUMNLISTBOX_H__C5D9EFCE_7B59_498F_88E4_890286A344A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TDLColumnListBox.h : header file
//

#include "TDCEnum.h"
#include "../../../CodeProject/Source/CheckListBoxEx.h"
#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CTDLColumnListBox window

class CTDLColumnListBox : public CCheckListBoxEx
{
// Construction
public:
	CTDLColumnListBox();

	void SetAllColumnsVisible(BOOL bVisible = TRUE);
	int GetAllColumns(CTDCColumnArray& aColumns) const;

	void SetColumnVisible(TDC_COLUMN nCol, BOOL bVisible = TRUE);
	void SetVisibleColumns(const CTDCColumnArray& aColumns);
	int GetVisibleColumns(CTDCColumnArray& aColumns) const;

// Attributes
protected:
	struct COLUMNVIS
	{
		COLUMNVIS() : nTDCCol(TDCC_NONE), bVisible(FALSE) {}
		COLUMNVIS(UINT nIDName, TDC_COLUMN tdcCol, BOOL bVis)
		{
			sName.LoadString(nIDName);
			nTDCCol = tdcCol;
			bVisible = bVis;
		}

		CString sName;
		TDC_COLUMN nTDCCol;
		BOOL bVisible;
	};
	CArray<COLUMNVIS, COLUMNVIS&> m_aColumns;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTDLColumnListBox)
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTDLColumnListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTDLColumnListBox)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg BOOL OnReflectCheckChange();
	afx_msg LRESULT OnInitListBox(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()

	int FindColumn(TDC_COLUMN nCol) const;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TDCOLUMNLISTBOX_H__C5D9EFCE_7B59_498F_88E4_890286A344A5__INCLUDED_)
