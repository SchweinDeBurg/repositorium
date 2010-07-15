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

// PreferencesTaskDefPage.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "PreferencesTaskDefPage.h"
#include "TDCEnum.h"

#include "../../../CodeProject/Source/EnString.h"
#include "../../../CodeProject/Source/Misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTaskDefPage property page

IMPLEMENT_DYNCREATE(CPreferencesTaskDefPage, CPreferencesPageBase)

CPreferencesTaskDefPage::CPreferencesTaskDefPage() : 
	CPreferencesPageBase(CPreferencesTaskDefPage::IDD),
	m_cbAllocByList(TRUE),
	m_cbAllocToList(TRUE),
	m_cbCategoryList(TRUE),
	m_cbStatusList(TRUE)
{
	//{{AFX_DATA_INIT(CPreferencesTaskDefPage)
	m_bUpdateInheritAttributes = FALSE;
	//}}AFX_DATA_INIT
	m_nSelAttribUse = -1;

	// attrib use
	m_aAttribPrefs.Add(ATTRIBPREF(IDS_TDLBC_PRIORITY, PTPA_PRIORITY, -1)); 
	m_aAttribPrefs.Add(ATTRIBPREF(IDS_TDLBC_RISK, PTPA_RISK, -1)); 
	m_aAttribPrefs.Add(ATTRIBPREF(IDS_TDLBC_TIMEEST, PTPA_TIMEEST, -1)); 
	m_aAttribPrefs.Add(ATTRIBPREF(IDS_TDLBC_ALLOCTO, PTPA_ALLOCTO, -1)); 
	m_aAttribPrefs.Add(ATTRIBPREF(IDS_TDLBC_ALLOCBY, PTPA_ALLOCBY, -1)); 
	m_aAttribPrefs.Add(ATTRIBPREF(IDS_TDLBC_STATUS, PTPA_STATUS, -1)); 
	m_aAttribPrefs.Add(ATTRIBPREF(IDS_TDLBC_CATEGORY, PTPA_CATEGORY, -1)); 
	m_aAttribPrefs.Add(ATTRIBPREF(IDS_PTDP_COLOR, PTPA_COLOR, -1)); 
	m_aAttribPrefs.Add(ATTRIBPREF(IDS_PTDP_DUEDATE, PTPA_DUEDATE, -1)); 
	m_aAttribPrefs.Add(ATTRIBPREF(IDS_PTDP_VERSION, PTPA_VERSION, -1)); 
	m_aAttribPrefs.Add(ATTRIBPREF(IDS_TDLBC_STARTDATE, PTPA_STARTDATE, -1)); 

	m_eCost.SetMask(".0123456789", ME_LOCALIZEDECIMAL);
}

CPreferencesTaskDefPage::~CPreferencesTaskDefPage()
{
}

void CPreferencesTaskDefPage::DoDataExchange(CDataExchange* pDX)
{
	CPreferencesPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesTaskDefPage)
	DDX_Control(pDX, IDC_DEFAULTRISK, m_cbDefRisk);
	DDX_Control(pDX, IDC_DEFAULTPRIORITY, m_cbDefPriority);
	DDX_Control(pDX, IDC_ALLOCBYLIST, m_cbAllocByList);
	DDX_Control(pDX, IDC_ALLOCTOLIST, m_cbAllocToList);
	DDX_Control(pDX, IDC_STATUSLIST, m_cbStatusList);
	DDX_Control(pDX, IDC_CATEGORYLIST, m_cbCategoryList);
	DDX_Text(pDX, IDC_DEFAULTCREATEDBY, m_sDefCreatedBy);
	DDX_Text(pDX, IDC_DEFAULTCOST, m_dDefCost);
	DDX_Check(pDX, IDC_UPDATEINHERITATTRIB, m_bUpdateInheritAttributes);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_DEFAULTTIMESPENT, m_eTimeSpent);
	DDX_Control(pDX, IDC_DEFAULTTIMEEST, m_eTimeEst);
	DDX_Control(pDX, IDC_DEFAULTCOST, m_eCost);
	DDX_Control(pDX, IDC_SETDEFAULTCOLOR, m_btDefColor);
	DDX_Control(pDX, IDC_INHERITATTRIBUTES, m_lbAttribUse);
	DDX_CBPriority(pDX, IDC_DEFAULTPRIORITY, m_nDefPriority);
	DDX_CBRisk(pDX, IDC_DEFAULTRISK, m_nDefRisk);
	DDX_Text(pDX, IDC_DEFAULTTIMEEST, m_dDefTimeEst);
	DDX_Text(pDX, IDC_DEFAULTTIMESPENT, m_dDefTimeSpent);
	DDX_Text(pDX, IDC_DEFAULTALLOCTO, m_sDefAllocTo);
	DDX_LBIndex(pDX, IDC_INHERITATTRIBUTES, m_nSelAttribUse);
	DDX_Text(pDX, IDC_DEFAULTALLOCBY, m_sDefAllocBy);
	DDX_Text(pDX, IDC_DEFAULTSTATUS, m_sDefStatus);
	DDX_Text(pDX, IDC_DEFAULTCATEGORY, m_sDefCategory);
	DDX_Check(pDX, IDC_USEPARENTATTRIB, m_bInheritParentAttributes);
	DDX_Check(pDX, IDC_USECREATIONFORDEFSTARTDATE, m_bUseCreationForDefStartDate);
}


