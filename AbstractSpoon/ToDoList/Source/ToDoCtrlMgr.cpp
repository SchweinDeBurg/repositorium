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

// ToDoCtrlMgr.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "ToDoCtrlMgr.h"
#include "TDStringRes.h"

#include "../../../CodeProject/Source/HoldRedraw.h"
#include "../../../CodeProject/Source/EnString.h"
#include "../../Common/Preferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToDoCtrlMgr dialog

enum { IM_NONE = -1, IM_READONLY, IM_CHECKEDIN, IM_CHECKEDOUT, IM_NOTLOADED };

#define ASSERTVALIDINDEX(i) { ASSERT (i >= 0 && i < GetCount()); }

CToDoCtrlMgr::CToDoCtrlMgr(CTabCtrl& tabCtrl) : m_tabCtrl(tabCtrl)
{
}

CToDoCtrlMgr::~CToDoCtrlMgr()
{
}

void CToDoCtrlMgr::SetPrefs(const CPreferencesDlg* pPrefs)
{
	m_pPrefs = pPrefs;
}

const CPreferencesDlg& CToDoCtrlMgr::Prefs() const
{
	ASSERT (m_pPrefs);
	return *m_pPrefs;
}

CFilteredToDoCtrl& CToDoCtrlMgr::GetToDoCtrl(int nIndex)
{
	ASSERTVALIDINDEX(nIndex);

	return *(m_aToDoCtrls[nIndex].pTDC);
}

const CFilteredToDoCtrl& CToDoCtrlMgr::GetToDoCtrl(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	return *(m_aToDoCtrls[nIndex].pTDC);
}

CToDoCtrlMgr::TDCITEM& CToDoCtrlMgr::GetTDCItem(int nIndex)
{
	ASSERTVALIDINDEX(nIndex);

	return m_aToDoCtrls[nIndex];
}

const CToDoCtrlMgr::TDCITEM& CToDoCtrlMgr::GetTDCItem(int nIndex) const
{
	// hack to get around const problem
	CToDoCtrlMgr* pThis = const_cast<CToDoCtrlMgr*>(this);
	
	return pThis->m_aToDoCtrls[nIndex];
}

CString CToDoCtrlMgr::GetFilePath(int nIndex, BOOL bStrict) const
{
	ASSERTVALIDINDEX(nIndex);

	CString sPath = GetToDoCtrl(nIndex).GetFilePath();

	if (sPath.IsEmpty() && !bStrict)
	{
		TCITEM tci;
		TCHAR szText[51];

		tci.mask = TCIF_TEXT;
		tci.cchTextMax = 50;
		tci.pszText = szText;

		m_tabCtrl.GetItem(nIndex, &tci);

		sPath = tci.pszText;
		FileMisc::ValidateFilename(sPath);
	}

	return sPath;
}

CString CToDoCtrlMgr::GetDisplayPath(int nIndex) const
{
	if (IsWebTaskList(nIndex))
		return GetUrl(nIndex);

	// else
	return GetFilePath(nIndex);
}

CString CToDoCtrlMgr::GetFriendlyProjectName(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	const TDCITEM& tdci = GetTDCItem(nIndex);

	return tdci.GetFriendlyProjectName();
}

void CToDoCtrlMgr::ClearFilePath(int nIndex)
{
	ASSERTVALIDINDEX(nIndex);

	GetToDoCtrl(nIndex).ClearFilePath();
}

BOOL CToDoCtrlMgr::IsFilePathEmpty(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	return GetFilePath(nIndex).IsEmpty();
}

TDCM_PATHTYPE CToDoCtrlMgr::GetFilePathType(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	return GetTDCItem(nIndex).GetPathType();
}

BOOL CToDoCtrlMgr::IsWebTaskList(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	return GetTDCItem(nIndex).IsWebTaskList();
}

CString CToDoCtrlMgr::GetUrl(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	const TDCITEM& tdci = GetTDCItem(nIndex);
	
	return tdci.sWebServer + tdci.sUrlPath;
}

