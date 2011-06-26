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
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

#if !defined(AFX_TDLCSVCOLUMNSETUPLISTCTRL_H__E379E120_FF91_417F_ADBB_0DD6A98089AA__INCLUDED_)
#define AFX_TDLCSVCOLUMNSETUPLISTCTRL_H__E379E120_FF91_417F_ADBB_0DD6A98089AA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TDLCsvColumnSetupListCtrl.h : header file
//

#include "TDCEnum.h"

#include "../../Common/InputListCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CTDLCsvAttributeSetupListCtrl window

struct CSVCOLUMNMAPPING
{
	CSVCOLUMNMAPPING():
	nTDCAttrib(TDCA_NONE)
	{
	}
	CSVCOLUMNMAPPING(const CString& sName, TDC_ATTRIBUTE tdcAttrib)
	{
		sColumnName = sName;
		nTDCAttrib = tdcAttrib;
	}

	TDC_ATTRIBUTE nTDCAttrib;
	CString sColumnName;
};

typedef CArray<CSVCOLUMNMAPPING, CSVCOLUMNMAPPING&> CTDCCsvColumnMapping;

class CTDLCsvAttributeSetupListCtrl : public CInputListCtrl
{
// Construction
public:
	CTDLCsvAttributeSetupListCtrl(BOOL bImporting);

	void SetColumnMapping(const CTDCCsvColumnMapping& aMapping);
	int GetColumnMapping(CTDCCsvColumnMapping& aMapping) const;

// attributes
protected:
	CComboBox m_cbAttributes;
	CTDCCsvColumnMapping m_aMapping;
	BOOL m_bImporting;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTDLCsvAttributeSetupListCtrl)
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTDLCsvAttributeSetupListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTDLCsvAttributeSetupListCtrl)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnAttribEditCancel();
	afx_msg void OnAttribEditOK();
	afx_msg void OnNameEditOK(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

protected:
	void BuildListCtrl();

	virtual void EditCell(int nItem, int nCol);
	virtual BOOL IsEditing() const;
	virtual BOOL CanDeleteSelectedCell() const;
	virtual BOOL DeleteSelectedCell();
	virtual BOOL CanEditSelectedCell() const;

	void PrepareEdit(int nRow, int nCol);

	int FindMappedAttribute(TDC_ATTRIBUTE nAttrib, int nIgnoreRow = -1) const;
	int FindMappedAttribute(const CString& sName, int nIgnoreRow = -1) const;

	static CString GetAttributeName(TDC_ATTRIBUTE nAttrib);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TDLCSVCOLUMNSETUPLISTCTRL_H__E379E120_FF91_417F_ADBB_0DD6A98089AA__INCLUDED_)