BEGIN_MESSAGE_MAP(CPreferencesTaskDefPage, CPreferencesPageBase)
	//{{AFX_MSG_MAP(CPreferencesTaskDefPage)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_SETDEFAULTCOLOR, OnSetdefaultcolor)
	ON_BN_CLICKED(IDC_USEPARENTATTRIB, OnUseparentattrib)
	ON_CLBN_CHKCHANGE(IDC_INHERITATTRIBUTES, OnAttribUseChange)
	ON_REGISTERED_MESSAGE(WM_ACB_ITEMADDED, OnListAddItem)
	ON_REGISTERED_MESSAGE(WM_ACB_ITEMDELETED, OnListDeleteItem)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesTaskDefPage message handlers

BOOL CPreferencesTaskDefPage::OnInitDialog() 
{
	CPreferencesPageBase::OnInitDialog();
	
	m_mgrGroupLines.AddGroupLine(IDC_DEFGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_INHERITGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_DROPLISTGROUP, *this);

	GetDlgItem(IDC_INHERITATTRIBUTES)->EnableWindow(m_bInheritParentAttributes);
	GetDlgItem(IDC_UPDATEINHERITATTRIB)->EnableWindow(m_bInheritParentAttributes);
	
	m_btDefColor.SetColor(m_crDef);

	int nIndex = m_aAttribPrefs.GetSize();
	
	while (nIndex--)
	{
		int nPos = m_lbAttribUse.InsertString(0, m_aAttribPrefs[nIndex].sName);
		m_lbAttribUse.SetCheck(nPos, m_aAttribPrefs[nIndex].bUse ? 1 : 0);
	}

	// init edit prompts()
	m_mgrPrompts.SetEditPrompt(IDC_DEFAULTALLOCTO, *this, CEnString(IDS_PTDP_NAMEPROMPT));
	m_mgrPrompts.SetEditPrompt(IDC_DEFAULTALLOCBY, *this, CEnString(IDS_PTDP_NAMEPROMPT));
	m_mgrPrompts.SetEditPrompt(IDC_DEFAULTSTATUS, *this, CEnString(IDS_PTDP_STATUSPROMPT));
	m_mgrPrompts.SetEditPrompt(IDC_DEFAULTCATEGORY, *this, CEnString(IDS_PTDP_CATEGORYPROMPT));
	m_mgrPrompts.SetEditPrompt(IDC_DEFAULTCREATEDBY, *this, CEnString(IDS_PTDP_NAMEPROMPT));
	m_mgrPrompts.SetComboEditPrompt(IDC_ALLOCTOLIST, *this, CEnString(IDS_PTDP_NAMEPROMPT));
	m_mgrPrompts.SetComboEditPrompt(IDC_ALLOCBYLIST, *this, CEnString(IDS_PTDP_NAMEPROMPT));
	m_mgrPrompts.SetComboEditPrompt(IDC_STATUSLIST, *this, CEnString(IDS_PTDP_STATUSPROMPT));
	m_mgrPrompts.SetComboEditPrompt(IDC_CATEGORYLIST, *this, CEnString(IDS_PTDP_CATEGORYPROMPT));

	// default string lists
	m_cbCategoryList.AddUniqueItems(m_aDefCats);
	m_cbStatusList.AddUniqueItems(m_aDefStatus);
	m_cbAllocToList.AddUniqueItems(m_aDefAllocTo);
	m_cbAllocByList.AddUniqueItems(m_aDefAllocBy);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesTaskDefPage::SetPriorityColors(const CDWordArray& aColors)
{
	m_cbDefPriority.SetColors(aColors);
}

void CPreferencesTaskDefPage::OnOK() 
{
	CPreferencesPageBase::OnOK();

	// default string lists
	m_cbCategoryList.GetItems(m_aDefCats);
	m_cbStatusList.GetItems(m_aDefStatus);
	m_cbAllocToList.GetItems(m_aDefAllocTo);
	m_cbAllocByList.GetItems(m_aDefAllocBy);
}

void CPreferencesTaskDefPage::OnSetdefaultcolor() 
{
	m_crDef = m_btDefColor.GetColor();
}

void CPreferencesTaskDefPage::OnUseparentattrib() 
{
	UpdateData();

	GetDlgItem(IDC_INHERITATTRIBUTES)->EnableWindow(m_bInheritParentAttributes);
	GetDlgItem(IDC_UPDATEINHERITATTRIB)->EnableWindow(m_bInheritParentAttributes);
}

int CPreferencesTaskDefPage::GetListItems(PTDP_LIST nList, CStringArray& aItems) const
{
	switch (nList)
	{
	case PTDP_ALLOCBY:	aItems.Copy(m_aDefAllocBy); break;
	case PTDP_ALLOCTO:	aItems.Copy(m_aDefAllocTo); break;
	case PTDP_STATUS:	aItems.Copy(m_aDefStatus);  break; 
	case PTDP_CATEGORY:	aItems.Copy(m_aDefCats);    break;

	default: ASSERT(0);	break;
	}

	return aItems.GetSize();
}

BOOL CPreferencesTaskDefPage::AddListItem(PTDP_LIST nList, LPCTSTR szItem)
{
	CAutoComboBox* pCB = NULL;
	CStringArray* pItems = NULL;

	switch (nList)
	{
	case PTDP_ALLOCBY:	pItems = &m_aDefAllocBy; pCB = &m_cbAllocByList;  break;
	case PTDP_ALLOCTO:	pItems = &m_aDefAllocTo; pCB = &m_cbAllocToList;  break;
	case PTDP_STATUS:	pItems = &m_aDefStatus;  pCB = &m_cbStatusList;   break; 
	case PTDP_CATEGORY:	pItems = &m_aDefCats;	 pCB = &m_cbCategoryList; break;

	default: ASSERT(0);	return FALSE;
	}

	// add to array first
	int nItem = Misc::Find(*pItems, szItem);

	if (nItem == -1)
	{
		pItems->Add(szItem);

		// then combo
		pCB->AddString(szItem);
		return TRUE;
	}

	return FALSE; // not unique
}

BOOL CPreferencesTaskDefPage::DeleteListItem(PTDP_LIST nList, LPCTSTR szItem)
{
	CAutoComboBox* pCB = NULL;
	CStringArray* pItems = NULL;

	switch (nList)
	{
	case PTDP_ALLOCBY:	pItems = &m_aDefAllocBy; pCB = &m_cbAllocByList;  break;
	case PTDP_ALLOCTO:	pItems = &m_aDefAllocTo; pCB = &m_cbAllocToList;  break;
	case PTDP_STATUS:	pItems = &m_aDefStatus;  pCB = &m_cbStatusList;   break; 
	case PTDP_CATEGORY:	pItems = &m_aDefCats;	 pCB = &m_cbCategoryList; break;

	default: ASSERT(0);	return FALSE;
	}

	// delete from array first
	int nItem = Misc::Find(*pItems, szItem);

	if (nItem != -1)
	{
		pItems->RemoveAt(nItem);

		// then combo
		pCB->DeleteString(szItem);
		return TRUE;
	}

	return FALSE; // not found
}

PTDP_LIST CPreferencesTaskDefPage::MapCtrlIDToList(UINT nListID)
{
	switch (nListID)
	{
	case IDC_ALLOCBYLIST:	return PTDP_ALLOCBY;
	case IDC_ALLOCTOLIST:	return PTDP_ALLOCTO;
	case IDC_STATUSLIST:	return PTDP_STATUS;
	case IDC_CATEGORYLIST:	return PTDP_CATEGORY;
	}

	ASSERT(0);
	return (PTDP_LIST)-1;
}

LRESULT CPreferencesTaskDefPage::OnListAddItem(WPARAM wp, LPARAM lp)
{
	UINT nCtrlID = LOWORD(wp);
	int nList = MapCtrlIDToList(nCtrlID);

	if (nList != -1)
		GetParent()->SendMessage(WM_PTDP_LISTCHANGE, MAKEWPARAM(nList, 1), lp);

	return 0L;
}

LRESULT CPreferencesTaskDefPage::OnListDeleteItem(WPARAM wp, LPARAM lp)
{
	UINT nCtrlID = LOWORD(wp);
	int nList = MapCtrlIDToList(nCtrlID);

	if (nList != -1)
		GetParent()->SendMessage(WM_PTDP_LISTCHANGE, MAKEWPARAM(nList, 0), lp);

	return 0L;
}

void CPreferencesTaskDefPage::OnAttribUseChange()
{
	UpdateData();

	ASSERT (m_nSelAttribUse >= 0);
	
	if (m_nSelAttribUse >= 0)
		m_aAttribPrefs[m_nSelAttribUse].bUse = m_lbAttribUse.GetCheck(m_nSelAttribUse);

	GetDlgItem(IDC_UPDATEINHERITATTRIB)->EnableWindow(m_bInheritParentAttributes);
}

BOOL CPreferencesTaskDefPage::HasCheckedAttributes() const
{
	int nAttrib = m_aAttribPrefs.GetSize();

	while (nAttrib--)
	{
		if (m_aAttribPrefs[nAttrib].bUse)
			return TRUE;
	}

	// else
	return FALSE;
}

int CPreferencesTaskDefPage::GetParentAttribsUsed(CTDCAttributeArray& aAttribs, BOOL& bUpdateAttrib) const
{
	aAttribs.RemoveAll();

	if (m_bInheritParentAttributes)
	{
		bUpdateAttrib = m_bUpdateInheritAttributes;
		int nIndex = (int)m_aAttribPrefs.GetSize();
		
		while (nIndex--)
		{
			if (m_aAttribPrefs[nIndex].bUse)
			{
				switch (m_aAttribPrefs[nIndex].nAttrib)
				{
				case PTPA_PRIORITY: aAttribs.Add(TDCA_PRIORITY); break;
				case PTPA_COLOR:	aAttribs.Add(TDCA_COLOR); break;
				case PTPA_ALLOCTO:	aAttribs.Add(TDCA_ALLOCTO); break;
				case PTPA_ALLOCBY:	aAttribs.Add(TDCA_ALLOCBY); break;
				case PTPA_STATUS:	aAttribs.Add(TDCA_STATUS); break;
				case PTPA_CATEGORY: aAttribs.Add(TDCA_CATEGORY); break;
				case PTPA_TIMEEST:	aAttribs.Add(TDCA_TIMEEST); break;
				case PTPA_RISK:		aAttribs.Add(TDCA_RISK); break;
				case PTPA_DUEDATE:	aAttribs.Add(TDCA_DUEDATE); break;
				case PTPA_VERSION:	aAttribs.Add(TDCA_VERSION); break;
				case PTPA_STARTDATE:aAttribs.Add(TDCA_STARTDATE); break;
				}
			}
		}
	}
	else
		bUpdateAttrib = FALSE;

	return aAttribs.GetSize();
}

int CPreferencesTaskDefPage::GetDefaultCategories(CStringArray& aCats) const
{
	return Misc::ParseIntoArray(m_sDefCategory, aCats);
}

int CPreferencesTaskDefPage::GetDefaultAllocTo(CStringArray& aAllocTo) const
{
	return Misc::ParseIntoArray(m_sDefAllocTo, aAllocTo);
}

double CPreferencesTaskDefPage::GetDefaultTimeEst(int& nUnits) const 
{ 
	nUnits = m_eTimeEst.GetUnits();

	return m_dDefTimeEst; 
}

double CPreferencesTaskDefPage::GetDefaultTimeSpent(int& nUnits) const 
{ 
	nUnits = m_eTimeSpent.GetUnits();

	return m_dDefTimeSpent; 
}

HBRUSH CPreferencesTaskDefPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CPreferencesPageBase::OnCtlColor(pDC, pWnd, nCtlColor);
	
	switch (pWnd->GetDlgCtrlID())
	{
	case IDC_DEFLISTLABEL:
		pDC->SetTextColor(GetSysColor(COLOR_3DDKSHADOW));
		break;
	}
	
	return hbr;
}