BOOL CToDoCtrlMgr::GetWebDetails(int nIndex, CString& sServer, CString& sPath, CString& sUser, CString& sPassword) const
{
	ASSERTVALIDINDEX(nIndex);

	if (!IsWebTaskList(nIndex))
		return FALSE;

	const TDCITEM& tdci = GetTDCItem(nIndex);
	
	sServer = tdci.sWebServer;
	sPath = tdci.sUrlPath;
	sUser = tdci.sUser;
	sPassword = tdci.sPassword;

	return TRUE;
}

void CToDoCtrlMgr::SetWebDetails(int nIndex, LPCTSTR szServer, LPCTSTR szPath, LPCTSTR szUser, LPCTSTR szPassword)
{
	ASSERTVALIDINDEX(nIndex);

	TDCITEM& tdci = GetTDCItem(nIndex);
	
	tdci.sWebServer = szServer;
	tdci.sUrlPath = szPath;
	tdci.sUser = szUser;
	tdci.sPassword = szPassword;

	tdci.RefreshPathType();

	UpdateTabItemText(nIndex);
}

TDCM_PATHTYPE CToDoCtrlMgr::RefreshPathType(int nIndex) 
{
	ASSERTVALIDINDEX(nIndex);

	TDCITEM& tdci = GetTDCItem(nIndex);

	tdci.RefreshPathType();
	return tdci.nPathType;
}

BOOL CToDoCtrlMgr::IsPristine(int nIndex) const
{
	return IsFilePathEmpty(nIndex) && !GetModifiedStatus(nIndex);
}

BOOL CToDoCtrlMgr::IsLoaded(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	return GetTDCItem(nIndex).bLoaded;
}

void CToDoCtrlMgr::SetLoaded(int nIndex, BOOL bLoaded)
{
	ASSERTVALIDINDEX(nIndex);

	GetTDCItem(nIndex).bLoaded = bLoaded;
}

BOOL CToDoCtrlMgr::RefreshLastModified(int nIndex)
{
	ASSERTVALIDINDEX(nIndex);
	TDCITEM& tdci = GetTDCItem(nIndex);

	time_t timeNow = FileMisc::GetLastModified(tdci.pTDC->GetFilePath());
	time_t timePrev = tdci.tLastMod;

	tdci.tLastMod = timeNow;

	// if the tasklist is checked out then always return FALSE because
	// noone else should be able to modify it (by any means).
	if (tdci.pTDC->IsCheckedOut())
		return FALSE;

	// else
	return (timeNow > 0 && timePrev > 0 && timeNow != timePrev);
}

BOOL CToDoCtrlMgr::RefreshReadOnlyStatus(int nIndex)
{
	ASSERTVALIDINDEX(nIndex);

	TDCITEM& tdci = GetTDCItem(nIndex);

	BOOL bReadOnlyNow = CDriveInfo::IsReadonlyPath(tdci.pTDC->GetFilePath()) > 0;
	BOOL bReadOnlyPrev = tdci.bLastStatusReadOnly;

	GetTDCItem(nIndex).bLastStatusReadOnly = bReadOnlyNow;

	return (bReadOnlyNow != -1 && bReadOnlyPrev != -1 && bReadOnlyNow != bReadOnlyPrev);
}

int CToDoCtrlMgr::GetReadOnlyStatus(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	return GetTDCItem(nIndex).bLastStatusReadOnly;
}

void CToDoCtrlMgr::SetDueItemStatus(int nIndex, TDCM_DUESTATUS nStatus)
{
	ASSERTVALIDINDEX(nIndex);

	GetTDCItem(nIndex).nDueStatus = nStatus;
}

TDCM_DUESTATUS CToDoCtrlMgr::GetDueItemStatus(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	return GetTDCItem(nIndex).nDueStatus;
}

int CToDoCtrlMgr::GetLastCheckoutStatus(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	return GetTDCItem(nIndex).bLastCheckoutSuccess;
}

