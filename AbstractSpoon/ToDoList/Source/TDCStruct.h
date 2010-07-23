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

#if !defined(AFX_TDCSTRUCT_H__5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_)
#define AFX_TDCSTRUCT_H__5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TDCStruct.h : header file
//

#include "TDCEnum.h"
#include "TaskFile.h"

#include "../../../CodeProject/Source/TreeSelectionHelper.h"
#include "../../../CodeProject/Source/TreeCtrlHelper.h"
#include "../../../CodeProject/Source/Misc.h"
#include "../../../CodeProject/Source/DateHelper.h"

typedef CMap<TDC_STYLE, TDC_STYLE, BOOL, BOOL&> CTDCStyles;
typedef CMap<CString, LPCTSTR, COLORREF, COLORREF&> CTDCColorMap;

struct TDIRECURRENCE
{
//  nRegularity       dwSpecific1        dwSpecific2

//	RTER_DAILY        every 'n' days     --- (0)
//	RTER_WEEKLY       every 'n' weeks    weekdays (TDIW_...)
//	RTER_MONTHLY      every 'n' months   day of month (1-31)
//	RTER_YEARLY       month (1-12)       day of month (1-31)

	TDIRECURRENCE() : nRegularity(TDIR_ONCE), dwSpecific1(1), dwSpecific2(0), bRecalcFromDue(TRUE), nReuse(TDIRO_REUSE) {}

	BOOL operator==(const TDIRECURRENCE& tr) const
	{
		return (tr.nRegularity == nRegularity && tr.dwSpecific1 == dwSpecific1 &&
				tr.dwSpecific2 == dwSpecific2 && tr.bRecalcFromDue == bRecalcFromDue &&
				tr.nReuse == nReuse);
	}

	BOOL operator!=(const TDIRECURRENCE& tr) const
	{
		return !(*this == tr);
	}

	BOOL GetNextOccurence(const COleDateTime& dtFrom, COleDateTime& dtNext) const
	{
		if (dtFrom.GetStatus() != COleDateTime::valid || dtFrom.m_dt == 0.0)
			return FALSE;

		if (nRegularity == TDIR_ONCE)
			return FALSE;

		dtNext = dtFrom;

		switch (nRegularity)		
		{
		case TDIR_DAILY:
			if ((int)dwSpecific1 <= 0)
				return FALSE;

			// add number of days specified by dwSpecific1
			CDateHelper::OffsetDate(dtNext, (int)dwSpecific1, DHU_DAYS);
			break;

		case TDIR_WEEKLY:
			{
				if ((int)dwSpecific1 <= 0/* || !dwSpecific2*/)
					return FALSE;

				// if no weekdays have been set we just add the specified number of weeks
				if (!dwSpecific2)
					CDateHelper::OffsetDate(dtNext, (int)dwSpecific1, DHU_WEEKS);

				else // go looking for the next specified weekday
				{
					// first add any weeks greater than one
					CDateHelper::OffsetDate(dtNext, (int)(dwSpecific1 - 1), DHU_WEEKS);

					// then look for the next weekday *after* this one

					// build a 2 week weekday array so we don't have to deal with 
					// wrapping around
					UINT nWeekdays[14] = 
					{
						(dwSpecific2 & TDIW_SUNDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_MONDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_TUESDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_WEDNESDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_THURSDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_FRIDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_SATURDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_SUNDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_MONDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_TUESDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_WEDNESDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_THURSDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_FRIDAY) ? 1 : 0,
						(dwSpecific2 & TDIW_SATURDAY) ? 1 : 0,
					};

					dtNext += 1.0; // always next day at least

					int nStart = dtFrom.GetDayOfWeek();
					int nEnd = nStart + 7; // week later

					for (int nWeekday = nStart; nWeekday < nEnd; nWeekday++)
					{
						if (nWeekdays[nWeekday] != 0)
							break;
						else
							dtNext += 1.0; 
					}
				}
			}
			break;

		case TDIR_MONTHLY:
			{
				if ((int)dwSpecific1 <= 0 || (dwSpecific2 < 1 || dwSpecific2 > 31))
					return FALSE;
				
				// add number of months specified by dwSpecific1
				CDateHelper::OffsetDate(dtNext, (int)dwSpecific1, DHU_MONTHS);

				// then enforce the day
				SYSTEMTIME st;
				dtNext.GetAsSystemTime(st);
				
				// clip dates to the end of the month
				st.wDay = min((WORD)dwSpecific2, (WORD)CDateHelper::GetDaysInMonth(st.wMonth, st.wYear));

				dtNext = COleDateTime(st);
			}
			break;

		case TDIR_YEARLY:
			{
				if ((dwSpecific1 < 1 || dwSpecific1 > 12) || (dwSpecific2 < 1 || dwSpecific2 > 31))
					return FALSE;
				
				// add a year 
				CDateHelper::OffsetDate(dtNext, 1, DHU_YEARS);

				// then enforce the month and the day
				SYSTEMTIME st;
				dtNext.GetAsSystemTime(st);
				st.wMonth = (WORD)dwSpecific1;
				
				// clip dates to the end of the month
				st.wDay = min((WORD)dwSpecific2, (WORD)CDateHelper::GetDaysInMonth(st.wMonth, st.wYear));

				dtNext = COleDateTime(st);
			}
			break;

		default:
			return FALSE;
		}

		return TRUE;
	}

