// Written for the ToDoList (http://www.codeproject.com/KB/applications/todolist2.aspx)
// Design and latest version - http://www.codeproject.com/KB/applications/TDL_Calendar.aspx
// by demon.code.monkey@googlemail.com

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - taken out from the original TDL_Calendar package for better sharing
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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

#include "StdAfx.h"
#include "CalendarUtils.h"
#include "CalendarData.h"
#include "IUIExtension.h"

#define CALENDAR_INI_SECTION_NAME	_T("UI_Extension_Plugin_CALENDAR")

CCalendarData::CCalendarData():
m_nTaskID(-1)
{
}

CCalendarData::~CCalendarData()
{
	ClearImportantDateMap();
}

BOOL CCalendarData::IsImportantDate(const COleDateTime& _dt) const
{
	BOOL bReturn = FALSE;

	time_t tDate = CCalendarUtils::DateToTime(_dt);

	CMapTimeToTaskItemList* pmapItems = NULL;
	if (m_mapDateToHTASKITEMs.Lookup(tDate, pmapItems))
	{
		bReturn = TRUE;
	}

	return bReturn;
}

#ifdef _DEBUG
void CCalendarData::DumpTasks(HTASKITEM _hParentTask)
{
	for (HTASKITEM hTask = m_tasks.GetFirstTask(_hParentTask);
		hTask != NULL;
		hTask = m_tasks.GetNextTask(hTask))
	{
		DWORD dwTaskID = m_tasks.GetTaskID(hTask);

		COleDateTime dtStart = m_tasks.GetTaskStartDateOle(hTask);
		COleDateTime dtDue = m_tasks.GetTaskDueDateOle(hTask);

		time_t tStart = 0;
		time_t tDue = 0;
		//these calls needed to get rid of seconds, minutes etc from time_t values
		if (CCalendarUtils::IsDateValid(dtStart))
		{
			tStart = CCalendarUtils::DateToTime(dtStart);
		}
		if (CCalendarUtils::IsDateValid(dtDue))
		{
			tDue = CCalendarUtils::DateToTime(dtDue);
		}

		CString strTaskTitle = m_tasks.GetTaskTitle(hTask);
		CString strTaskStartDate;
		CString strTaskDueDate;
		if (tStart != 0)
		{
			strTaskStartDate = CCalendarUtils::DateToString(dtStart);
		}
		if (tDue != 0)
		{
			strTaskDueDate = CCalendarUtils::DateToString(dtDue);
		}
		TRACE(_T("TASK ID=%d \"%s\": Start=%s, Due=%s\n"), dwTaskID, (LPCTSTR)strTaskTitle, (LPCTSTR)strTaskStartDate,
			(LPCTSTR)strTaskDueDate);

		DumpTasks(hTask);
	}
}
#endif

void CCalendarData::GetTasks(const COleDateTime& _dt, CTaskInfoList& _listTasks) const
{
	ASSERT(IsImportantDate(_dt));
	_listTasks.RemoveAll();

	time_t tDate = CCalendarUtils::DateToTime(_dt);

	CMapTimeToTaskItemList* pmapItems = NULL;
	if (m_mapDateToHTASKITEMs.Lookup(tDate, pmapItems))
	{
		for (POSITION pos = pmapItems->GetStartPosition(); pos;)
		{
			HTASKITEM hTask = NULL;
			void* pUnused = NULL;
			pmapItems->GetNextAssoc(pos, hTask, pUnused);

			int nTaskID = m_tasks.GetTaskID(hTask);
			CString strTaskTitle = m_tasks.GetTaskTitle(hTask);

			COleDateTime dtStartDate = m_tasks.GetTaskStartDateOle(hTask);
			BOOL bIsStart = (CCalendarUtils::CompareDates(dtStartDate, _dt) == 0);
			COleDateTime dtDueDate = m_tasks.GetTaskDueDateOle(hTask);
			BOOL bIsDue = (CCalendarUtils::CompareDates(dtDueDate, _dt) == 0);
			BOOL bIsComplete = m_tasks.IsTaskDone(hTask);

			CTaskInfo ti(nTaskID, strTaskTitle, bIsStart, bIsDue, bIsComplete);
			_listTasks.AddTail(ti);
		}
	}
}

BOOL CCalendarData::GetTaskTitle(DWORD _dwItemID, CString& _strTitle) const
{
	BOOL bReturn = FALSE;

	HTASKITEM hTask = m_tasks.FindTask(_dwItemID);
	if (hTask)
	{
		_strTitle = m_tasks.GetTaskTitle(hTask);
		bReturn = TRUE;
	}

	return bReturn;
}