void CToDoCtrlMgr::SetLastCheckoutStatus(int nIndex, BOOL bStatus)
{
	ASSERTVALIDINDEX(nIndex);

	GetTDCItem(nIndex).bLastCheckoutSuccess = bStatus;
}

int CToDoCtrlMgr::CanCheckOut(int nIndex) const
{
	return (CanCheckInOut(nIndex) && !GetToDoCtrl(nIndex).IsCheckedOut());
}

int CToDoCtrlMgr::CanCheckIn(int nIndex) const
{
	return (CanCheckInOut(nIndex) && GetToDoCtrl(nIndex).IsCheckedOut());
}

int CToDoCtrlMgr::CanCheckInOut(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	if (!Prefs().GetEnableSourceControl())
		return FALSE;

	const CFilteredToDoCtrl& tdc = GetToDoCtrl(nIndex);
	
	BOOL bCanCheckInOut = (tdc.CompareFileFormat() != TDCFF_NEWER);
	
	if (bCanCheckInOut)
	{
		bCanCheckInOut &= PathSupportsSourceControl(nIndex);
		
		if (bCanCheckInOut)
			bCanCheckInOut &= !GetReadOnlyStatus(nIndex);
	}

	return bCanCheckInOut;
}

void CToDoCtrlMgr::SetModifiedStatus(int nIndex, BOOL bMod)
{
	ASSERTVALIDINDEX(nIndex);

	GetTDCItem(nIndex).bModified = bMod;
}

BOOL CToDoCtrlMgr::GetModifiedStatus(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	return GetTDCItem(nIndex).bModified;
}

void CToDoCtrlMgr::RefreshModifiedStatus(int nIndex)
{
	ASSERTVALIDINDEX(nIndex);

	TDCITEM& tdci = GetTDCItem(nIndex);
	tdci.bModified = tdci.pTDC->IsModified();
}

void CToDoCtrlMgr::UpdateToDoCtrlReadOnlyUIState(int nIndex)
{
	ASSERTVALIDINDEX(nIndex);

	CFilteredToDoCtrl& tdc = GetToDoCtrl(nIndex);
	UpdateToDoCtrlReadOnlyUIState(tdc);
}

void CToDoCtrlMgr::UpdateToDoCtrlReadOnlyUIState(CFilteredToDoCtrl& tdc)
{
    LPCTSTR szPath = tdc.GetFilePath();
	BOOL bReadOnly = CDriveInfo::IsReadonlyPath(szPath) >  0;
	
    if (!bReadOnly)
        bReadOnly = (tdc.CompareFileFormat() == TDCFF_NEWER);
	
	if (!bReadOnly)
		bReadOnly = (PathSupportsSourceControl(szPath) && !tdc.IsCheckedOut());
	
	if (!bReadOnly && !Prefs().GetEnableSourceControl())
		bReadOnly = tdc.IsSourceControlled();

	tdc.SetReadonly(bReadOnly);
}

int CToDoCtrlMgr::RemoveToDoCtrl(int nIndex, BOOL bDelete)
{
	ASSERTVALIDINDEX(nIndex);

	int nSel = GetSelToDoCtrl(), nNewSel = -1;

	CFilteredToDoCtrl& tdc = GetToDoCtrl(nIndex);
	TDCITEM& tdci = GetTDCItem(nIndex);

	// save state first
	CPreferences prefs;

	SaveColumns(&tdci, prefs);

	if (bDelete)
	{
		// checkin as our final task
		if (tdci.bLoaded && Prefs().GetCheckinOnClose() && PathSupportsSourceControl(nIndex))
			tdc.CheckIn();
	}
	
	m_aToDoCtrls.RemoveAt(nIndex);
	m_tabCtrl.DeleteItem(nIndex);

	// set new selection as close to previous as possible
	// *before* deleting the tasklist
	if (GetCount())
	{
		if (nIndex <= nSel)
			nNewSel = max(0, nSel - 1);
		else
			nNewSel = nSel;

		m_tabCtrl.SetCurSel(nNewSel);
	}

	if (bDelete)
	{
		tdc.DestroyWindow();
		delete &tdc;
	}

   return nNewSel;
}

