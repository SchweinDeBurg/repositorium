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

// TaskFileHtmlExporter.cpp: implementation of the CTaskListHtmlExporter class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TasklistHtmlExporter.h"
#include "TDLSchemaDef.h"
#include "RecurringTaskEdit.h"

#include "../../Common/XmlFile.h"
#include "../../Common/Preferences.h"

#include <locale.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const LPCTSTR ENDL = _T("\n");

CTaskListHtmlExporter::CTaskListHtmlExporter()
{
}

CTaskListHtmlExporter::~CTaskListHtmlExporter()
{

}

bool CTaskListHtmlExporter::Export(const ITaskList* pSrcTaskFile, const TCHAR* szDestFilePath, BOOL /*bSilent*/)
{
	CPreferences prefs;

	CString sFont = prefs.GetProfileString(_T("Preferences"), _T("HTMLFont"), _T("Verdana"));
	int nFontSize = prefs.GetProfileInt(_T("Preferences"), _T("HtmlFontSize"), 3);

	DEFAULTFONT.Format(_T("<font face='%s' size='%d'>"), (LPCTSTR)sFont, nFontSize);
	HTMLNOTES.Empty();

	if (prefs.GetProfileInt(_T("Preferences"), _T("ExportSpaceForNotes"), FALSE))
	{
		int nLine = prefs.GetProfileInt(_T("Preferences"), _T("LineSpaces"), 8);

		if (nLine > 0)
		{
			HTMLNOTES = _T("<pre>");

			while (nLine--)
				HTMLNOTES += _T("\n");

			HTMLNOTES += _T("</pre>");
		}
	}

	STRIKETHRUDONE = prefs.GetProfileInt(_T("Preferences"), _T("StrikethroughDone"), TRUE);
	ROUNDTIMEFRACTIONS = prefs.GetProfileInt(_T("Preferences"), _T("RoundTimeFractions"), FALSE);

	CStdioFile fileOut;

	if (fileOut.Open(szDestFilePath, CFile::modeCreate | CFile::modeWrite))
	{
		CString sOutput;
		const ITaskList6* pTasks6 = static_cast<const ITaskList6*>(pSrcTaskFile);

		if (!ExportTask(pTasks6, NULL, 0, 0, sOutput).IsEmpty())
		{
			fileOut.WriteString(_T("<html>\n<head>\n"));
			fileOut.WriteString(FormatCharSet(pTasks6));
			fileOut.WriteString(_T("</head>\n<body>\n"));
			fileOut.WriteString(sOutput);
			fileOut.WriteString(_T("</body>\n</html>\n"));
			return true;
		}
	}

	return false;
}

CString CTaskListHtmlExporter::FormatCharSet(const ITaskList6* pTasks) const
{
	// charset
	CString sCharSet, sCS = ATL::CA2T(pTasks->GetHtmlCharSet());

	if (!sCS.IsEmpty())
		sCharSet.Format(_T("<meta http-equiv=\"content-type\" content=\"text/html; charset=%s\">\n"), (LPCTSTR)sCS);

	return sCharSet;
}

