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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

#if !defined(AFX_TDLSCHEMADEF_5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_)
#define AFX_TDLSCHEMADEF_5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_

static LPCTSTR TDL_ROOT                      = _T("TODOLIST");
static LPCTSTR TDL_CHARSET                   = _T("CHARSET");

static LPCTSTR TDL_LASTMODIFIED              = _T("LASTMODIFIED");
static LPCTSTR TDL_PROJECTNAME               = _T("PROJECTNAME");
static LPCTSTR TDL_LASTSORTBY                = _T("LASTSORTBY");
static LPCTSTR TDL_LASTSORTDIR               = _T("LASTSORTDIR");
static LPCTSTR TDL_NEXTUNIQUEID              = _T("NEXTUNIQUEID");
static LPCTSTR TDL_ARCHIVE                   = _T("ARCHIVE");
static LPCTSTR TDL_FILEFORMAT                = _T("FILEFORMAT");
static LPCTSTR TDL_FILEVERSION               = _T("FILEVERSION");
static LPCTSTR TDL_CHECKEDOUTTO              = _T("CHECKEDOUTTO");
static LPCTSTR TDL_COMMENTSTYPE              = _T("CUSTOMCOMMENTSTYPE");
static LPCTSTR TDL_REPORTTITLE               = _T("REPORTTITLE");
static LPCTSTR TDL_REPORTDATE                = _T("REPORTDATE");
static LPCTSTR TDL_REPORTDATEOLE             = _T("REPORTDATEOLE");
static LPCTSTR TDL_FILENAME                  = _T("FILENAME");

static LPCTSTR TDL_TASK                      = _T("TASK");