int CToDoCtrlMgr::AddToDoCtrl(CFilteredToDoCtrl* pCtrl, BOOL bLoaded)
{
	static int nNextIndex = 0;

	TDCITEM tdci(pCtrl, nNextIndex++, bLoaded);

	// restore previous state
	CPreferences prefs;

	RestoreColumns(&tdci, prefs);

	// add to tab
	int nSel = m_aToDoCtrls.Add(tdci);
	m_tabCtrl.InsertItem(nSel, _T(""));

	UpdateTabItemText(nSel);
	RefreshPathType(nSel);


	return nSel;
}

void CToDoCtrlMgr::RefreshColumns(int nIndex, CTDCColumnArray& aColumns /*out*/)
{
	ASSERTVALIDINDEX(nIndex);

	// if the todoctrl does not have its own columns we use the default
	// preferences set else we return it's own set.
	if (!HasOwnColumns(nIndex))
	{
		Prefs().GetVisibleColumns(aColumns);
		GetToDoCtrl(nIndex).SetVisibleColumns(aColumns);
	}
	else
		GetToDoCtrl(nIndex).GetVisibleColumns(aColumns);
}

BOOL CToDoCtrlMgr::HasOwnColumns(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	return GetTDCItem(nIndex).bHasOwnColumns;
}

void CToDoCtrlMgr::SetHasOwnColumns(int nIndex, BOOL bHas)
{
	ASSERTVALIDINDEX(nIndex);

	GetTDCItem(nIndex).bHasOwnColumns = bHas;
}

void CToDoCtrlMgr::RestoreColumns(TDCITEM* pTDCI, const CPreferences& prefs)
{
	CTDCColumnArray aColumns;
	CString sKey = pTDCI->pTDC->GetPreferencesKey(_T("Columns"));

	if (!sKey.IsEmpty())
	{
		int nItem = prefs.GetProfileInt(sKey, _T("Count"), -1);

		if (nItem < 0)
		{
			pTDCI->bHasOwnColumns = FALSE;
			Prefs().GetVisibleColumns(aColumns);
		}
		else
		{
			pTDCI->bHasOwnColumns = TRUE;

			while (nItem--)
			{
				int nCol = prefs.GetProfileInt(sKey, CEnString(_T("Item%d"), nItem), -1);
				
				if (nCol != -1)
					aColumns.Add((TDC_COLUMN)nCol);
			}
		}
	}
	else // new tasklist => use defaults
		Prefs().GetVisibleColumns(aColumns);

	pTDCI->pTDC->SetVisibleColumns(aColumns);
}

void CToDoCtrlMgr::SaveColumns(const TDCITEM* pTDCI, CPreferences& prefs) const
{
	CString sKey = pTDCI->pTDC->GetPreferencesKey(_T("Columns"));

	if (!sKey.IsEmpty())
	{		
		if (pTDCI->bHasOwnColumns)
		{
			CTDCColumnArray aColumns;
			int nItem = pTDCI->pTDC->GetVisibleColumns(aColumns);
			
			prefs.WriteProfileInt(sKey, _T("Count"), nItem);
			
			while (nItem--)
				prefs.WriteProfileInt(sKey, CEnString(_T("Item%d"), nItem), aColumns[nItem]);
		}
		else
			prefs.WriteProfileInt(sKey, _T("Count"), -1);
	}
}

time_t CToDoCtrlMgr::GetLastModified(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	return GetTDCItem(nIndex).tLastMod;
}