CString& CTaskListHtmlExporter::ExportTask(const ITaskList6* pTasks, HTASKITEM hTask, int nDepth, int nPos, CString& sOutput) const
{
	// handle locale specific decimal separator
	_tsetlocale(LC_NUMERIC, _T(""));

	// if depth == 0 then we're at the root so just add sub-tasks
	// else insert pItem first
	if (nDepth > 0)
	{
		// if there is a POS child item then this replaces nPos
		if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKPOS)))
			nPos = pTasks->GetTaskPosition(hTask);

		CString sTitle, sID, sItem, sPriority, sStartDate, sDueDate, sDoneDate;
		CString sAllocTo, sAllocBy, sCategory, sStatus, sColor(_T("#000000"));
		CString sComments, sPercent, sTimeEst, sTimeSpent, sFileRef;
		CString sLastMod, sRisk, sExtID, sCreateDate, sCreateBy, sCost;
		CString sVersion, sRecurrence, sDepends;
		char cTemp;

		// ID
		FormatAttribute(pTasks, hTask, TDL_TASKID, _T("(ID: %s) "), sID);

		// title
		if (nDepth == 1) // toplevel == bold
			sTitle.Format(_T("<b>%s</b>"), (LPTSTR)ATL::CA2T(pTasks->GetTaskTitle(hTask)));
		else
			sTitle = ATL::CA2T(pTasks->GetTaskTitle(hTask));

		// priority, start/due/done dates
		int nPercent = pTasks->GetTaskPercentDone(hTask, TRUE);
		BOOL bDone = pTasks->IsTaskDone(hTask) || (nPercent == 100);
		int nTimePlaces = ROUNDTIMEFRACTIONS ? 0 : 2;

		// format color string
		if (bDone)
			sColor = _T("#c4c4c4");
		else
			sColor = pTasks->GetTaskWebColor(hTask);

		if (bDone && pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKDONEDATESTRING)))
			sDoneDate.Format(_T(" (completed: %s)"), (LPTSTR)ATL::CA2T(pTasks->GetTaskDoneDateString(hTask)));
		else
		{
			if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKHIGHESTPRIORITY)) ||
				pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKPRIORITY)))
			{
				int nPriority = pTasks->GetTaskPriority(hTask, TRUE);

				if (nPriority >= 0)
				{
					CString sPriorityCol = ATL::CA2T(pTasks->GetTaskPriorityWebColor(hTask));
					sPriority.Format(_T("[<font color='%s'>%d</font>] "), (LPCTSTR)sPriorityCol, nPriority);
				}
				else
					sPriority = _T("[ ]");
			}

			FormatAttribute(pTasks, hTask, TDL_TASKSTARTDATESTRING, _T(" (start: %s)"), sStartDate);
			FormatAttribute(pTasks, hTask, TDL_TASKDUEDATESTRING, _T(" (due: %s)"), sDueDate);

			if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKTIMEESTIMATE)) ||
				pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKCALCTIMEESTIMATE)))
				sTimeEst.Format(_T(" (time est: %.*f hrs)"), nTimePlaces, pTasks->GetTaskTimeEstimate(hTask, cTemp, TRUE));

			if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKPERCENTDONE)))
				sPercent.Format(_T(" (%d%%) "), nPercent);
		}

		if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKTIMESPENT)) ||
			pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKCALCTIMESPENT)))
			sTimeSpent.Format(_T(" (time spent: %.*f hrs)"), nTimePlaces, pTasks->GetTaskTimeSpent(hTask, cTemp, TRUE));
		
		FormatAttribute(pTasks, hTask, TDL_TASKCREATIONDATESTRING, _T(" (created: %s)"), sCreateDate);
		FormatAttribute(pTasks, hTask, TDL_TASKCREATEDBY, _T(" (created by: %s)"), sCreateBy);
		FormatAttributeList(pTasks, hTask, TDL_TASKNUMALLOCTO, TDL_TASKALLOCTO, _T(" (allocated to: %s)"), sAllocTo);
		FormatAttribute(pTasks, hTask, TDL_TASKALLOCBY, _T(" (allocated by: %s)"), sAllocBy);
		FormatAttributeList(pTasks, hTask, TDL_TASKNUMCATEGORY, TDL_TASKCATEGORY, _T(" (category: %s)"), sCategory);
		FormatAttribute(pTasks, hTask, TDL_TASKSTATUS, _T(" (status: %s)"), sStatus);
		FormatAttribute(pTasks, hTask, TDL_TASKRISK, _T(" (risk: %s)"), sRisk);
		FormatAttribute(pTasks, hTask, TDL_TASKEXTERNALID, _T(" (ext.ID: %s)"), sExtID);
		FormatAttribute(pTasks, hTask, TDL_TASKLASTMODSTRING, _T(" (last mod: %s)"), sLastMod);
		FormatAttribute(pTasks, hTask, TDL_TASKCOST, _T(" (cost: %s)"), sCost);
		FormatAttribute(pTasks, hTask, TDL_TASKVERSION, _T(" (version: %s)"), sVersion);
		FormatAttribute(pTasks, hTask, TDL_TASKRECURRENCE, _T(" (occurs: %s)"), sRecurrence);
		FormatAttribute(pTasks, hTask, TDL_TASKDEPENDENCY, _T(" (depends on: %s)"), sDepends);

		// fileref
		if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKFILEREFPATH)))
		{
			CString sFilePath = ATL::CA2T(pTasks->GetTaskFileReferencePath(hTask));
			sFileRef.Format(_T("<br>(link: <a href=\"%s\">%s</a>)"), (LPCTSTR)sFilePath, (LPCTSTR)sFilePath);
		}
		
		// comments
		if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKHTMLCOMMENTS)))
		{
			CString sItemComments = ATL::CA2T(pTasks->GetTaskAttribute(hTask, ATL::CT2A(TDL_TASKHTMLCOMMENTS)));
			sItemComments.TrimRight();

			if (!sItemComments.IsEmpty())
			{
				// note: we reset the font after the comments because the font
				// face and size may well have been changed
				sComments.Format(bDone ? 
					_T("<br><blockquote><font color='#c4c4c4'>[</font>%s%s<font color='#c4c4c4'>]</font></blockquote>") : 
					_T("<br><blockquote>[%s%s]</blockquote>"), 
					(LPCTSTR)sItemComments, (LPCTSTR)DEFAULTFONT);
			}
		}
		else if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(TDL_TASKCOMMENTS)))
		{
			CString sItemComments = ATL::CA2T(pTasks->GetTaskComments(hTask));
			sItemComments.TrimRight();

			if (!sItemComments.IsEmpty())
			{
				TXT2XML(sItemComments); // TODO

				sComments.Format(bDone ?
					_T("<br><blockquote><font color='#c4c4c4'>[%s] </font></blockquote>") : 
					_T("<br><blockquote><font color='#606060'>[%s] </font></blockquote>"), 
					(LPCTSTR)sItemComments);

				// replace carriage returns with <br>
				sComments.Replace(ENDL, _T("<br>"));
				sComments.Replace(_T("\n"), _T("<br>"));

				// replace tab characters with multiple &nbsp;
				sComments.Replace(_T("\t"), _T("&nbsp;&nbsp;&nbsp;&nbsp;"));
			}
		}

		CString sFormat;
		
		if (bDone)
		{
			if (STRIKETHRUDONE)
				sFormat = _T("%s%s%s<font color='%s'><s>%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s</s></font>%s<s>%s</s>%s");
			else
				sFormat = _T("%s%s%s<font color='%s'>%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s</font>%s%s%s");
		}
		else
			sFormat = _T("%s%s%s<font color='%s'>%s</font>%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s");

		sItem.Format(sFormat, (LPCTSTR)sID, (LPCTSTR)sPriority, (LPCTSTR)sPercent, (LPCTSTR)sColor, (LPCTSTR)sTitle,
			(LPCTSTR)sRisk, (LPCTSTR)sAllocTo, (LPCTSTR)sAllocBy, (LPCTSTR)sDepends, (LPCTSTR)sVersion,
			(LPCTSTR)sRecurrence, (LPCTSTR)sCategory, (LPCTSTR)sStatus, (LPCTSTR)sCreateDate, (LPCTSTR)sCreateBy,
			(LPCTSTR)sStartDate, (LPCTSTR)sDueDate, (LPCTSTR)sTimeEst, (LPCTSTR)sTimeSpent, (LPCTSTR)sCost,
			(LPCTSTR)sDoneDate, (LPCTSTR)sExtID, (LPCTSTR)sLastMod, (LPCTSTR)sComments, (LPCTSTR)sFileRef);

		// notes section
		if (!bDone)
			sItem += HTMLNOTES;

		if (nDepth == 1) // toplevel
			sOutput += _T("<br>");

		sOutput += _T("<li>");
		sOutput += sItem;
	}
	else
	{
		sOutput += DEFAULTFONT;

		// title and date
		CString sTitle = ATL::CA2T(pTasks->GetReportTitle());
		CString sDate = ATL::CA2T(pTasks->GetReportDate());

		if (!sTitle.IsEmpty())
		{
			CString sProjTitle;
			sProjTitle.Format(_T("<h2>%s</h2>%s<br>"), (LPCTSTR)sTitle, (LPCTSTR)sDate);
			sOutput += sProjTitle;
		}
		else if (!sDate.IsEmpty())
		{
			sOutput += sDate;
			sOutput += _T("<br>");
		}

	}

	// begin new ordered list for sub-tasks
	hTask = pTasks->GetFirstTask(hTask);

	if (hTask) // at least one sub-task
	{
		sOutput += ENDL;
		sOutput += _T("<ol>");
		sOutput += ENDL;

		int nChildPos = 1;

		while (hTask)
		{
			CString sTask;
			sOutput += ENDL;
			sOutput += ExportTask(pTasks, hTask, nDepth + 1, nChildPos++, sTask);

			hTask = pTasks->GetNextTask(hTask);
		}

		// end ordered list
		sOutput += _T("</ol>");
		sOutput += ENDL;
	}

	// end of item
	if (nDepth > 0)
	{
		sOutput += _T("</li>");
		sOutput += ENDL;
	}
	else
		sOutput += _T("</font>"); // we're done

	// restore decimal separator to '.'
	_tsetlocale(LC_NUMERIC, _T("English"));

	return sOutput;
}