static LPCTSTR TDL_TASKID                    = _T("ID");
static LPCTSTR TDL_TASKPARENTID              = _T("PARENTID");
static LPCTSTR TDL_TASKLASTMOD               = _T("LASTMOD");
static LPCTSTR TDL_TASKLASTMODSTRING         = _T("LASTMODSTRING");
static LPCTSTR TDL_TASKTITLE                 = _T("TITLE");
static LPCTSTR TDL_TASKCOMMENTS              = _T("COMMENTS");
static LPCTSTR TDL_TASKALLOCTO               = _T("PERSON");
static LPCTSTR TDL_TASKNUMALLOCTO            = _T("NUMPERSON");
static LPCTSTR TDL_TASKALLOCBY               = _T("ALLOCATEDBY");
static LPCTSTR TDL_TASKCATEGORY              = _T("CATEGORY");
static LPCTSTR TDL_TASKNUMCATEGORY           = _T("NUMCATEGORY");
static LPCTSTR TDL_TASKSTATUS                = _T("STATUS");
static LPCTSTR TDL_TASKFILEREFPATH           = _T("FILEREFPATH");
static LPCTSTR TDL_TASKCREATEDBY             = _T("CREATEDBY");
static LPCTSTR TDL_TASKCOLOR                 = _T("COLOR");
static LPCTSTR TDL_TASKFLAG                  = _T("FLAG");
static LPCTSTR TDL_TASKWEBCOLOR              = _T("WEBCOLOR");
static LPCTSTR TDL_TASKPRIORITY              = _T("PRIORITY");
static LPCTSTR TDL_TASKPERCENTDONE           = _T("PERCENTDONE");
static LPCTSTR TDL_TASKTIMEESTIMATE          = _T("TIMEESTIMATE");
static LPCTSTR TDL_TASKTIMEESTUNITS          = _T("TIMEESTUNITS");
static LPCTSTR TDL_TASKTIMESPENT             = _T("TIMESPENT");
static LPCTSTR TDL_TASKTIMESPENTUNITS        = _T("TIMESPENTUNITS");
static LPCTSTR TDL_TASKTIMESPENT_OLD         = _T("TIMEESPENT"); // typo in name
static LPCTSTR TDL_TASKTIMESPENTUNITS_OLD    = _T("TIMEESPENTUNITS"); // typo in name
static LPCTSTR TDL_TASKDONEDATE              = _T("DONEDATE");
static LPCTSTR TDL_TASKDONEDATESTRING        = _T("DONEDATESTRING");
static LPCTSTR TDL_TASKDUEDATE               = _T("DUEDATE");
static LPCTSTR TDL_TASKDUEDATESTRING         = _T("DUEDATESTRING");
static LPCTSTR TDL_TASKSTARTDATE             = _T("STARTDATE");
static LPCTSTR TDL_TASKSTARTDATESTRING       = _T("STARTDATESTRING");
static LPCTSTR TDL_TASKTEXTCOLOR             = _T("TEXTCOLOR");
static LPCTSTR TDL_TASKCREATIONDATE          = _T("CREATIONDATE");
static LPCTSTR TDL_TASKCREATIONDATESTRING    = _T("CREATIONDATESTRING");
static LPCTSTR TDL_TASKTEXTWEBCOLOR          = _T("TEXTWEBCOLOR");
static LPCTSTR TDL_TASKPRIORITYCOLOR         = _T("PRIORITYCOLOR");
static LPCTSTR TDL_TASKPRIORITYWEBCOLOR      = _T("PRIORITYWEBCOLOR");
static LPCTSTR TDL_TASKPOS                   = _T("POS");
static LPCTSTR TDL_TASKCALCTIMEESTIMATE      = _T("CALCTIMEESTIMATE");
static LPCTSTR TDL_TASKCALCTIMESPENT         = _T("CALCTIMESPENT");
static LPCTSTR TDL_TASKEARLIESTDUEDATE       = _T("EARLIESTDUEDATE");
static LPCTSTR TDL_TASKEARLIESTDUEDATESTRING = _T("EARLIESTDUEDATESTRING");
static LPCTSTR TDL_TASKCALCCOMPLETION        = _T("CALCPERCENTDONE");
static LPCTSTR TDL_TASKHIGHESTPRIORITY       = _T("HIGHESTPRIORITY");
static LPCTSTR TDL_TASKCUSTOMCOMMENTS        = _T("CUSTOMCOMMENTS");
static LPCTSTR TDL_TASKHTMLCOMMENTS          = _T("HTMLCOMMENTS");
static LPCTSTR TDL_TASKRISK                  = _T("RISK");
static LPCTSTR TDL_TASKHIGHESTRISK           = _T("HIGHESTRISK");
static LPCTSTR TDL_TASKEXTERNALID            = _T("EXTERNALID");
static LPCTSTR TDL_TASKCOST                  = _T("COST");
static LPCTSTR TDL_TASKCALCCOST              = _T("CALCCOST");
static LPCTSTR TDL_TASKDEPENDENCY            = _T("DEPENDS");
static LPCTSTR TDL_TASKNUMDEPENDENCY         = _T("NUMDEPENDS");
static LPCTSTR TDL_TASKVERSION               = _T("VERSION");
static LPCTSTR TDL_TASKRECURRENCE            = _T("RECURRENCE");
static LPCTSTR TDL_TASKCOMMENTSTYPE          = _T("COMMENTSTYPE");
static LPCTSTR TDL_TASKICONINDEX             = _T("ICONINDEX");

static LPCTSTR TDL_TASKRECURRENCEREG         = _T("REGULARITY");
static LPCTSTR TDL_TASKRECURRENCESPEC1       = _T("REGPECIFIC1");
static LPCTSTR TDL_TASKRECURRENCESPEC2       = _T("REGPECIFIC2");
static LPCTSTR TDL_TASKRECURRENCEFROMDUE     = _T("REGFROMDUE");
static LPCTSTR TDL_TASKRECURRENCEREUSE       = _T("RECURREUSE");

#endif // AFX_TDLSCHEMADEF_5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_