BOOL CCalendarData::GetTaskStartDate(DWORD _dwItemID, COleDateTime& _date) const
{
	BOOL bReturn = FALSE;

	HTASKITEM hTask = m_tasks.FindTask(_dwItemID);
	if (hTask)
	{
		_date = m_tasks.GetTaskStartDateOle(hTask);
		bReturn = (_date.m_dt != 0);
	}

	return bReturn;
}

BOOL CCalendarData::GetTaskDueDate(DWORD _dwItemID, COleDateTime& _date) const
{
	BOOL bReturn = FALSE;

	HTASKITEM hTask = m_tasks.FindTask(_dwItemID);
	if (hTask)
	{
		_date = m_tasks.GetTaskDueDateOle(hTask);
		bReturn = (_date.m_dt != 0);
	}

	return bReturn;
}

BOOL CCalendarData::GetTaskDoneDate(DWORD _dwItemID, COleDateTime& _date) const
{
	BOOL bReturn = FALSE;

	HTASKITEM hTask = m_tasks.FindTask(_dwItemID);
	if (hTask)
	{
		if (m_tasks.IsTaskDone(hTask))
		{
			_date = m_tasks.GetTaskDoneDateOle(hTask);
			bReturn = (_date.m_dt != 0);
		}
	}

	return bReturn;
}

BOOL CCalendarData::GetTaskTimeEstimate(DWORD _dwItemID, double& _dTimeEstimate, TCHAR& _cUnit) const
{
	BOOL bReturn = FALSE;

	HTASKITEM hTask = m_tasks.FindTask(_dwItemID);
	if (hTask)
	{
		_dTimeEstimate = m_tasks.GetTaskTimeEstimate(hTask, _cUnit, FALSE);
		bReturn = (_dTimeEstimate > 0);
	}

	return bReturn;
}

BOOL CCalendarData::GetTaskTimeSpent(DWORD _dwItemID, double& _dTimeSpent, TCHAR& _cUnit) const
{
	BOOL bReturn = FALSE;

	HTASKITEM hTask = m_tasks.FindTask(_dwItemID);
	if (hTask)
	{
		_dTimeSpent = m_tasks.GetTaskTimeSpent(hTask, _cUnit, FALSE);
		bReturn = (_dTimeSpent > 0);
	}

	return bReturn;
}

CString CCalendarData::GetTaskParentsString(DWORD _dwItemID) const
{
	CString strReturn;
	HTASKITEM hTask = m_tasks.FindTask(_dwItemID);
	if (hTask)
	{
		int nNumParents = 0;
		strReturn = GetTaskParentsString(hTask, nNumParents);
	}
	else
	{
		ASSERT(FALSE);
	}
	return strReturn;
}

CString CCalendarData::GetTaskParentsString(HTASKITEM _hCurrentTask, int& _nNumParents) const
{
	CString strReturn;
	if (_hCurrentTask)
	{
		HTASKITEM hParentTask = m_tasks.GetTaskParent(_hCurrentTask);
		CString strParent = GetTaskParentsString(hParentTask, _nNumParents);
		CString strCurrent = m_tasks.GetTaskTitle(_hCurrentTask);
		if (!strParent.IsEmpty())
		{
			strReturn = strParent;
			strReturn += _T("\n");

			for (int i = 2; i < _nNumParents; i++)
			{
				strReturn += _T("     ");
			}
			strReturn += _T("  |--");
		}
		strReturn += strCurrent;
		_nNumParents++;
	}
	return strReturn;
}

BOOL CCalendarData::TasklistUpdated(const ITaskList* _pTasks, DWORD dwFlags)
{
	BOOL bTasksCopied = FALSE;

	// Need to check dwFlags for how to update m_tasks
	if (dwFlags == UIU_EDIT)
	{
		//_pTasks contains only certain tasks so we must update only those
		HTASKITEM hTask = _pTasks->GetFirstTask();
		bTasksCopied = UpdateTasks(_pTasks, hTask);
	}
	else // copy all tasks
	{
		bTasksCopied = m_tasks.Copy(_pTasks);
	}

	if (bTasksCopied)
	{
		//add tasks with start/due dates to the ImportantDate map
		ClearImportantDateMap();
		CalculateImportantDates();
	}

	// note bTasksCopied may be FALSE if some details were changed
	// that we are not interested in (such as comments, colour etc)

	return bTasksCopied;
}