void CToDoCtrlMgr::MoveToDoCtrl(int nIndex, int nNumPlaces)
{
	ASSERTVALIDINDEX(nIndex);

	int nOrgCount = m_tabCtrl.GetItemCount();

	if (!CanMoveToDoCtrl(nIndex, nNumPlaces))
		return;

	// cache selection so we can restore it afterwards
	int nSel = GetSelToDoCtrl(), nNewSel = nSel;

	// work out what the new selection should be
	if (nIndex == nSel)
		nNewSel = (nIndex + nNumPlaces);

	else if (nIndex > nSel && (nIndex + nNumPlaces) <= nSel)
		nNewSel++;

	else if (nIndex < nSel && (nIndex + nNumPlaces) >= nSel)
		nNewSel--;

	// make copies of existing
	TCITEM tci;
	TCHAR szText[128];
	tci.mask = TCIF_TEXT | TCIF_IMAGE;
	tci.pszText = szText;
	tci.cchTextMax = 127;
		
	m_tabCtrl.GetItem(nIndex, &tci);
	TDCITEM tdci = GetTDCItem(nIndex);

	// remove and re-add
	RemoveToDoCtrl(nIndex);

	nIndex += nNumPlaces;

	m_aToDoCtrls.InsertAt(nIndex, tdci);
	m_tabCtrl.InsertItem(nIndex, &tci);

	// set selection
	m_tabCtrl.SetCurSel(nNewSel);

	ASSERT (nOrgCount == m_tabCtrl.GetItemCount());
}

BOOL CToDoCtrlMgr::CanMoveToDoCtrl(int nIndex, int nNumPlaces) const
{
	ASSERTVALIDINDEX(nIndex);

	nIndex += nNumPlaces;

	return (nIndex >= 0 && nIndex < GetCount());
}

CString CToDoCtrlMgr::GetArchivePath(LPCTSTR szFilePath) const
{
	CString sArchivePath, sFilePath(szFilePath);
	sFilePath.MakeLower();

	if (!sFilePath.IsEmpty() && sFilePath.Find(_T(".done")) == -1) // don't archive archives!
	{
		CString sDrive, sPath, sFName, sExt;

		FileMisc::SplitPath(sFilePath, &sDrive, &sPath, &sFName, &sExt);
		FileMisc::MakePath(sArchivePath, sDrive, sPath, sFName, _T(".done") + sExt);
	}
	
	return sArchivePath;
}

CString CToDoCtrlMgr::GetArchivePath(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	const CFilteredToDoCtrl& tdc = GetToDoCtrl(nIndex);

	return GetArchivePath(tdc.GetFilePath());
}

int CToDoCtrlMgr::ArchiveDoneTasks(int nIndex)
{
	ASSERTVALIDINDEX(nIndex);

	CFilteredToDoCtrl& tdc = GetToDoCtrl(nIndex);

	TDC_ARCHIVE nRemove = TDC_REMOVENONE;
	
	if (Prefs().GetRemoveArchivedTasks())
	{
		if (Prefs().GetRemoveOnlyOnAbsoluteCompletion())
			nRemove = TDC_REMOVEIFSIBLINGSANDSUBTASKSCOMPLETE;
		else
			nRemove = TDC_REMOVEALL;
	}
	
	return tdc.ArchiveDoneTasks(GetArchivePath(tdc.GetFilePath()), nRemove, !Prefs().GetDontRemoveFlagged());
}

int CToDoCtrlMgr::ArchiveSelectedTasks(int nIndex)
{
	ASSERTVALIDINDEX(nIndex);

	CFilteredToDoCtrl& tdc = GetToDoCtrl(nIndex);
	
	return tdc.ArchiveSelectedTasks(GetArchivePath(tdc.GetFilePath()), Prefs().GetRemoveArchivedTasks());
}

int CToDoCtrlMgr::SortToDoCtrlsByName()
{
	int nSel = GetSelToDoCtrl();

	if (!AreToDoCtrlsSorted())
	{
		// save off current selection 
		CFilteredToDoCtrl* pTDC = &GetToDoCtrl(nSel);

		qsort(m_aToDoCtrls.GetData(), m_aToDoCtrls.GetSize(), sizeof(TDCITEM), SortProc);

		nSel = -1;

		// redo the tab text
		for (int nTDC = 0; nTDC < GetCount(); nTDC++)
		{
			UpdateTabItemText(nTDC);

			// check if this was the selected item
			if (pTDC == &GetToDoCtrl(nTDC))
				nSel = nTDC;
		}

		// set selection
		ASSERT (nSel != -1);
	}

	return nSel;
}