	TDI_REGULARITY nRegularity;
	DWORD dwSpecific1;
	DWORD dwSpecific2;
	BOOL bRecalcFromDue; // else recalc from 'completed'
	TDI_RECURREUSEOPTION nReuse;

};

struct TDCGETTASKS
{
	TDCGETTASKS() : nFilter(TDCGT_ALL), dwFlags(TDCGTF_ISODATES) 
	{ 
	}

	TDCGETTASKS(const TDCGETTASKS& filter)
	{ 
		nFilter = filter.nFilter;
		dateDueBy = filter.dateDueBy;
		dwFlags = filter.dwFlags;
		sAllocTo = filter.sAllocTo;
		aAttribs.Copy(filter.aAttribs);
	}

	TDCGETTASKS(TDC_GETTASKS filter, DWORD flags = 0, double dueBy = 0) :
		nFilter(filter), dwFlags(flags), dateDueBy(dueBy) 
	{ 
		InitDueByDate(); 
	}

	BOOL WantAttribute(TDC_ATTRIBUTE att) const
	{
		int nAttrib = aAttribs.GetSize();

		// no attributes means 'all attributes' unless
		// TDCGT_CUSTOMATTRIB is set
		if (!nAttrib)
		{
			if (dwFlags & TDCGTF_CUSTOMATTRIB)
				return FALSE;
			else
				return TRUE;
		}

		while (nAttrib--)
		{
			if (aAttribs[nAttrib] == att)
				return TRUE;
		}

		return FALSE;
	}

	TDC_GETTASKS nFilter;
	COleDateTime dateDueBy;
	DWORD dwFlags;
	CString sAllocTo;
	CTDCAttributeArray aAttribs;

protected:
	void InitDueByDate()
	{
		// initialize filter:dateDueBy if necessary
		switch (nFilter)
		{
		case TDCGT_DUE:
			dateDueBy = CDateHelper::GetDate(DHD_TODAY);
			break; // done
			
		case TDCGT_DUETOMORROW:
			dateDueBy += CDateHelper::CalcDaysFromTo(dateDueBy, DHD_TOMORROW, 0);
			break;
			
		case TDCGT_DUETHISWEEK:
			dateDueBy += CDateHelper::CalcDaysFromTo(dateDueBy, DHD_ENDTHISWEEK, 0);
			break;
			
		case TDCGT_DUENEXTWEEK:
			dateDueBy += CDateHelper::CalcDaysFromTo(dateDueBy, DHD_ENDNEXTWEEK, 0);
			break;
			
		case TDCGT_DUETHISMONTH:
			dateDueBy += CDateHelper::CalcDaysFromTo(dateDueBy, DHD_ENDTHISMONTH, 0);
			break;
			
		case TDCGT_DUENEXTMONTH:
			dateDueBy += CDateHelper::CalcDaysFromTo(dateDueBy, DHD_ENDNEXTMONTH, 0);
			break;
		}
	}
};

