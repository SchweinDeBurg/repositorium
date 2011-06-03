// Copyright (C) 2003-2011 AbstractSpoon Software.
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

#if !defined(AFX_TDLFINDRESULTSLISTCTRL_H__E3FBC372_D7CC_457E_B7BB_1036256A64E9__INCLUDED_)
#define AFX_TDLFINDRESULTSLISTCTRL_H__E3FBC372_D7CC_457E_B7BB_1036256A64E9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TDLFindResultsListCtrl.h : header file
//

#include "TDCStruct.h"
#include "../../../CodeProject/Source/EnListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CTDLFindResultsListCtrl window

struct FTDRESULT
{
	FTDRESULT(DWORD dwID = 0, const CFilteredToDoCtrl* pTaskList = NULL, BOOL done = FALSE) : dwTaskID(dwID), pTDC(pTaskList), bDone(done) {}

	DWORD dwTaskID;
	const CFilteredToDoCtrl* pTDC;
	BOOL bDone;
};

typedef CArray<FTDRESULT, FTDRESULT&> CFTDResultsArray;

class CTDLFindResultsListCtrl : public CEnListCtrl
{
// Construction
public:
	CTDLFindResultsListCtrl();

// Attributes
public:
	BOOL AddHeaderRow(LPCTSTR szText, BOOL bSpaceAbove = TRUE);
	int AddResult(const SEARCHRESULT& result, LPCTSTR szTask, LPCTSTR szPath, const CFilteredToDoCtrl* pTDC);

	int GetResultCount() const; // all tasklists
	int GetResultCount(const CFilteredToDoCtrl* pTDC) const;
	int GetAllResults(CFTDResultsArray& aResults) const;
	int GetResults(const CFilteredToDoCtrl* pTDC, CFTDResultsArray& aResults) const;
	int GetResultIDs(const CFilteredToDoCtrl* pTDC, CDWordArray& aTaskIDs) const;

	void DeleteResults(const CFilteredToDoCtrl* pTDC);
	void DeleteAllResults();

	FTDRESULT* GetResult(int nItem) const
	{
		return (FTDRESULT*)GetItemData(nItem);
	}

	void RefreshUserPreferences();

protected:
	CFont m_fontBold, m_fontStrike;
	COLORREF m_crDone;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTDLFindResultsListCtrl)
	//}}AFX_VIRTUAL
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void PreSubclassWindow();

// Implementation
public:
	virtual ~CTDLFindResultsListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTDLFindResultsListCtrl)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	virtual COLORREF GetItemTextColor(int nItem, int nSubItem, BOOL bSelected, BOOL bDropHighlighted, BOOL bWndFocus) const;
	virtual CFont* GetItemFont(int nItem, int nSubItem);
	int GetNextResult(int nItem, BOOL bDown);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TDLFINDRESULTSLISTCTRL_H__E3FBC372_D7CC_457E_B7BB_1036256A64E9__INCLUDED_)