//updates the current task and all its children, then moves onto its sibling. calls itself recursively
//returns TRUE if it or any of its children or siblings have been updated
BOOL CCalendarData::UpdateTasks(const ITaskList* _pTasks, HTASKITEM _hCurrentTask)
{
	BOOL bTasksCopied = FALSE;

	for (HTASKITEM hTask = _hCurrentTask; hTask != NULL; hTask = _pTasks->GetNextTask(hTask))
	{
		// find corresponding task in m_tasks
		DWORD dwID = _pTasks->GetTaskID(hTask);
		HTASKITEM hDest = m_tasks.FindTask(dwID);

		// update whatever interests us
		if (hDest)
		{
			CString strOldTitle = m_tasks.GetTaskTitle(hDest);
			CString strNewTitle = _pTasks->GetTaskTitle(hTask);
			TCHAR cUnit = _T('h');
			if ((strOldTitle != strNewTitle) ||
				(m_tasks.GetTaskStartDate(hDest) != _pTasks->GetTaskStartDate(hTask)) ||
				(m_tasks.GetTaskDueDate(hDest) != _pTasks->GetTaskDueDate(hTask)) ||
				(m_tasks.GetTaskDoneDate(hDest) != _pTasks->GetTaskDoneDate(hTask)) ||
				(m_tasks.GetTaskTimeEstimate(hDest, cUnit, FALSE) != _pTasks->GetTaskTimeEstimate(hTask, cUnit, FALSE)) ||
				(m_tasks.GetTaskTimeSpent(hDest, cUnit, FALSE) != _pTasks->GetTaskTimeSpent(hTask, cUnit, FALSE)) ||
				(m_tasks.GetTaskPercentDone(hDest, FALSE) != _pTasks->GetTaskPercentDone(hTask, FALSE)))
			{
				m_tasks.SetTaskTitle(hDest, ATL::CT2A(_pTasks->GetTaskTitle(hTask)));
				m_tasks.SetTaskStartDate(hDest, _pTasks->GetTaskStartDate(hTask));
				m_tasks.SetTaskDueDate(hDest, _pTasks->GetTaskDueDate(hTask));
				m_tasks.SetTaskDoneDate(hDest, _pTasks->GetTaskDoneDate(hTask));
				m_tasks.SetTaskTimeEstimate(hDest, _pTasks->GetTaskTimeEstimate(hTask, cUnit, FALSE), cUnit);
				m_tasks.SetTaskTimeSpent(hDest, _pTasks->GetTaskTimeSpent(hTask, cUnit, FALSE), cUnit);
				m_tasks.SetTaskPercentDone(hDest, _pTasks->GetTaskPercentDone(hTask, FALSE));
				//...any other attributes??

				bTasksCopied = TRUE;
			}
		}

		HTASKITEM hTaskFirstChild = _pTasks->GetFirstTask(hTask);
		if (hTaskFirstChild != NULL)
		{
			UpdateTasks(_pTasks, hTaskFirstChild);	//call self recursively
		}
	}

	return bTasksCopied;
}

//clear the cached list of important dates
void CCalendarData::ClearImportantDateMap()
{
	if (!m_mapDateToHTASKITEMs.IsEmpty())
	{
		for (POSITION posClear = m_mapDateToHTASKITEMs.GetStartPosition(); posClear;)
		{
			time_t tUnused = 0;
			CMapTimeToTaskItemList* pmapItems = NULL;
			m_mapDateToHTASKITEMs.GetNextAssoc(posClear, tUnused, pmapItems);
			delete pmapItems;
		}
		m_mapDateToHTASKITEMs.RemoveAll();
	}
}

void CCalendarData::CalculateImportantDates(HTASKITEM _hParentTask/*=NULL*/)
{
	//cache the important dates into m_mapDateToHTASKITEMs
	for (HTASKITEM hTask = m_tasks.GetFirstTask(_hParentTask); hTask != NULL; hTask = m_tasks.GetNextTask(hTask))
	{
		COleDateTime dtStart = m_tasks.GetTaskStartDateOle(hTask);
		COleDateTime dtDue = m_tasks.GetTaskDueDateOle(hTask);

		time_t tStart = 0;
		time_t tDue = 0;
		//these calls needed to get rid of seconds, minutes etc from time_t values
		if (CCalendarUtils::IsDateValid(dtStart))
		{
			tStart = CCalendarUtils::DateToTime(dtStart);
		}
		if (CCalendarUtils::IsDateValid(dtDue))
		{
			tDue = CCalendarUtils::DateToTime(dtDue);
		}

		if (tStart != 0)
		{
			CMapTimeToTaskItemList* pmapItems = NULL;
			if (!m_mapDateToHTASKITEMs.Lookup(tStart, pmapItems))
			{
				pmapItems = new CMapTimeToTaskItemList;
				m_mapDateToHTASKITEMs.SetAt(tStart, pmapItems);
			}
			pmapItems->SetAt(hTask, NULL);
		}

		if (tDue != 0)
		{
			CMapTimeToTaskItemList* pmapItems = NULL;
			if (!m_mapDateToHTASKITEMs.Lookup(tDue, pmapItems))
			{
				pmapItems = new CMapTimeToTaskItemList;
				m_mapDateToHTASKITEMs.SetAt(tDue, pmapItems);
			}
			pmapItems->SetAt(hTask, NULL);
		}

		//call self recursively to do child tasks
		CalculateImportantDates(hTask);
	}
}