static TDC_ATTRIBUTE MapColumnToAttribute(TDC_COLUMN col)
{
	switch (col)
	{
	case TDCC_PRIORITY:		return TDCA_PRIORITY;
	case TDCC_PERCENT:		return TDCA_PERCENT;
	case TDCC_TIMEEST:		return TDCA_TIMEEST;
	case TDCC_TIMESPENT:	return TDCA_TIMESPENT;
	case TDCC_STARTDATE:	return TDCA_STARTDATE;
	case TDCC_DUEDATE:		return TDCA_DUEDATE;
	case TDCC_DONEDATE:		return TDCA_DONEDATE;
	case TDCC_ALLOCTO:		return TDCA_ALLOCTO;
	case TDCC_ALLOCBY:		return TDCA_ALLOCBY;
	case TDCC_STATUS:		return TDCA_STATUS;
	case TDCC_CATEGORY:		return TDCA_CATEGORY;
	case TDCC_FILEREF:		return TDCA_FILEREF;
	case TDCC_POSITION:		return TDCA_POSITION;
	case TDCC_ID:			return TDCA_ID;
	case TDCC_FLAG:			return TDCA_FLAG;
	case TDCC_CREATIONDATE: return TDCA_CREATIONDATE;
	case TDCC_CREATEDBY:	return TDCA_CREATEDBY;
	case TDCC_LASTMOD:		return TDCA_LASTMOD;
	case TDCC_RISK:			return TDCA_RISK;
	case TDCC_EXTERNALID:	return TDCA_EXTERNALID;
	case TDCC_COST:			return TDCA_COST;
	case TDCC_DEPENDENCY:	return TDCA_DEPENDENCY;
	case TDCC_RECURRENCE:	return TDCA_RECURRENCE;
	case TDCC_VERSION:		return TDCA_VERSION;
	case TDCC_ICON:			return TDCA_ICON;
	}

	// everything else
	return TDCA_NONE;
}

static TDC_COLUMN MapSortByToColumn(TDC_SORTBY nSortBy)
{
	switch (nSortBy)
	{
	case TDC_SORTBYNAME:		return TDCC_CLIENT;		
	case TDC_SORTBYPRIORITY:	return TDCC_PRIORITY;		
	case TDC_SORTBYPERCENT:		return TDCC_PERCENT;		
	case TDC_SORTBYTIMEEST:		return TDCC_TIMEEST;		
	case TDC_SORTBYTIMESPENT:	return TDCC_TIMESPENT;	
	case TDC_SORTBYSTARTDATE:	return TDCC_STARTDATE;	
	case TDC_SORTBYDUEDATE:		return TDCC_DUEDATE;		
	case TDC_SORTBYDONEDATE:	return TDCC_DONEDATE;		
	case TDC_SORTBYALLOCTO:		return TDCC_ALLOCTO;		
	case TDC_SORTBYALLOCBY:		return TDCC_ALLOCBY;		
	case TDC_SORTBYSTATUS:		return TDCC_STATUS;		
	case TDC_SORTBYCATEGORY:	return TDCC_CATEGORY;		
	case TDC_SORTBYID:			return TDCC_ID;			
	case TDC_SORTBYFLAG:		return TDCC_FLAG;			
	case TDC_SORTBYCREATIONDATE:return TDCC_CREATIONDATE; 
	case TDC_SORTBYCREATEDBY:	return TDCC_CREATEDBY;	
	case TDC_SORTBYLASTMOD:		return TDCC_LASTMOD;		
	case TDC_SORTBYRISK:		return TDCC_RISK;			
	case TDC_SORTBYEXTERNALID:	return TDCC_EXTERNALID;	
	case TDC_SORTBYCOST:		return TDCC_COST;			
	case TDC_SORTBYRECURRENCE:	return TDCC_RECURRENCE;	
	case TDC_SORTBYVERSION:		return TDCC_VERSION;		
	case TDC_SORTBYRECENTEDIT:	return TDCC_RECENTEDIT;	
	case TDC_SORTBYREMAINING:	return TDCC_REMAINING;		
	case TDC_SORTBYDONE:		return TDCC_DONE;		
	case TDC_SORTBYICON:		return TDCC_ICON;		
	}

	// everything else
	return (TDC_COLUMN)-1;
}