BOOL CTaskListHtmlExporter::FormatAttribute(const ITaskList6* pTasks, HTASKITEM hTask, 
										   LPCTSTR szAttribName, LPCTSTR szFormat, CString& sAttribText)
{
	if (pTasks->TaskHasAttribute(hTask, ATL::CT2A(szAttribName)))
	{
		sAttribText.Format(szFormat, pTasks->GetTaskAttribute(hTask, ATL::CT2A(szAttribName)));
		return TRUE;
	}

	return FALSE;
}

BOOL CTaskListHtmlExporter::FormatAttributeList(const ITaskList6* pTasks, HTASKITEM hTask, 
										   LPCTSTR szNumAttribName, LPCTSTR szAttribName, 
                                           LPCTSTR szFormat, CString& sAttribText)
{
	int nItemCount = atoi(pTasks->GetTaskAttribute(hTask, ATL::CT2A(szNumAttribName)));

	if (nItemCount <= 1)
		return FormatAttribute(pTasks, hTask, szAttribName, szFormat, sAttribText);

	// else more than one (use plus sign as delimiter)
	CString sAttribs = ATL::CA2T(pTasks->GetTaskAttribute(hTask, ATL::CT2A(szAttribName)));
	
	for (int nItem = 1; nItem < nItemCount; nItem++)
	{
		CString sAttribName;
		sAttribName.Format(_T("%s%d"), szAttribName, nItem);
		
		sAttribs += _T('+');
		sAttribs += ATL::CA2T(pTasks->GetTaskAttribute(hTask, ATL::CT2A(sAttribName)));
	}
	
	sAttribText.Format(szFormat, sAttribs);
	return TRUE;
}
