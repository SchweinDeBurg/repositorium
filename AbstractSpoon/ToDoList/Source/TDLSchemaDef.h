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

#if !defined(AFX_TDLSCHEMADEF_5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_)
#define AFX_TDLSCHEMADEF_5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_


static LPCTSTR TDL_ROOT 					= "TODOLIST";
static LPCTSTR TDL_CHARSET 					= "CHARSET";

static LPCTSTR TDL_LASTMODIFIED 			= "LASTMODIFIED";
static LPCTSTR TDL_PROJECTNAME				= "PROJECTNAME";
static LPCTSTR TDL_LASTSORTBY				= "LASTSORTBY";
static LPCTSTR TDL_LASTSORTDIR				= "LASTSORTDIR";
static LPCTSTR TDL_NEXTUNIQUEID 			= "NEXTUNIQUEID";
static LPCTSTR TDL_ARCHIVE					= "ARCHIVE";
static LPCTSTR TDL_FILEFORMAT				= "FILEFORMAT";
static LPCTSTR TDL_FILEVERSION				= "FILEVERSION";
static LPCTSTR TDL_CHECKEDOUTTO 			= "CHECKEDOUTTO";
static LPCTSTR TDL_COMMENTSTYPE				= "CUSTOMCOMMENTSTYPE";		
static LPCTSTR TDL_REPORTTITLE				= "REPORTTITLE";		
static LPCTSTR TDL_REPORTDATE				= "REPORTDATE";		
static LPCTSTR TDL_REPORTDATEOLE			= "REPORTDATEOLE";		
static LPCTSTR TDL_FILENAME					= "FILENAME";	
	
static LPCTSTR TDL_TASK 					= "TASK";

static LPCTSTR TDL_TASKID					= "ID";
static LPCTSTR TDL_TASKLASTMOD				= "LASTMOD";
static LPCTSTR TDL_TASKLASTMODSTRING		= "LASTMODSTRING";
static LPCTSTR TDL_TASKTITLE				= "TITLE";
static LPCTSTR TDL_TASKCOMMENTS 			= "COMMENTS";
static LPCTSTR TDL_TASKALLOCTO				= "PERSON";
static LPCTSTR TDL_TASKNUMALLOCTO 			= "NUMPERSON";
static LPCTSTR TDL_TASKALLOCBY				= "ALLOCATEDBY";
static LPCTSTR TDL_TASKCATEGORY 			= "CATEGORY";
static LPCTSTR TDL_TASKNUMCATEGORY 			= "NUMCATEGORY";
static LPCTSTR TDL_TASKSTATUS				= "STATUS";
static LPCTSTR TDL_TASKFILEREFPATH			= "FILEREFPATH";
static LPCTSTR TDL_TASKCREATEDBY			= "CREATEDBY";
static LPCTSTR TDL_TASKCOLOR				= "COLOR";
static LPCTSTR TDL_TASKFLAG					= "FLAG";
static LPCTSTR TDL_TASKWEBCOLOR 			= "WEBCOLOR";
static LPCTSTR TDL_TASKPRIORITY 			= "PRIORITY";
static LPCTSTR TDL_TASKPERCENTDONE			= "PERCENTDONE";
static LPCTSTR TDL_TASKTIMEESTIMATE 		= "TIMEESTIMATE";
static LPCTSTR TDL_TASKTIMEESTUNITS 		= "TIMEESTUNITS";
static LPCTSTR TDL_TASKTIMESPENT			= "TIMESPENT";
static LPCTSTR TDL_TASKTIMESPENTUNITS		= "TIMESPENTUNITS";
static LPCTSTR TDL_TASKTIMESPENT_OLD		= "TIMEESPENT"; // typo in name
static LPCTSTR TDL_TASKTIMESPENTUNITS_OLD	= "TIMEESPENTUNITS"; // typo in name
static LPCTSTR TDL_TASKDONEDATE 			= "DONEDATE";
static LPCTSTR TDL_TASKDONEDATESTRING		= "DONEDATESTRING";		
static LPCTSTR TDL_TASKDUEDATE				= "DUEDATE";
static LPCTSTR TDL_TASKDUEDATESTRING		= "DUEDATESTRING"; 	
static LPCTSTR TDL_TASKSTARTDATE			= "STARTDATE";
static LPCTSTR TDL_TASKSTARTDATESTRING		= "STARTDATESTRING";		
static LPCTSTR TDL_TASKTEXTCOLOR			= "TEXTCOLOR";		
static LPCTSTR TDL_TASKCREATIONDATE			= "CREATIONDATE";
static LPCTSTR TDL_TASKCREATIONDATESTRING	= "CREATIONDATESTRING";		
static LPCTSTR TDL_TASKTEXTWEBCOLOR			= "TEXTWEBCOLOR";		
static LPCTSTR TDL_TASKPRIORITYCOLOR		= "PRIORITYCOLOR";		
static LPCTSTR TDL_TASKPRIORITYWEBCOLOR		= "PRIORITYWEBCOLOR";		
static LPCTSTR TDL_TASKPOS					= "POS";
static LPCTSTR TDL_TASKCALCTIMEESTIMATE		= "CALCTIMEESTIMATE";		
static LPCTSTR TDL_TASKCALCTIMESPENT		= "CALCTIMESPENT";		
static LPCTSTR TDL_TASKEARLIESTDUEDATE		= "EARLIESTDUEDATE";		
static LPCTSTR TDL_TASKEARLIESTDUEDATESTRING = "EARLIESTDUEDATESTRING";		
static LPCTSTR TDL_TASKCALCCOMPLETION		= "CALCPERCENTDONE";		
static LPCTSTR TDL_TASKHIGHESTPRIORITY		= "HIGHESTPRIORITY";		
static LPCTSTR TDL_TASKCUSTOMCOMMENTS		= "CUSTOMCOMMENTS";		
static LPCTSTR TDL_TASKHTMLCOMMENTS			= "HTMLCOMMENTS";		
static LPCTSTR TDL_TASKRISK					= "RISK";		
static LPCTSTR TDL_TASKHIGHESTRISK			= "HIGHESTRISK";		
static LPCTSTR TDL_TASKEXTERNALID			= "EXTERNALID";		
static LPCTSTR TDL_TASKCOST					= "COST";		
static LPCTSTR TDL_TASKCALCCOST				= "CALCCOST";		
static LPCTSTR TDL_TASKDEPENDENCY			= "DEPENDS";
static LPCTSTR TDL_TASKNUMDEPENDENCY		= "NUMDEPENDS";
static LPCTSTR TDL_TASKVERSION				= "VERSION";
static LPCTSTR TDL_TASKRECURRENCE			= "RECURRENCE";
static LPCTSTR TDL_TASKCOMMENTSTYPE			= "COMMENTSTYPE";
static LPCTSTR TDL_TASKICONINDEX			= "ICONINDEX";
	
static LPCTSTR TDL_TASKRECURRENCEREG		= "REGULARITY";
static LPCTSTR TDL_TASKRECURRENCESPEC1		= "REGPECIFIC1";
static LPCTSTR TDL_TASKRECURRENCESPEC2		= "REGPECIFIC2";
static LPCTSTR TDL_TASKRECURRENCEFROMDUE	= "REGFROMDUE";
static LPCTSTR TDL_TASKRECURRENCEREUSE		= "RECURREUSE";


#endif // AFX_TDLSCHEMADEF_5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_