static int MapColumnsToAttributes(const CTDCColumnArray& aCols, CTDCAttributeArray& aAttrib)
{
	aAttrib.RemoveAll();
	int nCol = aCols.GetSize();

	while (nCol--)
	{
		TDC_ATTRIBUTE att = MapColumnToAttribute(aCols[nCol]);

		if (att != TDCA_NONE)
			aAttrib.Add(att);
	}

	return aAttrib.GetSize();
}

struct TDLSELECTION
{
	TDLSELECTION(CTreeCtrl& tree) : selection(tree), htiLastHandledLBtnDown(NULL), 
									tch(tree), nLBtnDownFlags(0), nNcLBtnDownColID(-1) {}

	CTreeSelectionHelper selection;
	CTreeCtrlHelper tch;
	HTREEITEM htiLastHandledLBtnDown;
	UINT nLBtnDownFlags;
	int nNcLBtnDownColID;

	BOOL HasIncompleteSubtasks() const
	{
		POSITION pos = selection.GetFirstItemPos();

		while (pos)
		{
			HTREEITEM hti = selection.GetNextItem(pos);

			if (ItemHasIncompleteSubtasks(hti))
				return TRUE;
		}

		return FALSE;
	}
	
	BOOL ItemHasIncompleteSubtasks(HTREEITEM hti) const
	{
		const CTreeCtrl& tree = selection.TreeCtrl();
		HTREEITEM htiChild = tree.GetChildItem(hti);

		while (htiChild)
		{
			if (tch.GetItemCheckState(htiChild) != TCHC_CHECKED || ItemHasIncompleteSubtasks(htiChild))
				return TRUE;
			
			htiChild = tree.GetNextItem(htiChild, TVGN_NEXT);
		}

		return FALSE;
	}

	int GetTaskIDs(CDWordArray& aIDs) const
	{
		aIDs.RemoveAll();
		POSITION pos = selection.GetFirstItemPos();

		while (pos)
		{
			HTREEITEM hti = selection.GetNextItem(pos);
			aIDs.Add(tch.TreeCtrl().GetItemData(hti));
		}

		return aIDs.GetSize();
	}

	void SelectTasks(const CDWordArray& aIDs)
	{
		selection.RemoveAll();
		
		for (int nID = 0; nID < aIDs.GetSize(); nID++)
		{
			HTREEITEM hti = tch.FindItem(aIDs[nID], NULL);

			if (hti)
				selection.AddItem(hti, FALSE);
		}

		selection.FixupTreeSelection();
	}

	BOOL HasIncompleteTasks() const
	{
		// look for first incomplete task
		POSITION pos = selection.GetFirstItemPos();

		while (pos)
		{
			HTREEITEM hti = selection.GetNextItem(pos);

			if (tch.GetItemCheckState(hti) != TCHC_CHECKED)
				return TRUE;
		}

		return FALSE;
	}

};

struct TDLCLIPBOARD
{
	TDLCLIPBOARD() : hwndToDoCtrl(NULL) {}
	
	CTaskFile tasks;
	HWND hwndToDoCtrl;
};

struct TDCCONTROL
{
	LPCTSTR szClass;
	UINT nIDCaption;
	DWORD dwStyle;
	DWORD dwExStyle; 
	int nX;
	int nY;
	int nCx;
	int nCy;
	UINT nID;
};

struct TDCCOLUMN
{
	TDC_COLUMN nColID;
	UINT nIDName;
	UINT nIDLongName;
	TDC_SORTBY nSortBy;
	UINT nAlignment;
	BOOL bClickable;
	LPCTSTR szFont;
	BOOL bSymbolFont;
	BOOL bSortAscending;
};