void CPreferencesTaskDefPage::LoadPreferences(const CPreferences& prefs)
{
	// load settings
	m_nDefPriority = prefs.GetProfileInt("Preferences", "DefaultPriority", 5); 
	m_nDefRisk = prefs.GetProfileInt("Preferences", "DefaultRisk", 0); 
	m_sDefAllocTo = prefs.GetProfileString("Preferences", "DefaultAllocTo");
	m_sDefAllocBy = prefs.GetProfileString("Preferences", "DefaultAllocBy");
	m_sDefStatus = prefs.GetProfileString("Preferences", "DefaultStatus");
	m_sDefCategory = prefs.GetProfileString("Preferences", "DefaultCategory");
	m_sDefCreatedBy = prefs.GetProfileString("Preferences", "DefaultCreatedBy");
	m_crDef = prefs.GetProfileInt("Preferences", "DefaultColor", 0);
	m_bInheritParentAttributes = prefs.GetProfileInt("Preferences", "InheritParentAttributes", prefs.GetProfileInt("Preferences", "UseParentAttributes"));
	m_bUpdateInheritAttributes = prefs.GetProfileInt("Preferences", "UpdateInheritAttributes", FALSE);
	m_bUseCreationForDefStartDate = prefs.GetProfileInt("Preferences", "UseCreationForDefStartDate", TRUE);
	m_dDefCost = Misc::Atof(prefs.GetProfileString("Preferences", "DefaultCost", "0"));
	m_dDefTimeEst = prefs.GetProfileDouble("Preferences", "DefaultTimeEstimate", 0);
	m_eTimeEst.SetUnits(prefs.GetProfileInt("Preferences", "DefaultTimeEstUnits", THU_HOURS));
	m_dDefTimeSpent = prefs.GetProfileDouble("Preferences", "DefaultTimeSpent", 0);
	m_eTimeSpent.SetUnits(prefs.GetProfileInt("Preferences", "DefaultTimeSpentUnits", THU_HOURS));
	
   // attribute use
	int nIndex = m_aAttribPrefs.GetSize();
	
	while (nIndex--)
	{
		CString sKey;
		sKey.Format("Attrib%d", m_aAttribPrefs[nIndex].nAttrib);
		
		m_aAttribPrefs[nIndex].bUse = prefs.GetProfileInt("Preferences\\AttribUse", sKey, FALSE);
	}

	// combo lists
	prefs.GetArrayItems("Preferences\\CategoryList", m_aDefCats);
	prefs.GetArrayItems("Preferences\\StatusList", m_aDefStatus);
	prefs.GetArrayItems("Preferences\\AllocToList", m_aDefAllocTo);
	prefs.GetArrayItems("Preferences\\AllocByList", m_aDefAllocBy);
}