int CToDoCtrlMgr::SortProc(const void* v1, const void* v2)
{
	TDCITEM* pTDCI1 = (TDCITEM*)v1;
	TDCITEM* pTDCI2 = (TDCITEM*)v2;

	CString sName1 = pTDCI1->GetFriendlyProjectName();
	CString sName2 = pTDCI2->GetFriendlyProjectName();

	return sName1.CompareNoCase(sName2);
}

BOOL CToDoCtrlMgr::AreToDoCtrlsSorted() const
{
	if (GetCount() <= 1)
		return TRUE;

	// check that each item is 'less' than the next
	CString sPrev = GetTDCItem(0).GetFriendlyProjectName();

	for (int nTDC = 1; nTDC < GetCount(); nTDC++)
	{
		CString sTDC = GetTDCItem(nTDC).GetFriendlyProjectName();

		if (sPrev.CompareNoCase(sTDC) > 0)
			return FALSE;

		sPrev = sTDC;
	}

	return TRUE;
}

CString CToDoCtrlMgr::UpdateTabItemText(int nIndex)
{
	if (nIndex < 0)
	{
		nIndex = GetSelToDoCtrl();
		
		if (nIndex < 0)
			return _T("");
	}
	
	TDCITEM& tdci = GetTDCItem(nIndex);
	
	// project name
	CString sProjectName = tdci.GetFriendlyProjectName();

	// double up ampersands to avoid unexpected underlining
	sProjectName.Replace(_T("&"), _T("&&"));

	if (tdci.pTDC->IsModified() && !tdci.pTDC->IsReadOnly())
		sProjectName += _T("*");
	
	// appropriate icon
	int nImage = IM_NONE;

	if (!tdci.bLoaded)
		nImage = IM_NOTLOADED;
	
	else if (tdci.bLastStatusReadOnly > 0)
		nImage = IM_READONLY;
	
	else if (PathSupportsSourceControl(nIndex))
        nImage = tdci.pTDC->IsCheckedOut() ? IM_CHECKEDOUT : IM_CHECKEDIN;
	
	else if (tdci.pTDC->IsReadOnly())
		nImage = IM_READONLY;
	
    // update tab array
	TCITEM tci;
	tci.mask = TCIF_TEXT | TCIF_IMAGE;
	tci.pszText = (LPTSTR)(LPCTSTR)sProjectName;
	tci.iImage = nImage;
	
	m_tabCtrl.SetItem(nIndex, &tci);

	// remove doubled-up ampersands
	sProjectName.Replace("&&", "&");
	
	return sProjectName;
}

CString CToDoCtrlMgr::GetTabItemText(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	TCITEM tci;
	CString sText;
	
	tci.mask = TCIF_TEXT;
	tci.pszText = sText.GetBuffer(128);
	tci.cchTextMax = 127;
		
	m_tabCtrl.GetItem(nIndex, &tci);

	sText.ReleaseBuffer();
	sText.Replace(_T("&&"), _T("&")); // remove doubled-up ampersands

	return sText;
}

CString CToDoCtrlMgr::GetTabItemTooltip(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);

	TCITEM tci;
	tci.mask = TCIF_IMAGE;

	m_tabCtrl.GetItem(nIndex, &tci);

	switch (tci.iImage)
	{
	case IM_READONLY:	return CEnString(IDS_TABTIP_READONLY);
	case IM_CHECKEDIN:	return CEnString(IDS_TABTIP_CHECKEDIN);
	case IM_CHECKEDOUT: return CEnString(IDS_TABTIP_CHECKEDOUT);
	case IM_NOTLOADED:	return CEnString(IDS_TABTIP_NOTLOADED);

	case IM_NONE:
	default:
		break;
	}

	return _T("");
}