struct CTRLITEM
{
	UINT nCtrlID;
	UINT nLabelID;
	TDC_COLUMN nCol;
};

struct SEARCHPARAM
{
	SEARCHPARAM(TDC_ATTRIBUTE a = TDCA_NONE, FIND_OPERATOR o = FO_NONE) : 
		attrib(a), op(o), dValue(0), nValue(0), bAnd(TRUE), dwFlags(0) 
	{
		if (!HasValidOperator())
			op = FO_NONE;
	}
	
	SEARCHPARAM(TDC_ATTRIBUTE a, FIND_OPERATOR o, CString s, BOOL and = TRUE) : 
		attrib(a), op(o), dValue(0), nValue(0), bAnd(and), dwFlags(0) 
	{
		if (!HasValidOperator())
			op = FO_NONE;

		if (GetAttribType() == FT_STRING)
			sValue = s;
	}
	
	SEARCHPARAM(TDC_ATTRIBUTE a, FIND_OPERATOR o, double d, BOOL and = TRUE) : 
		attrib(a), op(o), dValue(0), nValue(0), bAnd(and), dwFlags(0) 
	{
		if (!HasValidOperator())
			op = FO_NONE;

		if (GetAttribType() == FT_DOUBLE || GetAttribType() == FT_DATE)
			dValue = d;
	}
	
	SEARCHPARAM(TDC_ATTRIBUTE a, FIND_OPERATOR o, int n, BOOL and = TRUE) : 
		attrib(a), op(o), dValue(0), nValue(0), bAnd(and), dwFlags(0) 
	{
		if (!HasValidOperator())
			op = FO_NONE;

		if (GetAttribType() == FT_INTEGER || GetAttribType() == FT_BOOL)
			nValue = n;
	}

	BOOL operator==(const SEARCHPARAM& rule) const
	{ 
		if (attrib == rule.attrib && op == rule.op && bAnd == rule.bAnd && dwFlags == rule.dwFlags)
		{
			switch (GetAttribType())
			{
			case FT_BOOL:
				return TRUE; // handled by operator

			case FT_DATE:
			case FT_DOUBLE:
			case FT_TIME:
				return (dValue == rule.dValue);

			case FT_INTEGER:
				return (nValue == rule.nValue);

			case FT_STRING:
				return (sValue == rule.sValue);
			}
		}

		return FALSE;
	}
	
	TDC_ATTRIBUTE attrib;
	FIND_OPERATOR op;
	CString sValue;
	int nValue;
	double dValue;
	BOOL bAnd;
	DWORD dwFlags;

	void SetAttribute(TDC_ATTRIBUTE a)
	{
		attrib = a;

		// validate operator
		if (!HasValidOperator())
			op = FO_NONE;
	}

	BOOL SetOperator(FIND_OPERATOR o) 
	{
		if (IsValidOperator(attrib, o))
		{
			op = o;
			return TRUE;
		}

		return FALSE;
	}

	FIND_ATTRIBTYPE GetAttribType() const
	{
		return GetAttribType(attrib);
	}

	static FIND_ATTRIBTYPE GetAttribType(TDC_ATTRIBUTE attrib)
	{
		switch (attrib)
		{
		case TDCA_TASKNAME:
		case TDCA_TASKNAMEORCOMMENTS:
		case TDCA_ANYTEXTATTRIBUTE:
		case TDCA_ALLOCTO:
		case TDCA_ALLOCBY:
		case TDCA_STATUS:
		case TDCA_CATEGORY:
		case TDCA_VERSION:
		case TDCA_COMMENTS:
		case TDCA_FILEREF:
		case TDCA_PROJNAME:
		case TDCA_CREATEDBY:
		case TDCA_EXTERNALID: 
		case TDCA_DEPENDENCY: 
			return FT_STRING;

		case TDCA_PRIORITY:
		case TDCA_COLOR:
		case TDCA_PERCENT:
		case TDCA_RISK:
		case TDCA_ID:
			return FT_INTEGER;

		case TDCA_TIMEEST:
		case TDCA_TIMESPENT:
			return FT_TIME;

		case TDCA_COST:
			return FT_DOUBLE;

		case TDCA_FLAG:
			return FT_BOOL;

		case TDCA_STARTDATE:
		case TDCA_CREATIONDATE:
		case TDCA_DONEDATE:
		case TDCA_DUEDATE:
		case TDCA_LASTMOD:
			return FT_DATE;

	//	case TDCA_RECURRENCE: 
	//	case TDCA_POSITION:
		}

		return FT_NONE;
	}