void CPreferencesTaskDefPage::SavePreferences(CPreferences& prefs)
{
	// save settings
	prefs.WriteProfileInt("Preferences", "DefaultPriority", m_nDefPriority);
	prefs.WriteProfileInt("Preferences", "DefaultRisk", m_nDefRisk);
	prefs.WriteProfileString("Preferences", "DefaultAllocTo", m_sDefAllocTo);
	prefs.WriteProfileString("Preferences", "DefaultAllocBy", m_sDefAllocBy);
	prefs.WriteProfileString("Preferences", "DefaultStatus", m_sDefStatus);
	prefs.WriteProfileString("Preferences", "DefaultCategory", m_sDefCategory);
	prefs.WriteProfileString("Preferences", "DefaultCreatedBy", m_sDefCreatedBy);
	prefs.WriteProfileInt("Preferences", "DefaultColor", m_crDef);
	prefs.WriteProfileInt("Preferences", "InheritParentAttributes", m_bInheritParentAttributes);
	prefs.WriteProfileInt("Preferences", "UpdateInheritAttributes", m_bUpdateInheritAttributes);
	prefs.WriteProfileInt("Preferences", "UseCreationForDefStartDate", m_bUseCreationForDefStartDate);
	prefs.WriteProfileDouble("Preferences", "DefaultCost", m_dDefCost);
	prefs.WriteProfileDouble("Preferences", "DefaultTimeEstimate", m_dDefTimeEst);
	prefs.WriteProfileInt("Preferences", "DefaultTimeEstUnits", m_eTimeEst.GetUnits());
	prefs.WriteProfileDouble("Preferences", "DefaultTimeSpent", m_dDefTimeSpent);
	prefs.WriteProfileInt("Preferences", "DefaultTimeSpentUnits", m_eTimeSpent.GetUnits());
	
	// attribute usage
	int nIndex = m_aAttribPrefs.GetSize();

	while (nIndex--)
	{
		CString sKey;
		sKey.Format("Attrib%d", m_aAttribPrefs[nIndex].nAttrib);

		prefs.WriteProfileInt("Preferences\\AttribUse", sKey, m_aAttribPrefs[nIndex].bUse);
	}

	prefs.WriteArrayItems(m_aDefCats, "Preferences\\CategoryList");
	prefs.WriteArrayItems(m_aDefStatus, "Preferences\\StatusList");
	prefs.WriteArrayItems(m_aDefAllocTo, "Preferences\\AllocToList");
	prefs.WriteArrayItems(m_aDefAllocBy, "Preferences\\AllocByList");
}