BOOL CToDoCtrlMgr::PathTypeSupportsSourceControl(TDCM_PATHTYPE nType) const
{
	switch (nType)
    {
    case TDCM_FIXED:          
    case TDCM_REMOTE:         
        break;
		
    default: // everything else
        return FALSE;
    }
	
	BOOL bEnableSourceControl = Prefs().GetEnableSourceControl();
	BOOL bLanOnly = Prefs().GetSourceControlLanOnly();
	
    return (bEnableSourceControl && (!bLanOnly || nType == TDCM_REMOTE));
}

BOOL CToDoCtrlMgr::PathSupportsSourceControl(LPCTSTR szPath) const
{
    int nType = CDriveInfo::GetPathType(szPath);

	return PathTypeSupportsSourceControl(TDCITEM::TranslatePathType(nType));
}

BOOL CToDoCtrlMgr::PathSupportsSourceControl(int nIndex) const
{
	TDCM_PATHTYPE nType = GetFilePathType(nIndex);
	
	return PathTypeSupportsSourceControl(nType);
}

BOOL CToDoCtrlMgr::IsSourceControlled(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);
	
	return GetToDoCtrl(nIndex).IsSourceControlled();
}

void CToDoCtrlMgr::SetNeedsPreferenceUpdate(int nIndex, BOOL bNeed)
{
	ASSERTVALIDINDEX(nIndex);
	
	GetTDCItem(nIndex).bNeedPrefUpdate = bNeed;
}

BOOL CToDoCtrlMgr::GetNeedsPreferenceUpdate(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);
	
	return GetTDCItem(nIndex).bNeedPrefUpdate;
}

void CToDoCtrlMgr::SetAllNeedPreferenceUpdate(BOOL bNeed, int nExcept)
{
	for (int nIndex = 0; nIndex < m_aToDoCtrls.GetSize(); nIndex++)
	{
		if (nIndex != nExcept)
			GetTDCItem(nIndex).bNeedPrefUpdate = bNeed;
	}
}

int CToDoCtrlMgr::FindToDoCtrl(LPCTSTR szFilePath) const
{
	if (!szFilePath || GetFileAttributes(szFilePath) != 0xffffffff)
	{
		int nCtrl = GetCount();
	
		while (nCtrl--)
		{
			if (GetFilePath(nCtrl).CompareNoCase(szFilePath) == 0)
				return nCtrl;
		}
	}

	return -1;
}

int CToDoCtrlMgr::FindToDoCtrl(const CFilteredToDoCtrl* pTDC) const
{
	int nCtrl = GetCount();
	
	while (nCtrl--)
	{
		if (pTDC == GetTDCItem(nCtrl).pTDC)
			return nCtrl;
	}

	return -1;
}

void CToDoCtrlMgr::PreparePopupMenu(CMenu& menu, UINT nID1, int nMax) const
{
	//fabio_2005
	int nTDC = 0;
	for (nTDC = 0; nTDC < GetCount() && nTDC < nMax; nTDC++)
	{
		CString sTaskList = GetFriendlyProjectName(nTDC);

		menu.ModifyMenu(nID1 + nTDC, MF_BYCOMMAND, nID1 + nTDC, sTaskList);
	}

	// delete any unused items
	for (; nTDC < nMax; nTDC++)
		menu.RemoveMenu(nID1 + nTDC, MF_BYCOMMAND);

}

BOOL CToDoCtrlMgr::HasTasks(int nIndex) const
{
	ASSERTVALIDINDEX(nIndex);
	
	return GetToDoCtrl(nIndex).GetTaskCount();
}

BOOL CToDoCtrlMgr::AnyHasTasks() const
{
	int nCtrl = GetCount();
	
	while (nCtrl--)
	{
		if (HasTasks(nCtrl))
			return TRUE;
	}
	
	return FALSE;
}