	static BOOL IsValidOperator(TDC_ATTRIBUTE attrib, FIND_OPERATOR op)
	{
		FIND_ATTRIBTYPE ft = GetAttribType(attrib);

		switch (op)
		{
		case FO_NONE:
			return TRUE;

		case FO_EQUALS:
		case FO_NOT_EQUALS:
			return (ft != FT_BOOL);

		case FO_INCLUDES:
		case FO_NOT_INCLUDES:
			return (ft == FT_STRING);

		case FO_ON_OR_BEFORE:
		case FO_BEFORE:
		case FO_ON_OR_AFTER:
		case FO_AFTER:
			return (ft == FT_DATE);

		case FO_GREATER_OR_EQUAL:
		case FO_GREATER:
		case FO_LESS_OR_EQUAL:
		case FO_LESS:
			return (ft == FT_INTEGER || ft == FT_DOUBLE || ft == FT_TIME);

		case FO_SET:
		case FO_NOT_SET:
			return TRUE;
		}

		return FALSE;
	}

	BOOL HasValidOperator() const
	{
		return IsValidOperator(attrib, op);
	}

	CString ValueAsString() const
	{
		switch (GetAttribType())
		{
		case FT_DATE:
		case FT_DOUBLE:
		case FT_TIME:
			{
				CString sVal;
				sVal.Format(_T("%.3f"), dValue);
				return sVal;
			}
			break;

		case FT_INTEGER:
		case FT_BOOL:
			{
				CString sVal;
				sVal.Format(_T("%d"), nValue);
				return sVal;
			}
			break;

		case FT_STRING:
		default:
			return sValue;
		}
	}
};

typedef CArray<SEARCHPARAM, SEARCHPARAM> CSearchParamArray;


struct SEARCHPARAMS
{
	SEARCHPARAMS() : bIgnoreDone(FALSE), bIgnoreOverDue(FALSE) {}

	BOOL operator!=(const SEARCHPARAMS& params) const
	{
		return !(*this == params);
	}

	BOOL operator==(const SEARCHPARAMS& params) const
	{
		return Misc::ArraysMatch(rules, params.rules, TRUE) && 
			(bIgnoreDone == params.bIgnoreDone) && 
			(bIgnoreOverDue == params.bIgnoreOverDue);
	}

	CSearchParamArray rules;
	BOOL bIgnoreDone, bIgnoreOverDue;
};

struct SEARCHRESULT
{
	SEARCHRESULT() : dwID(0), dwFlags(0) {}

	void operator=(const SEARCHRESULT& res)
	{
		dwID = res.dwID;
		aMatched.Copy(res.aMatched);
		dwFlags = res.dwFlags;
	}

	BOOL HasFlag(DWORD dwFlag) const 
	{
		return ((dwFlags & dwFlag) == dwFlag) ? 1 : 0;
	}

	DWORD dwID;
	DWORD dwFlags;
	CStringArray aMatched;
};

typedef CArray<SEARCHRESULT, SEARCHRESULT&> CResultArray;

struct FTDCFILTER
{
	FTDCFILTER() : nFilter(FT_ALL), nPriority(FT_ANYPRIORITY), nRisk(FT_ANYRISK), 
					dwFlags(FT_ANYALLOCTO | FT_ANYCATEGORY) 
	{
	}

	FTDCFILTER(const FTDCFILTER& filter)
	{
		*this = filter;
	}

	void operator=(const FTDCFILTER& filter)
	{
		nFilter = filter.nFilter;
		nPriority = filter.nPriority;
		nRisk = filter.nRisk;
		aAllocTo.Copy(filter.aAllocTo);
		aStatus.Copy(filter.aStatus);
		aAllocBy.Copy(filter.aAllocBy);
		aCategories.Copy(filter.aCategories);
		aVersions.Copy(filter.aVersions);
		dwFlags = filter.dwFlags;
	}