void CCalendarData::ReadValueFromIni(const CString& _strKey, CString& _strValue, const char* _pszDefault)
{
	CString strIniFilename = AfxGetApp()->m_pszProfileName;

	CString strDefault;
	if (_pszDefault != NULL)
	{
		strDefault = _pszDefault;
	}

	const DWORD dwChunkSize = 512;

	DWORD dwLen = dwChunkSize;
	TCHAR* pszValue = new TCHAR[dwLen + 1];
	DWORD dwCopied = ::GetPrivateProfileString(CALENDAR_INI_SECTION_NAME, _strKey, strDefault, pszValue, dwLen, strIniFilename);
	while (dwCopied + 1 >= dwLen)
	{
		dwLen += dwChunkSize;
		delete[] pszValue;
		pszValue = new TCHAR[dwLen + 1];
		dwCopied = ::GetPrivateProfileString(CALENDAR_INI_SECTION_NAME, _strKey, strDefault, pszValue, dwLen, strIniFilename);
	}

	_strValue = pszValue;
	delete[] pszValue;
}

void CCalendarData::ReadValueFromIni(const CString& _strKey, DWORD& _dwValue, DWORD _dwDefault)
{
	CString strDefault;
	strDefault.Format(_T("%d"), _dwDefault);

	CString strValue;
	ReadValueFromIni(_strKey, strValue, ATL::CT2A(strDefault));

	_dwValue = _ttol(strValue);
}

void CCalendarData::ReadValueFromIni(const CString& _strKey, int& _nValue, int _nDefault)
{
	ReadValueFromIni(_strKey, (DWORD&)_nValue, _nDefault);
}

void CCalendarData::ReadValueFromIni(const CString& _strKey, CRect& _rcValue, const CRect& _rcDefault)
{
	CString strDefault = RectToString(_rcDefault);
	CString strValue;
	ReadValueFromIni(_strKey, strValue, ATL::CT2A(strDefault));
	_rcValue = StringToRect(strValue);
}

void CCalendarData::WriteValueToIni(const CString& _strKey, const CString& _strValue)
{
	CString strIniFilename = AfxGetApp()->m_pszProfileName;
	VERIFY(::WritePrivateProfileString(CALENDAR_INI_SECTION_NAME, _strKey, _strValue, strIniFilename));
}

void CCalendarData::WriteValueToIni(const CString& _strKey, DWORD _dwValue)
{
	CString strValue;
	strValue.Format(_T("%d"), _dwValue);
	WriteValueToIni(_strKey, strValue);
}

void CCalendarData::WriteValueToIni(const CString& _strKey, const CRect& _rcValue)
{
	CString strValue = RectToString(_rcValue);
	WriteValueToIni(_strKey, strValue);
}

CRect CCalendarData::StringToRect(const CString& _strRect)
{
	CString strRect(_strRect);
	CRect rcReturn(0, 0, 0, 0);

	TCHAR* pch = _tcstok(strRect.GetBuffer(MAX_PATH), _T(","));
	CRect rcValue(0, 0, 0, 0);
	int nLoop = 0;
	while (pch != NULL)
	{
		TRACE(_T("pch:%s\n"), pch);
		int nVal = _ttoi(pch);
		switch (nLoop)
		{
		case 0:
			rcValue.top = nVal;
			break;
		case 1:
			rcValue.left = nVal;
			break;
		case 2:
			rcValue.bottom = nVal;
			break;
		case 3:
			rcValue.right = nVal;
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		nLoop++;
		pch = _tcstok(NULL, _T(","));
	}
	strRect.ReleaseBuffer();

	if (nLoop == 4)
	{
		rcReturn = rcValue;
	}
	else
	{
		ASSERT(FALSE);	//malformed CRect details
	}

	return rcReturn;
}

CString CCalendarData::RectToString(const CRect& _rcRect)
{
	CString strOut;
	strOut.Format(_T("%d,%d,%d,%d"), _rcRect.top, _rcRect.left, _rcRect.bottom, _rcRect.right);
	return strOut;
}
