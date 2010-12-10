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

#if !defined(AFX_PREFERENCESTASKDEFPAGE_H__852964E3_4ABD_4B66_88BA_F553177616F2__INCLUDED_)
#define AFX_PREFERENCESTASKDEFPAGE_H__852964E3_4ABD_4B66_88BA_F553177616F2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// PreferencesTaskDefPage.h : header file
//

#include "TDCEnum.h"
#include "TDLPriorityComboBox.h"
#include "TDLRiskComboBox.h"

#include <afxtempl.h>

#include "../../Common/PreferencesBase.h"
#include "../../Common/ColorButton.h"
#include "../../Common/TimeEdit.h"
#include "../../../CodeProject/Source/WndPrompt.h"
#include "../../../CodeProject/Source/AutoComboBox.h"
#include "../../Common/GroupLine.h"
#include "../../../CodeProject/Source/DialogHelper.h"
#include "../../../CodeProject/Source/MaskEdit.h"
#include "../../../CodeProject/Source/CheckListBoxEx.h"

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTaskDefPage dialog

enum PTP_ATTRIB
{
	PTPA_PRIORITY,
	PTPA_COLOR,
	PTPA_ALLOCTO,
	PTPA_ALLOCBY,
	PTPA_STATUS,
	PTPA_CATEGORY,
	PTPA_TIMEEST,
	PTPA_RISK,
	PTPA_DUEDATE,
	PTPA_VERSION,
	PTPA_STARTDATE,
	// add to end
};

enum PTDP_LIST
{ 
	PTDP_CATEGORY, 
	PTDP_STATUS, 
	PTDP_ALLOCTO, 
	PTDP_ALLOCBY 
};
// wParam = MAKEWPARAM(enum, 0 for delete, 1 for add)
// lParam = LPCTSTR
const UINT WM_PTDP_LISTCHANGE = ::RegisterWindowMessage(_T("WM_PTDP_LISTCHANGE"));

class CPreferencesTaskDefPage : public CPreferencesPageBase, public CDialogHelper
{
	DECLARE_DYNCREATE(CPreferencesTaskDefPage)

// Construction
public:
	CPreferencesTaskDefPage();
	~CPreferencesTaskDefPage();

	void SetPriorityColors(const CDWordArray& aColors);

	int GetDefaultPriority() const { return m_nDefPriority; }
	int GetDefaultRisk() const { return m_nDefRisk; }
//	CString GetDefaultAllocTo() const { return m_sDefAllocTo; }
	int GetDefaultAllocTo(CStringArray& aAllocTo) const;
	CString GetDefaultAllocBy() const { return m_sDefAllocBy; }
	CString GetDefaultStatus() const { return m_sDefStatus; }
	int GetDefaultCategories(CStringArray& aCats) const;
	CString GetDefaultCreatedBy() const { return m_sDefCreatedBy; }
	double GetDefaultTimeEst(int& nUnits) const;
	double GetDefaultTimeSpent(int& nUnits) const;
	double GetDefaultCost() const { return m_dDefCost; }
	COLORREF GetDefaultColor() const { return m_crDef; }
	BOOL GetAutoDefaultStartDate() const { return m_bUseCreationForDefStartDate; }
	int GetParentAttribsUsed(CTDCAttributeArray& aAttribs, BOOL& bUpdateAttrib) const;

	int GetListItems(PTDP_LIST nList, CStringArray& aItems) const;
	BOOL AddListItem(PTDP_LIST nList, LPCTSTR szItem);
	BOOL DeleteListItem(PTDP_LIST nList, LPCTSTR szItem);

protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesTaskDefPage)
	enum { IDD = IDD_PREFTASKDEF_PAGE };
	CTDLRiskComboBox	m_cbDefRisk;
	CTDLPriorityComboBox	m_cbDefPriority;
	CAutoComboBox	m_cbAllocByList;
	CAutoComboBox	m_cbAllocToList;
	CAutoComboBox	m_cbStatusList;
	CAutoComboBox	m_cbCategoryList;
	CString	m_sDefCreatedBy;
	double	m_dDefCost;
	BOOL	m_bUpdateInheritAttributes;
	//}}AFX_DATA
	CTimeEdit	m_eTimeEst;
	CTimeEdit	m_eTimeSpent;
	CMaskEdit m_eCost;
	CCheckListBoxEx	m_lbAttribUse;
	int		m_nDefPriority;
	int		m_nDefRisk;
	double		m_dDefTimeEst, m_dDefTimeSpent;
	CString	m_sDefAllocTo;
	CString	m_sDefAllocBy;
	CString	m_sDefStatus;
	CString	m_sDefCategory;
	CColorButton	m_btDefColor;
	COLORREF m_crDef;
	BOOL	m_bInheritParentAttributes;
	int		m_nSelAttribUse;
	BOOL	m_bUseCreationForDefStartDate;
	CWndPromptManager m_mgrPrompts;
	CGroupLineManager m_mgrGroupLines;
	CStringArray m_aDefCats, m_aDefStatus, m_aDefAllocTo, m_aDefAllocBy;

	struct ATTRIBPREF
	{
		ATTRIBPREF() {}
		ATTRIBPREF(UINT nIDName, PTP_ATTRIB attrib, BOOL use) { sName.LoadString(nIDName); nAttrib = attrib; bUse = use; }

		CString sName;
		PTP_ATTRIB nAttrib;
		BOOL bUse;
	};
	CArray<ATTRIBPREF, ATTRIBPREF&> m_aAttribPrefs;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesTaskDefPage)
	public:
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesTaskDefPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSetdefaultcolor();
	afx_msg void OnUseparentattrib();
	afx_msg void OnAttribUseChange();
	afx_msg LRESULT OnListAddItem(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnListDeleteItem(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()
		
	virtual void LoadPreferences(const CPreferences& prefs);
	virtual void SavePreferences(CPreferences& prefs);
	
	BOOL HasCheckedAttributes() const;
	
	static PTDP_LIST MapCtrlIDToList(UINT nListID);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESTASKDEFPAGE_H__852964E3_4ABD_4B66_88BA_F553177616F2__INCLUDED_)