	BOOL operator==(const FTDCFILTER& filter) const
	{
		return FiltersMatch(*this, filter);
	}

	BOOL operator!=(const FTDCFILTER& filter) const
	{
		return !FiltersMatch(*this, filter);
	}
	
	BOOL IsSet(DWORD dwIgnore = 0) const
	{
		FTDCFILTER filterEmpty;

		return !FiltersMatch(*this, filterEmpty, dwIgnore);
	}

	BOOL HasFlag(DWORD dwFlag) const
	{
		BOOL bHasFlag = ((dwFlags & dwFlag) == dwFlag) ? 1 : 0;

		// some flags depend also on the main filter
		if (bHasFlag)
		{
			switch (dwFlag)
			{
			case FT_ANYCATEGORY:
			case FT_ANYALLOCTO:
			case FT_HIDEPARENTS:
			case FT_HIDECOLLAPSED:
				break; // always allowed

			case FT_HIDEOVERDUE:
				bHasFlag = (nFilter == FT_DUETODAY) ||
							(nFilter == FT_DUETOMORROW) ||
							(nFilter == FT_DUEENDTHISWEEK) || 
							(nFilter == FT_DUEENDNEXTWEEK) || 
							(nFilter == FT_DUEENDTHISMONTH) ||
							(nFilter == FT_DUEENDNEXTMONTH) ||
							(nFilter == FT_DUEENDTHISYEAR) ||
							(nFilter == FT_DUEENDNEXTYEAR);
				break;

			case FT_HIDEDONE:
				bHasFlag = (nFilter != FT_NOTDONE && nFilter != FT_DONE);
				break;

			default:
				ASSERT(0);
				break;		
			}	
		}

		return bHasFlag;
	}

	void SetFlag(DWORD dwFlag, BOOL bOn = TRUE)
	{
		if (bOn)
			dwFlags |= dwFlag;
		else
			dwFlags &= ~dwFlag;
	}

	BOOL MatchAllocTo(const CStringArray& aAllocTos) const
	{
		return MatchItems(aAllocTo, aAllocTos, HasFlag(FT_ANYALLOCTO));
	}

	BOOL MatchCategories(const CStringArray& aCats) const
	{
		return MatchItems(aCategories, aCats, HasFlag(FT_ANYCATEGORY));
	}

	void Reset()
	{
		*this = FTDCFILTER(); // empty filter
	}

	static BOOL FiltersMatch(const FTDCFILTER& filter1, const FTDCFILTER& filter2, DWORD dwIgnore = 0)
	{
		// simple exclusion test first
		if (filter1.nFilter != filter2.nFilter ||
			filter1.nPriority != filter2.nPriority ||
			filter1.nRisk != filter2.nRisk)
		{
			return FALSE;
		}

		// options
		if ((dwIgnore & FT_HIDEPARENTS) == 0)
		{
			if (filter1.HasFlag(FT_HIDEPARENTS) != filter2.HasFlag(FT_HIDEPARENTS))
				return FALSE;
		}

		if ((dwIgnore & FT_HIDEDONE) == 0)
		{
			if (filter1.HasFlag(FT_HIDEDONE) != filter2.HasFlag(FT_HIDEDONE))
				return FALSE;
		}

		if ((dwIgnore & FT_HIDEOVERDUE) == 0)
		{
			if (filter1.HasFlag(FT_HIDEOVERDUE) != filter2.HasFlag(FT_HIDEOVERDUE))
				return FALSE;
		}

		if ((dwIgnore & FT_HIDECOLLAPSED) == 0)
		{
			if (filter1.HasFlag(FT_HIDECOLLAPSED) != filter2.HasFlag(FT_HIDECOLLAPSED))
				return FALSE;
		}

		// compare categories disregarding dwFlags if there's not actually something to compare
		if (filter1.aCategories.GetSize() || filter2.aCategories.GetSize())
		{
			if (!Misc::ArraysMatch(filter1.aCategories, filter2.aCategories) ||
				filter1.HasFlag(FT_ANYCATEGORY) != filter2.HasFlag(FT_ANYCATEGORY))
			{
				return FALSE;
			}
		}

		// do the same with Alloc_To
		if (filter1.aAllocTo.GetSize() || filter2.aAllocTo.GetSize())
		{
			if (!Misc::ArraysMatch(filter1.aAllocTo, filter2.aAllocTo) ||
				filter1.HasFlag(FT_ANYALLOCTO) != filter2.HasFlag(FT_ANYALLOCTO))
			{
				return FALSE;
			}
		}

		// do the same with Alloc_By
		if (filter1.aAllocBy.GetSize() || filter2.aAllocBy.GetSize())
		{
			if (!Misc::ArraysMatch(filter1.aAllocBy, filter2.aAllocBy))
				return FALSE;
		}

		// do the same with Status
		if (filter1.aStatus.GetSize() || filter2.aStatus.GetSize())
		{
			if (!Misc::ArraysMatch(filter1.aStatus, filter2.aStatus))
				return FALSE;
		}

		// do the same with Version
		if (filter1.aVersions.GetSize() || filter2.aVersions.GetSize())
		{
			if (!Misc::ArraysMatch(filter1.aVersions, filter2.aVersions))
				return FALSE;
		}

		return TRUE;
	}
	
	FILTER_TYPE nFilter;
	int nPriority, nRisk;
	CStringArray aCategories, aAllocTo;
	CStringArray aStatus, aAllocBy, aVersions;
	DWORD dwFlags;

	protected:


	static BOOL MatchItems(const CStringArray& aItems1, const CStringArray& aItems2, BOOL bAny)
	{
		// if the filter has no items then everything is a match
		if (aItems1.GetSize() == 0)
			return TRUE;
		
		if (bAny)
		{
			// special case: if aItems2 is empty, test for empty
			// string in aCategories
			if (aItems2.GetSize() == 0)
				return (Misc::Find(aItems1, _T("")) != -1);
			else
				return Misc::MatchAny(aItems1, aItems2);
		}

		// else exact match required
		// since we cannot filter against both having an item
		// and not having an item at the same time
		// we ignore the empty string unless that's all there is
		if (aItems1.GetSize() == 1 && aItems1[0].IsEmpty())
			return (aItems2.GetSize() == 0);

		int nBlank = Misc::Find(aItems1, _T(""));
		
		if (nBlank != -1)
		{
			CStringArray aCopy; 
			aCopy.Copy(aItems1);
			
			while (nBlank != -1)
			{
				aCopy.RemoveAt(nBlank);
				nBlank = Misc::Find(aCopy, _T(""));
			}
			
			// compare what's left
			return Misc::ArraysMatch(aCopy, aItems2);
		}
		else // simple compare
			return Misc::ArraysMatch(aItems1, aItems2);
	}
};

// sort helper structure
class CToDoCtrlData;

struct TDSORTCOLUMNS
{
	TDC_SORTBY nBy1;
	TDC_SORTBY nBy2;
	TDC_SORTBY nBy3;
	BOOL bAscending1;
	BOOL bAscending2;
	BOOL bAscending3;
};

struct TDSORTPARAMS
{
	TDSORTCOLUMNS sort;
	CToDoCtrlData* pData;
	BOOL bSortChildren;
	BOOL bSortDueTodayHigh;
};

class CFilteredToDoCtrl;

struct TDCREMINDER
{
	TDCREMINDER() : dwTaskID(0), pTDC(NULL), dDaysLeadIn(0.0), dDaysSnooze(0.0), nFromWhen(TDCR_DUEDATE), bEnabled(TRUE) {}

	DWORD dwTaskID;
	const CFilteredToDoCtrl* pTDC;
	double dDaysLeadIn;
	double dDaysSnooze;
	TDC_REMINDER nFromWhen;
	BOOL bEnabled;
	CString sSoundFile;
};

#endif // AFX_TDCSTRUCT_H__5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_
