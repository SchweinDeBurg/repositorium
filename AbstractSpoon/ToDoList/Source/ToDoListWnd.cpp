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
// - merged with ToDoList version 6.1.2-6.2.4 sources
//*****************************************************************************

// ToDoListWnd.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "ToDoListWnd.h"
#include "ToolsCmdlineParser.h"
#include "ToolsUserInputDlg.h"
#include "ToolsHelper.h"
#include "TDLExportDlg.h"
#include "TasklistHtmlExporter.h"
#include "TaskListTxtExporter.h"
#include "TDCMsg.h"
#include "TDLPrintDialog.h"
#include "TDLTransformDialog.h"
#include "TDStringRes.h"
#include "TDLColumnSelectionDlg.h"
#include "TDLFilterDlg.h"
#include "OffsetDatesDlg.h"
#include "KeyboardShortcutDisplayDlg.h"
#include "TDLImportDialog.h"
#include "TDLSetReminderDlg.h"
#include "TDLShowReminderDlg.h"
#include "TDLMultiSortDlg.h"
#include "TDLAddLoggedTimeDlg.h"
#include "MultiTaskFile.h"
#include "TDCStatic.h"

#include "../../Common/AboutDlg.h"
#include "../../../CodeProject/Source/HoldRedraw.h"
#include "../../../CodeProject/Source/AutoFlag.h"
#include "../../../CodeProject/Source/EnBitmap.h"
#include "../../Common/SpellCheckDlg.h"
#include "../../Common/EnColorDialog.h"
#include "../../../CodeProject/Source/WinClasses.h"
#include "../../../CodeProject/Source/WClassDefines.h"
#include "../../../CodeProject/Source/DateHelper.h"
#include "../../../CodeProject/Source/OSVersion.h"
#include "../../../CodeProject/Source/Misc.h"
#include "../../../CodeProject/Source/GraphicsMisc.h"
#include "../../../CodeProject/Source/FileMisc.h"
#include "../../../CodeProject/Source/Themed.h"
#include "../../../CodeProject/Source/EnString.h"
#include "../../../CodeProject/Source/FileRegister.h"
#include "../../../CodeProject/Source/MouseWheelMgr.h"
#include "../../../CodeProject/Source/EditShortcutMgr.h"
#include "../../../CodeProject/Source/DlgUnits.h"
#include "../../Common/PasswordDialog.h"
#include "../../../CodeProject/Source/SysImageList.h"
#include "../../../CodeProject/Source/AfxRegKey.h"
#include "../../Common/UIExtensionUIHelper.h"
#include "../../../CodeProject/Source/LightBox.h"
#include "../../Common/RemoteFile.h"
#include "../../Common/ServerDlg.h"
#include "../../../CodeProject/Source/FocusWatcher.h"

#include "../../../CodeProject/Source/GUI.h"
#include "../../../CodeProject/Source/SendFileTo.h"

#include <shlwapi.h>
#include <windowsx.h>
#include <direct.h>
#include <math.h>

#pragma warning(disable:4706)
#define COMPILE_MULTIMON_STUBS
#include <multimon.h>
#pragma warning(default:4706)

#include <afxpriv.h>        // for WM_KICKIDLE

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToDoListWnd dialog

// popup menus
enum
{
	TRAYICON,
	TASKCONTEXT,
	TABCTRLCONTEXT,
	HEADERCONTEXT
};
enum
{
	TB_TOOLBARHIDDEN,
	TB_DUMMY,
	TB_TOOLBARANDMENU
};
enum
{
	FILEALL,
	NEWTASK,
	EDITTASK,
	VIEW,
	MOVE,
	SORTTASK,
	DELETETASK,
	TOOLS,
	HELP
};

const int TD_VERSION = 31000;
const int BEVEL = 2; // pixels
const int BORDER = 3; // pixels
const int TB_VOFFSET = 4;

enum
{
	WM_POSTONCREATE = (WM_APP + 1),
	WM_WEBUPDATEWIZARD,
	WM_ADDTOOLBARTOOLS,
	WM_APPRESTOREFOCUS
};

enum
{
	TIMER_READONLYSTATUS = 1,
	TIMER_TIMESTAMPCHANGE,
	TIMER_AUTOSAVE,
	TIMER_CHECKOUTSTATUS,
	TIMER_DUEITEMS,
	TIMER_TIMETRACKING
};

enum
{
	INTERVAL_READONLYSTATUS = 1000,
	INTERVAL_TIMESTAMPCHANGE = 10000,
	INTERVAL_CHECKOUTSTATUS = 5000,
	INTERVAL_DUEITEMS = 60000,
	INTERVAL_TIMETRACKING = 5000
};

/////////////////////////////////////////////////////////////////////////////

CToDoListWnd::CToDoListWnd():
CFrameWnd(),
m_bVisible(-1),
m_mruList(0, _T("MRU"), _T("TaskList%d"), 16, AFX_ABBREV_FILENAME_LEN, CEnString(IDS_RECENTFILES)),
m_nLastSelItem(-1),
m_nMaxState(TDCMS_NORMAL),
m_nPrevMaxState(TDCMS_NORMAL),
m_bShowFilterBar(TRUE),
m_bShowStatusBar(TRUE),
m_bInNewTask(FALSE),
m_bSaving(FALSE),
m_mgrShortcuts(FALSE),
m_pPrefs(NULL),
m_bClosing(FALSE),
m_mgrToDoCtrls(m_tabCtrl),
m_bFindShowing(FALSE),
m_bShowProjectName(TRUE),
m_bQueryOpenAllow(FALSE),
m_bPasswordPrompting(TRUE),
m_bShowToolbar(TRUE),
m_bReloading(FALSE),
m_hIcon(NULL),
m_hwndLastFocus(NULL),
m_bStartHidden(FALSE),
m_cbQuickFind(ACBS_ALLOWDELETE | ACBS_ADDTOSTART),
m_bShowTasklistBar(TRUE),
m_bShowTreeListBar(TRUE),
m_dlgNotifyDue(TRUE, _T("DueTasksDialog"))
{
	// init content manager BEFORE userPrefs
	VERIFY(m_mgrContent.Initialize());

	// init preferences
	ResetPrefs();

	SetupUIStrings();

	CToDoCtrl::EnableExtendedSelection(FALSE, TRUE);

	m_bAutoMenuEnable = FALSE;
}

CToDoListWnd::~CToDoListWnd()
{
	CToDoCtrl::ReleaseClipboard();

	delete m_pPrefs;

	if (m_hIcon)
	{
		DestroyIcon(m_hIcon);
	}

	// cleanup temp files
	::DeleteFile(FileMisc::GetTempFileName(_T("ToDoList.print"), _T("html")));
	::DeleteFile(FileMisc::GetTempFileName(_T("ToDoList.due"), _T("html")));
	::DeleteFile(FileMisc::GetTempFileName(_T("ToDoList.due"), _T("txt")));
	::DeleteFile(FileMisc::GetTempFileName(_T("ToDoList.import"), _T("txt")));
}

BEGIN_MESSAGE_MAP(CToDoListWnd, CFrameWnd)
	//{{AFX_MSG_MAP(CToDoListWnd)
	ON_COMMAND(ID_VIEW_EXPANDTASK, OnViewExpandtask)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EXPANDTASK, OnUpdateViewExpandtask)
	ON_COMMAND(ID_VIEW_COLLAPSETASK, OnViewCollapsetask)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COLLAPSETASK, OnUpdateViewCollapsetask)
	ON_COMMAND(ID_VIEW_COLLAPSEALL, OnViewCollapseall)
	ON_COMMAND(ID_VIEW_EXPANDALL, OnViewExpandall)
	ON_UPDATE_COMMAND_UI(ID_VIEW_EXPANDALL, OnUpdateViewExpandall)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COLLAPSEALL, OnUpdateViewCollapseall)
	ON_COMMAND(ID_VIEW_TOGGLETASKEXPANDED, OnViewToggletaskexpanded)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOGGLETASKEXPANDED, OnUpdateViewToggletaskexpanded)
	ON_UPDATE_COMMAND_UI(ID_WINDOW1, OnUpdateWindow)
	ON_WM_ENABLE()
	ON_COMMAND(ID_NEWTASK, OnNewtask)
	ON_COMMAND(ID_NEWSUBTASK, OnNewsubtask)
	ON_UPDATE_COMMAND_UI(ID_NEWSUBTASK, OnUpdateNewsubtask)
	ON_COMMAND(ID_TOOLS_TRANSFORM, OnToolsTransformactivetasklist)
	ON_COMMAND(ID_VIEW_SORTTASKLISTTABS, OnViewSorttasklisttabs)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SORTTASKLISTTABS, OnUpdateViewSorttasklisttabs)
	ON_WM_MEASUREITEM()
	ON_COMMAND(ID_EDIT_INCTASKPERCENTDONE, OnEditInctaskpercentdone)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INCTASKPERCENTDONE, OnUpdateEditInctaskpercentdone)
	ON_COMMAND(ID_EDIT_DECTASKPERCENTDONE, OnEditDectaskpercentdone)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DECTASKPERCENTDONE, OnUpdateEditDectaskpercentdone)
	ON_COMMAND(ID_EDIT_DECTASKPRIORITY, OnEditDectaskpriority)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DECTASKPRIORITY, OnUpdateEditDectaskpriority)
	ON_COMMAND(ID_EDIT_INCTASKPRIORITY, OnEditInctaskpriority)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INCTASKPRIORITY, OnUpdateEditInctaskpriority)
	ON_COMMAND(ID_EDIT_FLAGTASK, OnEditFlagtask)
	ON_UPDATE_COMMAND_UI(ID_EDIT_FLAGTASK, OnUpdateEditFlagtask)
	ON_COMMAND(ID_FILE_OPENARCHIVE, OnFileOpenarchive)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPENARCHIVE, OnUpdateFileOpenarchive)
	ON_WM_WINDOWPOSCHANGED()
	ON_COMMAND(ID_VIEW_SHOWFILTERBAR, OnViewShowfilterbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOWFILTERBAR, OnUpdateViewShowfilterbar)
	ON_COMMAND(ID_VIEW_CLEARFILTER, OnViewClearfilter)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CLEARFILTER, OnUpdateViewClearfilter)
	ON_COMMAND(ID_VIEW_TOGGLEFILTER, OnViewTogglefilter)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOGGLEFILTER, OnUpdateViewTogglefilter)
	ON_COMMAND(ID_VIEW_FILTER, OnViewFilter)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FILTER, OnUpdateViewFilter)
	ON_COMMAND(ID_TABCTRL_PREFERENCES, OnTabctrlPreferences)
	ON_COMMAND(ID_TASKLIST_SELECTCOLUMNS, OnTasklistSelectColumns)
	ON_COMMAND(ID_VIEW_PROJECTNAME, OnViewProjectname)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PROJECTNAME, OnUpdateViewProjectname)
	ON_COMMAND(ID_EDIT_OFFSETDATES, OnEditOffsetdates)
	ON_UPDATE_COMMAND_UI(ID_EDIT_OFFSETDATES, OnUpdateEditOffsetdates)
	ON_COMMAND(ID_PRINTPREVIEW, OnPrintpreview)
	ON_COMMAND(ID_SHOWTIMELOGFILE, OnShowTimelogfile)
	ON_UPDATE_COMMAND_UI(ID_SHOWTIMELOGFILE, OnUpdateShowTimelogfile)
	ON_COMMAND(ID_VIEW_STATUS_BAR, OnViewStatusBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STATUS_BAR, OnUpdateViewStatusBar)
	ON_WM_QUERYOPEN()
	ON_COMMAND(ID_EDIT_COPYAS_REF, OnCopyTaskasRef)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPYAS_REF, OnUpdateCopyTaskasRef)
	ON_COMMAND(ID_EDIT_COPYAS_DEPEND, OnCopyTaskasDependency)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPYAS_DEPEND, OnUpdateCopyTaskasDependency)
	ON_COMMAND(ID_EDIT_COPYAS_REFFULL, OnCopyTaskasRefFull)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPYAS_REFFULL, OnUpdateCopyTaskasRefFull)
	ON_COMMAND(ID_EDIT_COPYAS_DEPENDFULL, OnCopyTaskasDependencyFull)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPYAS_DEPENDFULL, OnUpdateCopyTaskasDependencyFull)
	ON_COMMAND(ID_EDIT_COPYAS_PATH, OnCopyTaskasPath)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPYAS_PATH, OnUpdateCopyTaskasPath)
	ON_WM_WINDOWPOSCHANGING()
	ON_COMMAND(ID_TOOLS_CHECKFORUPDATES, OnToolsCheckforupdates)
	ON_COMMAND(ID_HELP_KEYBOARDSHORTCUTS, OnShowKeyboardshortcuts)
	ON_COMMAND(ID_EDIT_INSERTDATETIME, OnEditInsertdatetime)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSERTDATETIME, OnUpdateEditInsertdatetime)
	ON_WM_SYSCOLORCHANGE()
	ON_COMMAND(ID_EDIT_CLEARTASKCOLOR, OnEditCleartaskcolor)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEARTASKCOLOR, OnUpdateEditCleartaskcolor)
	ON_COMMAND(ID_EDIT_SELECTALL, OnEditSelectall)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECTALL, OnUpdateEditSelectall)
	ON_COMMAND(ID_CLOSEALLBUTTHIS, OnCloseallbutthis)
	ON_UPDATE_COMMAND_UI(ID_CLOSEALLBUTTHIS, OnUpdateCloseallbutthis)
	ON_COMMAND(ID_SENDTASKS, OnSendtasks)
	ON_COMMAND(ID_EDIT_INSERTDATE, OnEditInsertdate)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSERTDATE, OnUpdateEditInsertdate)
	ON_COMMAND(ID_EDIT_INSERTTIME, OnEditInserttime)
	ON_UPDATE_COMMAND_UI(ID_EDIT_INSERTTIME, OnUpdateEditInserttime)
	ON_COMMAND(ID_NEXTTASK, OnGotoNexttask)
	ON_COMMAND(ID_PREVTASK, OnGotoPrevtask)
	ON_UPDATE_COMMAND_UI(ID_PREVTASK, OnUpdateGotoPrevtask)
	ON_UPDATE_COMMAND_UI(ID_NEXTTASK, OnUpdateGotoNexttask)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, OnUpdateEditRedo)
	ON_COMMAND(ID_VIEW_TOGGLETREEANDLIST, OnViewToggleTreeandList)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOGGLETREEANDLIST, OnUpdateViewToggleTreeandList)
	ON_COMMAND(ID_VIEW_TOGGLETASKSANDCOMMENTS, OnViewToggletasksandcomments)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOGGLETASKSANDCOMMENTS, OnUpdateViewToggletasksandcomments)
	ON_COMMAND(ID_EDIT_QUICKFIND, OnQuickFind)
	ON_COMMAND(ID_EDIT_QUICKFINDNEXT, OnQuickFindNext)
	ON_UPDATE_COMMAND_UI(ID_EDIT_QUICKFINDNEXT, OnUpdateQuickFindNext)
	ON_COMMAND(ID_EDIT_QUICKFINDPREV, OnQuickFindPrev)
	ON_UPDATE_COMMAND_UI(ID_EDIT_QUICKFINDPREV, OnUpdateQuickFindPrev)
	ON_COMMAND(ID_EDIT_SETTASKICON, OnEditSettaskicon)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SETTASKICON, OnUpdateEditSettaskicon)
	ON_COMMAND(ID_EDIT_SETREMINDER, OnEditSetReminder)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SETREMINDER, OnUpdateEditSetReminder)
	ON_COMMAND(ID_EDIT_CLEARREMINDER, OnEditClearReminder)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEARREMINDER, OnUpdateEditClearReminder)
	ON_COMMAND(ID_EDIT_CLEARTASKICON, OnEditCleartaskicon)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEARTASKICON, OnUpdateEditCleartaskicon)
	ON_COMMAND(ID_SORT_MULTI, OnSortMulti)
	ON_UPDATE_COMMAND_UI(ID_SORT_MULTI, OnUpdateSortMulti)
	ON_COMMAND(ID_ARCHIVE_SELECTEDTASKS, OnArchiveSelectedTasks)
	ON_UPDATE_COMMAND_UI(ID_ARCHIVE_SELECTEDTASKS, OnUpdateArchiveSelectedCompletedTasks)
	ON_COMMAND(ID_ADDTIMETOLOGFILE, OnAddtimetologfile)
	ON_UPDATE_COMMAND_UI(ID_ADDTIMETOLOGFILE, OnUpdateAddtimetologfile)
	ON_WM_ACTIVATEAPP()
	ON_WM_SYSCOMMAND()
	ON_WM_INITMENUPOPUP()
	ON_WM_MOVE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_ADDTIMETOLOGFILE, OnAddtimetologfile)
	ON_UPDATE_COMMAND_UI(ID_ADDTIMETOLOGFILE, OnUpdateAddtimetologfile)
	ON_COMMAND(ID_VIEW_SHOWTASKLISTTABBAR, OnViewShowTasklistTabbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOWTASKLISTTABBAR, OnUpdateViewShowTasklistTabbar)
	ON_COMMAND(ID_VIEW_SHOWTREELISTTABBAR, OnViewShowTreeListTabbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SHOWTREELISTTABBAR, OnUpdateViewShowTreeListTabbar)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_TOOLS_REMOVEFROMSOURCECONTROL, OnToolsRemovefromsourcecontrol)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_REMOVEFROMSOURCECONTROL, OnUpdateToolsRemovefromsourcecontrol)
	ON_COMMAND(ID_LOAD_FROMWEB, OnLoadFromWeb)
	ON_COMMAND(ID_SAVE_TOWEB, OnSaveToWeb)
	ON_UPDATE_COMMAND_UI(ID_SAVE_TOWEB, OnUpdateSaveToWeb)
	ON_COMMAND(ID_VIEW_REFRESHFILTER, OnViewRefreshfilter)
	ON_UPDATE_COMMAND_UI(ID_VIEW_REFRESHFILTER, OnUpdateViewRefreshfilter)
	ON_WM_CREATE()
	ON_WM_HELPINFO()
	ON_COMMAND_RANGE(ID_WINDOW1, ID_WINDOW16, OnWindow)
	ON_COMMAND(ID_VIEW_NEXT, OnViewNext)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NEXT, OnUpdateViewNext)
	ON_COMMAND(ID_VIEW_PREV, OnViewPrev)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PREV, OnUpdateViewPrev)
	ON_WM_SYSCOMMAND()
	ON_UPDATE_COMMAND_UI(ID_TOOLS_IMPORT, OnUpdateImport)
	ON_UPDATE_COMMAND_UI(ID_NEWTASK, OnUpdateNewtask)
	ON_COMMAND(ID_TOOLS_CHECKOUT, OnToolsCheckout)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_CHECKOUT, OnUpdateToolsCheckout)
	ON_COMMAND(ID_TOOLS_CHECKIN, OnToolsCheckin)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_CHECKIN, OnUpdateToolsCheckin)
	ON_COMMAND(ID_TOOLS_TOGGLECHECKIN, OnToolsToggleCheckin)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_TOGGLECHECKIN, OnUpdateToolsToggleCheckin)
	ON_COMMAND(ID_TOOLS_EXPORT, OnExport)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_EXPORT, OnUpdateExport)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_TRANSFORM, OnUpdateExport) // use same text as export
	ON_COMMAND(ID_NEXTTOPLEVELTASK, OnNexttopleveltask)
	ON_COMMAND(ID_PREVTOPLEVELTASK, OnPrevtopleveltask)
	ON_UPDATE_COMMAND_UI(ID_NEXTTOPLEVELTASK, OnUpdateNexttopleveltask)
	ON_UPDATE_COMMAND_UI(ID_PREVTOPLEVELTASK, OnUpdatePrevtopleveltask)
	ON_COMMAND(ID_EDIT_FINDTASKS, OnFindTasks)
	ON_COMMAND(ID_VIEW_MOVETASKLISTRIGHT, OnViewMovetasklistright)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MOVETASKLISTRIGHT, OnUpdateViewMovetasklistright)
	ON_COMMAND(ID_VIEW_MOVETASKLISTLEFT, OnViewMovetasklistleft)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MOVETASKLISTLEFT, OnUpdateViewMovetasklistleft)
	ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateViewToolbar)
	ON_COMMAND(ID_SORT, OnSort)
	ON_UPDATE_COMMAND_UI(ID_SORT, OnUpdateSort)
	ON_COMMAND(ID_NEWTASK_ATTOP, OnNewtaskAttop)
	ON_UPDATE_COMMAND_UI(ID_NEWTASK_ATTOP, OnUpdateNewtaskAttop)
	ON_COMMAND(ID_NEWTASK_ATBOTTOM, OnNewtaskAtbottom)
	ON_UPDATE_COMMAND_UI(ID_NEWTASK_ATBOTTOM, OnUpdateNewtaskAtbottom)
	ON_COMMAND(ID_EDIT_SPELLCHECKCOMMENTS, OnSpellcheckcomments)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SPELLCHECKCOMMENTS, OnUpdateSpellcheckcomments)
	ON_COMMAND(ID_EDIT_SPELLCHECKTITLE, OnSpellchecktitle)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SPELLCHECKTITLE, OnUpdateSpellchecktitle)
	ON_COMMAND(ID_FILE_ENCRYPT, OnFileEncrypt)
	ON_UPDATE_COMMAND_UI(ID_FILE_ENCRYPT, OnUpdateFileEncrypt)
	ON_COMMAND(ID_FILE_RESETVERSION, OnFileResetversion)
	ON_UPDATE_COMMAND_UI(ID_FILE_RESETVERSION, OnUpdateFileResetversion)
	ON_COMMAND(ID_TOOLS_SPELLCHECKTASKLIST, OnSpellcheckTasklist)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SPELLCHECKTASKLIST, OnUpdateSpellcheckTasklist)
	ON_COMMAND(ID_EDIT_PASTEAFTER, OnEditPasteAfter)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTEAFTER, OnUpdateEditPasteAfter)
	ON_COMMAND(ID_EDIT_CLOCK_TASK, OnEditTimeTrackTask)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLOCK_TASK, OnUpdateEditTimeTrackTask)
	ON_WM_DRAWITEM()
	ON_COMMAND(ID_VIEW_NEXT_SEL, OnViewNextSel)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NEXT_SEL, OnUpdateViewNextSel)
	ON_COMMAND(ID_VIEW_PREV_SEL, OnViewPrevSel)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PREV_SEL, OnUpdateViewPrevSel)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_COMMAND_RANGE(ID_NEWTASK_SPLITTASKINTO_TWO, ID_NEWTASK_SPLITTASKINTO_FIVE, OnSplitTaskIntoPieces)
	ON_UPDATE_COMMAND_UI_RANGE(ID_NEWTASK_SPLITTASKINTO_TWO, ID_NEWTASK_SPLITTASKINTO_FIVE, OnUpdateSplitTaskIntoPieces)
	ON_COMMAND_RANGE(ID_TOOLS_SHOWTASKS_DUETODAY, ID_TOOLS_SHOWTASKS_DUEENDNEXTMONTH, OnToolsShowtasksDue)
	ON_COMMAND(ID_DELETETASK, OnDeleteTask)
	ON_COMMAND(ID_DELETEALLTASKS, OnDeleteAllTasks)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_COMMAND(ID_SAVE_NORMAL, OnSave)
	ON_COMMAND(ID_LOAD_NORMAL, OnLoad)
	ON_COMMAND(ID_NEW, OnNew)
	ON_UPDATE_COMMAND_UI(ID_DELETETASK, OnUpdateDeletetask)
	ON_UPDATE_COMMAND_UI(ID_EDIT_TASKCOLOR, OnUpdateTaskcolor)
	ON_UPDATE_COMMAND_UI(ID_EDIT_TASKDONE, OnUpdateTaskdone)
	ON_UPDATE_COMMAND_UI(ID_DELETEALLTASKS, OnUpdateDeletealltasks)
	ON_UPDATE_COMMAND_UI(ID_SAVE_NORMAL, OnUpdateSave)
	ON_UPDATE_COMMAND_UI(ID_NEW, OnUpdateNew)
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_NEWTASK_ATTOPSELECTED, OnNewtaskAttopSelected)
	ON_COMMAND(ID_NEWTASK_ATBOTTOMSELECTED, OnNewtaskAtbottomSelected)
	ON_COMMAND(ID_NEWTASK_AFTERSELECTEDTASK, OnNewtaskAfterselectedtask)
	ON_COMMAND(ID_NEWTASK_BEFORESELECTEDTASK, OnNewtaskBeforeselectedtask)
	ON_COMMAND(ID_NEWSUBTASK_ATBOTTOM, OnNewsubtaskAtbottom)
	ON_COMMAND(ID_NEWSUBTASK_ATTOP, OnNewsubtaskAttop)
	ON_COMMAND(ID_EDIT_TASKCOLOR, OnEditTaskcolor)
	ON_COMMAND(ID_EDIT_TASKDONE, OnEditTaskdone)
	ON_COMMAND(ID_EDIT_TASKTEXT, OnEditTasktext)
	ON_COMMAND(ID_MOVETASKDOWN, OnMovetaskdown)
	ON_UPDATE_COMMAND_UI(ID_MOVETASKDOWN, OnUpdateMovetaskdown)
	ON_COMMAND(ID_MOVETASKUP, OnMovetaskup)
	ON_UPDATE_COMMAND_UI(ID_MOVETASKUP, OnUpdateMovetaskup)
	ON_WM_CLOSE()
	ON_COMMAND(ID_TRAYICON_CLOSE, OnTrayiconClose)
	ON_UPDATE_COMMAND_UI(ID_NEWSUBTASK_ATBOTTOM, OnUpdateNewsubtaskAtBottom)
	ON_COMMAND(ID_SAVEAS, OnSaveas)
	ON_UPDATE_COMMAND_UI(ID_SAVEAS, OnUpdateSaveas)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_TRAYICON_SHOW, OnTrayiconShow)
	ON_COMMAND_RANGE(ID_TRAYICON_SHOWDUETASKS1, ID_TRAYICON_SHOWDUETASKS20, OnTrayiconShowDueTasks)
	ON_WM_QUERYENDSESSION()
	ON_UPDATE_COMMAND_UI(ID_FILE_MRU_FILE1, OnUpdateRecentFileMenu)
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_COMMAND(ID_PREFERENCES, OnPreferences)
	ON_WM_COPYDATA()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTESUB, OnEditPasteSub)
	ON_COMMAND(ID_EDIT_COPYAS_TEXT, OnEditCopyastext)
	ON_COMMAND(ID_EDIT_COPYAS_HTML, OnEditCopyashtml)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTESUB, OnUpdateEditPasteSub)
	ON_UPDATE_COMMAND_UI(ID_NEWTASK_ATTOPSELECTED, OnUpdateNewtaskAttopSelected)
	ON_UPDATE_COMMAND_UI(ID_NEWTASK_ATBOTTOMSELECTED, OnUpdateNewtaskAtbottomSelected)
	ON_UPDATE_COMMAND_UI(ID_NEWTASK_AFTERSELECTEDTASK, OnUpdateNewtaskAfterselectedtask)
	ON_UPDATE_COMMAND_UI(ID_NEWTASK_BEFORESELECTEDTASK, OnUpdateNewtaskBeforeselectedtask)
	ON_UPDATE_COMMAND_UI(ID_NEWSUBTASK_ATTOP, OnUpdateNewsubtaskAttop)
	ON_COMMAND(ID_MAXTASKLIST, OnMaximizeTasklist)
	ON_UPDATE_COMMAND_UI(ID_MAXTASKLIST, OnUpdateMaximizeTasklist)
	ON_COMMAND(ID_MAXCOMMENTS, OnMaximizeComments)
	ON_UPDATE_COMMAND_UI(ID_MAXCOMMENTS, OnUpdateMaximizeComments)
	ON_COMMAND(ID_OPEN_RELOAD, OnReload)
	ON_UPDATE_COMMAND_UI(ID_OPEN_RELOAD, OnUpdateReload)
	ON_COMMAND(ID_ARCHIVE_COMPLETEDTASKS, OnArchiveCompletedtasks)
	ON_UPDATE_COMMAND_UI(ID_ARCHIVE_COMPLETEDTASKS, OnUpdateArchiveCompletedtasks)
	ON_COMMAND(ID_PRINT, OnPrint)
	ON_UPDATE_COMMAND_UI(ID_PRINT, OnUpdatePrint)
	ON_COMMAND(ID_MOVETASKRIGHT, OnMovetaskright)
	ON_UPDATE_COMMAND_UI(ID_MOVETASKRIGHT, OnUpdateMovetaskright)
	ON_COMMAND(ID_MOVETASKLEFT, OnMovetaskleft)
	ON_UPDATE_COMMAND_UI(ID_MOVETASKLEFT, OnUpdateMovetaskleft)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TABCONTROL, OnSelchangeTabcontrol)
	ON_NOTIFY(TCN_SELCHANGING, IDC_TABCONTROL, OnSelchangingTabcontrol)
	ON_NOTIFY(NM_MCLICK, IDC_TABCONTROL, OnMBtnClickTabcontrol)
	ON_COMMAND(ID_CLOSE, OnCloseTasklist)
	ON_COMMAND(ID_SAVEALL, OnSaveall)
	ON_UPDATE_COMMAND_UI(ID_SAVEALL, OnUpdateSaveall)
	ON_COMMAND(ID_CLOSEALL, OnCloseall)
	ON_UPDATE_COMMAND_UI(ID_CLOSEALL, OnUpdateCloseall)
	ON_COMMAND(ID_EXIT, OnExit)
	ON_COMMAND(ID_MINIMIZETOTRAY, OnMinimizeToTray)
	ON_UPDATE_COMMAND_UI(ID_MOVETASK, OnUpdateMovetask)
	ON_WM_TIMER()
	ON_COMMAND(ID_TOOLS_IMPORT, OnImportTasklist)
	ON_COMMAND_RANGE(ID_SORT_BYCOST, ID_SORT_NONE, OnSortBy)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SORT_BYCOST, ID_SORT_NONE, OnUpdateSortBy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_TASKTEXT, OnUpdateEditTasktext)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_COMMAND(ID_TRAYICON_CREATETASK, OnTrayiconCreatetask)
	ON_COMMAND_RANGE(ID_EDIT_SETPRIORITYNONE, ID_EDIT_SETPRIORITY10, OnSetPriority)
	ON_UPDATE_COMMAND_UI_RANGE(ID_EDIT_SETPRIORITYNONE, ID_EDIT_SETPRIORITY10, OnUpdateSetPriority)
	ON_COMMAND(ID_EDIT_SETFILEREF, OnEditSetfileref)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SETFILEREF, OnUpdateEditSetfileref)
	ON_COMMAND(ID_EDIT_OPENFILEREF, OnEditOpenfileref)
	ON_UPDATE_COMMAND_UI(ID_EDIT_OPENFILEREF, OnUpdateEditOpenfileref)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPYAS_HTML, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPYAS_TEXT, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI_RANGE(ID_TOOLS_USEREXT1, ID_TOOLS_USEREXT16, OnUpdateUserUIExtension)
	ON_COMMAND_RANGE(ID_TOOLS_USEREXT1, ID_TOOLS_USEREXT16, OnUserUIExtension)
	ON_UPDATE_COMMAND_UI_RANGE(ID_TOOLS_USERTOOL1, ID_TOOLS_USERTOOL16, OnUpdateUserTool)
	ON_COMMAND_RANGE(ID_TOOLS_USERTOOL1, ID_TOOLS_USERTOOL16, OnUserTool)
	ON_WM_INITMENUPOPUP()
	ON_NOTIFY(NM_CLICK, IDC_TRAYICON, OnTrayIconClick)
	ON_NOTIFY(NM_DBLCLK, IDC_TRAYICON, OnTrayIconDblClk)
	ON_NOTIFY(NM_RCLICK, IDC_TRAYICON, OnTrayIconRClick)
	ON_NOTIFY(TTN_NEEDTEXT, 0, OnNeedTooltipText)
	ON_UPDATE_COMMAND_UI(ID_SB_SELCOUNT, OnUpdateSBSelectionCount)
	ON_UPDATE_COMMAND_UI(ID_SB_TASKCOUNT, OnUpdateSBTaskCount)
	ON_REGISTERED_MESSAGE(WM_FW_FOCUSCHANGE, OnFocusChange)
	ON_REGISTERED_MESSAGE(WM_FBN_FILTERCHNG, OnSelchangeFilter)
	ON_REGISTERED_MESSAGE(WM_TDCN_LISTCHANGE, OnToDoCtrlNotifyListChange)
	ON_REGISTERED_MESSAGE(WM_TDCN_MODIFY, OnToDoCtrlNotifyMod)
	ON_REGISTERED_MESSAGE(WM_TDCN_MINWIDTHCHANGE, OnToDoCtrlNotifyMinWidthChange)
	ON_REGISTERED_MESSAGE(WM_TDCN_TIMETRACK, OnToDoCtrlNotifyTimeTrack)
	ON_REGISTERED_MESSAGE(WM_TDCN_VIEWCHANGE, OnToDoCtrlNotifyViewChange)
	ON_REGISTERED_MESSAGE(WM_TDCN_RECREATERECURRINGTASK, OnToDoCtrlNotifyRecreateRecurringTask)
	ON_REGISTERED_MESSAGE(WM_TDCM_TASKLINK, OnToDoCtrlDoTaskLink)
	ON_REGISTERED_MESSAGE(WM_TDCM_TASKISDONE, OnToDoCtrlTaskIsDone)
	ON_REGISTERED_MESSAGE(WM_TDCM_TASKHASREMINDER, OnToDoCtrlTaskHasReminder)
	ON_REGISTERED_MESSAGE(WM_TDL_SHOWWINDOW , OnToDoListShowWindow)
	ON_REGISTERED_MESSAGE(WM_TDL_GETVERSION , OnToDoListGetVersion)
	ON_REGISTERED_MESSAGE(WM_TDL_ISCLOSING , OnToDoListIsClosing)
	ON_REGISTERED_MESSAGE(WM_TDL_REFRESHPREFS , OnToDoListRefreshPrefs)
	ON_REGISTERED_MESSAGE(WM_TDL_RESTORE , OnToDoListRestore)
	ON_COMMAND_EX_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnOpenRecentFile)
	ON_REGISTERED_MESSAGE(WM_FTD_FIND, OnFindDlgFind)
	ON_REGISTERED_MESSAGE(WM_FTD_SELECTRESULT, OnFindSelectResult)
	ON_REGISTERED_MESSAGE(WM_FTD_SELECTALL, OnFindSelectAll)
	ON_REGISTERED_MESSAGE(WM_FTD_CLOSE, OnFindDlgClose)
	ON_REGISTERED_MESSAGE(WM_FTD_APPLYASFILTER, OnFindApplyAsFilter)
	ON_REGISTERED_MESSAGE(WM_TLDT_DROPFILE, OnDropFile)
	ON_REGISTERED_MESSAGE(WM_PTP_TESTTOOL, OnPreferencesTestTool)
	ON_REGISTERED_MESSAGE(WM_PGP_CLEARMRU, OnPreferencesClearMRU)
	ON_REGISTERED_MESSAGE(WM_PTDP_LISTCHANGE, OnPreferencesDefaultListChange)
	ON_MESSAGE(WM_GETICON, OnGetIcon)
	ON_MESSAGE(WM_POSTONCREATE, OnPostOnCreate)
	ON_MESSAGE(WM_WEBUPDATEWIZARD, OnWebUpdateWizard)
	ON_MESSAGE(WM_ADDTOOLBARTOOLS, OnAddToolbarTools)
	ON_MESSAGE(WM_HOTKEY, OnHotkey)
	ON_MESSAGE(WM_POWERBROADCAST, OnPowerBroadcast)
	ON_MESSAGE(WM_GETFONT, OnGetFont)
	ON_CBN_EDITCHANGE(IDC_QUICKFIND, OnEditChangeQuickFind)
	ON_CBN_SELCHANGE(IDC_QUICKFIND, OnSelChangeQuickFind)
	ON_REGISTERED_MESSAGE(WM_ACB_ITEMADDED, OnQuickFindItemAdded)
	ON_REGISTERED_MESSAGE(WM_TD_REMINDER, OnToDoCtrlReminder)
	ON_MESSAGE(WM_APPRESTOREFOCUS, OnAppRestoreFocus)
	ON_REGISTERED_MESSAGE(WM_TDCN_DOUBLECLKREMINDERCOL, OnDoubleClkReminderCol)
#ifdef _DEBUG
	ON_COMMAND(ID_DEBUGQUERYENDSESSION, OnDebugQueryEndSession)
#endif
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToDoListWnd message handlers

void CToDoListWnd::SetupUIStrings()
{
	// set up UI strings for helper classes
	CTimeEdit::SetUnits(THU_MINS, CEnString(IDS_TE_MINS), CEnString(IDS_MINS_ABBREV));
	CTimeEdit::SetUnits(THU_HOURS, CEnString(IDS_TE_HOURS), CEnString(IDS_HOURS_ABBREV));
	CTimeEdit::SetUnits(THU_DAYS, CEnString(IDS_TE_DAYS), CEnString(IDS_DAYS_ABBREV));
	CTimeEdit::SetUnits(THU_WEEKS, CEnString(IDS_TE_WEEKS), CEnString(IDS_WEEKS_ABBREV));
	CTimeEdit::SetUnits(THU_MONTHS, CEnString(IDS_TE_MONTHS), CEnString(IDS_MONTHS_ABBREV));
	CTimeEdit::SetUnits(THU_YEARS, CEnString(IDS_TE_YEARS), CEnString(IDS_YEARS_ABBREV));

	CServerDlg::SetItemText(SD_TITLE, IDS_SCD_TITLE);
	CServerDlg::SetItemText(IDC_SD_SERVERLABEL, IDS_SD_SERVERLABEL);
	CServerDlg::SetItemText(IDC_SD_USERNAMELABEL, IDS_SD_USERNAMELABEL);
	CServerDlg::SetItemText(IDC_SD_PASSWORDLABEL, IDS_SD_PASSWORDLABEL);
	CServerDlg::SetItemText(IDC_SD_ANONLOGIN, IDS_SD_ANONLOGIN);
	CServerDlg::SetItemText(IDOK, IDS_OK);
	CServerDlg::SetItemText(IDCANCEL, IDS_CANCEL);

	CXmlFileEx::SetUIStrings(IDS_ENCRYPTEDFILE, IDS_DECRYPTFAILED);

	CPasswordDialog::SetItemText(PD_TITLE, IDS_PD_TITLE);
	CPasswordDialog::SetItemText(IDC_PD_PASSWORDLABEL, IDS_PD_PASSWORDLABEL);
	CPasswordDialog::SetItemText(IDC_PD_CONFIRMLABEL, IDS_PD_CONFIRMLABEL);
	CPasswordDialog::SetItemText(DLG_PD_CONFIRMFAILED, IDS_PD_CONFIRMFAILED);
	CPasswordDialog::SetItemText(IDOK, IDS_OK);
	CPasswordDialog::SetItemText(IDCANCEL, IDS_CANCEL);

	CSpellCheckDlg::SetItemText(IDC_SCD_DICTLABEL, IDS_SCD_DICTLABEL);
	CSpellCheckDlg::SetItemText(IDC_SCD_BROWSE, IDS_SCD_BROWSE);
	CSpellCheckDlg::SetItemText(IDC_SCD_URL, IDS_SCD_URL);
	CSpellCheckDlg::SetItemText(IDC_SCD_CHECKINGLABEL, IDS_SCD_CHECKINGLABEL);
	CSpellCheckDlg::SetItemText(IDC_SCD_RESTART, IDS_SCD_RESTART);
	CSpellCheckDlg::SetItemText(IDC_SCD_REPLACELABEL, IDS_SCD_REPLACELABEL);
	CSpellCheckDlg::SetItemText(IDC_SCD_WITHLABEL, IDS_SCD_WITHLABEL);
	CSpellCheckDlg::SetItemText(IDC_SCD_REPLACE, IDS_SCD_REPLACE);
	CSpellCheckDlg::SetItemText(IDC_SCD_NEXT, IDS_SCD_NEXT);
	CSpellCheckDlg::SetItemText(IDOK, IDS_OK);
	CSpellCheckDlg::SetItemText(IDCANCEL, IDS_CANCEL);
	CSpellCheckDlg::SetItemText(SCD_TITLE, IDS_SCD_TITLE);
	CSpellCheckDlg::SetItemText(DLG_SCD_BROWSETITLE, IDS_SCD_BROWSETITLE);
	CSpellCheckDlg::SetItemText(DLG_SCD_SETUPMSG, IDS_SCD_SETUPMSG);
	CSpellCheckDlg::SetItemText(DLG_SCD_DICTFILTER, IDS_SCD_DICTFILTER);
	CSpellCheckDlg::SetItemText(DLG_SCD_ENGINEFILTER, IDS_SCD_ENGINEFILTER);
	CSpellCheckDlg::SetItemText(DLG_SCD_ENGINETITLE, IDS_SCD_ENGINETITLE);
}

BOOL CToDoListWnd::OnHelpInfo(HELPINFO* /*pHelpInfo*/)
{
	// always eat this because we handle the F1 ourselves
	return FALSE;
}

void CToDoListWnd::SetUITheme(const CString& sThemeFile)
{
	if (COSVersion() < OSV_XP)
	{
		return;
	}

	if (CThemed::IsThemeActive() && m_theme.LoadThemeFile(sThemeFile))
	{
		m_sThemeFile = sThemeFile;
	}
	else
	{
		m_sThemeFile.Empty();
		m_theme.Reset();
	}

	// update the UI
	m_toolbar.SetBackgroundColors(m_theme.crToolbarLight, m_theme.crToolbarDark, m_theme.nStyle == UIS_GRADIENT);
	m_statusBar.SetUIColors(m_theme.crStatusBarLight, m_theme.crStatusBarDark, m_theme.nStyle == UIS_GRADIENT,
		m_theme.crStatusBarText);
	m_menubar.SetBackgroundColor(m_theme.crMenuBack);
	m_filterBar.SetUIColors(m_theme.crAppBackLight, m_theme.crAppText);
	m_tabCtrl.SetBackgroundColor(m_theme.crAppBackDark);
	m_mgrUIExtensions.SetUITheme(m_theme);

	for (int nCtl = 0; nCtl < GetTDCCount(); nCtl++)
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtl);
		tdc.SetUITheme(m_theme);
	}

	if (m_findDlg.GetSafeHwnd())
	{
		m_findDlg.SetUITheme(m_theme);
	}

	DrawMenuBar();
	Invalidate();
}

BOOL CToDoListWnd::Create(const TDCSTARTUP& startup)
{
	m_startupOptions = startup;
	m_bVisible = startup.HasFlag(TLD_FORCEVISIBLE) ? 1 : -1;
	m_bPasswordPrompting = startup.HasFlag(TLD_PASSWORDPROMPTING);
	m_bLogging = startup.HasFlag(TLD_LOGGING);

	if (m_bLogging)
	{
		::DeleteFile(FileMisc::GetAppFolder() + _T("\\ToDoList.log"));
	}

	if (LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, NULL, NULL))
	{
		SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);
		return TRUE;
	}

	return FALSE;
}

int CToDoListWnd::GetVersion()
{
	return TD_VERSION;
}

int CToDoListWnd::MessageBox(UINT nIDText, UINT nIDCaption, UINT nType)
{
	return CFrameWnd::MessageBox(CEnString(nIDText), CEnString(nIDCaption), nType);
}

int CToDoListWnd::MessageBox(LPCTSTR szText, UINT nIDCaption, UINT nType)
{
	return CFrameWnd::MessageBox(szText, CEnString(nIDCaption), nType);
}

int CToDoListWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	if (!CMouseWheelMgr::Initialize())
	{
		return -1;
	}

	if (!CEditShortcutMgr::Initialize())
	{
		return -1;
	}

	// menu
	if (!LoadMenubar())
	{
		return -1;
	}

	// import export manager
	m_mgrImportExport.Initialize();

	// UI extension mgr
	m_mgrUIExtensions.Initialize();

	// reminders
	m_reminders.Initialize(this);

	// drop target
	if (!m_dropTarget.Register(this, this))
	{
		return -1;
	}

	// shortcut manager
	InitShortcutManager();

	// trayicon
	// we always create the trayicon (for simplicity) but we only
	// show it if required
	BOOL bUseSysTray = Prefs().GetUseSysTray();
	UINT nIDTrayIcon = (COSVersion() >= OSV_XP) ? IDI_TRAYICONXP : IDI_TRAYICON;

	m_ti.Create(WS_CHILD | (bUseSysTray ? WS_VISIBLE : 0), this, IDC_TRAYICON, nIDTrayIcon, CEnString(IDS_COPYRIGHT));

	// toolbar
	if (!InitToolbar())
	{
		return -1;
	}

	// statusbar
	if (!InitStatusbar())
	{
		return -1;
	}

	// menu icons
	InitMenuIconManager();

	// filterbar
	if (!m_filterBar.Create(this, (UINT)IDC_STATIC))
	{
		return -1;
	}

	m_filterBar.EnableMultiSelection(Prefs().GetMultiSelFilters());

	// tabctrl
	if (!m_tabCtrl.Create(WS_CHILD | WS_VISIBLE | TCS_HOTTRACK | TCS_TABS | TCS_SINGLELINE | TCS_RIGHTJUSTIFY |
		TCS_TOOLTIPS, CRect(0, 0, 10, 10), this, IDC_TABCONTROL))
	{
		return -1;
	}

	m_tabCtrl.GetToolTips()->ModifyStyle(0, TTS_ALWAYSTIP);

	BOOL bStackTabbar = Prefs().GetStackTabbarItems();

	m_tabCtrl.ModifyStyle(bStackTabbar ? 0 : TCS_MULTILINE, bStackTabbar ? TCS_MULTILINE : 0);
	UpdateTabSwitchTooltip();

	if (m_ilTabCtrl.Create(16, 16, ILC_COLOR32 | ILC_MASK, 4, 1))
	{
		CBitmap bm;
		bm.LoadBitmap(IDB_SOURCECONTROL);
		m_ilTabCtrl.Add(&bm, RGB(255, 0, 255));
		m_tabCtrl.SetImageList(&m_ilTabCtrl);
	}
	else
	{
		return -1;
	}

	// UI Font
	InitUIFont();

	LoadSettings();

	// add a barebones tasklist while we're still hidden
	if (!CreateNewTaskList(FALSE))
	{
		return -1;
	}

	// timers
	SetTimer(TIMER_DUEITEMS, TRUE);
	SetTimer(TIMER_TIMETRACKING, Prefs().GetUseSysTray());

	// notify users about dodgy content plugins
	if (m_mgrContent.SomePluginsHaveBadversions())
	{
		if (MessageBox(IDS_BADPLUGINVERSIONS, IDS_BADPLUGINTITLE, MB_OKCANCEL) == IDCANCEL)
		{
			return -1;
		}
	}

	// inherited parent task attributes for new tasks
	CTDCAttributeArray aParentAttrib;
	BOOL bUpdateAttrib;

	Prefs().GetParentAttribsUsed(aParentAttrib, bUpdateAttrib);
	CToDoCtrl::SetInheritedParentAttributes(aParentAttrib, bUpdateAttrib);

	// theme
	SetUITheme(Prefs().GetUITheme());

	if (Prefs().GetEnableLightboxMgr())
	{
		CLightBoxMgr::Initialize(this, m_theme.crAppBackDark);
	}

	// late initialization
	PostMessage(WM_POSTONCREATE);

	return 0; // success
}

void CToDoListWnd::InitUIFont()
{
	m_fontMain.DeleteObject();

	HFONT hFontUI = GraphicsMisc::CreateFont(_T("MS Shell Dlg 2"), 8);

	if (m_fontMain.Attach(hFontUI))
	{
		CDialogHelper::SetFont(this, m_fontMain);   // will update all child controls
	}
}

void CToDoListWnd::InitShortcutManager()
{
	// setup defaults first
	m_mgrShortcuts.AddShortcut(ID_LOAD_NORMAL, 'O', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_OPEN_RELOAD, 'R', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_SAVE_NORMAL, 'S', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_NEWTASK_BEFORESELECTEDTASK, 'N', HOTKEYF_CONTROL | HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_NEWTASK_AFTERSELECTEDTASK, 'N', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_NEWSUBTASK_ATBOTTOM, 'N', HOTKEYF_CONTROL | HOTKEYF_SHIFT);
	m_mgrShortcuts.AddShortcut(ID_MAXTASKLIST, 'M', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_MAXCOMMENTS, 'M', HOTKEYF_CONTROL | HOTKEYF_SHIFT);
	m_mgrShortcuts.AddShortcut(ID_PRINT, 'P', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_VIEW_NEXT, VK_TAB, HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_VIEW_PREV, VK_TAB, HOTKEYF_CONTROL | HOTKEYF_SHIFT);
	m_mgrShortcuts.AddShortcut(ID_EDIT_CUT, 'X', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_EDIT_COPY, 'C', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_EDIT_PASTEAFTER, 'V', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_EDIT_PASTESUB, 'V', HOTKEYF_CONTROL | HOTKEYF_SHIFT);
	m_mgrShortcuts.AddShortcut(ID_EDIT_INSERTDATETIME, 'D', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_EDIT_OPENFILEREF, 'G', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_EXIT, VK_F4, HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_CLOSE, VK_F4, HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_MOVETASKDOWN, VK_DOWN, HOTKEYF_CONTROL | HOTKEYF_EXT);
	m_mgrShortcuts.AddShortcut(ID_MOVETASKUP, VK_UP, HOTKEYF_CONTROL | HOTKEYF_EXT);
	m_mgrShortcuts.AddShortcut(ID_MOVETASKLEFT, VK_LEFT, HOTKEYF_CONTROL | HOTKEYF_EXT);
	m_mgrShortcuts.AddShortcut(ID_MOVETASKRIGHT, VK_RIGHT, HOTKEYF_CONTROL | HOTKEYF_EXT);
	m_mgrShortcuts.AddShortcut(ID_DELETETASK, VK_DELETE, HOTKEYF_EXT);
	m_mgrShortcuts.AddShortcut(ID_EDIT_TASKTEXT, VK_F2, 0);
	m_mgrShortcuts.AddShortcut(ID_EDIT_TASKDONE, VK_SPACE, HOTKEYF_CONTROL | HOTKEYF_SHIFT);
	m_mgrShortcuts.AddShortcut(ID_TOOLS_IMPORT, 'I', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_TOOLS_EXPORT, 'E', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_HELP, VK_F1, 0);
	m_mgrShortcuts.AddShortcut(ID_WINDOW1, '1', HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_WINDOW2, '2', HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_WINDOW3, '3', HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_WINDOW4, '4', HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_WINDOW5, '5', HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_WINDOW6, '6', HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_WINDOW7, '7', HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_WINDOW8, '8', HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_WINDOW9, '9', HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_TOOLS_TRANSFORM, 'T', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_EDIT_FINDTASKS, 'F', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_EDIT_QUICKFIND, 'Q', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_EDIT_QUICKFINDNEXT, VK_F3, 0);
	m_mgrShortcuts.AddShortcut(ID_EDIT_QUICKFINDPREV, VK_F3, HOTKEYF_SHIFT);
	m_mgrShortcuts.AddShortcut(ID_VIEW_REFRESHFILTER, VK_F5, HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_VIEW_TOGGLEFILTER, VK_F12, 0);
	m_mgrShortcuts.AddShortcut(ID_EDIT_SELECTALL, 'A', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_PREVTASK, VK_F7, 0);
	m_mgrShortcuts.AddShortcut(ID_NEXTTASK, VK_F8, 0);
	m_mgrShortcuts.AddShortcut(ID_EDIT_UNDO, 'Z', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_EDIT_REDO, 'Y', HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_VIEW_TOGGLETREEANDLIST, VK_F10, 0);
	m_mgrShortcuts.AddShortcut(ID_VIEW_TOGGLETASKSANDCOMMENTS, VK_F11, 0);
	m_mgrShortcuts.AddShortcut(ID_VIEW_TOGGLETASKEXPANDED, VK_SPACE, HOTKEYF_CONTROL | HOTKEYF_ALT);
	m_mgrShortcuts.AddShortcut(ID_EDIT_INCTASKPERCENTDONE, VK_ADD, HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_EDIT_DECTASKPERCENTDONE, VK_SUBTRACT, HOTKEYF_CONTROL);
	m_mgrShortcuts.AddShortcut(ID_VIEW_PREV_SEL, VK_LEFT, HOTKEYF_ALT | HOTKEYF_EXT);
	m_mgrShortcuts.AddShortcut(ID_VIEW_NEXT_SEL, VK_RIGHT, HOTKEYF_ALT | HOTKEYF_EXT);

	// now init shortcut mgr which will override the defaults
	// with the users actual settings
	if (m_mgrShortcuts.Initialize(this))
	{
		// fix for previously adding escape key as a shortcut for IDCLOSE
		// (big mistake)
		if (m_mgrShortcuts.GetShortcut(IDCLOSE) == VK_ESCAPE)
		{
			m_mgrShortcuts.DeleteShortcut(IDCLOSE);
		}

		// fix for paste being wrongly set up
		if (m_mgrShortcuts.GetShortcut(ID_EDIT_PASTE))
		{
			// delete existing
			m_mgrShortcuts.DeleteShortcut(ID_EDIT_PASTE);

			// if nothing already assigned use Ctrl+V
			if (!m_mgrShortcuts.GetShortcut(ID_EDIT_PASTESUB))
			{
				m_mgrShortcuts.AddShortcut(ID_EDIT_PASTESUB, 'V', HOTKEYF_CONTROL);
			}
		}
	}
}

void CToDoListWnd::InitMenuIconManager()
{
	if (!m_mgrMenuIcons.Initialize(this))
	{
		return;
	}

	m_mgrMenuIcons.ClearImages();

	CUIntArray aCmdIDs;

	aCmdIDs.Add(ID_LOAD_NORMAL);
	aCmdIDs.Add(ID_SAVE_NORMAL);
	aCmdIDs.Add(ID_SAVEALL);

	// new tasks
	aCmdIDs.Add(GetNewTaskCmdID());
	aCmdIDs.Add(GetNewSubtaskCmdID());

	aCmdIDs.Add(ID_EDIT_TASKTEXT);
	aCmdIDs.Add(ID_EDIT_SETTASKICON);
	aCmdIDs.Add(ID_EDIT_SETREMINDER);
	aCmdIDs.Add(ID_EDIT_UNDO);
	aCmdIDs.Add(ID_EDIT_REDO);
	aCmdIDs.Add(ID_MAXTASKLIST);
	aCmdIDs.Add(ID_VIEW_EXPANDTASK);
	aCmdIDs.Add(ID_VIEW_COLLAPSETASK);
	aCmdIDs.Add(ID_VIEW_PREV_SEL);
	aCmdIDs.Add(ID_VIEW_NEXT_SEL);
	aCmdIDs.Add(ID_EDIT_FINDTASKS);
	aCmdIDs.Add(ID_SORT);
	aCmdIDs.Add(ID_DELETETASK);

	// checkin
	if (GetTDCCount() && m_mgrToDoCtrls.PathSupportsSourceControl(GetSelToDoCtrl()))
	{
		CToDoCtrl& tdc = GetToDoCtrl();

		if (tdc.IsCheckedOut())
		{
			aCmdIDs.Add(ID_TOOLS_CHECKIN);
		}
		else
		{
			aCmdIDs.Add(ID_TOOLS_CHECKOUT);
		}
	}
	else
	{
		aCmdIDs.Add(ID_TOOLS_TOGGLECHECKIN);
	}

	aCmdIDs.Add(ID_PREFERENCES);
	aCmdIDs.Add(ID_HELP_WIKI);
	aCmdIDs.Add(ID_HELP_DONATE);
	aCmdIDs.Add(ID_HELP);

	m_mgrMenuIcons.AddImages(aCmdIDs, IDB_TOOLBAR16, 16, RGB(255, 0, 255));
}

void CToDoListWnd::OnShowKeyboardshortcuts()
{
	CStringArray aMapping;

	if (m_mgrShortcuts.BuildMapping(IDR_MAINFRAME, aMapping, '|'))
	{
		// add a few misc items that don't appear in the menus
		CString sMisc;

		for (int nItem = 0; nItem < NUM_MISCSHORTCUTS; nItem++)
		{
			if (MISC_SHORTCUTS[nItem].dwShortcut)
				sMisc.Format(_T("%s|%s"), m_mgrShortcuts.GetShortcutText(MISC_SHORTCUTS[nItem].dwShortcut),
					CEnString(MISC_SHORTCUTS[nItem].nIDShortcut));
			else
			{
				sMisc.Empty();
			}

			aMapping.Add(sMisc);
		}

		CKeyboardShortcutDisplayDlg dialog(aMapping, '|');

		dialog.DoModal();
	}
}

LRESULT CToDoListWnd::OnFocusChange(WPARAM wp, LPARAM /*lp*/)
{
	if (m_statusBar.GetSafeHwnd() && IsWindowEnabled() && GetTDCCount() && wp)
	{
		// grab the previous window in the z-order and if its
		// static text then use that as the focus hint
		CWnd* pFocus = CWnd::FromHandle((HWND)wp);
		const CFilteredToDoCtrl& tdc = GetToDoCtrl();
		m_sCurrentFocus.Empty();

		if (CDialogHelper::IsChildOrSame(tdc.GetSafeHwnd(), (HWND)wp))
		{
			m_sCurrentFocus.LoadString(IDS_FOCUS_TASKS);
			m_sCurrentFocus += _T(": ");
			m_sCurrentFocus += tdc.GetControlDescription(pFocus);
		}
		else
		{
			if (pFocus == m_cbQuickFind.GetWindow(GW_CHILD))
			{
				m_sCurrentFocus.LoadString(IDS_QUICKFIND);
			}
			else
			{
				if (m_filterBar.IsChild(pFocus))
				{
					m_sCurrentFocus.LoadString(IDS_FOCUS_FILTERBAR);
					m_sCurrentFocus += _T(": ");
				}
				m_sCurrentFocus += GetControlLabel(pFocus);
			}
		}

		m_statusBar.SetPaneText(m_statusBar.CommandToIndex(ID_SB_FOCUS), m_sCurrentFocus);

		// if the status bar is hidden then add text to title bar
		if (!m_bShowStatusBar)
		{
			UpdateCaption();
		}
	}

	return 0L;
}

LRESULT CToDoListWnd::OnGetIcon(WPARAM bLargeIcon, LPARAM /*not used*/)
{
	if (!bLargeIcon)
	{
		// cache small icon for reuse
		if (!m_hIcon)
		{
			m_hIcon = CSysImageList(FALSE).ExtractAppIcon();
		}

		return (LRESULT)m_hIcon;
	}
	else
	{
		return Default();
	}
}

BOOL CToDoListWnd::InitStatusbar()
{
	static SBACTPANEINFO SB_PANES[] =
	{
		{ ID_SB_FILEPATH, MAKEINTRESOURCE(IDS_SB_FILEPATH_TIP), SBACTF_STRETCHY | SBACTF_RESOURCETIP },
		{ ID_SB_FILEVERSION, MAKEINTRESOURCE(IDS_SB_FILEVERSION_TIP), SBACTF_AUTOFIT | SBACTF_RESOURCETIP },
		{ ID_SB_TASKCOUNT, MAKEINTRESOURCE(IDS_SB_TASKCOUNT_TIP), SBACTF_AUTOFIT | SBACTF_RESOURCETIP },
		{ ID_SB_SELCOUNT, MAKEINTRESOURCE(0), SBACTF_AUTOFIT | SBACTF_RESOURCETIP },
		{ ID_SB_SELTIMEEST, MAKEINTRESOURCE(IDS_SB_SELTIMEEST_TIP), SBACTF_AUTOFIT | SBACTF_RESOURCETIP },
		{ ID_SB_SELTIMESPENT, MAKEINTRESOURCE(IDS_SB_SELTIMESPENT_TIP), SBACTF_AUTOFIT | SBACTF_RESOURCETIP },
		{ ID_SB_SELCOST, MAKEINTRESOURCE(IDS_SB_SELCOST_TIP), SBACTF_AUTOFIT | SBACTF_RESOURCETIP },
		{ ID_SB_FOCUS, MAKEINTRESOURCE(IDS_SB_FOCUS_TIP), SBACTF_AUTOFIT | SBACTF_RESOURCETIP },
	};

	static int SB_PANECOUNT = sizeof(SB_PANES) / sizeof(SBACTPANEINFO);

	if (!m_statusBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, IDC_FILENAME))
	{
		return FALSE;
	}

	if (!m_statusBar.SetPanes(SB_PANES, SB_PANECOUNT))
	{
		return FALSE;
	}

	// for hooking focus messages
	if (!CFocusWatcher::Initialize(this))
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CToDoListWnd::InitToolbar()
{
	if (!m_toolbar.GetSafeHwnd())
	{
		if (m_toolbar.CreateEx(this, TBSTYLE_FLAT | TBSTYLE_WRAPABLE, WS_CHILD | WS_VISIBLE |
			CBRS_ALIGN_TOP | WS_CLIPCHILDREN | CBRS_TOOLTIPS))
		{
			m_toolbar.LoadToolBar(IDR_TOOLBAR_NEW);
			m_toolbar.SetImage(IDB_TOOLBAR16_NEW, RGB(255, 0, 255));
			m_toolbar.GetToolBarCtrl().HideButton(ID_TOOLS_TOGGLECHECKIN, !Prefs().GetEnableSourceControl());

			// colors
			if (CThemed::IsThemeActive())
			{
				m_toolbar.SetBackgroundColors(m_theme.crToolbarLight, m_theme.crToolbarDark, m_theme.nStyle == UIS_GRADIENT);
			}

			// resize the toolbar in one row so that our subsequent calculations work
			m_toolbar.MoveWindow(0, 0, 1000, 32);

			// insert combobox for quick Find after Find Tasks button
			int nPos = m_toolbar.CommandToIndex(ID_EDIT_FINDTASKS) + 1;

			TBBUTTON tbbQuickFind = { 0, nPos, 0, TBSTYLE_SEP, 0, NULL };
			TBBUTTON tbbSep = { 0, nPos + 1, 0, TBSTYLE_SEP, 0, NULL };

			m_toolbar.GetToolBarCtrl().InsertButton(nPos, &tbbQuickFind);
			m_toolbar.GetToolBarCtrl().InsertButton(nPos + 1, &tbbSep);

			TBBUTTONINFO tbi;
			tbi.cbSize = sizeof(TBBUTTONINFO);
			tbi.cx = 150;
			tbi.dwMask = TBIF_SIZE;  // By index

			m_toolbar.GetToolBarCtrl().SetButtonInfo(nPos + 1, &tbi);

			CRect rect;
			m_toolbar.GetToolBarCtrl().GetItemRect(nPos + 1, &rect);
			rect.bottom += 200;

			if (!m_cbQuickFind.Create(WS_CHILD | WS_VSCROLL | WS_VISIBLE | CBS_AUTOHSCROLL |
				CBS_DROPDOWN, rect, &m_toolbar, IDC_QUICKFIND))
			{
				return FALSE;
			}

			m_cbQuickFind.SetFont(&m_fontMain);
			m_mgrPrompts.SetComboEditPrompt(m_cbQuickFind, IDS_QUICKFIND);

			// add UI extensions once only
			CUIExtensionUIHelper uih(m_mgrUIExtensions, ID_TOOLS_USEREXT1);
			uih.AppendExtensionsToToolbar(m_toolbar, ID_PREFERENCES);
		}
	}

	return (NULL != m_toolbar.GetSafeHwnd());
}

void CToDoListWnd::OnEditChangeQuickFind()
{
	m_cbQuickFind.GetWindowText(m_sQuickFind);

	if (!GetToDoCtrl().SelectTask(m_sQuickFind, TDC_SELECTNEXTINCLCURRENT))
	{
		GetToDoCtrl().SelectTask(m_sQuickFind, TDC_SELECTFIRST);
	}
}

void CToDoListWnd::OnSelChangeQuickFind()
{
	int nSel = m_cbQuickFind.GetCurSel();

	if (nSel != CB_ERR)
	{
		m_cbQuickFind.GetLBText(nSel, m_sQuickFind);

		if (!GetToDoCtrl().SelectTask(m_sQuickFind, TDC_SELECTNEXT))
		{
			GetToDoCtrl().SelectTask(m_sQuickFind, TDC_SELECTFIRST);
		}
	}
}

BOOL CToDoListWnd::PreTranslateMessage(MSG* pMsg)
{
	// the only way we get a WM_CLOSE here is if it was sent from an external app
	// so we shut down as gracefully as possible
	if (pMsg->message == WM_CLOSE && IsWindowEnabled())
	{
		DoExit();
		return TRUE;
	}

	if (ProcessDialogControlShortcut(pMsg))
	{
		return TRUE;
	}

	if (IsDroppedComboBox(pMsg->hwnd))
	{
		return FALSE;
	}

	// process for app level shortcuts first so we can handle
	// reserved shortcuts
	DWORD dwShortcut = 0;
	UINT nCmdID = m_mgrShortcuts.ProcessMessage(pMsg, &dwShortcut);

	// if it's a reserved shortcut let's notify the user to change it
	if (CToDoCtrl::IsReservedShortcut(dwShortcut))
	{
		int nRet = MessageBox(IDS_RESERVEDSHORTCUT_MSG, IDS_RESERVEDSHORTCUT_TITLE, MB_YESNOCANCEL);

		if (nRet == IDYES)
		{
			DoPreferences(PREFPAGE_SHORTCUT);
		}

		// and keep eating it until the user changes it
		return TRUE;
	}

	// also we handle undo/redo
	if (nCmdID != ID_EDIT_UNDO && nCmdID != ID_EDIT_REDO)
	{
		// now try active task list
		if (GetTDCCount() && GetToDoCtrl().PreTranslateMessage(pMsg))
		{
			return TRUE;
		}
	}

	if (nCmdID)
	{
		BOOL bSendMessage = TRUE; // default

		// focus checks
		switch (nCmdID)
		{
		case ID_EDIT_CUT:
		case ID_EDIT_COPY:
			// tree must have the focus
			if (!GetToDoCtrl().TasksHaveFocus())
			{
				bSendMessage = FALSE;
				GetToDoCtrl().ClearCopiedItem();
			}
			break;

			// tree must have the focus
		case ID_EDIT_SELECT_ALL:
		case ID_EDIT_PASTE:
		case ID_DELETEALLTASKS:
		case ID_DELETETASK:
			bSendMessage = GetToDoCtrl().TasksHaveFocus();
			break;
		}

		// send off
		if (bSendMessage)
		{
			SendMessage(WM_COMMAND, nCmdID);
			return TRUE;
		}
	}

	// we need to check for <escape>, <tab> and <return>
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		{
			switch (pMsg->wParam)
			{
			case VK_ESCAPE:
				if (Prefs().GetEscapeMinimizes() && GetCapture() == NULL)
				{
					// if the window with the target is either a combobox or
					// the child edit of a combobox and the combo is
					// dropped down then let it thru else if the target is
					// a child of ours then treat as a cancel
					BOOL bHandle = TRUE;

					if (CWinClasses::IsClass(pMsg->hwnd, WC_COMBOBOX))
					{
						bHandle = !ComboBox_GetDroppedState(pMsg->hwnd);
					}

					else if (CWinClasses::IsClass(::GetParent(pMsg->hwnd), WC_COMBOBOX))
					{
						bHandle = !ComboBox_GetDroppedState(::GetParent(pMsg->hwnd));
					}

					else if (GetTDCCount() && GetToDoCtrl().IsTaskLabelEditing())
					{
						bHandle = FALSE;
					}

					if (bHandle && ::IsChild(*this, pMsg->hwnd))
					{
						OnCancel();
						return TRUE;
					}
				}
				break;

			case VK_TAB: // tabbing away from Quick Find -> tasks
				if (::IsChild(m_cbQuickFind, pMsg->hwnd))
				{
					GetToDoCtrl().SetFocusToTasks();
					return TRUE;
				}
				break;

			case VK_RETURN: // hitting return in filter bar and quick find
				if (Prefs().GetFocusTreeOnEnter())
				{
					CWnd* pFocus = GetFocus();

					if (pFocus && (m_filterBar.IsChild(pFocus) || m_cbQuickFind.IsChild(pFocus)))
					{
						if (!ControlWantsEnter(*pFocus))
						{
							GetToDoCtrl().SetFocusToTasks();
						}

						return FALSE; // continue routing
					}
				}
				break;
			}
		}
		break;
	}

	return CFrameWnd::PreTranslateMessage(pMsg);
}

void CToDoListWnd::OnCancel()
{
	ASSERT(Prefs().GetEscapeMinimizes());

	// if the close button has been configured to Minimize to tray
	// then do that here else normal minimize
	int nOption = Prefs().GetSysTrayOption();

	if (nOption == STO_ONMINCLOSE || nOption == STO_ONCLOSE)
	{
		MinimizeToTray();
	}
	else
	{
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}
}

void CToDoListWnd::OnDeleteTask()
{
	if (GetToDoCtrl().GetSelectedItem())
	{
		GetToDoCtrl().DeleteSelectedTask();
	}
}

void CToDoListWnd::OnDeleteAllTasks()
{
	if (GetToDoCtrl().DeleteAllTasks())
	{
		UpdateStatusbar();
	}
}

void CToDoListWnd::OnSave()
{
	if (SaveTaskList(GetSelToDoCtrl()) == TDCO_SUCCESS)
	{
		UpdateCaption();
	}
}

BOOL CToDoListWnd::DoBackup(int nIndex)
{
	if (!Prefs().GetBackupOnSave())
	{
		return TRUE;
	}

	CString sTDLPath = m_mgrToDoCtrls.GetFilePath(nIndex);

	if (sTDLPath.IsEmpty())
	{
		return TRUE;   // not yet saved
	}

	// get backup path
	CString sBackupFolder = Prefs().GetBackupLocation();
	sBackupFolder.TrimRight();

	// cull old backups
	int nKeepBackups = Prefs().GetKeepBackupCount();

	if (nKeepBackups)
	{
		CStringArray aFiles;
		CString sPath = CFileBackup::BuildBackupPath(sTDLPath, sBackupFolder, FALSE, _T(""));

		CString sDrive, sFolder, sFName, sExt, sPattern;

		FileMisc::SplitPath(sPath, &sDrive, &sFolder, &sFName, &sExt);
		FileMisc::MakePath(sPath, sDrive, sFolder);

		int nFiles = FileMisc::FindFiles(sPath, aFiles, sFName + _T("*") + sExt);

		if (nFiles >= nKeepBackups)
		{
			Misc::SortArray(aFiles); // sorts oldest backups first

			// cull as required
			while (aFiles.GetSize() >= nKeepBackups)
			{
				DeleteFile(aFiles[0]);
				aFiles.RemoveAt(0);
			}
		}
	}

	CFileBackup backup;
	return backup.MakeBackup(sTDLPath, sBackupFolder, TRUE, _T(""));
}

TDC_FILE CToDoListWnd::SaveTaskList(int nIndex, LPCTSTR szFilePath, BOOL bAuto)
{
	CAutoFlag af(m_bSaving, TRUE);
	CString sFilePath = szFilePath ? szFilePath : m_mgrToDoCtrls.GetFilePath(nIndex);

	CFilteredToDoCtrl& tdc = GetToDoCtrl(nIndex);

	tdc.Flush();

	// build dialog title, incorporating tasklist name
	CString sName = m_mgrToDoCtrls.GetFriendlyProjectName(nIndex);
	CEnString sTitle(IDS_SAVETASKLIST_TITLE, sName);

	// conditions for saving
	// 1. Save As... ie szFilePath != NULL and not empty
	// 2. tasklist has been modified
	if ((szFilePath && !sFilePath.IsEmpty()) || tdc.IsModified())
	{
		CPreferencesDlg& userPrefs = Prefs();

		// do this in a loop in case the save fails for _any_ reason
		while (TRUE)
		{
			if (sFilePath.IsEmpty()) // means first time save
			{
				// activate tasklist
				if (!SelectToDoCtrl(nIndex, (nIndex != GetSelToDoCtrl())))
				{
					return TDCO_CANCELLED;
				}

				// use tab text as hint to user
				sFilePath = m_mgrToDoCtrls.GetFilePath(nIndex, FALSE);

				CFileDialog dialog(FALSE, GetDefaultFileExt(), sFilePath,
					OFN_OVERWRITEPROMPT, GetFileFilter(), this);

				dialog.m_ofn.lpstrTitle = sTitle;

				if (dialog.DoModal() != IDOK)
				{
					return TDCO_CANCELLED;   // user elected not to proceed
				}

				// else make sure the file is not readonly
				sFilePath = dialog.GetPathName();

				if (CDriveInfo::IsReadonlyPath(sFilePath) > 0)
				{
					CEnString sMessage(IDS_SAVEREADONLY, sFilePath);

					if (MessageBox(sMessage, sTitle, MB_OKCANCEL) == IDCANCEL)
					{
						return TDCO_CANCELLED;   // user elected not to proceed
					}
					else
					{
						sFilePath.Empty(); // try again
						continue;
					}
				}
			}

			// back file up
			DoBackup(nIndex);

			// update source control status
			BOOL bSrcControl = m_mgrToDoCtrls.PathSupportsSourceControl(sFilePath);

			tdc.SetStyle(TDCS_ENABLESOURCECONTROL, bSrcControl);
			tdc.SetStyle(TDCS_CHECKOUTONLOAD, bSrcControl ? userPrefs.GetAutoCheckOut() : FALSE);

			CWaitCursor cursor;
			CTaskFile tasks;

			TDC_FILE nSave = tdc.Save(tasks, sFilePath);

			if (nSave == TDCO_SUCCESS)
			{
				m_mgrToDoCtrls.SetModifiedStatus(nIndex, FALSE);
				m_mgrToDoCtrls.RefreshLastModified(nIndex);
				m_mgrToDoCtrls.RefreshReadOnlyStatus(nIndex);
				m_mgrToDoCtrls.RefreshPathType(nIndex);

				if (userPrefs.GetAddFilesToMRU() && !m_mgrToDoCtrls.IsWebTaskList(nIndex))
				{
					m_mruList.Add(sFilePath);
				}

				UpdateCaption();
				UpdateStatusbar();

				// auto-export after saving
				CString sExt;

				if (userPrefs.GetAutoExport() && GetAutoExportExtension(sExt))
				{
					// construct output path
					CString sExportPath = userPrefs.GetAutoExportFolderPath();
					CString sDrive, sFolder, sFName;

					FileMisc::SplitPath(sFilePath, &sDrive, &sFolder, &sFName);

					if (!sExportPath.IsEmpty() && FileMisc::CreateFolder(sExportPath))
					{
						FileMisc::MakePath(sFilePath, NULL, sExportPath, sFName, sExt);
					}
					else
					{
						FileMisc::MakePath(sFilePath, sDrive, sFolder, sFName, sExt);
					}

					// The current contents of 'tasks' is 'All Tasks' and
					// 'All Columns' but NOT 'Html Comments'.
					// So if user either wants 'Filtered Tasks' or 'Html Comments' or
					// only 'Visible Columns' we need to grab the tasks again.
					BOOL bFiltered = (userPrefs.GetExportFilteredOnly() && (tdc.HasCustomFilter() || tdc.HasFilter()));

					if (bFiltered || userPrefs.GetExportToHTML() || !userPrefs.GetExportAllAttributes())
					{
						TSD_TASKS nWhatTasks = bFiltered ? TSDT_FILTERED : TSDT_ALL;
						TDCGETTASKS filter;

						if (!userPrefs.GetExportAllAttributes())
						{
							CTDCColumnArray aCols;
							tdc.GetVisibleColumns(aCols);

							MapColumnsToAttributes(aCols, filter.aAttribs);
							filter.dwFlags |= TDCGTF_CUSTOMATTRIB;

							// add comments always
							filter.aAttribs.Add(TDCA_COMMENTS);
						}

						BOOL bHtmlComments = userPrefs.GetExportToHTML();
						BOOL bTransform = FileMisc::FileExists(userPrefs.GetSaveExportStylesheet());

						tasks.Reset();
						GetTasks(tdc, bHtmlComments, bTransform, nWhatTasks, filter, tasks);
					}

					// want HTML
					if (userPrefs.GetExportToHTML())
					{
						Export2Html(tasks, sFilePath, userPrefs.GetSaveExportStylesheet());
					}
					else if (userPrefs.GetOtherExporter() != -1)
					{
						int nExporter = userPrefs.GetOtherExporter();
						m_mgrImportExport.ExportTaskList(&tasks, sFilePath, nExporter, TRUE);
					}
				}

				// upload if web tasklist
				if (m_mgrToDoCtrls.IsWebTaskList(nIndex))
				{
					CString sServer, sRemotePath, sUser, sPassword;
					m_mgrToDoCtrls.GetWebDetails(nIndex, sServer, sRemotePath, sUser, sPassword);

					CRemoteFile rf(NULL, sServer, sUser, sPassword);
					CString sLocalPath = m_mgrToDoCtrls.GetFilePath(nIndex);

					DWORD dwOptions = RMO_NOCANCELPROGRESS;
					rf.SetFile(sLocalPath, sRemotePath, dwOptions);

					// delete temp upload file
					if (FileMisc::IsTempFile(sLocalPath))
					{
						::DeleteFile(sLocalPath);
					}
				}

				// we're done
				break;
			}
			else if (!bAuto)
			{
				// error handling if this is not an auto-save
				if (nSave == TDCO_NOTALLOWED)
				{
					CEnString sMessage(IDS_SAVEACCESSDENIED, sFilePath);

					if (IDYES == MessageBox(sMessage, sTitle, MB_YESNOCANCEL | MB_ICONEXCLAMATION))
					{
						sFilePath.Empty(); // try again
						continue;
					}
					else // clear modified status
					{
						tdc.SetModified(FALSE);
						m_mgrToDoCtrls.SetModifiedStatus(nIndex, FALSE);
						break;
					}
				}
				else
				{
					CString sMessage;

					switch (nSave)
					{
					case TDCO_CANCELLED:
						break;

					case TDCO_BADMSXML:
						sMessage.Format(IDS_SAVEBADXML, sFilePath);
						break;

					case TDCO_INUSE:
						sMessage.Format(IDS_SAVESHARINGVIOLATION, sFilePath);
						break;

					default:
						sMessage.Format(IDS_UNKNOWNSAVEERROR2, sFilePath, (nSave - (int)TDCO_OTHER));
						break;
					}

					if (!sMessage.IsEmpty())
					{
						MessageBox(sMessage, sTitle, MB_OK);
					}

					return nSave;
				}
			}
			else // bAuto == TRUE
			{
				return nSave;
			}
		}
	}

	return TDCO_SUCCESS;
}

BOOL CToDoListWnd::GetAutoExportExtension(CString& sExt) const
{
	sExt.Empty();

	if (Prefs().GetExportToHTML())
	{
		sExt = _T(".html");
	}
	else
	{
		int nExporter = Prefs().GetOtherExporter();

		if (nExporter != -1)
		{
			sExt = m_mgrImportExport.GetExporterFileExtension(nExporter);
		}
	}

	return !sExt.IsEmpty();
}

LPCTSTR CToDoListWnd::GetFileFilter()
{
	static CEnString TDLFILEFILTER(IDS_TDLFILEFILTER);
	static CEnString XMLFILEFILTER(IDS_XMLFILEFILTER);

	if (Prefs().GetEnableTDLExtension())
	{
		return TDLFILEFILTER;
	}
	else
	{
		return XMLFILEFILTER;
	}
}

LPCTSTR CToDoListWnd::GetDefaultFileExt()
{
	static LPCTSTR TDLEXT = _T("tdl");
	static LPCTSTR XMLEXT = _T("xml");

	if (Prefs().GetEnableTDLExtension())
	{
		return TDLEXT;
	}
	else
	{
		return XMLEXT;
	}
}

void CToDoListWnd::UpdateStatusbar()
{
	if (m_bShowStatusBar && GetTDCCount())
	{
		// get display path
		int nTasklist = GetSelToDoCtrl();
		CString sText = m_mgrToDoCtrls.GetDisplayPath(nTasklist);

		if (sText.IsEmpty())
		{
			sText.LoadString(ID_SB_FILEPATH);
		}

		m_statusBar.SetPaneText(m_statusBar.CommandToIndex(ID_SB_FILEPATH), sText);

		// get file version
		sText.Format(ID_SB_FILEVERSION, m_mgrToDoCtrls.GetToDoCtrl(nTasklist).GetFileVersion());
		m_statusBar.SetPaneText(m_statusBar.CommandToIndex(ID_SB_FILEVERSION), sText);
	}
}

void CToDoListWnd::OnLoad()
{
	CFileDialog dialog(TRUE, GetDefaultFileExt(), NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT |
		OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST,
		GetFileFilter(), this);

	const UINT BUFSIZE = 1024 * 5;
	static TCHAR FILEBUF[BUFSIZE] = { 0 };

	CEnString sTitle(IDS_OPENTASKLIST_TITLE);

	dialog.m_ofn.lpstrTitle = (LPCTSTR)sTitle;
	dialog.m_ofn.lpstrFile = FILEBUF;
	dialog.m_ofn.nMaxFile = BUFSIZE;

	if (dialog.DoModal() == IDOK)
	{
		CWaitCursor cursor;
		POSITION pos = dialog.GetStartPosition();

		while (pos)
		{
			CString sTaskList = dialog.GetNextPathName(pos);
			TDC_FILE nOpen = OpenTaskList(sTaskList);

			if (nOpen == TDCO_SUCCESS)
			{
				ResizeDlg();
				UpdateWindow();
			}
			else
			{
				HandleLoadTasklistError(nOpen, sTaskList);
			}
		}

		RefreshTabOrder();
	}
}

void CToDoListWnd::HandleLoadTasklistError(TDC_FILE nErr, LPCTSTR szTaskList)
{
	CEnString sMessage, sTitle(IDS_OPENTASKLIST_TITLE);

	switch (nErr)
	{
	case TDCO_SUCCESS:
		break; // not an error!

	case TDCO_CANCELLED:
		break;

	case TDCO_NOTEXIST:
		sMessage.Format(IDS_TASKLISTNOTFOUND, szTaskList);
		break;

	case TDCO_NOTTASKLIST:
		sMessage.Format(IDS_INVALIDTASKLIST, szTaskList);
		break;

	case TDCO_NOTALLOWED:
		sMessage.Format(IDS_OPENACCESSDENIED, szTaskList);
		break;

	case TDCO_INUSE:
		sMessage.Format(IDS_OPENSHARINGVIOLATION, szTaskList);
		break;

	case TDCO_BADMSXML:
		sMessage.Format(IDS_BADXML, szTaskList);
		break;

	case TDCO_NOENCRYPTIONDLL:
		sMessage.Format(IDS_NOENCRYPTIONDLL, szTaskList);
		break;

	case TDCO_UNKNOWNENCRYPTION:
		sMessage.Format(IDS_UNKNOWNENCRYPTION, szTaskList);
		break;

	default: // all the other errors
		sMessage.Format(IDS_UNKNOWNOPENERROR, szTaskList, nErr - (int)TDCO_OTHER);
		break;
	}

	if (!sMessage.IsEmpty())
	{
		MessageBox(sMessage, sTitle, MB_OK);
	}
}

void CToDoListWnd::SaveSettings()
{
	CPreferences prefs;

	// pos
	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);

	prefs.WriteProfileInt(_T("Pos"), _T("TopLeft"), MAKELPARAM(wp.rcNormalPosition.left, wp.rcNormalPosition.top));
	prefs.WriteProfileInt(_T("Pos"), _T("BottomRight"), MAKELPARAM(wp.rcNormalPosition.right, wp.rcNormalPosition.bottom));
	prefs.WriteProfileInt(_T("Pos"), _T("MaxPos"), MAKELPARAM(wp.ptMaxPosition.x, wp.ptMaxPosition.y));
	prefs.WriteProfileInt(_T("Pos"), _T("MinPos"), MAKELPARAM(wp.ptMinPosition.x, wp.ptMinPosition.y));

	prefs.WriteProfileInt(_T("Pos"), _T("Hidden"), !m_bVisible);
	prefs.WriteProfileInt(_T("Pos"), _T("Maximized"), IsZoomed());

	// version
	prefs.WriteProfileInt(_T("Version"), _T("Version"), GetVersion());

	// last open files
	int nCount = GetTDCCount();
	int nSel = GetSelToDoCtrl(); // and last active file

	if (nCount) // but don't overwrite files saved in OnQueryEndSession() or OnClose()
	{
		for (int nTDC = 0, nItem = 0; nTDC < nCount; nTDC++)
		{
			CString sFilePath = m_mgrToDoCtrls.GetFilePath(nTDC);
			TDCM_PATHTYPE nType = m_mgrToDoCtrls.GetFilePathType(nTDC);

			// if the tasklist is on a removable drive then remove the drive letter
			if (nType == TDCM_WEB) // don't save for now
			{
				nCount--;
				continue;
			}
			else // make relative
			{
				FileMisc::MakeRelativePath(sFilePath, FileMisc::GetAppFolder(), FALSE);
			}

			CEnString sKey(_T("LastFile%d"), nItem);
			prefs.WriteProfileString(_T("Settings"), sKey, sFilePath);

			if (nSel == nTDC)
			{
				prefs.WriteProfileString(_T("Settings"), _T("LastActiveFile"), sFilePath);
			}

			nItem++;
		}

		prefs.WriteProfileInt(_T("Settings"), _T("NumLastFiles"), nCount);
	}

	// other settings
	prefs.WriteProfileInt(_T("Settings"), _T("ViewState"), m_nMaxState);
	prefs.WriteProfileInt(_T("Settings"), _T("ShowFilterBar"), m_bShowFilterBar);
	prefs.WriteProfileInt(_T("Settings"), _T("ToolbarOption"), m_bShowToolbar ? TB_TOOLBARANDMENU : TB_TOOLBARHIDDEN);
	prefs.WriteProfileInt(_T("Settings"), _T("ShowProjectName"), m_bShowProjectName);
	prefs.WriteProfileInt(_T("Settings"), _T("ShowStatusBar"), m_bShowStatusBar);
	prefs.WriteProfileInt(_T("Settings"), _T("ShowTasklistBar"), m_bShowTasklistBar);
	prefs.WriteProfileInt(_T("Settings"), _T("ShowTreeListBar"), m_bShowTreeListBar);

	if (m_findDlg.GetSafeHwnd())
	{
		prefs.WriteProfileInt(_T("Settings"), _T("FindTasksVisible"), m_bFindShowing && m_findDlg.IsWindowVisible());
	}

	if (Prefs().GetAddFilesToMRU())
	{
		m_mruList.WriteList(prefs, TRUE);
	}

	// quick find items
	nCount = m_cbQuickFind.GetCount();
	prefs.WriteProfileInt(_T("QuickFind"), _T("Count"), nCount);

	for (int nItem = 0; nItem < nCount; nItem++)
	{
		CString sKey, sItem;
		sKey.Format(_T("Item%d"), nItem);
		m_cbQuickFind.GetLBText(nItem, sItem);

		prefs.WriteProfileString(_T("QuickFind"), sKey, sItem);
	}
}

LRESULT CToDoListWnd::OnWebUpdateWizard(WPARAM /*wp*/, LPARAM /*lp*/)
{
	ASSERT(Prefs().GetAutoCheckForUpdates());

	CheckForUpdates(FALSE);
	return 0L;
}

LRESULT CToDoListWnd::OnAddToolbarTools(WPARAM /*wp*/, LPARAM /*lp*/)
{
	Misc::ProcessMsgLoop();
	AppendTools2Toolbar();
	return 0L;
}

void CToDoListWnd::PrepareOpenFilePath(CString& sFilePath)
{
	// if it starts with a colon then we need to find the
	// removable drive it's stored on
	if (!sFilePath.IsEmpty())
	{
		if (sFilePath[0] == _T(':'))
		{
			for (int nDrive = 4; nDrive <= 26; nDrive++) // from D: upwards
			{
				if (CDriveInfo::GetType(nDrive) == DRIVE_REMOVABLE)
				{
					CString sTryPath = CDriveInfo::GetLetter(nDrive) + sFilePath;

					if (FileMisc::FileExists(sTryPath))
					{
						sFilePath = sTryPath;
						break; // finished
					}
				}
			}
		}
		else
		{
			FileMisc::MakeFullPath(sFilePath, FileMisc::GetAppFolder());   // handle relative paths
		}
	}
}

LRESULT CToDoListWnd::OnPostOnCreate(WPARAM /*wp*/, LPARAM /*lp*/)
{
	CAutoFlag af(m_bReloading, TRUE);
	CPreferencesDlg& userPrefs = Prefs();
	CPreferences prefs;

	RestoreVisibility();
	LockWindowUpdate();

	// open cmdline tasklist
	int nTDCCount = prefs.GetProfileInt(_T("Settings"), _T("NumLastFiles"), 0);

	// if we have a file on the commandline or any previous tasklists
	// set the prompt of the initial tasklist to something appropriate
	if (m_startupOptions.GetFilePath().IsEmpty() || nTDCCount)
	{
		//	TODO
	}

	// theme
	SetUITheme(userPrefs.GetUITheme());

	BOOL bOpen = FALSE;

	if (!m_startupOptions.GetFilePath().IsEmpty())
	{
		ProcessStartupOptions(m_startupOptions);
		bOpen = !m_mgrToDoCtrls.IsPristine(0);
	}

	m_startupOptions.Clear(); // always

	// load last files
	if (!bOpen)
	{
		CString sLastActiveFile = prefs.GetProfileString(_T("Settings"), _T("LastActiveFile")), sOrgLastActiveFile;
		PrepareOpenFilePath(sLastActiveFile);

		BOOL bCanDelayLoad = userPrefs.GetEnableDelayedLoading();

		for (int nTDC = 0; nTDC < nTDCCount; nTDC++)
		{
			CEnString sKey(_T("LastFile%d"), nTDC);
			CString sLastFile = prefs.GetProfileString(_T("Settings"), sKey);

			PrepareOpenFilePath(sLastFile);

			if (!sLastFile.IsEmpty())
			{
				// delay open all but the non-active tasklist
				BOOL bActiveTDC = (sLastFile == sLastActiveFile);

				if (!bActiveTDC && bCanDelayLoad)
				{
					DelayOpenTaskList(sLastFile);
				}
				else
				{
					TDC_FILE nResult = OpenTaskList(sLastFile, NULL, FALSE);

					if (bActiveTDC)
					{
						// if the last active tasklist was cancelled then
						// delay load it and mark the last active todoctrl as not found
						if (nResult != TDCO_SUCCESS)
						{
							sOrgLastActiveFile = sLastActiveFile;
							sLastActiveFile.Empty();

							if (nResult == TDCO_CANCELLED && bCanDelayLoad)
							{
								DelayOpenTaskList(sLastFile);
							}
						}
					}
				}
			}

			// process all pending messages
			Misc::ProcessMsgLoop();
		}

		// if the last active tasklist could not be loaded then we need to find another
		if (GetTDCCount())
		{
			if (sLastActiveFile.IsEmpty())
			{
				for (int nTDC = 0; nTDC < GetTDCCount() && sLastActiveFile.IsEmpty(); nTDC++)
				{
					CFilteredToDoCtrl& tdc = GetToDoCtrl(nTDC);

					// ignore original active tasklist
					if (tdc.GetFilePath() != sOrgLastActiveFile)
					{
						if (VerifyTaskListOpen(nTDC, FALSE))
						{
							sLastActiveFile = tdc.GetFilePath();
						}
					}
				}
			}

			// if nothing suitable found then create an empty tasklist
			if (sLastActiveFile.IsEmpty())
			{
				CreateNewTaskList(FALSE);
			}

			else if (!SelectToDoCtrl(sLastActiveFile, FALSE))
			{
				SelectToDoCtrl(0, FALSE);   // the first one
			}

			ResizeDlg();
		}
	}

	UnlockWindowUpdate();

	// if there's only one tasklist open and it's pristine then
	// it's the original one so add a sample task
	if (m_mgrToDoCtrls.IsPristine(0))
	{
		ASSERT(GetTDCCount() == 1);

		CToDoCtrl& tdc = GetToDoCtrl();
		ASSERT(tdc.GetTaskCount() == 0);

		tdc.NewTask(CEnString(IDS_SAMPLETASK), TDC_INSERTATTOP, FALSE);
		tdc.SetModified(FALSE);

		m_mgrToDoCtrls.SetModifiedStatus(0, FALSE);

		UpdateCaption();
	}
	else // due task notifications
	{
		int nDueBy = userPrefs.GetNotifyDueByOnLoad();

		if (nDueBy != PFP_DONTNOTIFY)
		{
			UpdateWindow();
			m_tabCtrl.UpdateWindow();

			int nCtrls = GetTDCCount();

			for (int nCtrl = 0; nCtrl < nCtrls; nCtrl++)
			{
				if (m_mgrToDoCtrls.IsLoaded(nCtrl))
				{
					DoDueTaskNotification(&GetToDoCtrl(nCtrl), nDueBy);
				}
			}
		}
	}

	// find tasks dialog
	if (prefs.GetProfileInt(_T("Settings"), _T("FindTasksVisible"), 0))
	{
		OnFindTasks();

		if (userPrefs.GetRefreshFindOnLoad())
		{
			m_findDlg.RefreshSearch();
		}
	}

	// refresh toolbar 'tools' buttons unless minimized because
	// we must handle it when we're first shown
	if (m_bShowToolbar && AfxGetApp()->m_nCmdShow != SW_SHOWMINIMIZED)
	{
		AppendTools2Toolbar();
	}

	InitMenuIconManager();

	// web update
	if (Prefs().GetAutoCheckForUpdates())
	{
		PostMessage(WM_WEBUPDATEWIZARD);
	}

	// current focus
	PostMessage(WM_FW_FOCUSCHANGE, (WPARAM)::GetFocus(), 0L);

	RefreshTabOrder();

	return 0L;
}

void CToDoListWnd::CheckForUpdates(BOOL bManual)
{
	CPreferences prefs;

	// only check once a day
	int nLastUpdate = prefs.GetProfileInt(_T("Updates"), _T("LastUpdate"), 0);

	if (!bManual && nLastUpdate >= (int)COleDateTime::GetCurrentTime().m_dt)
	{
		return;
	}

	prefs.WriteProfileInt(_T("Updates"), _T("LastUpdate"), (int)COleDateTime::GetCurrentTime().m_dt);

	// get the app wuw path
	CString sFolder, sDrive;
	CString sWuwPath = FileMisc::GetAppFolder() + _T("\\WebUpdateSvc.exe");

	// check for existence if manual
	if (bManual && !FileMisc::FileExists(sWuwPath))
	{
		const LPCTSTR DOWNLOAD_WUW_PATH = _T("http://www.abstractspoon.com/todolist_wuw.zip");

		if (AfxMessageBox(IDS_NOWUW, MB_YESNO) == IDYES)
		{
			ShellExecute(NULL, _T("open"), DOWNLOAD_WUW_PATH, NULL, NULL, SW_HIDE);
		}
		else
		{
			return;
		}
	}

	const LPCTSTR UPDATE_SCRIPT_PATH = _T("http://www.abstractspoon.com/todolist_update.txt");
	const LPCTSTR UPDATE_SCRIPT_PATH_MANUAL = _T("http://www.abstractspoon.com/todolist_update_manual.txt");

	if (bManual)
	{
		ShellExecute(NULL, _T("open"), sWuwPath, UPDATE_SCRIPT_PATH_MANUAL, NULL, SW_HIDE);
	}
	else
	{
		ShellExecute(NULL, _T("open"), sWuwPath, UPDATE_SCRIPT_PATH, NULL, SW_HIDE);
	}
}

void CToDoListWnd::LoadSettings()
{
	// settings
	CPreferences prefs;

	BOOL bMaxTasklists = prefs.GetProfileInt(_T("Settings"), _T("SimpleMode"), FALSE); // backward compatibility
	m_nMaxState = (TDC_MAXSTATE)prefs.GetProfileInt(_T("Settings"), _T("ViewState"),
		bMaxTasklists ? TDCMS_MAXTASKLIST : TDCMS_NORMAL);

	m_bShowFilterBar = prefs.GetProfileInt(_T("Settings"), _T("ShowFilterBar"), m_bShowFilterBar);
	m_bShowProjectName = prefs.GetProfileInt(_T("Settings"), _T("ShowProjectName"), m_bShowProjectName);

	m_bShowStatusBar = prefs.GetProfileInt(_T("Settings"), _T("ShowStatusBar"), m_bShowStatusBar);
	m_statusBar.ShowWindow(m_bShowStatusBar ? SW_SHOW : SW_HIDE);

	// toolbar
	m_bShowToolbar = (prefs.GetProfileInt(_T("Settings"), _T("ToolbarOption"), TB_TOOLBARANDMENU) != TB_TOOLBARHIDDEN);
	m_toolbar.ShowWindow(m_bShowToolbar ? SW_SHOW : SW_HIDE);
	m_toolbar.EnableWindow(m_bShowToolbar);

	// tabbars
	m_bShowTasklistBar = prefs.GetProfileInt(_T("Settings"), _T("ShowTasklistBar"), TRUE);
	m_bShowTreeListBar = prefs.GetProfileInt(_T("Settings"), _T("ShowTreeListBar"), TRUE);

	// pos
	RestorePosition();

	// user preferences
	CPreferencesDlg& userPrefs = Prefs();

	// MRU
	if (userPrefs.GetAddFilesToMRU())
	{
		m_mruList.ReadList(prefs);
	}

	RefreshFilterLabelAlignment();

	// note: we do not restore visibility until OnPostOnCreate

	// hotkey
	UpdateGlobalHotkey();

	// time periods
	CTimeHelper::SetHoursInOneDay(userPrefs.GetHoursInOneDay());
	CTimeHelper::SetDaysInOneWeek(userPrefs.GetDaysInOneWeek());

	// support for .tdl
	CFileRegister filereg(_T("tdl"), _T("tdl_TaskList"));

	if (userPrefs.GetEnableTDLExtension())
	{
		filereg.RegisterFileType(_T("Tasklist"), 0);
	}
	else
	{
		filereg.UnRegisterFileType();
	}

	// support for tdl protocol
	EnableTDLProtocol(userPrefs.GetEnableTDLProtocol());

	// previous quick find items
	int nCount = prefs.GetProfileInt(_T("QuickFind"), _T("Count"), 0);

	for (int nItem = 0; nItem < nCount; nItem++)
	{
		CString sKey;
		sKey.Format(_T("Item%d"), nItem);
		m_cbQuickFind.AddUniqueItem(prefs.GetProfileString(_T("QuickFind"), sKey));
	}
}

void CToDoListWnd::EnableTDLProtocol(BOOL bEnable)
{
	if (bEnable)
	{
		CAfxRegKey reg;

		if (reg.Open(HKEY_CLASSES_ROOT, _T("tdl")) == ERROR_SUCCESS)
		{
			reg.Write(_T(""), _T("URL: ToDoList protocol"));
			reg.Write(_T("URL Protocol"), _T(""));

			// write exe name out
			CString sAppPath = FileMisc::GetAppFileName() + _T(" -l \"%1\"");

			reg.Close();

			if (reg.Open(HKEY_CLASSES_ROOT, _T("tdl\\shell\\open\\command")) == ERROR_SUCCESS)
			{
				reg.Write(_T(""), sAppPath);
			}
		}
	}
	else
	{
		CAfxRegKey::Delete(HKEY_CLASSES_ROOT, _T("tdl"));
	}
}

void CToDoListWnd::RestoreVisibility()
{
	CPreferencesDlg& userPrefs = Prefs();
	CPreferences prefs;

	int nDefShowState = AfxGetApp()->m_nCmdShow;
	BOOL bShowOnStartup = userPrefs.GetShowOnStartup();

	BOOL bMaximized = prefs.GetProfileInt(_T("Pos"), _T("Maximized"), FALSE) || (nDefShowState == SW_SHOWMAXIMIZED);
	BOOL bMinimized = !bShowOnStartup && (nDefShowState == SW_SHOWMINIMIZED || nDefShowState == SW_SHOWMINNOACTIVE);

	if (bMinimized)
	{
		bMaximized = FALSE; // can't be max-ed and min-ed
		m_bStartHidden = TRUE;
	}

	if (m_bVisible == -1) // not yet set
	{
		m_bVisible = TRUE;

		// the only reason it can be hidden is if it uses the systray
		// and the user has elected to not have it show at startup
		// and it was hidden the last time it closed or its set to run
		// minimized and that is the trigger to hide it
		if (!bShowOnStartup && userPrefs.GetUseSysTray())
		{
			if (prefs.GetProfileInt(_T("Pos"), _T("Hidden"), FALSE))
			{
				m_bVisible = FALSE;
			}

			// also if wp.showCmd == minimized and we would hide to sys
			// tray when minimized then hide here too
			else if (nDefShowState == SW_SHOWMINIMIZED || nDefShowState == SW_SHOWMINNOACTIVE)
			{
				int nSysTrayOption = Prefs().GetSysTrayOption();

				if (nSysTrayOption == STO_ONMINIMIZE || nSysTrayOption == STO_ONMINCLOSE)
				{
					m_bVisible = FALSE;
				}
			}
		}
	}

	if (m_bVisible)
	{
		int nShowCmd = (bMaximized ? SW_SHOWMAXIMIZED : (bMinimized ? SW_SHOWMINIMIZED : SW_SHOW));

		ShowWindow(nShowCmd);

		Invalidate();
		UpdateWindow();
	}
	else
	{
		m_bStartHidden = TRUE;
	}

	// don't set topmost if maximized
	if (userPrefs.GetAlwaysOnTop() && !bMaximized)
	{
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}
}

void CToDoListWnd::RestorePosition()
{
	CPreferences prefs;

	DWORD dwTopLeft = (DWORD)prefs.GetProfileInt(_T("Pos"), _T("TopLeft"), -1);
	DWORD dwBottomRight = (DWORD)prefs.GetProfileInt(_T("Pos"), _T("BottomRight"), -1);
	DWORD dwMax = prefs.GetProfileInt(_T("Pos"), _T("MaxPos"), -1);
	DWORD dwMin = prefs.GetProfileInt(_T("Pos"), _T("MinPos"), -1);

	CRect rect(0, 0, 0, 0);

	if (dwTopLeft != -1 && dwBottomRight != -1)
	{
		// get min size before we resize
		MINMAXINFO mmi;
		SendMessage(WM_GETMINMAXINFO, 0, (LPARAM)&mmi);

		CRect rTemp(GET_X_LPARAM(dwTopLeft), GET_Y_LPARAM(dwTopLeft),
			GET_X_LPARAM(dwBottomRight), GET_Y_LPARAM(dwBottomRight));

		rTemp.right = max(rTemp.right, rTemp.left + mmi.ptMinTrackSize.x);
		rTemp.bottom = max(rTemp.bottom, rTemp.top + mmi.ptMinTrackSize.y);

		// ensure this intersects with the desktop by a decent amount
		int BORDER = 200;
		rTemp.DeflateRect(BORDER, BORDER);

		if (NULL != MonitorFromRect(rTemp, MONITOR_DEFAULTTONULL))
		{
			rTemp.InflateRect(BORDER, BORDER);
			rect = rTemp;

			// because the position was save using the results of
			// GetWindowPlacement we must use SetWindowPlacement
			// to restore the window
			WINDOWPLACEMENT wp;
			ZeroMemory(&wp, sizeof(wp));

			wp.rcNormalPosition = rect;
			wp.ptMaxPosition.x = GET_X_LPARAM(dwMax);
			wp.ptMaxPosition.y = GET_Y_LPARAM(dwMax);
			wp.ptMinPosition.x = GET_X_LPARAM(dwMin);
			wp.ptMinPosition.y = GET_Y_LPARAM(dwMin);

			SetWindowPlacement(&wp);
		}
	}

	// first time or monitors changed?
	if (rect.IsRectEmpty())
	{
		MoveWindow(0, 0, 1024, 730);
		CenterWindow();
	}
}

void CToDoListWnd::OnNew()
{
	CreateNewTaskList(FALSE);
	RefreshTabOrder();
}

BOOL CToDoListWnd::CreateNewTaskList(BOOL bAddDefTask)
{
	CFilteredToDoCtrl* pNew = NewToDoCtrl();

	if (pNew)
	{
		int nNew = AddToDoCtrl(pNew);

		// insert a default task
		if (bAddDefTask)
		{
			if (pNew->GetTaskCount() == 0)
			{
				VERIFY(NewTask(CEnString(IDS_TASK), TDC_INSERTATTOP, FALSE));
			}
		}
		else // ensure it is empty
		{
			pNew->DeleteAllTasks();
		}

		// clear modified flag
		pNew->SetModified(FALSE);
		m_mgrToDoCtrls.SetModifiedStatus(nNew, FALSE);

		// update filter controls
		RefreshFilterControls();

		UpdateCaption();
		UpdateStatusbar();
	}

	return (pNew != NULL);
}

void CToDoListWnd::OnUpdateDeletetask(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	pCmdUI->Enable(!tdc.IsReadOnly() && tdc.HasSelection());
}

void CToDoListWnd::OnUpdateEditTasktext(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	pCmdUI->Enable(nSelCount == 1 && !tdc.IsReadOnly());
}

void CToDoListWnd::OnUpdateTaskcolor(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	pCmdUI->Enable(!tdc.IsReadOnly() && tdc.HasSelection() &&
		Prefs().GetTextColorOption() == COLOROPT_DEFAULT);
}

void CToDoListWnd::OnUpdateTaskdone(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	if (nSelCount == 1)
	{
		pCmdUI->SetCheck(tdc.IsSelectedTaskDone() ? 1 : 0);
	}

	pCmdUI->Enable(!tdc.IsReadOnly() && GetToDoCtrl().GetSelectedItem());
}

void CToDoListWnd::OnUpdateDeletealltasks(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	pCmdUI->Enable(!tdc.IsReadOnly() && GetToDoCtrl().GetTaskCount());
}

void CToDoListWnd::OnUpdateSave(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	pCmdUI->Enable(GetTDCCount() && tdc.IsModified() && !tdc.IsReadOnly());
}

void CToDoListWnd::OnUpdateNew(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

BOOL CToDoListWnd::OnEraseBkgnd(CDC* pDC)
{
	CDialogHelper::ExcludeChild(this, pDC, &m_toolbar);
	CDialogHelper::ExcludeChild(this, pDC, &m_statusBar);
	CDialogHelper::ExcludeChild(this, pDC, &m_tabCtrl);

	CRect rClient;
	GetClientRect(rClient);

	if (CThemed::IsThemeActive())
	{
		// paint between the window top and the top of the toolbar
		// in toolbar color
		if (m_bShowToolbar)
		{
			CRect rToolbar = CDialogHelper::OffsetCtrl(this, &m_toolbar);

			pDC->FillSolidRect(rClient.left, rClient.top, rClient.Width(), rToolbar.top, m_theme.crToolbarLight);
			rClient.top += rToolbar.bottom + 2;
		}

		// we need to paint a smidgen between the base of the toolbar
		// and the top of the tab bar in crAppBackDark
		if (WantTasklistTabbarVisible())
		{
			pDC->FillSolidRect(rClient.left, rClient.top, rClient.Width(), 5, m_theme.crAppBackDark);
			rClient.top += 5;
		}

		// and then the rest in crAppBackLight
		pDC->FillSolidRect(rClient, m_theme.crAppBackLight);
	}
	else
	{
		pDC->FillSolidRect(rClient, GetSysColor(COLOR_3DFACE));
	}

	// we must draw out own bevel below the toolbar (or menu if the toolbar is not visible)
	int nVPos = 0;

	if (m_bShowToolbar)
	{
		pDC->FillSolidRect(rClient.left, nVPos, rClient.Width(), 1, m_theme.crAppLinesDark);
		nVPos = m_toolbar.CalcHeightRequired(rClient.Width()) + TB_VOFFSET;
	}

	pDC->FillSolidRect(rClient.left, nVPos, rClient.Width(), 1, m_theme.crAppLinesDark);
	pDC->FillSolidRect(rClient.left, nVPos + 1, rClient.Width(), 1, m_theme.crAppLinesLight);

	// bevel below filter bar
	if (m_bShowFilterBar)
	{
		CRect rFilter;
		m_filterBar.GetWindowRect(rFilter);
		ScreenToClient(rFilter);

		int nVPos = rFilter.bottom;

		pDC->FillSolidRect(rClient.left, nVPos, rClient.Width(), 1, m_theme.crAppLinesDark);
		pDC->FillSolidRect(rClient.left, nVPos + 1, rClient.Width(), 1, m_theme.crAppLinesLight);
	}

	// bevel above the statusbar if themed and not maximized
	if (m_bShowStatusBar && CThemed::IsThemeActive() &&
		(m_nMaxState == TDCMS_NORMAL || (m_nMaxState == TDCMS_MAXTASKLIST && m_bShowTreeListBar)))
	{
		CRect rStatBar;
		m_statusBar.GetWindowRect(rStatBar);
		ScreenToClient(rStatBar);

		pDC->FillSolidRect(0, rStatBar.top - 2, rClient.Width(), 1, m_theme.crAppLinesDark);
		pDC->FillSolidRect(0, rStatBar.top - 1, rClient.Width(), 1, m_theme.crAppLinesLight);
	}

	// this is definitely amongst the worst hacks I've ever had to implement.
	// It occurs because the CSysImageList class seems to not initialize
	// properly unless the main window is visible. so in the case of starting hidden
	// or starting minimized we must wait until we become visible before
	// adding the tools to the toolbar.
	if (m_bStartHidden)
	{
		m_bStartHidden = FALSE;
		PostMessage(WM_ADDTOOLBARTOOLS);
	}

	return TRUE;
}

void CToDoListWnd::OnSortBy(UINT nCmdID)
{
	TDC_SORTBY nSortBy = GetSortBy(nCmdID);

	// update todoctrl
	GetToDoCtrl().Sort(nSortBy);
}

void CToDoListWnd::OnUpdateSortBy(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	// disable if column not visible
	TDC_SORTBY nSortBy = GetSortBy(pCmdUI->m_nID);
	TDC_COLUMN nColID = MapSortByToColumn(nSortBy);

	pCmdUI->Enable(tdc.IsColumnShowing(nColID));

	// only set the radio button if we're not multisorting
	BOOL bChecked = !GetToDoCtrl().IsMultiSorting() && tdc.GetSortBy() == nSortBy;
	pCmdUI->SetRadio(bChecked);

	// let menu icon manager handle setting selected state
	switch (pCmdUI->m_nID)
	{
	case ID_SORT_NONE:
		pCmdUI->Enable(TRUE);
		break;

	case ID_SORT_BYCOLOR:
		pCmdUI->Enable(Prefs().GetTextColorOption() == COLOROPT_DEFAULT);
		break;
	}
}

TDC_SORTBY CToDoListWnd::GetSortBy(UINT nSortID)
{
	switch (nSortID)
	{
	case ID_SORT_BYNAME:
		return TDC_SORTBYNAME;
	case ID_SORT_BYID:
		return TDC_SORTBYID;
	case ID_SORT_BYALLOCTO:
		return TDC_SORTBYALLOCTO;
	case ID_SORT_BYALLOCBY:
		return TDC_SORTBYALLOCBY;
	case ID_SORT_BYSTATUS:
		return TDC_SORTBYSTATUS;
	case ID_SORT_BYCATEGORY:
		return TDC_SORTBYCATEGORY;
	case ID_SORT_BYPERCENT:
		return TDC_SORTBYPERCENT;
	case ID_SORT_BYCOLOR:
		return TDC_SORTBYCOLOR;
	case ID_SORT_BYTIMEEST:
		return TDC_SORTBYTIMEEST;
	case ID_SORT_BYTIMESPENT:
		return TDC_SORTBYTIMESPENT;
	case ID_SORT_BYSTARTDATE:
		return TDC_SORTBYSTARTDATE;
	case ID_SORT_BYDUEDATE:
		return TDC_SORTBYDUEDATE;
	case ID_SORT_BYDONEDATE:
		return TDC_SORTBYDONEDATE;
	case ID_SORT_BYDONE:
		return TDC_SORTBYDONE;
	case ID_SORT_BYPRIORITY:
		return TDC_SORTBYPRIORITY;
	case ID_SORT_BYCREATEDBY:
		return TDC_SORTBYCREATEDBY;
	case ID_SORT_BYCREATIONDATE:
		return TDC_SORTBYCREATIONDATE;
	case ID_SORT_BYMODIFYDATE:
		return TDC_SORTBYLASTMOD;
	case ID_SORT_BYRISK:
		return TDC_SORTBYRISK;
	case ID_SORT_BYEXTERNALID:
		return TDC_SORTBYEXTERNALID;
	case ID_SORT_BYCOST:
		return TDC_SORTBYCOST;
	case ID_SORT_BYVERSION:
		return TDC_SORTBYVERSION;
	case ID_SORT_BYRECURRENCE:
		return TDC_SORTBYRECURRENCE;
	case ID_SORT_NONE:
		return TDC_UNSORTED;
	case ID_SORT_BYFLAG:
		return TDC_SORTBYFLAG;
	case ID_SORT_BYREMAINING:
		return TDC_SORTBYREMAINING;
	case ID_SORT_BYRECENTEDIT:
		return TDC_SORTBYRECENTEDIT;
	case ID_SORT_BYICON:
		return TDC_SORTBYICON;
	case ID_SORT_BYFILEREF:
		return TDC_SORTBYFILEREF;
	case ID_SORT_BYTIMETRACKING:
		return TDC_SORTBYTIMETRACKING;
	}

	ASSERT(0);
	return TDC_UNSORTED;
}

UINT CToDoListWnd::GetSortID(TDC_SORTBY nSortBy)
{
	switch (nSortBy)
	{
	case TDC_SORTBYNAME:
		return ID_SORT_BYNAME;
	case TDC_SORTBYID:
		return ID_SORT_BYID;
	case TDC_SORTBYALLOCTO:
		return ID_SORT_BYALLOCTO;
	case TDC_SORTBYALLOCBY:
		return ID_SORT_BYALLOCBY;
	case TDC_SORTBYSTATUS:
		return ID_SORT_BYSTATUS;
	case TDC_SORTBYCATEGORY:
		return ID_SORT_BYCATEGORY;
	case TDC_SORTBYPERCENT:
		return ID_SORT_BYPERCENT;
	case TDC_SORTBYCOLOR:
		return ID_SORT_BYCOLOR;
	case TDC_SORTBYTIMEEST:
		return ID_SORT_BYTIMEEST;
	case TDC_SORTBYTIMESPENT:
		return ID_SORT_BYTIMESPENT;
	case TDC_SORTBYSTARTDATE:
		return ID_SORT_BYSTARTDATE;
	case TDC_SORTBYDUEDATE:
		return ID_SORT_BYDUEDATE;
	case TDC_SORTBYDONEDATE:
		return ID_SORT_BYDONEDATE;
	case TDC_SORTBYDONE:
		return ID_SORT_BYDONE;
	case TDC_SORTBYPRIORITY:
		return ID_SORT_BYPRIORITY;
	case TDC_SORTBYFLAG:
		return ID_SORT_BYFLAG;
	case TDC_SORTBYCREATEDBY:
		return ID_SORT_BYCREATEDBY;
	case TDC_SORTBYCREATIONDATE:
		return ID_SORT_BYCREATIONDATE;
	case TDC_SORTBYLASTMOD:
		return ID_SORT_BYMODIFYDATE;
	case TDC_SORTBYRISK:
		return ID_SORT_BYRISK;
	case TDC_SORTBYEXTERNALID:
		return ID_SORT_BYEXTERNALID;
	case TDC_SORTBYCOST:
		return ID_SORT_BYCOST;
	case TDC_SORTBYVERSION:
		return ID_SORT_BYVERSION;
	case TDC_SORTBYRECURRENCE:
		return ID_SORT_BYRECURRENCE;
	case TDC_SORTBYREMAINING:
		return ID_SORT_BYREMAINING;
	case TDC_SORTBYRECENTEDIT:
		return ID_SORT_BYRECENTEDIT;
	case TDC_UNSORTED:
		return ID_SORT_NONE;
	case TDC_SORTBYICON:
		return ID_SORT_BYICON;
	case TDC_SORTBYFILEREF:
		return ID_SORT_BYFILEREF;
	case TDC_SORTBYTIMETRACKING:
		return ID_SORT_BYTIMETRACKING;
	}

	ASSERT(0);
	return 0;
}

void CToDoListWnd::OnNewtaskAttopSelected()
{
	VERIFY(NewTask(CEnString(IDS_TASK), TDC_INSERTATTOPOFSELTASKPARENT));
}

void CToDoListWnd::OnNewtaskAtbottomSelected()
{
	VERIFY(NewTask(CEnString(IDS_TASK), TDC_INSERTATBOTTOMOFSELTASKPARENT));
}

void CToDoListWnd::OnNewtaskAfterselectedtask()
{
	VERIFY(NewTask(CEnString(IDS_TASK), TDC_INSERTAFTERSELTASK));
}

void CToDoListWnd::OnNewtaskBeforeselectedtask()
{
	VERIFY(NewTask(CEnString(IDS_TASK), TDC_INSERTBEFORESELTASK));
}

void CToDoListWnd::OnNewsubtaskAtbottom()
{
	VERIFY(NewTask(CEnString(IDS_TASK), TDC_INSERTATBOTTOMOFSELTASK));
}

void CToDoListWnd::OnNewsubtaskAttop()
{
	VERIFY(NewTask(CEnString(IDS_TASK), TDC_INSERTATTOPOFSELTASK));
}

BOOL CToDoListWnd::NewTask(LPCTSTR szTitle, TDC_INSERTWHERE nInsertWhere, BOOL bEdit)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	if (tdc.IsReadOnly())
	{
		return FALSE;
	}

	// handle special case when tasklist is empty
	if (!tdc.GetTaskCount())
	{
		nInsertWhere = TDC_INSERTATBOTTOM;
	}

	if (!tdc.NewTask(szTitle, nInsertWhere, bEdit))
	{
		return FALSE;
	}

	return TRUE;
}

void CToDoListWnd::OnUpdateSort(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	// note: we're still allowed to sort when readonly but the result is not saved
	pCmdUI->Enable(tdc.IsSortable() && tdc.GetTaskCount());
}

void CToDoListWnd::OnEditTaskcolor()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	if (!tdc.IsReadOnly() && tdc.HasSelection())
	{
		CEnColorDialog dialog(tdc.GetSelectedTaskColor(), CC_FULLOPEN | CC_RGBINIT);

		if (dialog.DoModal() == IDOK)
		{
			tdc.SetSelectedTaskColor(dialog.GetColor());
		}
	}
}

void CToDoListWnd::OnEditCleartaskcolor()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	if (!tdc.IsReadOnly() && tdc.HasSelection())
	{
		tdc.ClearSelectedTaskColor();
	}
}

void CToDoListWnd::OnUpdateEditCleartaskcolor(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	pCmdUI->Enable(!tdc.IsReadOnly() && tdc.HasSelection() &&
		Prefs().GetTextColorOption() == COLOROPT_DEFAULT &&
		tdc.GetSelectedTaskColor() != 0);
}

void CToDoListWnd::OnEditTaskdone()
{
	GetToDoCtrl().SetSelectedTaskDone(!GetToDoCtrl().IsSelectedTaskDone());
}

void CToDoListWnd::OnEditTasktext()
{
	GetToDoCtrl().EditSelectedTask();
}

void CToDoListWnd::OnTrayIconClick(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	Show(Prefs().GetToggleTrayVisibility());
	*pResult = 0;
}

LRESULT CToDoListWnd::OnToDoListShowWindow(WPARAM /*wp*/, LPARAM /*lp*/)
{
	Show(FALSE);
	return 0;
}

LRESULT CToDoListWnd::OnToDoListGetVersion(WPARAM /*wp*/, LPARAM /*lp*/)
{
	return GetVersion();
}

LRESULT CToDoListWnd::OnToDoListRefreshPrefs(WPARAM /*wp*/, LPARAM /*lp*/)
{
	// sent by the app object if registry settings have changed
	ResetPrefs();

	// mark all tasklists as needing update
	m_mgrToDoCtrls.SetAllNeedPreferenceUpdate(TRUE);

	// then update active tasklist
	UpdateToDoCtrlPreferences();

	return 0;
}

void CToDoListWnd::OnTrayIconDblClk(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	Show(FALSE);

	*pResult = 0;
}

void CToDoListWnd::OnTrayiconCreatetask()
{
	Show(FALSE);

	// create a task at the top of the tree
	GetToDoCtrl().NewTask(CEnString(IDS_TASK), TDC_INSERTATTOP);
}

void CToDoListWnd::OnTrayIconRClick(NMHDR* pNMHDR, LRESULT* pResult)
{
	SetForegroundWindow();

	// show context menu
	CMenu menu;

	if (menu.LoadMenu(IDR_MISC))
	{
		CMenu* pSubMenu = menu.GetSubMenu(TRAYICON);
		pSubMenu->SetDefaultItem(ID_TRAYICON_SHOW);

		if (pSubMenu)
		{
			m_mgrToDoCtrls.PreparePopupMenu(*pSubMenu, ID_TRAYICON_SHOWDUETASKS1);

			NM_TRAYICON* pNMTI = (NM_TRAYICON*)pNMHDR;

			// in order to ensure that multiple password dialogs cannot
			// appear we must make sure that all the command handling is
			// done before we return from here
			UINT nCmdID = ::TrackPopupMenu(*pSubMenu, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_LEFTBUTTON, pNMTI->ptAction.x,
				pNMTI->ptAction.y, 0, *this, NULL);

			PostMessage(WM_NULL);

			if (nCmdID != (UINT)-1)
			{
				SendMessage(WM_COMMAND, nCmdID);
			}
		}
	}

	*pResult = 0;
}

void CToDoListWnd::OnClose()
{
	int nSysTrayOption = Prefs().GetSysTrayOption();

	if (nSysTrayOption == STO_ONCLOSE || nSysTrayOption == STO_ONMINCLOSE)
	{
		MinimizeToTray();
	}

	else // shutdown but user can cancel
	{
		DoExit();
	}
}

void CToDoListWnd::MinimizeToTray()
{
	// end whatever the user is doing
	GetToDoCtrl().Flush();

	// save prev state so we can restore properly
	AfxGetApp()->WriteProfileInt(_T("Pos"), _T("Maximized"), IsZoomed());

	if (Prefs().GetAutoSaveOnSwitchApp())
	{
		// save all
		SaveAll(TDLS_FLUSH | TDLS_AUTOSAVE);
	}

	// hide extension windows
	m_mgrUIExtensions.ShowAllExtensionsWindows((DWORD)GetToDoCtrl().GetSafeHwnd(), UIS_HIDE);

	// hide main window and release resources
	GUI::MinToTray(*this); // courtesy of floyd

	SetProcessWorkingSetSize(GetCurrentProcess(), (DWORD)-1, (DWORD)-1);
	m_bVisible = FALSE;

	// hide find dialog
	ShowFindDialog(FALSE);
}

void CToDoListWnd::ShowFindDialog(BOOL bShow)
{
	if (bShow)
	{
		if (m_bVisible && m_findDlg.GetSafeHwnd() && IsWindowVisible())
		{
			m_findDlg.Show(TRUE);
		}
	}
	else // hide
	{
		if (m_findDlg.GetSafeHwnd())
		{
			m_bFindShowing = m_findDlg.IsWindowVisible();
			m_findDlg.Show(FALSE);
		}
		else
		{
			m_bFindShowing = FALSE;
		}
	}
}

void CToDoListWnd::OnTrayiconClose()
{
	DoExit();
}

LRESULT CToDoListWnd::OnToDoCtrlNotifyMinWidthChange(WPARAM /*wp*/, LPARAM /*lp*/)
{
	CheckMinWidth();

	return 0;
}

LRESULT CToDoListWnd::OnToDoCtrlNotifyListChange(WPARAM /*wp*/, LPARAM lp)
{
	// decide whether the filter controls need updating
	switch (lp)
	{
	case TDCA_ALLOCTO:
	case TDCA_ALLOCBY:
	case TDCA_STATUS:
	case TDCA_CATEGORY:
	case TDCA_VERSION:
		RefreshFilterControls();
		break;
	}

	return 0L;
}

LRESULT CToDoListWnd::OnToDoCtrlNotifyViewChange(WPARAM /*wp*/, LPARAM /*lp*/)
{
	m_filterBar.RefreshFilterControls(GetToDoCtrl());
	return 0L;
}

LRESULT CToDoListWnd::OnToDoCtrlNotifyTimeTrack(WPARAM /*wp*/, LPARAM lp)
{
	BOOL bTrack = lp;

	if (bTrack && Prefs().GetExclusiveTimeTracking())
	{
		// end time tracking on every other tasklist
		int nSel = GetSelToDoCtrl();
		ASSERT(nSel != -1);

		for (int nCtrl = 0; nCtrl < GetTDCCount(); nCtrl++)
		{
			if (nCtrl != nSel)
			{
				GetToDoCtrl(nCtrl).EndTimeTracking();
			}
		}
	}

	return 0L;
}

LRESULT CToDoListWnd::OnToDoCtrlNotifyRecreateRecurringTask(WPARAM wp, LPARAM lp)
{
	DWORD dwTaskID = wp, dwNewTaskID = lp;

	// is there a reminder that we need to copy for the new task
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	TDCREMINDER rem;

	int nRem = m_reminders.FindReminder(dwTaskID, &tdc);

	if (nRem != -1)
	{
		// get the existing reminder
		m_reminders.GetReminder(nRem, rem);

		// init for new task
		rem.bEnabled = TRUE;
		rem.dDaysSnooze = 0.0;
		rem.dwTaskID = dwNewTaskID;

		// add for the new task ID
		m_reminders.SetReminder(rem);

		// delete the original
		m_reminders.RemoveReminder(dwTaskID, rem.pTDC);
	}

	return 0L;
}

LRESULT CToDoListWnd::OnToDoCtrlNotifyMod(WPARAM /*wp*/, LPARAM lp)
{
	int nSel = GetSelToDoCtrl();

	if (nSel == -1)
	{
		return 0L;
	}

	BOOL bWasMod = m_mgrToDoCtrls.GetModifiedStatus(nSel);
	m_mgrToDoCtrls.SetModifiedStatus(nSel, TRUE);

	// update the caption only if the control is not currently modified
	// or the project name changed
	if (!bWasMod || lp == TDCA_PROJNAME)
	{
		UpdateCaption();
	}

	// update due items
	if (lp == TDCA_DONEDATE || lp == TDCA_DUEDATE)
	{
		OnTimerDueItems(nSel);
	}

	// update tab order
	if (lp == TDCA_PROJNAME && Prefs().GetKeepTabsOrdered())
	{
		RefreshTabOrder();
	}

	// status bar
	UpdateStatusbar();

	PostMessage(WM_IDLEUPDATECMDUI, TRUE);

	// do we need to update the ui plugins
	switch (lp)
	{
	case TDCA_TASKNAME:
	case TDCA_DONEDATE:
	case TDCA_DUEDATE:
	case TDCA_STARTDATE:
	case TDCA_PRIORITY:
	case TDCA_COLOR:
	case TDCA_ALLOCTO:
	case TDCA_ALLOCBY:
	case TDCA_STATUS:
	case TDCA_CATEGORY:
	case TDCA_PERCENT:
	case TDCA_TIMEEST:
	case TDCA_TIMESPENT:
	case TDCA_FILEREF:
	case TDCA_COMMENTS:
	case TDCA_FLAG:
	case TDCA_CREATIONDATE:
	case TDCA_CREATEDBY:
	case TDCA_RISK:
	case TDCA_EXTERNALID:
	case TDCA_COST:
	case TDCA_DEPENDENCY:
	case TDCA_RECURRENCE:
	case TDCA_VERSION:
		RefreshUIExtensions(TRUE); // simple edit == selected items only
		break;

	case TDCA_NEWTASK:
	case TDCA_DELETE:
	case TDCA_UNDO:
		RefreshUIExtensions(FALSE); // all items
		break;

	case TDCA_PROJNAME:
	case TDCA_ENCRYPT:
	case TDCA_ARCHIVE:
	case TDCA_SORT:
	case TDCA_MOVE:
	default:
		// do nothing
		break;
	}

	return 0L;
}

void CToDoListWnd::RefreshUIExtensions(BOOL bEdit)
{
	// any visible extension windows
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	DWORD dwToDoCtrl = (DWORD)tdc.GetSafeHwnd();

	if (m_mgrUIExtensions.HasAnyExtensionWindows(dwToDoCtrl, TRUE))
	{
		CTaskFile tasks;
		DWORD dwUpdateFlags = 0;

		if (bEdit)
		{
			// for a basic edit we just update the modified tasks
			GetTasks(tdc, TSDT_SELECTED, tasks);
			dwUpdateFlags = UIU_EDIT;
		}
		else
		{
			// update filtered tasks (for now)
			GetTasks(tdc, TSDT_FILTERED, tasks);
			dwUpdateFlags = UIU_ALL;
		}

		// add project name
		tasks.SetProjectName(ATL::CT2A(m_mgrToDoCtrls.GetFriendlyProjectName(GetSelToDoCtrl())));

		m_mgrUIExtensions.UpdateAllExtensionsWindow(dwToDoCtrl, &tasks, dwUpdateFlags);
	}
}

void CToDoListWnd::UpdateCaption()
{
	int nSel = GetSelToDoCtrl();

	CString sProjectName = m_mgrToDoCtrls.UpdateTabItemText(nSel);
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	UINT nIDStatus = 0;

	if (m_mgrToDoCtrls.GetReadOnlyStatus(nSel) > 0)
	{
		nIDStatus = IDS_STATUSREADONLY;
	}

	else if (tdc.CompareFileFormat() == TDCFF_NEWER)
	{
		nIDStatus = IDS_STATUSNEWERFORMAT;
	}

	else if (m_mgrToDoCtrls.GetReadOnlyStatus(nSel) == 0 &&
		m_mgrToDoCtrls.PathSupportsSourceControl(nSel))
	{
		if (tdc.IsCheckedOut())
		{
			nIDStatus = IDS_STATUSCHECKEDOUT;
		}
		else
		{
			nIDStatus = IDS_STATUSCHECKEDIN;
		}
	}
	else if (!Prefs().GetEnableSourceControl() && m_mgrToDoCtrls.IsSourceControlled(nSel))
	{
		nIDStatus = IDS_STATUSSOURCECONTROLLED;
	}

	CString sCaption;
	CEnString sCopyright(IDS_COPYRIGHT), sStatus(nIDStatus);

	// add current focus text as required
	if (nIDStatus)
	{
		if (m_bShowStatusBar || m_sCurrentFocus.IsEmpty())
		{
			sCaption.Format(_T("%s [%s] - %s"), (LPCTSTR)sProjectName, (LPCTSTR)sStatus, (LPCTSTR)sCopyright);
		}
		else
		{
			sCaption.Format(_T("%s [%s][%s] - %s"), (LPCTSTR)sProjectName, (LPCTSTR)m_sCurrentFocus, (LPCTSTR)sStatus,
				(LPCTSTR)sCopyright);
		}
	}
	else
	{
		if (m_bShowStatusBar || m_sCurrentFocus.IsEmpty())
		{
			sCaption.Format(_T("%s - %s"), (LPCTSTR)sProjectName, (LPCTSTR)sCopyright);
		}
		else
		{
			sCaption.Format(_T("%s [%s] - %s"), (LPCTSTR)sProjectName, (LPCTSTR)m_sCurrentFocus, (LPCTSTR)sCopyright);
		}
	}

	SetWindowText(sCaption);

	// set tray tip too
	UpdateTooltip();
}

void CToDoListWnd::UpdateTooltip()
{
	// base the tooltip on our current caption
	CString sTooltip;
	GetWindowText(sTooltip);

	// move copyright to next line and remove '-'
	sTooltip.Replace(_T(" - "), _T("\n"));

	// prefix with selected task as first line
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	DWORD dwSelID = tdc.GetSelectedTaskID();

	if (dwSelID)
	{
		CString sSelItem = tdc.GetTaskTitle(dwSelID);

		// maximum length of tooltip is 128 including null
		if (sSelItem.GetLength() > (128 - sTooltip.GetLength() - 6))
		{
			sSelItem = sSelItem.Left(128 - sTooltip.GetLength() - 6);
			sSelItem += _T("...");
		}
		else if (tdc.GetSelectedCount() > 1)
		{
			sSelItem += _T(", ...");
		}

		sTooltip = sSelItem + _T("\n") + sTooltip;
	}

	m_ti.SetTip(sTooltip);
}

void CToDoListWnd::OnUpdateNewsubtaskAtBottom(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	pCmdUI->Enable(!tdc.IsReadOnly() && nSelCount == 1);
}

BOOL CToDoListWnd::Export2Html(CFilteredToDoCtrl& tdc, LPCTSTR szFilePath, TSD_TASKS nWhatTasks,
	LPCTSTR szStylesheet) const
{
	CWaitCursor cursor;
	tdc.Flush();

	BOOL bTransform = FileMisc::FileExists(szStylesheet);

	CTaskFile tasks;
	TDCGETTASKS filter;

	if (Prefs().GetExportAllAttributes())
	{
		GetTasks(tdc, TRUE, bTransform, nWhatTasks, filter, tasks);
	}

	return Export2Html(tasks, szFilePath, szStylesheet);
}

BOOL CToDoListWnd::Export2Html(const CTaskFile& tasks, LPCTSTR szFilePath, LPCTSTR szStylesheet) const
{
	CWaitCursor cursor;

	if (FileMisc::FileExists(szStylesheet))
	{
		return tasks.TransformToFile(szStylesheet, szFilePath, Prefs().GetHtmlCharSet());
	}

	// else default export
	return m_mgrImportExport.ExportTaskListToHtml(&tasks, szFilePath);
}

void CToDoListWnd::OnSaveas()
{
	int nSel = GetSelToDoCtrl();

	// use tab text as hint to user
	CString sFilePath = m_mgrToDoCtrls.GetFilePath(nSel, FALSE);

	// display the dialog
	CFileDialog dialog(FALSE, GetDefaultFileExt(), sFilePath,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, GetFileFilter(), this);

	CEnString sTitle(IDS_SAVETASKLISTAS_TITLE);

	dialog.m_ofn.lpstrTitle = (LPCTSTR)sTitle;

	int nRes = dialog.DoModal();

	if (nRes == IDOK)
	{
		VERIFY(SaveTaskList(nSel, dialog.GetPathName()) == TDCO_SUCCESS);

		if (SaveTaskList(nSel, dialog.GetPathName()) == TDCO_SUCCESS)
		{
			m_mgrToDoCtrls.ClearWebDetails(nSel);
		}
	}
}

void CToDoListWnd::OnUpdateSaveas(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	pCmdUI->Enable(tdc.GetTaskCount() || tdc.IsModified());
}

void CToDoListWnd::OnContextMenu(CWnd* pWnd, CPoint point)
{
	static UINT nActiveMenuID = 0; // prevent reentrancy
	UINT nMenuID = 0;

	if (pWnd == &m_tabCtrl)
	{
		// if point.x,y are both -1 then we just use the cursor pos
		// which is what windows appears to do mostly/sometimes
		if (point.x == -1 && point.y == -1)
		{
			CRect rTab;

			if (m_tabCtrl.GetItemRect(m_tabCtrl.GetCurSel(), rTab))
			{
				point = rTab.CenterPoint();
				m_tabCtrl.ClientToScreen(&point);

				// load popup menu
				nMenuID = TABCTRLCONTEXT;
			}
		}
		else
		{
			// activate clicked tab
			TCHITTESTINFO tcht = { { point.x, point.y }, TCHT_NOWHERE  };
			m_tabCtrl.ScreenToClient(&tcht.pt);

			int nTab = m_tabCtrl.HitTest(&tcht);

			if (nTab != -1 && !(tcht.flags & TCHT_NOWHERE))
			{
				if (nTab != m_tabCtrl.GetCurSel())
				{
					if (!SelectToDoCtrl(nTab, TRUE))
					{
						return;   // user cancelled
					}
				}

				m_tabCtrl.SetFocus(); // give user feedback

				// load popup menu
				nMenuID = TABCTRLCONTEXT;
			}
		}
	}
	else if (pWnd == &GetToDoCtrl()) // try active todoctrl
	{
		if (GetToDoCtrl().PtInHeader(point))
		{
			nMenuID = HEADERCONTEXT;
		}
		else
		{
			nMenuID = TASKCONTEXT;
		}
	}

	// show the menu
	if (nMenuID && nMenuID != nActiveMenuID)
	{
		CMenu menu;

		if (menu.LoadMenu(IDR_MISC))
		{
			CMenu* pPopup = menu.GetSubMenu(nMenuID);

			if (pPopup)
			{
				// some special handling
				switch (nMenuID)
				{
				case TASKCONTEXT:
					PrepareEditMenu(pPopup);
					break;

				case TABCTRLCONTEXT:
					PrepareSourceControlMenu(pPopup);
					break;
				}

				CToolbarHelper::PrepareMenuItems(pPopup, this);

				nActiveMenuID = nMenuID;
				pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
				nActiveMenuID = 0;
			}
		}
	}
	else
	{
		CFrameWnd::OnContextMenu(pWnd, point);
	}
}

void CToDoListWnd::OnTrayiconShow()
{
	Show(FALSE);
}

void CToDoListWnd::OnTrayiconShowDueTasks(UINT nCmdID)
{
	int nTDC = nCmdID - ID_TRAYICON_SHOWDUETASKS1;
	int nSelTDC = GetSelToDoCtrl();

	// verify password if encrypted tasklist is active
	// unless app is already visible
	if (!m_bVisible || IsIconic() || (nTDC != nSelTDC))
	{
		if (!VerifyToDoCtrlPassword(nTDC))
		{
			return;
		}
	}

	CFilteredToDoCtrl& tdc = GetToDoCtrl(nTDC);

	if (!DoDueTaskNotification(&tdc, PFP_DUETODAY))
	{
		CEnString sMessage(IDS_NODUETODAY, tdc.GetFriendlyProjectName());
		MessageBox(sMessage, IDS_DUETASKS_TITLE);
	}
}

LRESULT CToDoListWnd::OnHotkey(WPARAM /*wp*/, LPARAM /*lp*/)
{
	Show(TRUE);
	return 0L;
}

BOOL CToDoListWnd::VerifyToDoCtrlPassword() const
{
	return VerifyToDoCtrlPassword(GetSelToDoCtrl());
}

BOOL CToDoListWnd::VerifyToDoCtrlPassword(int nIndex) const
{
	if (m_bPasswordPrompting)
	{
		CEnString sExplanation(IDS_SELECTENCRYPTED,
			m_mgrToDoCtrls.GetFriendlyProjectName(nIndex));

		return GetToDoCtrl(nIndex).VerifyPassword(sExplanation);
	}

	// else
	return TRUE;
}

void CToDoListWnd::Show(BOOL bAllowToggle)
{
	if (GetSelToDoCtrl() == -1)
	{
		return;
	}

	if (!m_bVisible || !IsWindowVisible()) // restore from the tray
	{
		SetForegroundWindow();

		if (!VerifyToDoCtrlPassword())
		{
			return;
		}

		m_bVisible = TRUE;
		SetProcessWorkingSetSize(GetCurrentProcess(), 0, 0);
		GUI::RestoreFromTray(*this, AfxGetApp()->GetProfileInt(_T("Pos"), _T("Maximized"), FALSE));

		// restore extension windows
		m_mgrUIExtensions.ShowAllExtensionsWindows((DWORD)GetToDoCtrl().GetSafeHwnd(), UIS_RESTORE);

		// restore find dialog
		if (m_bFindShowing)
		{
			m_findDlg.Show();
		}
	}
	else if (IsIconic())
	{
		SetForegroundWindow();
		ShowWindow(SW_RESTORE); // this will force a password check
	}
	// if we're already visible then either bring to the foreground
	// or hide if we're right at the top of the z-order
	else if (!bAllowToggle || GUI::IsObscured(*this) || !GUI::HasFocus(*this, TRUE))
	{
		SetForegroundWindow();
	}

	else if (Prefs().GetSysTrayOption() == STO_NONE)
	{
		ShowWindow(SW_MINIMIZE);
	}

	else // hide to system tray
	{
		MinimizeToTray();
	}

	// refresh all tasklists if we are visible
	if (m_bVisible && !IsIconic())
	{
		const CPreferencesDlg& userPrefs = Prefs();

		if (userPrefs.GetReadonlyReloadOption() != RO_NO)
		{
			OnTimerReadOnlyStatus();
		}

		if (userPrefs.GetTimestampReloadOption() != RO_NO)
		{
			OnTimerTimestampChange();
		}

		if (userPrefs.GetEnableSourceControl())
		{
			OnTimerCheckoutStatus();
		}
	}

	GetToDoCtrl().SetFocusToTasks();
}

BOOL CToDoListWnd::OnQueryEndSession()
{
	if (!CFrameWnd::OnQueryEndSession())
	{
		return FALSE;
	}

	// if we can't save some of the user's data (as opposed to their decision
	// not to save it) then we *must* prevent Windows shutting down
	TDC_FILE nSaveAll = SaveAll(TDLS_INCLUDEUNSAVED | TDLS_CLOSINGTASKLISTS |
		TDLS_CLOSINGWINDOWS | TDLS_FLUSH);

	ASSERT(nSaveAll != TDCO_CANCELLED);  // shouldn't be possible

	if (nSaveAll != TDCO_SUCCESS)
	{
		return FALSE;
	}

	SaveSettings();

	// simple cleanup so that encrypted tasklists do not become visible
	// inadvertently
	m_bClosing = TRUE;

	int nCtrl = GetTDCCount();

	while (nCtrl--)
	{
		CToDoCtrl* pTDC = &GetToDoCtrl(nCtrl);
		pTDC->DestroyWindow();
		delete pTDC;
	}

	DestroyWindow(); // because otherwise we would get left in an undefined state

	return TRUE;
}

void CToDoListWnd::OnUpdateRecentFileMenu(CCmdUI* pCmdUI)
{
	// check that this is not occurring because our CFrameWnd
	// base class is routing this to the first item in a submenu
	if (pCmdUI->m_pMenu &&
		pCmdUI->m_pMenu->GetMenuItemID(pCmdUI->m_nIndex) == (UINT)-1)
	{
		return;
	}

	m_mruList.UpdateMenu(pCmdUI);
}

BOOL CToDoListWnd::OnOpenRecentFile(UINT nID)
{
	ASSERT(nID >= ID_FILE_MRU_FILE1);
	ASSERT(nID < ID_FILE_MRU_FILE1 + (UINT)m_mruList.GetSize());

	int nIndex = nID - ID_FILE_MRU_FILE1;

	CString sTaskList = m_mruList[nIndex];
	PrepareOpenFilePath(sTaskList);

	TDC_FILE nOpen = OpenTaskList(sTaskList);

	if (nOpen == TDCO_SUCCESS)
	{
		ResizeDlg();
		UpdateWindow();
	}
	else
	{
		HandleLoadTasklistError(nOpen, sTaskList);

		if (nOpen != TDCO_CANCELLED)
		{
			m_mruList.Remove(nIndex);
		}
	}

	RefreshTabOrder();

	// always return TRUE to say we handled it
	return TRUE;
}

void CToDoListWnd::RefreshTabOrder()
{
	if (Prefs().GetKeepTabsOrdered())
	{
		int nSelOrg = GetSelToDoCtrl();
		int nSel = m_mgrToDoCtrls.SortToDoCtrlsByName();

		if (nSel != nSelOrg)
		{
			SelectToDoCtrl(nSel, FALSE);
		}
	}
}

TDC_FILE CToDoListWnd::DelayOpenTaskList(LPCTSTR szFilePath)
{
	ASSERT(Prefs().GetEnableDelayedLoading());  // sanity check

	// see if the tasklist is already open
	if (SelectToDoCtrl(szFilePath, TRUE))
	{
		return TDCO_SUCCESS;
	}

	CFilteredToDoCtrl* pCtrl = NewToDoCtrl();
	COleDateTime dtEarliest;

	BOOL bDelayLoad = pCtrl->DelayLoad(szFilePath, dtEarliest);

	if (bDelayLoad)
	{
		// now we have to check for whether the tasklist has due tasks
		// and the user wants notification
		int nNotifyDueBy = Prefs().GetNotifyDueByOnLoad();

		if (nNotifyDueBy != PFP_DONTNOTIFY && dtEarliest.m_dt != 0.0)
		{
			// check the date against when the user wants notifying
			DH_DATE nDate = DHD_TODAY;

			switch (nNotifyDueBy)
			{
			case PFP_DUETODAY:		/*nDate = DHD_TODAY;*/
				break;
			case PFP_DUETOMORROW:
				nDate = DHD_TOMORROW;
				break;
			case PFP_DUETHISWEEK:
				nDate = DHD_ENDTHISWEEK;
				break;
			case PFP_DUENEXTWEEK:
				nDate = DHD_ENDNEXTWEEK;
				break;
			case PFP_DUETHISMONTH:
				nDate = DHD_ENDTHISMONTH;
				break;
			case PFP_DUENEXTMONTH:
				nDate = DHD_ENDNEXTMONTH;
				break;
			default:
				ASSERT(0);
			}

			COleDateTime dtDueWhen = CDateHelper::GetDate(nDate);

			bDelayLoad = (dtDueWhen < dtEarliest);
		}
	}

	// if the delay load failed for any reason we need to delete the tasklist
	// and fallback on the default load mechanism
	if (!bDelayLoad)
	{
		pCtrl->DestroyWindow();
		delete pCtrl;

		return OpenTaskList(szFilePath, NULL, FALSE);
	}

	int nCtrl = m_mgrToDoCtrls.AddToDoCtrl(pCtrl, FALSE); // FALSE == not yet loaded

	// update due item status
	if (dtEarliest.m_dt != 0.0)
	{
		TDCM_DUESTATUS nStatus = TDCM_FUTURE;
		COleDateTime dtToday = COleDateTime::GetCurrentTime();

		if (floor(dtEarliest) < floor(dtToday))
		{
			nStatus = TDCM_PAST;
		}

		else if (floor(dtEarliest) == floor(dtToday))
		{
			nStatus = TDCM_TODAY;
		}

		m_mgrToDoCtrls.SetDueItemStatus(nCtrl, nStatus);
	}

	UpdateCaption();
	UpdateStatusbar();

	// notify user if file is readonly for any reason except checked in
	if (Prefs().GetNotifyReadOnly())
	{
		CString sMessage;

		if (CDriveInfo::IsReadonlyPath(szFilePath) > 0)
		{
			sMessage.Format(IDS_OPENREADONLY, szFilePath);
			MessageBox(sMessage, IDS_OPENTASKLIST_TITLE);
		}
	}

	return TDCO_SUCCESS;
}

TDC_ARCHIVE CToDoListWnd::GetAutoArchiveOptions(LPCTSTR szFilePath, CString& sArchivePath, BOOL& bRemoveFlagged) const
{
	TDC_ARCHIVE nRemove = TDC_REMOVENONE;
	bRemoveFlagged = FALSE;

	CPreferencesDlg& userPrefs = Prefs();

	if (userPrefs.GetAutoArchive())
	{
		if (userPrefs.GetRemoveArchivedTasks())
		{
			if (userPrefs.GetRemoveOnlyOnAbsoluteCompletion())
			{
				nRemove = TDC_REMOVEIFSIBLINGSANDSUBTASKSCOMPLETE;
			}
			else
			{
				nRemove = TDC_REMOVEALL;
			}

			bRemoveFlagged = !userPrefs.GetDontRemoveFlagged();
		}

		sArchivePath = m_mgrToDoCtrls.GetArchivePath(szFilePath);
	}
	else
	{
		sArchivePath.Empty();
	}

	return nRemove;
}

TDC_FILE CToDoListWnd::OpenTaskList(LPCTSTR szFilePath, LPCTSTR szDisplayPath, BOOL bNotifyDueTasks)
{
	// see if the tasklist is already open
	if (SelectToDoCtrl(szFilePath, TRUE))
	{
		return TDCO_SUCCESS;
	}

	CPreferencesDlg& userPrefs = Prefs();
	CFilteredToDoCtrl* pCtrl = NewToDoCtrl();

	TDC_FILE nOpen = OpenTaskList(pCtrl, szFilePath);

	if (nOpen == TDCO_SUCCESS)
	{
		int nIndex = AddToDoCtrl(pCtrl);

		// reload any reminders
		m_reminders.AddToDoCtrl(*pCtrl);

		// notify user of due tasks if req
		if (bNotifyDueTasks)
		{
			DoDueTaskNotification(pCtrl, userPrefs.GetNotifyDueByOnLoad());
		}

		// save checkout status
		if (userPrefs.GetAutoCheckOut())
		{
			m_mgrToDoCtrls.SetLastCheckoutStatus(nIndex, pCtrl->IsCheckedOut());
		}

		UpdateCaption();
		UpdateStatusbar();
		OnTimerDueItems(nIndex);

		// notify user if file is readonly for any reason except checked in
		if (userPrefs.GetNotifyReadOnly())
		{
			CString sMessage;

			if (!szDisplayPath)
			{
				szDisplayPath = szFilePath;
			}

			if (CDriveInfo::IsReadonlyPath(szFilePath) > 0)
			{
				sMessage.Format(IDS_OPENREADONLY, szDisplayPath);
			}
			else if (!userPrefs.GetEnableSourceControl() && m_mgrToDoCtrls.IsSourceControlled(nIndex))
			{
				sMessage.Format(IDS_OPENSOURCECONTROLLED, szDisplayPath);
			}
			else if (pCtrl->CompareFileFormat() == TDCFF_NEWER)
			{
				sMessage.Format(IDS_OPENNEWER, szDisplayPath);
			}

			if (!sMessage.IsEmpty())
			{
				MessageBox(sMessage, IDS_OPENTASKLIST_TITLE);
			}
		}

		// update search
		if (userPrefs.GetRefreshFindOnLoad() && m_findDlg.GetSafeHwnd())
		{
			m_findDlg.RefreshSearch();
		}
	}
	else if (GetTDCCount() >= 1) // only delete if there's another ctrl existing
	{
		pCtrl->DestroyWindow();
		delete pCtrl;
	}
	else // re-add
	{
		AddToDoCtrl(pCtrl);
	}

	return nOpen;
}

TDC_FILE CToDoListWnd::OpenTaskList(CFilteredToDoCtrl* pCtrl, LPCTSTR szFilePath)
{
	CPreferencesDlg& userPrefs = Prefs();
	CWaitCursor cursor;
	CString sFilePath(szFilePath);

	if (sFilePath.IsEmpty())
	{
		sFilePath = (LPCTSTR)pCtrl->GetFilePath();   // ie. reload
	}

	else // initialize source control prior to loading
	{
		BOOL bSrcControl = m_mgrToDoCtrls.PathSupportsSourceControl(szFilePath);

		pCtrl->SetStyle(TDCS_ENABLESOURCECONTROL, bSrcControl);
		pCtrl->SetStyle(TDCS_CHECKOUTONLOAD, bSrcControl ? userPrefs.GetAutoCheckOut() : FALSE);
	}

	CTaskFile tasks;
	TDC_FILE nOpen = pCtrl->Load(sFilePath, tasks);

	if (nOpen == TDCO_SUCCESS)
	{
		// archive completed tasks?
		CString sArchivePath;
		BOOL bRemoveFlagged;
		TDC_ARCHIVE nRemove = GetAutoArchiveOptions(szFilePath, sArchivePath, bRemoveFlagged);

		if (!sArchivePath.IsEmpty())
		{
			pCtrl->ArchiveDoneTasks(sArchivePath, nRemove, bRemoveFlagged);
		}

		if (userPrefs.GetAddFilesToMRU())
		{
			m_mruList.Add(sFilePath);
		}

		if (userPrefs.GetExpandTasksOnLoad())
		{
			pCtrl->ExpandAllTasks();
		}

		// update readonly status
		m_mgrToDoCtrls.UpdateToDoCtrlReadOnlyUIState(*pCtrl);

		// update any UI extension windows
		m_mgrUIExtensions.UpdateAllExtensionsWindow((DWORD)pCtrl->GetSafeHwnd(), &tasks, TRUE);
	}
	else
	{
		pCtrl->SetModified(FALSE);
	}

	return nOpen;
}

BOOL CToDoListWnd::DoDueTaskNotification(int nDueBy)
{
	return DoDueTaskNotification(&GetToDoCtrl(), nDueBy);
}

BOOL CToDoListWnd::DoDueTaskNotification(const CFilteredToDoCtrl* pCtrl, int nDueBy)
{
	// check userPrefs
	if (nDueBy == -1)
	{
		return TRUE;   // nothing to do
	}

	ASSERT(pCtrl);

	if (!pCtrl)
	{
		return FALSE;
	}

	int nTDC = m_mgrToDoCtrls.FindToDoCtrl(pCtrl);

	if (nTDC != -1 && !VerifyTaskListOpen(nTDC, FALSE))
	{
		return TRUE;   // no error. user cancelled
	}

	CPreferencesDlg& userPrefs = Prefs();

	// preferences
	BOOL bParentTitleCommentsOnly = userPrefs.GetExportParentTitleCommentsOnly();
	BOOL bDueTaskTitlesOnly = userPrefs.GetDueTaskTitlesOnly();
	CString sStylesheet = userPrefs.GetDueTaskStylesheet();
	BOOL bTransform = FileMisc::FileExists(sStylesheet);
	BOOL bHtmlNotify = userPrefs.GetDisplayDueTasksInHtml();

	DWORD dwFlags = TDCGTF_ISODATES | TDCGTF_FILENAME;

	if (bParentTitleCommentsOnly)
	{
		dwFlags |= TDCGTF_PARENTTITLECOMMENTSONLY;
	}

	if (bHtmlNotify)
	{
		dwFlags |= TDCGTF_HTMLCOMMENTS;
	}

	if (bDueTaskTitlesOnly)
	{
		dwFlags |= TDCGTF_TITLESONLY;
	}

	if (bTransform)
	{
		dwFlags |= TDCGTF_TRANSFORM;
	}

	TDC_GETTASKS nFilter = TDCGT_DUE;
	UINT nIDDueBy = IDS_DUETODAY;

	switch (nDueBy)
	{
	case PFP_DUETODAY:
		break; // done

	case PFP_DUETOMORROW:
		nIDDueBy = IDS_DUETOMORROW;
		nFilter = TDCGT_DUETOMORROW;
		break;

	case PFP_DUETHISWEEK:
		nIDDueBy = IDS_DUETHISWEEK;
		nFilter = TDCGT_DUETHISWEEK;
		break;

	case PFP_DUENEXTWEEK:
		nIDDueBy = IDS_DUENEXTWEEK;
		nFilter = TDCGT_DUENEXTWEEK;
		break;

	case PFP_DUETHISMONTH:
		nIDDueBy = IDS_DUETHISMONTH;
		nFilter = TDCGT_DUETHISMONTH;
		break;

	case PFP_DUENEXTMONTH:
		nIDDueBy = IDS_DUENEXTMONTH;
		nFilter = TDCGT_DUENEXTMONTH;
		break;

	default:
		ASSERT(0);
		return FALSE;
	}

	TDCGETTASKS filter(nFilter, dwFlags);
	filter.sAllocTo = userPrefs.GetDueTaskPerson();

	CTaskFile tasks;
	if (!pCtrl->GetTasks(tasks, filter))
	{
		return FALSE;
	}

	// set an appropriate title
	tasks.SetReportAttributes(CEnString(nIDDueBy));
	tasks.SetCharSet(userPrefs.GetHtmlCharSet());

	// nasty hack to prevent exporters adding space for notes
	BOOL bSpaceForNotes = userPrefs.GetExportSpaceForNotes();

	if (bSpaceForNotes)
	{
		AfxGetApp()->WriteProfileInt(_T("Preferences"), _T("ExportSpaceForNotes"), FALSE);
	}

	// always use the same file
	CString sTempFile = FileMisc::GetTempFileName(_T("ToDoList.due"), bHtmlNotify ? _T("html") : _T("txt"));

	BOOL bRes = FALSE;

	if (bHtmlNotify) // display in browser?
	{
		if (bTransform)
		{
			bRes = tasks.TransformToFile(sStylesheet, sTempFile, userPrefs.GetHtmlCharSet());
		}
		else
		{
			bRes = m_mgrImportExport.ExportTaskListToHtml(&tasks, sTempFile);
		}
	}
	else
	{
		bRes = m_mgrImportExport.ExportTaskListToText(&tasks, sTempFile);
	}

	if (bRes)
	{
		Show(FALSE);

		CEnString sTitle(IDS_DUETASKS_TITLE, pCtrl->GetFriendlyProjectName());
		m_dlgNotifyDue.DoModal(sTitle, sTempFile, this);
	}

	// undo hack
	if (bSpaceForNotes)
	{
		AfxGetApp()->WriteProfileInt(_T("Preferences"), _T("ExportSpaceForNotes"), TRUE);
	}

	return TRUE;
}

void CToDoListWnd::OnAbout()
{
	CAboutDlg dialog(IDR_MAINFRAME, ABS_EDITCOPYRIGHT, _T("<b>ToDoList 6.2.4</b> (mod by Elijah Zarezky)"),
		CEnString(IDS_ABOUTHEADING), CEnString(IDS_ABOUTCOPYRIGHT), CEnString(IDS_LICENSE), 1, 2, 8);

	dialog.DoModal();
}

void CToDoListWnd::OnPreferences()
{
	DoPreferences();
}

void CToDoListWnd::DoPreferences(int nInitPage)
{
	// take a copy of current userPrefs to check changes against
	CPreferencesDlg curPrefs;

	// kill timers
	SetTimer(TIMER_READONLYSTATUS, FALSE);
	SetTimer(TIMER_TIMESTAMPCHANGE, FALSE);
	SetTimer(TIMER_CHECKOUTSTATUS, FALSE);
	SetTimer(TIMER_AUTOSAVE, FALSE);
	SetTimer(TIMER_TIMETRACKING, FALSE);

	UINT nRet = Prefs().DoModal(nInitPage);

	// updates userPrefs
	ResetPrefs();

	const CPreferencesDlg& userPrefs = Prefs();

	if (nRet == IDOK)
	{
		SetUITheme(userPrefs.GetUITheme());

		// lightbox mgr
		if (curPrefs.GetEnableLightboxMgr() != userPrefs.GetEnableLightboxMgr())
		{
			if (userPrefs.GetEnableLightboxMgr())
			{
				CLightBoxMgr::Initialize(this, m_theme.crAppBackDark);
			}
			else
			{
				CLightBoxMgr::Disable();
				AfxMessageBox(IDS_NEEDRESTARTTODOLISTLIGHTBOXCHANGE);
			}
		}

		// mark all todoctrls as needing refreshing
		m_mgrToDoCtrls.SetAllNeedPreferenceUpdate(TRUE);

		// delete fonts if they appear to have changed
		// and recreate in UpdateToDoCtrlPrefs
		CString sFaceName;
		int nFontSize;

		if (!userPrefs.GetTreeFont(sFaceName, nFontSize) || !GraphicsMisc::SameFont(m_fontTree, sFaceName, nFontSize))
		{
			m_fontTree.DeleteObject();
		}

		if (!userPrefs.GetCommentsFont(sFaceName, nFontSize) || !GraphicsMisc::SameFont(m_fontComments, sFaceName, nFontSize))
		{
			m_fontComments.DeleteObject();
		}

		BOOL bResizeDlg = FALSE;

		// topmost
		BOOL bTopMost = (userPrefs.GetAlwaysOnTop() && !IsZoomed());

		SetWindowPos(bTopMost ? &wndTopMost : &wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);

		// tray icon
		m_ti.ShowTrayIcon(userPrefs.GetUseSysTray());

		// support for .tdl
		if (curPrefs.GetEnableTDLExtension() != userPrefs.GetEnableTDLExtension())
		{
			CFileRegister filereg(_T("tdl"), _T("tdl_TaskList"));

			if (userPrefs.GetEnableTDLExtension())
			{
				filereg.RegisterFileType(_T("Tasklist"), 0);
			}
			else
			{
				filereg.UnRegisterFileType();
			}
		}

		// support for tdl web protocol
		if (curPrefs.GetEnableTDLProtocol() != userPrefs.GetEnableTDLProtocol())
		{
			EnableTDLProtocol(userPrefs.GetEnableTDLProtocol());
		}

		// source control
		BOOL bSourceControl = userPrefs.GetEnableSourceControl();

		if (curPrefs.GetEnableSourceControl() != bSourceControl ||
			curPrefs.GetSourceControlLanOnly() != userPrefs.GetSourceControlLanOnly())
		{
			// update all open files to ensure they're in the right state
			int nCtrl = GetTDCCount();

			while (nCtrl--)
			{
				CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtrl);

				// check files in if we're disabling sc and this file is
				// checked out. however although we
				// are checking in, the file cannot be edited by the user
				// until they remove the file from under source control
				if (!bSourceControl && tdc.IsCheckedOut())
				{
					if (tdc.IsModified())
					{
						tdc.Save(NULL);
					}

					tdc.CheckIn();
				}
				// else checkout if we're enabling and auto-checkout is also enabled
				else if (bSourceControl)
				{
					// there can be two reasons for wanting to check out a file
					// either the autocheckout preference is set or its a local
					// file which is not checked out but has been modified and source
					// control now covers all files in which case we save it first
					BOOL bPathSupports = m_mgrToDoCtrls.PathSupportsSourceControl(nCtrl);
					BOOL bNeedsSave = bPathSupports && !tdc.IsCheckedOut() && tdc.IsModified();
					BOOL bWantCheckOut = bNeedsSave || (bPathSupports && userPrefs.GetAutoCheckOut());

					if (bNeedsSave)
					{
						tdc.Save(NULL);   // save silently
					}

					tdc.SetStyle(TDCS_ENABLESOURCECONTROL, bPathSupports);
					tdc.SetStyle(TDCS_CHECKOUTONLOAD, bPathSupports && userPrefs.GetAutoCheckOut());

					if (bWantCheckOut && !tdc.IsCheckedOut())
					{
						tdc.CheckOut();
					}
				}

				// re-sync
				m_mgrToDoCtrls.UpdateToDoCtrlReadOnlyUIState(nCtrl);
				m_mgrToDoCtrls.RefreshModifiedStatus(nCtrl);
				m_mgrToDoCtrls.RefreshLastModified(nCtrl);
				m_mgrToDoCtrls.UpdateTabItemText(nCtrl);
			}
		}

		m_toolbar.GetToolBarCtrl().HideButton(ID_TOOLS_TOGGLECHECKIN, !bSourceControl);

		// check box in front of task title.
		// this is tricky because the checkbox won't display if the completion
		// column is visible. ie. the completion column take precedence.
		// so if the user has just turned on the checkbox in front of
		// the task title then we need to turn off the completion column
		// on all those tasklists currently showing it. But we only need to do
		// this on those tasklists which are managing their own columns else
		// it will be handled when we update the preferences in due course.
		if (userPrefs.GetTreeCheckboxes() && !curPrefs.GetTreeCheckboxes())
		{
			int nCtrl = GetTDCCount();

			while (nCtrl--)
			{
				if (m_mgrToDoCtrls.HasOwnColumns(nCtrl))
				{
					CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtrl);

					if (tdc.IsColumnShowing(TDCC_DONE))
					{
						CTDCColumnArray aColumns;
						int nCol = tdc.GetVisibleColumns(aColumns);

						while (nCol--)
						{
							if (aColumns[nCol] == TDCC_DONE)
							{
								aColumns.RemoveAt(nCol);
								break;
							}
						}

						tdc.SetVisibleColumns(aColumns);
					}
				}
			}
		}

		// same again for task icons
		if (userPrefs.GetTreeTaskIcons() && !curPrefs.GetTreeTaskIcons())
		{
			int nCtrl = GetTDCCount();

			while (nCtrl--)
			{
				if (m_mgrToDoCtrls.HasOwnColumns(nCtrl))
				{
					CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtrl);

					if (tdc.IsColumnShowing(TDCC_ICON))
					{
						CTDCColumnArray aColumns;
						int nCol = tdc.GetVisibleColumns(aColumns);

						while (nCol--)
						{
							if (aColumns[nCol] == TDCC_ICON)
							{
								aColumns.RemoveAt(nCol);
								break;
							}
						}

						tdc.SetVisibleColumns(aColumns);
					}
				}
			}
		}

		// menu icons
		UINT nPrevID = MapNewTaskPos(curPrefs.GetNewTaskPos(), FALSE);
		m_mgrMenuIcons.ChangeImageID(nPrevID, GetNewTaskCmdID());

		nPrevID = MapNewTaskPos(curPrefs.GetNewSubtaskPos(), TRUE);
		m_mgrMenuIcons.ChangeImageID(nPrevID, GetNewSubtaskCmdID());

		// reload menu
		LoadMenubar();

		// tab bar
		bResizeDlg |= (curPrefs.GetAutoHideTabbar() != userPrefs.GetAutoHideTabbar());

		if (curPrefs.GetStackTabbarItems() != userPrefs.GetStackTabbarItems())
		{
			BOOL bStackTabbar = userPrefs.GetStackTabbarItems();

			bResizeDlg = TRUE;
			m_tabCtrl.ModifyStyle(bStackTabbar ? 0 : TCS_MULTILINE, bStackTabbar ? TCS_MULTILINE : 0);
		}
		else
		{
			m_tabCtrl.Invalidate();   // handle priority colour changes
		}

		// filtering
		RefreshFilterLabelAlignment();

		// and visible filter controls
		if (m_bShowFilterBar)
		{
			bResizeDlg = TRUE;
		}

		BOOL bEnableMultiSel = userPrefs.GetMultiSelFilters();
		BOOL bPrevMultiSel = curPrefs.GetMultiSelFilters();

		if (bPrevMultiSel != bEnableMultiSel)
		{
			m_filterBar.EnableMultiSelection(bEnableMultiSel);

			// if it was was previously multisel (but not now) then
			// refresh the filter because we may have gone from
			// multiple selection down to only one
			OnViewRefreshfilter();
		}

		// inherited parent task attributes for new tasks
		CTDCAttributeArray aParentAttrib;
		BOOL bUpdateAttrib;

		userPrefs.GetParentAttribsUsed(aParentAttrib, bUpdateAttrib);
		CToDoCtrl::SetInheritedParentAttributes(aParentAttrib, bUpdateAttrib);

		// hotkey
		UpdateGlobalHotkey();

		// time periods
		CTimeHelper::SetHoursInOneDay(userPrefs.GetHoursInOneDay());
		CTimeHelper::SetDaysInOneWeek(userPrefs.GetDaysInOneWeek());

		RefreshTabOrder();

		// time tracking
		if (curPrefs.GetTrackNonActiveTasklists() != userPrefs.GetTrackNonActiveTasklists())
		{
			RefreshPauseTimeTracking();
		}

		UpdateCaption();
		UpdateTabSwitchTooltip();

		// colours
		if (m_findDlg.GetSafeHwnd())
		{
			m_findDlg.RefreshUserPreferences();
		}

		// active tasklist userPrefs
		UpdateToDoCtrlPreferences();

		// then refresh filter bar for any new default cats, statuses, etc
		m_filterBar.RefreshFilterControls(GetToDoCtrl());

		if (bResizeDlg)
		{
			ResizeDlg();
		}

		// don't ask me for the full details on this but it seems as
		// though the CSysImageList class is waiting to process a
		// message before we can switch image sizes so we put it
		// right at the end after everything is done.
		Misc::ProcessMsgLoop();
		AppendTools2Toolbar(TRUE);
	}

	// finally set or terminate the various status check timers
	SetTimer(TIMER_READONLYSTATUS, userPrefs.GetReadonlyReloadOption() != RO_NO);
	SetTimer(TIMER_TIMESTAMPCHANGE, userPrefs.GetTimestampReloadOption() != RO_NO);
	SetTimer(TIMER_AUTOSAVE, userPrefs.GetAutoSaveFrequency());
	SetTimer(TIMER_CHECKOUTSTATUS, userPrefs.GetCheckoutOnCheckin() || userPrefs.GetCheckinOnNoEditTime());
	SetTimer(TIMER_TIMETRACKING, userPrefs.GetUseSysTray());
}

BOOL CToDoListWnd::LoadMenubar()
{
	m_menubar.DestroyMenu();

	if (!m_menubar.LoadMenu(IDR_MAINFRAME))
	{
		return FALSE;
	}

	SetMenu(&m_menubar);
	m_hMenuDefault = m_menubar;

#ifdef _DEBUG
	// add menu option to simulate WM_QUERYENDSESSION
	m_menubar.InsertMenu((UINT)-1, MFT_STRING, ID_DEBUGQUERYENDSESSION, _T("QueryEndSession"));
#endif

	if (Prefs().GetShowTasklistCloseButton())
	{
		m_menubar.AddMDIButton(MEB_CLOSE, ID_CLOSE);
	}

	if (CThemed::IsThemeActive())
	{
		m_menubar.SetBackgroundColor(m_theme.crMenuBack);
	}

	DrawMenuBar();

	return TRUE;
}

void CToDoListWnd::UpdateGlobalHotkey()
{
	static DWORD dwPrevHotkey = 0;
	DWORD dwHotkey = Prefs().GetGlobalHotkey();

	if (dwPrevHotkey == dwHotkey)
	{
		return;
	}

	if (dwHotkey == 0) // disabled
	{
		::UnregisterHotKey(*this, 1);
	}
	else
	{
		// map modifiers to those wanted by RegisterHotKey
		DWORD dwPrefMods = HIWORD(dwHotkey);
		DWORD dwVKey = LOWORD(dwHotkey);

		DWORD dwMods = (dwPrefMods & HOTKEYF_ALT) ? MOD_ALT : 0;
		dwMods |= (dwPrefMods & HOTKEYF_CONTROL) ? MOD_CONTROL : 0;
		dwMods |= (dwPrefMods & HOTKEYF_SHIFT) ? MOD_SHIFT : 0;

		RegisterHotKey(*this, 1, dwMods, dwVKey);
	}

	dwPrevHotkey = dwHotkey;
}

void CToDoListWnd::RefreshPauseTimeTracking()
{
	// time tracking
	int nCtrl = GetTDCCount();
	int nSel = GetSelToDoCtrl();
	BOOL bTrackActiveOnly = !Prefs().GetTrackNonActiveTasklists();

	while (nCtrl--)
	{
		BOOL bSel = (nCtrl == nSel);
		CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtrl);

		tdc.PauseTimeTracking(bSel ? FALSE : bTrackActiveOnly);
	}
}

void CToDoListWnd::CheckMinWidth()
{
	if (GetTDCCount())
	{
		CRect rTDC;
		GetToDoCtrl().GetClientRect(rTDC);
		int nMinWidth = GetToDoCtrl().GetMinWidth();

		if (rTDC.Width() < nMinWidth)
		{
			CRect rWindow;
			GetWindowRect(rWindow);

			SetWindowPos(NULL, 0, 0, rWindow.Width() - 1, rWindow.Height(), SWP_NOZORDER | SWP_NOMOVE);
		}
	}
}

BOOL CToDoListWnd::ProcessStartupOptions(const TDCSTARTUP& startup)
{
	BOOL bRes = FALSE;

	CString sFilePath = startup.GetFilePath();

	// open/import file
	if (!sFilePath.IsEmpty())
	{
		PrepareOpenFilePath(sFilePath);

		if (startup.HasFlag(TLD_IMPORTFILE))
		{
			return ImportFile(sFilePath);
		}

		else if (OpenTaskList(sFilePath, NULL, FALSE) != TDCO_SUCCESS)
		{
			return FALSE;
		}
	}

	CToDoCtrl& tdc = GetToDoCtrl();

	CString sNewTask = startup.GetNewTask();
	CString sComments = startup.GetComments();
	CString sExternalID = startup.GetExternalID();

	if (startup.HasFlag(TLD_NEWTASK))
	{
		BOOL bEditTask = sNewTask.IsEmpty();
		CString sTitle = bEditTask ? CEnString(IDS_TASK) : sNewTask;

		// do we have a parent task ?
		if (startup.dwPid && tdc.SelectTask(startup.dwPid))
		{
			bRes = NewTask(sTitle, TDC_INSERTATTOPOFSELTASK, FALSE);
		}
		else
		{
			bRes = NewTask(sTitle, TDC_INSERTATTOP, FALSE);
		}

		if (bRes)
		{
			if (!sComments.IsEmpty())
			{
				bRes = tdc.SetSelectedTaskComments(sComments, sComments);
			}

			if (bRes && !sExternalID.IsEmpty())
			{
				bRes = tdc.SetSelectedTaskExtID(sExternalID);
			}

			if (bRes && bEditTask)
			{
				PostMessage(WM_COMMAND, ID_EDIT_TASKTEXT);
			}
		}
	}
	else if (startup.dwIDSel && tdc.SelectTask(startup.dwIDSel))
	{
		if (sComments.GetLength())
		{
			bRes = tdc.SetSelectedTaskComments(sComments, sComments);
		}

		if (bRes && !sExternalID.IsEmpty())
		{
			bRes = tdc.SetSelectedTaskExtID(sExternalID);
		}
	}

	return bRes;
}

BOOL CToDoListWnd::OnCopyData(CWnd* /*pWnd*/, COPYDATASTRUCT* pCopyDataStruct)
{
	BOOL bRes = FALSE;

	switch (pCopyDataStruct->dwData)
	{
	case TDL_STARTUP:
		{
			const TDCSTARTUP* pStartup = (TDCSTARTUP*)(pCopyDataStruct->lpData);

			if (pStartup)
			{
				bRes = ProcessStartupOptions(*pStartup/*, TRUE*/);
			}
		}
		break;
	}

	return bRes;
}

BOOL CToDoListWnd::ImportFile(LPCTSTR szFilePath)
{
	int nImporter = m_mgrImportExport.FindImporter(szFilePath);

	if (nImporter == -1)
	{
		return FALSE;
	}

	CTaskFile tasks;

	m_mgrImportExport.ImportTaskList(szFilePath, &tasks, nImporter);

	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	if (tdc.InsertTasks(tasks, TDC_INSERTATTOP))
	{
		UpdateCaption();
	}

	return TRUE;
}

void CToDoListWnd::OnEditCopy()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	tdc.Flush();
	tdc.CopySelectedTask();
}

void CToDoListWnd::OnEditCut()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	tdc.Flush();
	tdc.CutSelectedTask();
}

void CToDoListWnd::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	pCmdUI->Enable(!tdc.IsReadOnly() && nSelCount);
}

void CToDoListWnd::OnEditPasteSub()
{
	CWaitCursor wait;
	GetToDoCtrl().PasteTasks(TDCP_ONSELTASK);
}

void CToDoListWnd::OnUpdateEditPasteSub(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	pCmdUI->Enable(!tdc.IsReadOnly() && tdc.CanPaste() && nSelCount == 1);
}

void CToDoListWnd::OnEditPasteAfter()
{
	CWaitCursor wait;
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	if (nSelCount == 0)
	{
		tdc.PasteTasks(TDCP_ATBOTTOM);
	}
	else
	{
		tdc.PasteTasks(TDCP_BELOWSELTASK);
	}
}

void CToDoListWnd::OnUpdateEditPasteAfter(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	// modify the text appropriately if the tasklist is empty
	if (nSelCount == 0)
	{
		pCmdUI->SetText(CEnString(IDS_PASTETOPLEVELTASK));
	}

	pCmdUI->Enable(!tdc.IsReadOnly() && tdc.CanPaste());
}

void CToDoListWnd::OnEditCopyastext()
{
	CopySelectedTasksToClipboard(CT_ASTEXT);
}

void CToDoListWnd::OnEditCopyashtml()
{
	CopySelectedTasksToClipboard(CT_ASHTML);
}

void CToDoListWnd::CopySelectedTasksToClipboard(int nAsFormat)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.Flush();

	BOOL bParentTitleCommentsOnly = Prefs().GetExportParentTitleCommentsOnly();
	DWORD dwFlags = TDCGTF_ISODATES | (bParentTitleCommentsOnly ? TDCGTF_PARENTTITLECOMMENTSONLY : 0);

	CTaskFile tasks;
	CString sTasks;
	tdc.GetSelectedTasks(tasks, TDCGETTASKS(TDCGT_ALL, dwFlags));

	switch (nAsFormat)
	{
	case CT_ASHTML:
		sTasks = m_mgrImportExport.ExportTaskListToHtml(&tasks);
		break;

	case CT_ASTEXT:
		sTasks = m_mgrImportExport.ExportTaskListToText(&tasks);
		break;

	case CT_ASREF:
		sTasks.Format(_T("tdl://%ld"), tdc.GetSelectedTaskID());
		break;

	case CT_ASDEPENDS:
		sTasks.Format(_T("%ld"), tdc.GetSelectedTaskID());
		break;

	case CT_ASREFFULL:
		sTasks.Format(_T("tdl://%s?%ld"), (LPCTSTR)tdc.GetFilePath(), tdc.GetSelectedTaskID());
		sTasks.Replace(_T(" "), _T("%20"));
		break;

	case CT_ASDEPENDSFULL:
		sTasks.Format(_T("%s?%ld"), (LPCTSTR)tdc.GetFilePath(), tdc.GetSelectedTaskID());
		break;

	case CT_ASPATH:
		sTasks = tdc.GetSelectedTaskPath(TRUE);
		break;
	}

	Misc::CopyTexttoClipboard(sTasks, *this);
}

void CToDoListWnd::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().GetSelectedCount() > 0);
}

void CToDoListWnd::OnUpdateNewtaskAttopSelected(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	// enable this when the tasklist is empty even if no
	// item is selected.
	pCmdUI->Enable(!tdc.IsReadOnly() && nSelCount <= 1);
}

void CToDoListWnd::OnUpdateNewtaskAtbottomSelected(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	// enable this when the tasklist is empty even if no
	// item is selected.
	pCmdUI->Enable(!tdc.IsReadOnly() && nSelCount <= 1);
}

void CToDoListWnd::OnUpdateNewtaskAfterselectedtask(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	// enable this when the tasklist is empty even if no
	// item is selected.
	pCmdUI->Enable(!tdc.IsReadOnly() && nSelCount <= 1);
}

void CToDoListWnd::OnUpdateNewtaskBeforeselectedtask(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	// enable this when the tasklist is empty even if no
	// item is selected.
	pCmdUI->Enable(!tdc.IsReadOnly() && nSelCount <= 1);
}

void CToDoListWnd::OnUpdateNewsubtaskAttop(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	pCmdUI->Enable(!tdc.IsReadOnly() && nSelCount <= 1);
}

void CToDoListWnd::OnMaximizeTasklist()
{
	// toggle max state on or off
	switch (m_nMaxState)
	{
	case TDCMS_MAXTASKLIST:
		// turn off maximize tasklist by restoring previous max state
		m_nMaxState = m_nPrevMaxState;
		m_nPrevMaxState = TDCMS_NORMAL; // reset
		break;

	case TDCMS_MAXCOMMENTS:
		// turn on maximize tasklist and save previous max state
		m_nMaxState = TDCMS_MAXTASKLIST;
		m_nPrevMaxState = TDCMS_MAXCOMMENTS;
		break;

	case TDCMS_NORMAL:
		// turn on maximize tasklist
		m_nMaxState = TDCMS_MAXTASKLIST;
		m_nPrevMaxState = TDCMS_NORMAL; // reset
		break;
	}

	// update active tasklist
	GetToDoCtrl().SetMaximizeState(m_nMaxState);
	Invalidate();
}

void CToDoListWnd::OnUpdateMaximizeTasklist(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nMaxState == TDCMS_MAXTASKLIST ? 1 : 0);
}

void CToDoListWnd::OnMaximizeComments()
{
	// toggle max state on or off
	switch (m_nMaxState)
	{
	case TDCMS_MAXCOMMENTS:
		// toggle off maximize comments by restoring previous max state
		m_nMaxState = m_nPrevMaxState;
		m_nPrevMaxState = TDCMS_NORMAL; // reset
		break;

	case TDCMS_MAXTASKLIST:
		// turn on maximize comments and save previous max state
		m_nMaxState = TDCMS_MAXCOMMENTS;
		m_nPrevMaxState = TDCMS_MAXTASKLIST;
		break;

	case TDCMS_NORMAL:
		// turn on maximize comments
		m_nMaxState = TDCMS_MAXCOMMENTS;
		m_nPrevMaxState = TDCMS_NORMAL; // reset
		break;
	}

	// update active tasklist
	GetToDoCtrl().SetMaximizeState(m_nMaxState);
	Invalidate();
}

void CToDoListWnd::OnUpdateMaximizeComments(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nMaxState == TDCMS_MAXCOMMENTS ? 1 : 0);
}

void CToDoListWnd::OnReload()
{
	int nSel = GetSelToDoCtrl();

	if (m_mgrToDoCtrls.GetModifiedStatus(nSel))
	{
		if (IDYES != MessageBox(IDS_CONFIRMRELOAD, IDS_CONFIRMRELOAD_TITLE, MB_YESNOCANCEL | MB_DEFBUTTON2))
		{
			return;
		}
	}

	// else reload
	ReloadTaskList(nSel, TRUE);
	RefreshTabOrder();
}

void CToDoListWnd::ReloadTaskList(int nIndex, BOOL bNotifyDueTasks)
{
	int nSel = m_tabCtrl.GetCurSel();
	CFilteredToDoCtrl& tdc = GetToDoCtrl(nIndex);

	TDC_FILE nRes = OpenTaskList(&tdc);

	if (nRes == TDCO_SUCCESS)
	{
		CPreferencesDlg& userPrefs = Prefs();

		// update file status
		if (userPrefs.GetAutoCheckOut())
		{
			m_mgrToDoCtrls.SetLastCheckoutStatus(nIndex, tdc.IsCheckedOut());
		}

		m_mgrToDoCtrls.RefreshLastModified(nIndex);
		m_mgrToDoCtrls.SetModifiedStatus(nIndex, FALSE);
		m_mgrToDoCtrls.UpdateTabItemText(nIndex);

		// notify user of due tasks if req
		if (bNotifyDueTasks)
		{
			DoDueTaskNotification(&tdc, userPrefs.GetNotifyDueByOnLoad());
		}

		UpdateCaption();
		UpdateStatusbar();

		// there's a funny redraw/show bug when reloading a non-active tasklist
		// so we force the just loaded tasklist back into the background if
		// it is not active
		if (nIndex != nSel)
		{
			tdc.ShowWindow(SW_HIDE);
		}
	}
	else
	{
		HandleLoadTasklistError(nRes, tdc.GetFilePath());
	}
}

void CToDoListWnd::OnUpdateReload(CCmdUI* pCmdUI)
{
	int nSel = GetSelToDoCtrl();

	pCmdUI->Enable(!m_mgrToDoCtrls.GetFilePath(nSel).IsEmpty());
}

void CToDoListWnd::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	// ensure m_cbQuickFind is positioned correctly
	int nPos = m_toolbar.CommandToIndex(ID_EDIT_FINDTASKS) + 2;

	CRect rNewPos, rOrgPos;
	m_toolbar.GetItemRect(nPos, rNewPos);
	m_toolbar.ClientToScreen(rNewPos);
	m_cbQuickFind.CWnd::GetWindowRect(rOrgPos);

	// check if it needs to be moved
	if (rNewPos.TopLeft() != rOrgPos.TopLeft())
	{
		m_toolbar.ScreenToClient(rNewPos);
		rNewPos.bottom = rNewPos.top + 200;
		m_cbQuickFind.MoveWindow(rNewPos);
	}

	// topmost?
	BOOL bMaximized = (nType == SIZE_MAXIMIZED);

	if (nType != SIZE_MINIMIZED)
	{
		ResizeDlg(cx, cy, bMaximized);
	}

	// if not maximized then set topmost if that's the preference
	BOOL bTopMost = (Prefs().GetAlwaysOnTop() && !bMaximized) ? 1 : 0;

	// do nothing if no change
	BOOL bIsTopMost = (GetExStyle() & WS_EX_TOPMOST) ? 1 : 0;

	if (bTopMost != bIsTopMost)
	{
		SetWindowPos(bTopMost ? &wndTopMost : &wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}
}

BOOL CToDoListWnd::CalcToDoCtrlRect(CRect& rect, int cx, int cy, BOOL bMaximized)
{
	if (!cx && !cy)
	{
		CRect rClient;
		GetClientRect(rClient);

		cx = rClient.right;
		cy = rClient.bottom;
		bMaximized = IsZoomed();

		// check again
		if (!cx && !cy)
		{
			return FALSE;
		}
	}

	CRect rTaskList(0, BEVEL, cx - BEVEL, cy);

	// toolbar
	if (m_bShowToolbar)
	{
		rTaskList.top += m_toolbar.CalcHeightRequired(cx) + TB_VOFFSET;
	}

	// resize tabctrl
	CDeferWndMove dwm(0); // dummy

	CPoint ptOrg(0, rTaskList.top);
	int nTabHeight = ReposTabBar(dwm, ptOrg, cx, TRUE);

	if (nTabHeight)
	{
		rTaskList.top += nTabHeight + 1;   // hide the bottom of the tab ctrl
	}

	// filter controls
	int nInset = (CThemed().IsNonClientThemed() ? BORDER : BEVEL);
	int nFilterWidth = cx - 2 * nInset;
	int nFilterHeight = m_bShowFilterBar ? m_filterBar.CalcHeight(nFilterWidth) : 0;

	if (nFilterHeight)
	{
		rTaskList.top += nFilterHeight + 4;
	}

	// statusbar
	if (m_bShowStatusBar)
	{
		CRect rStatus;
		m_statusBar.GetWindowRect(rStatus);
		ScreenToClient(rStatus);
		rTaskList.bottom = rStatus.top - 2;
	}
	else
	{
		rTaskList.bottom = cy;
	}

	// shrink slightly so that edit controls do not merge with window border
	rTaskList.left += nInset;

	rect = rTaskList;

	return TRUE;
}

void CToDoListWnd::ResizeDlg(int cx, int cy, BOOL bMaximized)
{
	static int nLastCx = 0, nLastCy = 0;

	if (!cx && !cy)
	{
		CRect rClient;
		GetClientRect(rClient);

		cx = rClient.right;
		cy = rClient.bottom;
		bMaximized = IsZoomed();

		// check again
		if (!cx && !cy)
		{
			return;
		}
	}

	if (cx == nLastCx && cy == nLastCy && !GetTDCCount())
	{
		return;
	}

	nLastCx = cx;
	nLastCy = cy;

	// resize in one go
	CDlgUnits dlu(*this);
	CDeferWndMove dwm(6);
	CRect rTaskList(0, BEVEL, cx - BEVEL, cy);

	// toolbar
	if (m_bShowToolbar) // showing toolbar
	{
		rTaskList.top += m_toolbar.Resize(cx, CPoint(0, TB_VOFFSET)) + TB_VOFFSET;
	}

	// resize tabctrl
	CPoint ptOrg(0, rTaskList.top);
	int nTabHeight = ReposTabBar(dwm, ptOrg, cx);

	if (nTabHeight)
	{
		rTaskList.top += nTabHeight + 1;   // hide the bottom of the tab ctrl
	}

	// filter controls
	int nInset = (CThemed().IsNonClientThemed() ? BORDER : BEVEL);
	int nFilterWidth = cx - 2 * nInset;
	int nFilterHeight = m_bShowFilterBar ? m_filterBar.CalcHeight(nFilterWidth) : 0;

	dwm.MoveWindow(&m_filterBar, nInset, rTaskList.top, nFilterWidth, nFilterHeight);

	if (nFilterHeight)
	{
		rTaskList.top += nFilterHeight + 4;
	}

	// statusbar has already been automatically resized unless it's invisible
	CRect rStatus(0, cy, cx, cy);

	if (m_bShowStatusBar)
	{
		m_statusBar.GetWindowRect(rStatus);
		ScreenToClient(rStatus);
	}
	else
	{
		dwm.MoveWindow(&m_statusBar, rStatus, FALSE);
	}

	// finally the active todoctrl
	if (GetTDCCount())
	{
		if (m_bShowStatusBar)
		{
			rTaskList.bottom = rStatus.top - 2;
		}
		else
		{
			rTaskList.bottom = rStatus.bottom;
		}

		// shrink slightly so that edit controls do not merge with window border
		rTaskList.left = nInset;

		dwm.MoveWindow(&GetToDoCtrl(), rTaskList);

#ifdef _DEBUG
		CRect rect;
		CalcToDoCtrlRect(rect, cx, cy, IsZoomed());
		ASSERT(rect == rTaskList);
#endif

	}
}

BOOL CToDoListWnd::WantTasklistTabbarVisible() const
{
	BOOL bWantTabbar = (GetTDCCount() > 1 || !Prefs().GetAutoHideTabbar());
	bWantTabbar &= m_bShowTasklistBar;

	return bWantTabbar;
}

int CToDoListWnd::ReposTabBar(CDeferWndMove& dwm, const CPoint& ptOrg, int nWidth, BOOL bCalcOnly)
{
	CRect rTabs(0, 0, nWidth, 0);
	m_tabCtrl.AdjustRect(TRUE, rTabs);
	int nTabHeight = rTabs.Height() - 4;

	rTabs = dwm.OffsetCtrl(this, IDC_TABCONTROL); // not actually a move
	rTabs.right = nWidth + 1;
	rTabs.bottom = rTabs.top + nTabHeight;
	rTabs.OffsetRect(0, ptOrg.y - rTabs.top + 1); // add a pixel between tabbar and toolbar

	BOOL bNeedTabCtrl = WantTasklistTabbarVisible();

	if (!bCalcOnly)
	{
		dwm.MoveWindow(&m_tabCtrl, rTabs);

		// hide and disable tabctrl if not needed
		m_tabCtrl.ShowWindow(bNeedTabCtrl ? SW_SHOW : SW_HIDE);
		m_tabCtrl.EnableWindow(bNeedTabCtrl);
	}

	return bNeedTabCtrl ? rTabs.Height() : 0;
}

CRect CToDoListWnd::OffsetCtrl(UINT uCtrlID, int cx, int cy)
{
	CWnd* pCtrl = GetDlgItem(uCtrlID);

	if (pCtrl)
	{
		CRect rChild;
		pCtrl->GetWindowRect(rChild);
		ScreenToClient(rChild);

		if (cx || cy)
		{
			rChild.OffsetRect(cx, cy);
			pCtrl->MoveWindow(rChild);
		}
		return rChild;
	}

	return CRect(0, 0, 0, 0);
}

void CToDoListWnd::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	CFrameWnd::OnGetMinMaxInfo(lpMMI);

	// don't handle if maximized or minimized
	if (!IsZoomed() && !IsIconic())
	{
		int nMinWidth = 352;

		if (GetTDCCount()) // ie there is at least CToDoCtrl
		{
			CRect rClient, rWindow;

			GetClientRect(rClient);
			GetWindowRect(rWindow);

			// odd 'bug'. when the window is being restored the client
			// rect is NULL and this generates an invalid min size
			if (!rClient.IsRectNull())
			{
				nMinWidth = GetToDoCtrl().GetMinWidth() + (rWindow.Width() - rClient.Width());
			}
		}

		lpMMI->ptMinTrackSize.x = nMinWidth; // so caption and toolbar is fully visible
		lpMMI->ptMinTrackSize.y = 460; // arbitrary
	}
}

void CToDoListWnd::OnPrint()
{
	DoPrint();
}

void CToDoListWnd::DoPrint(BOOL bPreview)
{
	int nSelTDC = GetSelToDoCtrl();

	// pass the project name as the title field
	CString sTitle = m_mgrToDoCtrls.GetFriendlyProjectName(nSelTDC);

	// export to html and then print in IE
	BOOL bShowSubtaskCheckbox = (GetToDoCtrl().GetView() == FTCV_TASKTREE);
	CTDLPrintDialog dialog(sTitle, bPreview, bShowSubtaskCheckbox);

	if (dialog.DoModal() != IDOK)
	{
		return;
	}

	RedrawWindow();

	// always use the same file
	CString sTempFile = FileMisc::GetTempFileName(_T("ToDoList.print"), _T("html"));

	// stylesheets don't seem to like the way we do html comments
	CString sStylesheet = dialog.GetStylesheet();
	BOOL bTransform = FileMisc::FileExists(sStylesheet);

	sTitle = dialog.GetTitle();

	// export
	CWaitCursor cursor;
	CTaskFile tasks;

	GetTasks(GetToDoCtrl(), TRUE, bTransform, dialog.GetTaskSelection(), tasks);

	// add title and date
	COleDateTime date;

	if (dialog.GetWantDate())
	{
		date = COleDateTime::GetCurrentTime();
	}

	tasks.SetReportAttributes(sTitle, date);

	if (!Export2Html(tasks, sTempFile, sStylesheet))
	{
		return;
	}

	// print from browser
	CRect rHidden(-20, -20, -10, -10); // create IE off screen

	if (m_IE.GetSafeHwnd() || m_IE.Create(NULL, WS_CHILD | WS_VISIBLE, rHidden, this, (UINT)IDC_STATIC))
	{
		if (bPreview)
		{
			m_IE.PrintPreview(sTempFile);
		}
		else
		{
			m_IE.Print(sTempFile);
		}
	}
	else // try sending to browser
	{
		int nRes = (int)ShellExecute(*this, bPreview ? _T("print") : NULL, sTempFile, NULL, NULL, SW_HIDE);

		if (nRes < 32)
		{
			MessageBox(IDS_PRINTFAILED, IDS_PRINTFAILED_TITLE, MB_OK);
		}
	}
}

void CToDoListWnd::OnUpdatePrint(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().GetTaskCount());
}

int CToDoListWnd::AddToDoCtrl(CFilteredToDoCtrl* pCtrl, BOOL bResizeDlg)
{
	// add tdc first to ensure tab controls has some
	// items before we query it for its size
	int nSel = m_mgrToDoCtrls.AddToDoCtrl(pCtrl);

	// make sure size is right
	CRect rTDC;

	if (CalcToDoCtrlRect(rTDC))
	{
		pCtrl->MoveWindow(rTDC);
	}

	SelectToDoCtrl(nSel, FALSE);
	pCtrl->SetFocusToTasks();

	// make sure the tab control is correctly sized
	if (bResizeDlg)
	{
		ResizeDlg();
	}

	// if this is the only control then set or terminate the various status
	// check timers
	if (GetTDCCount() == 1)
	{
		CPreferencesDlg& userPrefs = Prefs();

		SetTimer(TIMER_READONLYSTATUS, userPrefs.GetReadonlyReloadOption() != RO_NO);
		SetTimer(TIMER_TIMESTAMPCHANGE, userPrefs.GetTimestampReloadOption() != RO_NO);
		SetTimer(TIMER_AUTOSAVE, userPrefs.GetAutoSaveFrequency());
		SetTimer(TIMER_CHECKOUTSTATUS, userPrefs.GetCheckoutOnCheckin() || userPrefs.GetCheckinOnNoEditTime());
	}

	// update filter controls
	RefreshFilterControls();

	// make sure everything looks okay
	Invalidate();
	UpdateWindow();

	return nSel;
}

void CToDoListWnd::SetTimer(UINT nTimerID, BOOL bOn)
{
	if (bOn)
	{
		UINT nPeriod = 0;

		switch (nTimerID)
		{
		case TIMER_READONLYSTATUS:
			nPeriod = INTERVAL_READONLYSTATUS;
			break;

		case TIMER_TIMESTAMPCHANGE:
			nPeriod = INTERVAL_TIMESTAMPCHANGE;
			break;

		case TIMER_AUTOSAVE:
			nPeriod = Prefs().GetAutoSaveFrequency() * 60000;
			break;

		case TIMER_CHECKOUTSTATUS:
			nPeriod = INTERVAL_CHECKOUTSTATUS;
			break;

		case TIMER_DUEITEMS:
			nPeriod = INTERVAL_DUEITEMS;
			break;

		case TIMER_TIMETRACKING:
			nPeriod = INTERVAL_TIMETRACKING;
			break;
		}

		if (nPeriod)
		{
			UINT nID = CFrameWnd::SetTimer(nTimerID, nPeriod, NULL);
			ASSERT(nID);
		}
	}
	else
	{
		KillTimer(nTimerID);
	}
}

void CToDoListWnd::OnTimer(UINT nIDEvent)
{
	CFrameWnd::OnTimer(nIDEvent);

	// if we are disabled (== modal dialog visible) then do not respond
	if (!IsWindowEnabled())
	{
		return;
	}

	// don't check whilst in the middle of saving or closing
	if (m_bSaving || m_bClosing)
	{
		return;
	}

	// if no controls are active kill the timers
	if (!GetTDCCount())
	{
		SetTimer(TIMER_READONLYSTATUS, FALSE);
		SetTimer(TIMER_TIMESTAMPCHANGE, FALSE);
		SetTimer(TIMER_AUTOSAVE, FALSE);
		SetTimer(TIMER_CHECKOUTSTATUS, FALSE);
		return;
	}

	switch (nIDEvent)
	{
	case TIMER_READONLYSTATUS:
		OnTimerReadOnlyStatus();
		break;

	case TIMER_TIMESTAMPCHANGE:
		OnTimerTimestampChange();
		break;

	case TIMER_AUTOSAVE:
		OnTimerAutoSave();
		break;

	case TIMER_CHECKOUTSTATUS:
		OnTimerCheckoutStatus();
		break;

	case TIMER_DUEITEMS:
		OnTimerDueItems();
		break;

	case TIMER_TIMETRACKING:
		OnTimerTimeTracking();
		break;
	}
}

void CToDoListWnd::OnTimerTimeTracking()
{
	AF_NOREENTRANT // macro helper

	static BOOL bWasTimeTracking = FALSE;
	BOOL bNowTimeTracking = FALSE;

	// cycle thru tasklists until we find a time tracker
	int nCtrls = GetTDCCount();

	for (int nCtrl = 0; nCtrl < nCtrls && !bNowTimeTracking; nCtrl++)
	{
		const CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtrl);

		if (tdc.IsActivelyTimeTracking())
		{
			bNowTimeTracking = TRUE;
		}
	}

	if (bWasTimeTracking != bNowTimeTracking)
	{
		UINT nIDTrayIcon = 0;

		if (bNowTimeTracking)
		{
			nIDTrayIcon = (COSVersion() >= OSV_XP) ? IDI_TRAYICONXPTRACK : IDI_TRAYICONTRACK;
		}
		else
		{
			nIDTrayIcon = (COSVersion() >= OSV_XP) ? IDI_TRAYICONXP : IDI_TRAYICON;
		}

		m_ti.SetIcon(nIDTrayIcon);

		// set the main window icon also as this helps the user know what's going on
		if (COSVersion() >= OSV_XP)
		{
			HICON hIcon = AfxGetApp()->LoadIcon(nIDTrayIcon);
			SetIcon(hIcon, FALSE);
			DestroyIcon(hIcon);
		}
	}

	bWasTimeTracking = bNowTimeTracking;
}

void CToDoListWnd::OnTimerDueItems(int nCtrl)
{
	AF_NOREENTRANT // macro helper

	// first we search for overdue items on each tasklist and if that
	// fails to find anything we then search for items due today
	// but only if the tasklist is fully loaded
	SEARCHRESULT result;
	double dToday = floor(COleDateTime::GetCurrentTime());

	SEARCHPARAMS spDue;
	spDue.rules.Add(SEARCHPARAM(TDCA_DUEDATE, FO_BEFORE, dToday));
	spDue.rules.Add(SEARCHPARAM(TDCA_DONEDATE, FO_NOT_SET, dToday));

	SEARCHPARAMS spDueToday;
	spDueToday.rules.Add(SEARCHPARAM(TDCA_DUEDATE, FO_EQUALS, dToday));
	spDueToday.rules.Add(SEARCHPARAM(TDCA_DONEDATE, FO_NOT_SET, dToday));

	int nFrom = (nCtrl == -1) ? 0 : nCtrl;
	int nTo = (nCtrl == -1) ? GetTDCCount() - 1 : nCtrl;
	BOOL bRepaint = FALSE;

	for (nCtrl = nFrom; nCtrl <= nTo; nCtrl++)
	{
		if (m_mgrToDoCtrls.IsLoaded(nCtrl))
		{
			CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtrl);
			TDCM_DUESTATUS nStatus = TDCM_NONE;

			if (tdc.FindFirstTask(spDue, result))
			{
				nStatus = TDCM_PAST;
			}

			else if (tdc.FindFirstTask(spDueToday, result))
			{
				nStatus = TDCM_TODAY;
			}

			if (nStatus != m_mgrToDoCtrls.GetDueItemStatus(nCtrl))
			{
				m_mgrToDoCtrls.SetDueItemStatus(nCtrl, nStatus);
				bRepaint = TRUE;
			}
		}
	}

	if (bRepaint)
	{
		m_tabCtrl.Invalidate(FALSE);
	}
}

void CToDoListWnd::OnTimerReadOnlyStatus(int nCtrl)
{
	AF_NOREENTRANT // macro helper

	CPreferencesDlg& userPrefs = Prefs();

	// work out whether we should check remote files or not
	BOOL bCheckRemoteFiles = (nCtrl != -1);

	if (!bCheckRemoteFiles)
	{
		static int nElapsed = 0;
		UINT nRemoteFileCheckInterval = userPrefs.GetRemoteFileCheckFrequency() * 1000; // in ms

		nElapsed %= nRemoteFileCheckInterval;
		bCheckRemoteFiles = !nElapsed;

		nElapsed += INTERVAL_READONLYSTATUS;
	}

	int nReloadOption = userPrefs.GetReadonlyReloadOption();

	ASSERT(nReloadOption != RO_NO);

	// process files
	CString sFileList;
	int nFrom = (nCtrl == -1) ? 0 : nCtrl;
	int nTo = (nCtrl == -1) ? GetTDCCount() - 1 : nCtrl;

	for (nCtrl = nFrom; nCtrl <= nTo; nCtrl++)
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtrl);

		// don't check removeable drives
		int nType = m_mgrToDoCtrls.GetFilePathType(nCtrl);

		if (nType == TDCM_UNDEF || nType == TDCM_REMOVABLE)
		{
			continue;
		}

		// check remote files?
		if (!bCheckRemoteFiles && nType == TDCM_REMOTE)
		{
			continue;
		}

		CString sFilePath = tdc.GetFilePath();

		if (m_mgrToDoCtrls.RefreshReadOnlyStatus(nCtrl))
		{
			BOOL bReadOnly = m_mgrToDoCtrls.GetReadOnlyStatus(nCtrl);
			BOOL bReload = FALSE;

			if (nReloadOption == RO_ASK)
			{
				CEnString sMessage(bReadOnly ? IDS_WRITABLETOREADONLY : IDS_READONLYTOWRITABLE, sFilePath);

				if (!bReadOnly) // might been modified
				{
					sMessage += CEnString(IDS_WANTRELOAD);
				}

				UINT nRet = MessageBox(sMessage, IDS_STATUSCHANGE_TITLE, !bReadOnly ? MB_YESNOCANCEL : MB_OK);

				bReload = (nRet == IDYES || nRet == IDOK);
			}
			else
			{
				bReload = !bReadOnly;   // now writable
			}

			if (bReload)
			{
				ReloadTaskList(nCtrl, FALSE);

				// notify the user if we haven't already and the trayicon is visible
				if (nReloadOption == RO_NOTIFY && userPrefs.GetSysTrayOption() != STO_NONE)
				{
					sFileList += tdc.GetFriendlyProjectName();
					sFilePath += _T("\n");
				}
			}
			else // update the UI
			{
				if (nCtrl == m_tabCtrl.GetCurSel())
				{
					UpdateCaption();
				}

				m_mgrToDoCtrls.UpdateToDoCtrlReadOnlyUIState(nCtrl);
				m_mgrToDoCtrls.UpdateTabItemText(nCtrl);
			}
		}
	}

	// do we need to notify the user?
	if (!sFileList.IsEmpty())
	{
		CEnString sMessage(IDS_TASKLISTSRELOADED, sFileList);
		m_ti.ShowBalloon(sMessage, _T("Readonly Status Change"), NIIF_INFO);
	}
}

void CToDoListWnd::OnTimerTimestampChange(int nCtrl)
{
	AF_NOREENTRANT // macro helper

	CPreferencesDlg& userPrefs = Prefs();
	int nReloadOption = userPrefs.GetTimestampReloadOption();

	ASSERT(nReloadOption != RO_NO);

	// work out whether we should check remote files or not
	BOOL bCheckRemoteFiles = (nCtrl != -1);

	if (!bCheckRemoteFiles)
	{
		static int nElapsed = 0;
		UINT nRemoteFileCheckInterval = userPrefs.GetRemoteFileCheckFrequency() * 1000; // in ms

		nElapsed %= nRemoteFileCheckInterval;
		bCheckRemoteFiles = !nElapsed;

		nElapsed += INTERVAL_TIMESTAMPCHANGE;
	}

	// process files
	CString sFileList;
	int nFrom = (nCtrl == -1) ? 0 : nCtrl;
	int nTo = (nCtrl == -1) ? GetTDCCount() - 1 : nCtrl;

	for (nCtrl = nFrom; nCtrl <= nTo; nCtrl++)
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtrl);
		CString sFilePath = tdc.GetFilePath();

		// don't check removeable drives
		int nType = m_mgrToDoCtrls.GetFilePathType(nCtrl);

		if (nType == TDCM_UNDEF || nType == TDCM_REMOVABLE)
		{
			continue;
		}

		// check remote files?
		if (!bCheckRemoteFiles && nType == TDCM_REMOTE)
		{
			continue;
		}

		if (m_mgrToDoCtrls.RefreshLastModified(nCtrl))
		{
			BOOL bReload = TRUE; // default

			if (nReloadOption == RO_ASK)
			{
				CEnString sMessage(IDS_MODIFIEDELSEWHERE, sFilePath);
				sMessage += CEnString(IDS_WANTRELOAD);

				bReload = (MessageBox(sMessage, IDS_TIMESTAMPCHANGE_TITLE, MB_YESNOCANCEL) == IDYES);
			}

			if (bReload)
			{
				ReloadTaskList(nCtrl, FALSE);

				// notify the user if we haven't already and the trayicon is visible
				if (nReloadOption == RO_NOTIFY && userPrefs.GetSysTrayOption() != STO_NONE)
				{
					sFileList += tdc.GetFriendlyProjectName();
					sFilePath += _T("\n");
				}

				// update UI
				if (nCtrl == m_tabCtrl.GetCurSel())
				{
					UpdateCaption();
				}

				m_mgrToDoCtrls.UpdateToDoCtrlReadOnlyUIState(nCtrl);
				m_mgrToDoCtrls.UpdateTabItemText(nCtrl);
			}
		}
	}

	// do we need to notify the user?
	if (!sFileList.IsEmpty())
	{
		CEnString sMessage(IDS_TASKLISTSRELOADED, sFileList);
		m_ti.ShowBalloon(sMessage, _T("File Timestamp Change"), NIIF_INFO);
	}
}

void CToDoListWnd::OnTimerAutoSave()
{
	AF_NOREENTRANT // macro helper

	// don't save if the user is editing a task label
	if (!GetToDoCtrl().IsTaskLabelEditing())
	{
		SaveAll(TDLS_AUTOSAVE);
	}
}

void CToDoListWnd::OnTimerCheckoutStatus(int nCtrl)
{
	AF_NOREENTRANT // macro helper

	CPreferencesDlg& userPrefs = Prefs();

	// work out whether we should check remote files or not
	BOOL bCheckRemoteFiles = (nCtrl != -1);

	if (!bCheckRemoteFiles)
	{
		static int nElapsed = 0;
		UINT nRemoteFileCheckInterval = userPrefs.GetRemoteFileCheckFrequency() * 1000; // in ms

		nElapsed %= nRemoteFileCheckInterval;
		bCheckRemoteFiles = !nElapsed;

		nElapsed += INTERVAL_CHECKOUTSTATUS;
	}

	// process files
	CString sFileList;
	int nFrom = (nCtrl == -1) ? 0 : nCtrl;
	int nTo = (nCtrl == -1) ? GetTDCCount() - 1 : nCtrl;

	for (nCtrl = nFrom; nCtrl <= nTo; nCtrl++)
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtrl);

		if (!m_mgrToDoCtrls.PathSupportsSourceControl(nCtrl))
		{
			continue;
		}

		// try to check out only if the previous attempt failed
		if (!tdc.IsCheckedOut() && userPrefs.GetCheckoutOnCheckin())
		{
			// we only try to check if the previous checkout failed
			if (!m_mgrToDoCtrls.GetLastCheckoutStatus(nCtrl))
			{
				if (tdc.CheckOut())
				{
					// update timestamp
					m_mgrToDoCtrls.RefreshLastModified(nCtrl);
					m_mgrToDoCtrls.SetLastCheckoutStatus(nCtrl, TRUE);

					if (nCtrl == m_tabCtrl.GetCurSel())
					{
						UpdateCaption();
					}

					m_mgrToDoCtrls.UpdateToDoCtrlReadOnlyUIState(nCtrl);
					m_mgrToDoCtrls.UpdateTabItemText(nCtrl);

					// notify the user if the trayicon is visible
					if (userPrefs.GetSysTrayOption() != STO_NONE)
					{
						sFileList += tdc.GetFriendlyProjectName();
						sFileList += _T("\n");
					}
				}
				else // make sure we try again later
				{
					m_mgrToDoCtrls.SetLastCheckoutStatus(nCtrl, FALSE);
				}
			}
		}
		// only checkin if sufficient time has elapsed since last mod
		// and there are no mods outstanding
		else if (tdc.IsCheckedOut() && userPrefs.GetCheckinOnNoEditTime())
		{
			if (!tdc.IsModified())
			{
				double dElapsed = COleDateTime::GetCurrentTime() - tdc.GetLastTaskModified();
				dElapsed *= 24 * 60; // convert to minutes

				if (dElapsed > (double)userPrefs.GetCheckinOnNoEditTime())
				{
					if (tdc.CheckIn())
					{
						m_mgrToDoCtrls.UpdateToDoCtrlReadOnlyUIState(nCtrl);
						m_mgrToDoCtrls.RefreshLastModified(nCtrl);
						m_mgrToDoCtrls.SetLastCheckoutStatus(nCtrl, TRUE);
						m_mgrToDoCtrls.UpdateTabItemText(nCtrl);

						UpdateCaption();
					}
				}
			}
		}
	}
	// do we need to notify the user?
	if (!sFileList.IsEmpty())
	{
		CEnString sMessage(IDS_TASKLISTSCHECKEDOUT, sFileList);
		m_ti.ShowBalloon(sMessage, _T("Source Control Change(s)"), NIIF_INFO);
	}
}

void CToDoListWnd::OnNeedTooltipText(NMHDR* pNMHDR, LRESULT* pResult)
{
	static CString sTipText;
	sTipText.Empty();

	switch (pNMHDR->idFrom)
	{
	case ID_TOOLS_USERTOOL1:
	case ID_TOOLS_USERTOOL2:
	case ID_TOOLS_USERTOOL3:
	case ID_TOOLS_USERTOOL4:
	case ID_TOOLS_USERTOOL5:
	case ID_TOOLS_USERTOOL6:
	case ID_TOOLS_USERTOOL7:
	case ID_TOOLS_USERTOOL8:
	case ID_TOOLS_USERTOOL9:
	case ID_TOOLS_USERTOOL10:
	case ID_TOOLS_USERTOOL11:
	case ID_TOOLS_USERTOOL12:
	case ID_TOOLS_USERTOOL13:
	case ID_TOOLS_USERTOOL14:
	case ID_TOOLS_USERTOOL15:
	case ID_TOOLS_USERTOOL16:
		{
			USERTOOL ut;

			if (Prefs().GetUserTool(pNMHDR->idFrom - ID_TOOLS_USERTOOL1, ut))
			{
				sTipText = ut.sToolName;
			}
		}
		break;

	case ID_TOOLS_USEREXT1:
	case ID_TOOLS_USEREXT2:
	case ID_TOOLS_USEREXT3:
	case ID_TOOLS_USEREXT4:
	case ID_TOOLS_USEREXT5:
	case ID_TOOLS_USEREXT6:
	case ID_TOOLS_USEREXT7:
	case ID_TOOLS_USEREXT8:
	case ID_TOOLS_USEREXT9:
	case ID_TOOLS_USEREXT10:
	case ID_TOOLS_USEREXT11:
	case ID_TOOLS_USEREXT12:
	case ID_TOOLS_USEREXT13:
	case ID_TOOLS_USEREXT14:
	case ID_TOOLS_USEREXT15:
	case ID_TOOLS_USEREXT16:
		{
			int nExt = (pNMHDR->idFrom - ID_TOOLS_USEREXT1);

			if (nExt < m_mgrUIExtensions.GetNumUIExtensions())
			{
				sTipText = m_mgrUIExtensions.GetUIExtensionMenuText(nExt);
			}
		}
		break;

	default:
		// tab control popups
		if (pNMHDR->idFrom >= 0 && pNMHDR->idFrom < (UINT)m_mgrToDoCtrls.GetCount())
		{
			sTipText = m_mgrToDoCtrls.GetTabItemTooltip(pNMHDR->idFrom);
		}
		break;
	}

	if (!sTipText.IsEmpty())
	{
		TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;
		pTTT->lpszText = (LPTSTR)(LPCTSTR)sTipText;
	}

	*pResult = 0;
}

void CToDoListWnd::OnUpdateUserTool(CCmdUI* pCmdUI)
{
	if (pCmdUI->m_pMenu && pCmdUI->m_nID == ID_TOOLS_USERTOOL1) // only handle first item
	{
		CUserToolArray aTools;
		Prefs().GetUserTools(aTools);

		CToolsHelper th(Prefs().GetEnableTDLExtension(), ID_TOOLS_USERTOOL1);
		th.UpdateMenu(pCmdUI, aTools, m_mgrMenuIcons);
	}
	else if (m_bShowToolbar)
	{
		int nTool = pCmdUI->m_nID - ID_TOOLS_USERTOOL1;
		ASSERT(nTool >= 0 && nTool < 16);

		USERTOOL ut;

		if (Prefs().GetUserTool(nTool, ut))
		{
			pCmdUI->Enable(TRUE);
		}
	}
}

void CToDoListWnd::OnUserTool(UINT nCmdID)
{
	int nTool = nCmdID - ID_TOOLS_USERTOOL1;
	USERTOOL ut;

	ASSERT(nTool >= 0 && nTool < 16);

	if (Prefs().GetUserTool(nTool, ut))
	{
		CToolsHelper th(Prefs().GetEnableTDLExtension(), ID_TOOLS_USERTOOL1);
		CFilteredToDoCtrl& tdc = GetToDoCtrl();

		// Save all tasklists before executing the user tool
		if (SaveAll(TDLS_FLUSH) == TDCO_CANCELLED)
		{
			return;
		}

		USERTOOLARGS args;
		args.sTasklist = tdc.GetFilePath();
		args.dwTaskID = tdc.GetSelectedTaskID();
		args.sTaskTitle = tdc.GetSelectedTaskTitle();
		args.sTaskExtID = tdc.GetSelectedTaskExtID();
		args.sTaskComments = tdc.GetSelectedTaskComments();
		args.sTaskFileLink = tdc.GetSelectedTaskFileRef();
		args.sTaskAllocBy = tdc.GetSelectedTaskAllocBy();

		CStringArray aAllocTo;

		if (tdc.GetSelectedTaskAllocTo(aAllocTo))
		{
			args.sTaskAllocTo = Misc::FormatArray(aAllocTo);
		}

		th.RunTool(ut, args, this);
	}
}

void CToDoListWnd::OnUpdateUserUIExtension(CCmdUI* pCmdUI)
{
	if (pCmdUI->m_pMenu && pCmdUI->m_nID == ID_TOOLS_USEREXT1) // only handle first item
	{
		CUIExtensionUIHelper helper(m_mgrUIExtensions, ID_TOOLS_USEREXT1, 16);

		helper.UpdateMenu(pCmdUI, m_mgrMenuIcons);
	}
	else if (!pCmdUI->m_pMenu) // toolbar
	{
		// set check state depending on whether the current tasklist has any visible
		// extension windows or not
		DWORD dwTDC = (DWORD)GetToDoCtrl().GetSafeHwnd();
		int nExt = (pCmdUI->m_nID - ID_TOOLS_USEREXT1);

		IUIExtensionWindow* pUIWnd = m_mgrUIExtensions.GetExtensionWindow(nExt, dwTDC, NULL, FALSE);

		pCmdUI->SetCheck((pUIWnd && pUIWnd->IsShowing()) ? 1 : 0);
	}
}

void CToDoListWnd::OnUserUIExtension(UINT nCmdID)
{
	int nExt = nCmdID - ID_TOOLS_USEREXT1;

	ASSERT(nExt >= 0 && nExt < 16);

	// dwItemData for extension window is CurrentTasklist HWND
	const CFilteredToDoCtrl& tdc = GetToDoCtrl();
	DWORD dwItemData = (DWORD)tdc.GetSafeHwnd();

	// check for existing window
	IUIExtensionWindow* pWindow = m_mgrUIExtensions.GetExtensionWindow(nExt, dwItemData, *this, FALSE);

	// if there is an existing window and it's visible then hide it
	if (pWindow && pWindow->IsShowing())
	{
		pWindow->Show(FALSE);
	}
	else
	{
		BOOL bHadWindow = (pWindow != NULL);

		if (!pWindow)
		{
			pWindow = m_mgrUIExtensions.GetExtensionWindow(nExt, dwItemData, *this);
		}

		if (pWindow)
		{
			pWindow->SetUITheme(m_theme);

			// update if a new window or it's currently hidden
			BOOL bNeedUpdate = (!bHadWindow || !pWindow->IsShowing());

			// update with tasks
			if (bNeedUpdate)
			{
				CTaskFile tasks;
				GetTasks(GetToDoCtrl(), TSDT_FILTERED, tasks);

				// add project name
				tasks.SetProjectName(ATL::CT2A(m_mgrToDoCtrls.GetFriendlyProjectName(GetSelToDoCtrl())));

				pWindow->Update(&tasks);
			}

			if (!pWindow->IsShowing())
			{
				pWindow->Show();
			}
		}
	}
}

void CToDoListWnd::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CFrameWnd::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	if (!bSysMenu)
	{
		switch (nIndex)
		{
		case FILEALL:
			// insert Min to sys tray if appropriate
			// first make sure this is the file menu
			if (::GetMenuString(*pPopupMenu, ID_EXIT, NULL, 0, MF_BYCOMMAND) != 0)
			{
				BOOL bHasMinToTray = (::GetMenuString(*pPopupMenu, ID_MINIMIZETOTRAY, NULL, 0, MF_BYCOMMAND) != 0);

				if (Prefs().GetSysTrayOption() == STO_ONCLOSE || Prefs().GetSysTrayOption() == STO_ONMINCLOSE)
				{
					if (!bHasMinToTray)
					{
						pPopupMenu->InsertMenu(ID_EXIT, MF_BYCOMMAND, ID_MINIMIZETOTRAY, CEnString(ID_MINIMIZETOTRAY));
					}
				}
				else if (bHasMinToTray) // then remove
				{
					pPopupMenu->DeleteMenu(ID_MINIMIZETOTRAY, MF_BYCOMMAND);
				}
			}
			break;

		case EDITTASK:
			// remove relevant commands from the edit menu
			PrepareEditMenu(pPopupMenu);
			break;

		case SORTTASK:
			// remove relevant commands from the sort menu
			PrepareSortMenu(pPopupMenu);
			break;

		case TOOLS:
			// remove relevant commands from the sort menu
			PrepareSourceControlMenu(pPopupMenu);
			break;
		}
	}
}

void CToDoListWnd::OnViewToolbar()
{
	m_bShowToolbar = !m_bShowToolbar;
	m_toolbar.ShowWindow(m_bShowToolbar ? SW_SHOW : SW_HIDE);
	m_toolbar.EnableWindow(m_bShowToolbar);

	ResizeDlg();
}

void CToDoListWnd::OnUpdateViewToolbar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowToolbar);
}

void CToDoListWnd::AppendTools2Toolbar(BOOL bAppend)
{
	CToolsHelper th(Prefs().GetEnableTDLExtension(), ID_TOOLS_USERTOOL1);
	CUIExtensionUIHelper uih(m_mgrUIExtensions, ID_TOOLS_USEREXT1);

	if (bAppend)
	{
		// then re-add
		CUserToolArray aTools;
		Prefs().GetUserTools(aTools);

		th.AppendToolsToToolbar(aTools, m_toolbar, ID_PREFERENCES);
	}
	else // remove
	{
		th.RemoveToolsFromToolbar(m_toolbar, ID_PREFERENCES);
	}

	// resize toolbar to accept the additional buttons
	ResizeDlg();
}

void CToDoListWnd::OnSort()
{
	// we should get this only when both the toolbar and menu are visible
	ASSERT(m_bShowToolbar);

	GetToDoCtrl().Resort(TRUE);
}

void CToDoListWnd::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos)
{
	CFrameWnd::OnWindowPosChanged(lpwndpos);
}

void CToDoListWnd::RefreshFilterLabelAlignment()
{
	m_filterBar.SetFilterLabelAlignment(!Prefs().GetRightAlignLabels());
}


void CToDoListWnd::OnArchiveCompletedtasks()
{
	CWaitCursor cursor;
	int nSelTDC = GetSelToDoCtrl();

	if (m_mgrToDoCtrls.ArchiveDoneTasks(nSelTDC))
	{
		// auto-reload archive if it's loaded
		CString sArchivePath = m_mgrToDoCtrls.GetArchivePath(nSelTDC);
		int nArchiveTDC = m_mgrToDoCtrls.FindToDoCtrl(sArchivePath);

		if (nArchiveTDC != -1 && m_mgrToDoCtrls.IsLoaded(nArchiveTDC))
		{
			ReloadTaskList(nArchiveTDC, FALSE);
		}

		UpdateCaption();
	}
}

void CToDoListWnd::OnUpdateArchiveCompletedtasks(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	pCmdUI->Enable(!tdc.IsReadOnly() && !m_mgrToDoCtrls.GetArchivePath(tdc.GetFilePath()).IsEmpty());
}

void CToDoListWnd::OnArchiveSelectedTasks()
{
	CWaitCursor cursor;
	int nSelTDC = GetSelToDoCtrl();

	if (m_mgrToDoCtrls.ArchiveSelectedTasks(nSelTDC))
	{
		// auto-reload archive if it's loaded
		CString sArchivePath = m_mgrToDoCtrls.GetArchivePath(nSelTDC);
		int nArchiveTDC = m_mgrToDoCtrls.FindToDoCtrl(sArchivePath);

		if (nArchiveTDC != -1 && m_mgrToDoCtrls.IsLoaded(nArchiveTDC))
		{
			ReloadTaskList(nArchiveTDC, FALSE);
		}

		UpdateCaption();
	}
}

void CToDoListWnd::OnUpdateArchiveSelectedCompletedTasks(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	pCmdUI->Enable(!tdc.IsReadOnly() && !m_mgrToDoCtrls.GetArchivePath(tdc.GetFilePath()).IsEmpty());
}

void CToDoListWnd::OnMovetaskdown()
{
	GetToDoCtrl().MoveSelectedTask(TDCM_DOWN);
}

void CToDoListWnd::OnUpdateMovetaskdown(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().CanMoveSelectedTask(TDCM_DOWN));
}

void CToDoListWnd::OnMovetaskup()
{
	GetToDoCtrl().MoveSelectedTask(TDCM_UP);
}

void CToDoListWnd::OnUpdateMovetaskup(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().CanMoveSelectedTask(TDCM_UP));
}

void CToDoListWnd::OnMovetaskright()
{
	GetToDoCtrl().MoveSelectedTask(TDCM_RIGHT);
}

void CToDoListWnd::OnUpdateMovetaskright(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().CanMoveSelectedTask(TDCM_RIGHT));
}

void CToDoListWnd::OnMovetaskleft()
{
	GetToDoCtrl().MoveSelectedTask(TDCM_LEFT);
}

void CToDoListWnd::OnUpdateMovetaskleft(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().CanMoveSelectedTask(TDCM_LEFT));
}

CFilteredToDoCtrl& CToDoListWnd::GetToDoCtrl()
{
	return GetToDoCtrl(GetSelToDoCtrl());
}

CFilteredToDoCtrl& CToDoListWnd::GetToDoCtrl(int nIndex)
{
	return m_mgrToDoCtrls.GetToDoCtrl(nIndex);
}

const CFilteredToDoCtrl& CToDoListWnd::GetToDoCtrl() const
{
	return GetToDoCtrl(GetSelToDoCtrl());
}

const CFilteredToDoCtrl& CToDoListWnd::GetToDoCtrl(int nIndex) const
{
	return m_mgrToDoCtrls.GetToDoCtrl(nIndex);
}

CFilteredToDoCtrl* CToDoListWnd::NewToDoCtrl()
{
	CFilteredToDoCtrl* pCtrl = NULL;

	// if the active tasklist is untitled and unmodified then reuse it
	if (GetTDCCount())
	{
		int nSel = GetSelToDoCtrl();
		CFilteredToDoCtrl& tdc = GetToDoCtrl();

		// make sure that we don't accidently reuse a just edited tasklist
		tdc.Flush();

		if (m_mgrToDoCtrls.IsPristine(nSel))
		{
			pCtrl = &tdc;
			m_mgrToDoCtrls.RemoveToDoCtrl(nSel, FALSE);

			// make sure we reset the selection to a valid index
			if (GetTDCCount())
			{
				// try leaving the selection as-is
				if (nSel >= GetTDCCount())
				{
					nSel--;   // try the preceding item
				}

				SelectToDoCtrl(nSel, FALSE);
			}

			return pCtrl;
		}
	}

	// else
	pCtrl = new CFilteredToDoCtrl(m_mgrContent, Prefs().GetDefaultCommentsFormat());

	if (pCtrl && CreateToDoCtrl(pCtrl))
	{
		return pCtrl;
	}

	// else
	delete pCtrl;
	return NULL;
}

BOOL CToDoListWnd::CreateToDoCtrl(CFilteredToDoCtrl* pCtrl)
{
	// create somewhere out in space
	CRect rCtrl(-32010, -32010, -32000, -32000);

	if (pCtrl->Create(rCtrl, this, IDC_TODOLIST, FALSE))
	{
		// set font to our font
		CDialogHelper::SetFont(pCtrl, m_fontMain, FALSE);

		if (!m_ilToDoCtrl.GetSafeHandle())
		{
			InitCheckboxImageList();
		}

		UpdateToDoCtrlPreferences(pCtrl);

		if (CThemed::IsThemeActive())
		{
			pCtrl->SetUITheme(m_theme);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CToDoListWnd::InitCheckboxImageList()
{
	if (m_ilToDoCtrl.GetSafeHandle())
	{
		return TRUE;
	}

	const int nStates[] =
	{
		-1,
		CBS_UNCHECKEDNORMAL,
		CBS_CHECKEDNORMAL
	};
	const int nNumStates = sizeof(nStates) / sizeof(int);

	CThemed th;

	if (th.Open(this, _T("BUTTON")) && th.AreControlsThemed())
	{
		th.BuildImageList(m_ilToDoCtrl, BP_CHECKBOX, nStates, nNumStates);
	}

	// unthemed + fallback
	if (!m_ilToDoCtrl.GetSafeHandle())
	{
		CBitmap bitmap;
		bitmap.LoadBitmap(IDB_TODO);

		BITMAP BM;
		bitmap.GetBitmap(&BM);

		if (m_ilToDoCtrl.Create(BM.bmWidth / nNumStates, BM.bmHeight, ILC_COLOR32 | ILC_MASK, 0, 1))
		{
			m_ilToDoCtrl.Add(&bitmap, 255);
		}
	}

	return (NULL != m_ilToDoCtrl.GetSafeHandle());
}

void CToDoListWnd::OnMBtnClickTabcontrol(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMTCMBTNCLK* pTCHDR = (NMTCMBTNCLK*)pNMHDR;

	// check valid tab
	if (pTCHDR->iTab >= 0)
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl(pTCHDR->iTab);
		tdc.Flush();

		CloseToDoCtrl(pTCHDR->iTab);

		if (!GetTDCCount())
		{
			CreateNewTaskList(FALSE);
		}
	}
	*pResult = 0;
}

void CToDoListWnd::OnSelchangeTabcontrol(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	// show the incoming selection
	int nCurSel = GetSelToDoCtrl();

	// check password if necessary
	if (m_nLastSelItem != -1 && !VerifyToDoCtrlPassword())
	{
		m_tabCtrl.SetCurSel(m_nLastSelItem);
		return;
	}

	int nDueBy = Prefs().GetNotifyDueByOnSwitch();

	if (nCurSel != -1)
	{
		// make sure it's loaded
		if (!VerifyTaskListOpen(nCurSel, (nDueBy == -1)))
		{
			// restore the previous tab
			m_tabCtrl.SetCurSel(m_nLastSelItem);
			return;
		}

		UpdateToDoCtrlPreferences();

		CFilteredToDoCtrl& tdc = GetToDoCtrl(nCurSel);

		// refresh view state
		tdc.SetMaximizeState(m_nMaxState);

		// make sure size is right
		CRect rTDC;

		if (CalcToDoCtrlRect(rTDC))
		{
			tdc.MoveWindow(rTDC);
		}

		tdc.EnableWindow(TRUE);
		tdc.SetFocusToTasks();
		tdc.ShowWindow(SW_SHOW);
		tdc.PauseTimeTracking(FALSE); // always

		// show ui extension windows
		m_mgrUIExtensions.ShowAllExtensionsWindows((DWORD)tdc.GetSafeHwnd());

		// update the filter selection
		RefreshFilterControls();

		// update status bar
		UpdateStatusbar();
		UpdateCaption();
	}

	// hide the outgoing selection
	if (m_nLastSelItem != -1)
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl(m_nLastSelItem);

		tdc.ShowWindow(SW_HIDE);
		tdc.EnableWindow(FALSE);
		tdc.PauseTimeTracking(!Prefs().GetTrackNonActiveTasklists());

		// hide ui extension windows
		m_mgrUIExtensions.ShowAllExtensionsWindows((DWORD)tdc.GetSafeHwnd(), UIS_HIDE);

		m_nLastSelItem = -1; // reset
	}

	if (nCurSel != -1)
	{
		// notify user of due tasks if req
		DoDueTaskNotification(nDueBy);
	}

	InitMenuIconManager();

	*pResult = 0;
}

void CToDoListWnd::RefreshFilterControls()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	// get existing filter bar size so we can determine if a
	// resize if required
	CRect rFilter;
	m_filterBar.GetClientRect(rFilter);

	m_filterBar.RefreshFilterControls(tdc);

	CRect rClient;
	GetClientRect(rClient);

	if (m_filterBar.CalcHeight(rClient.Width()) != rFilter.Height())
	{
		ResizeDlg();
	}
}

void CToDoListWnd::OnSelchangingTabcontrol(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	// cache the outgoing selection
	m_nLastSelItem = GetSelToDoCtrl();

	// and flush
	if (m_nLastSelItem != -1)
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl(m_nLastSelItem);
		tdc.Flush();

		// and save
		if (Prefs().GetAutoSaveOnSwitchTasklist() && !tdc.GetFilePath().IsEmpty() && tdc.IsModified())
		{
			tdc.Save();

			m_mgrToDoCtrls.SetModifiedStatus(m_nLastSelItem, FALSE);
			m_mgrToDoCtrls.RefreshLastModified(m_nLastSelItem);
			m_mgrToDoCtrls.RefreshReadOnlyStatus(m_nLastSelItem);
			m_mgrToDoCtrls.RefreshPathType(m_nLastSelItem);
		}
	}

	*pResult = 0;
}

TDC_FILE CToDoListWnd::ConfirmSaveTaskList(int nIndex, DWORD dwFlags)
{
	BOOL bClosingWindows = Misc::HasFlag(dwFlags, TDLS_CLOSINGWINDOWS);
	BOOL bClosingTaskList = Misc::HasFlag(dwFlags, TDLS_CLOSINGTASKLISTS) || bClosingWindows; // sanity check
	TDC_FILE nSave = TDCO_SUCCESS;

	// save changes
	CFilteredToDoCtrl& tdc = GetToDoCtrl(nIndex);

	if (tdc.IsModified())
	{
		BOOL bFirstTimeSave = tdc.GetFilePath().IsEmpty();

		if (bClosingTaskList && (bFirstTimeSave || Prefs().GetConfirmSaveOnExit()))
		{
			// make sure app is visible
			Show(FALSE);

			// save tasklist
			CString sName = m_mgrToDoCtrls.GetFriendlyProjectName(nIndex);
			CEnString sMessage(IDS_SAVEBEFORECLOSE, sName);

			// don't allow user to cancel if closing down
			int nRet = MessageBox(sMessage, IDS_CONFIRMSAVE_TITLE, bClosingWindows ? MB_YESNO : MB_YESNOCANCEL);

			if (nRet == IDYES)
			{
				// note: we omit the auto save parameter here because we want the user to
				// be notified of any problems
				nSave = SaveTaskList(nIndex);

				// if the save failed (as opposed to cancelled) then we must
				// propagate this upwards
				if (nSave != TDCO_SUCCESS && nSave != TDCO_CANCELLED)
				{
					return nSave;
				}

				// user can still cancel save dialog even if closing down
				// but not if closing windows
				else if (nSave == TDCO_CANCELLED)
				{
					nRet = bClosingWindows ? IDNO : IDCANCEL;
				}
			}

			ASSERT(!(bClosingWindows && nRet == IDCANCEL));  // sanity check

			if (nRet == IDCANCEL)
			{
				return TDCO_CANCELLED;
			}
			else
			{
				tdc.SetModified(FALSE); // so we don't get prompted again
				m_mgrToDoCtrls.SetModifiedStatus(nIndex, FALSE);
			}
		}
		else
		{
			nSave = SaveTaskList(nIndex, NULL, Misc::HasFlag(dwFlags, TDLS_AUTOSAVE));
		}
	}

	return nSave; // user did not cancel
}

BOOL CToDoListWnd::CloseToDoCtrl(int nIndex)
{
	ASSERT(nIndex >= 0);
	ASSERT(nIndex < GetTDCCount());

	CFilteredToDoCtrl& tdcSel = GetToDoCtrl();
	CFilteredToDoCtrl& tdc = GetToDoCtrl(nIndex);

	tdc.Flush(TRUE);

	if (ConfirmSaveTaskList(nIndex, TDLS_CLOSINGTASKLISTS) != TDCO_SUCCESS)
	{
		return FALSE;
	}

	// remove any find results associated with this tasklist
	if (m_findDlg.GetSafeHwnd())
	{
		m_findDlg.DeleteResults(&tdc);
	}

	CWaitCursor cursor;

	// save off current reminders
	m_reminders.CloseToDoCtrl(tdc);

	// remove any ui extension windows
	m_mgrUIExtensions.RemoveAllExtensionsWindows((DWORD)tdc.GetSafeHwnd());

	int nNewSel = m_mgrToDoCtrls.RemoveToDoCtrl(nIndex, TRUE);

	if (nNewSel != -1)
	{
		// if we're closing TDL then the main window will not
		// be visible at this point so we don't have to worry about
		// encrypted tasklists becoming visible. however if as a result
		// of this closure an encrypted tasklist would become visible
		// then we need to prompt for a password and if this fails
		// we must create another tasklist to hide the encrypted one.
		// unless the tasklist being closed was not active and the
		// new selection hasn't actually changed
		BOOL bCheckPassword = !m_bClosing && (&GetToDoCtrl(nNewSel) != &tdcSel);

		if (!SelectToDoCtrl(nNewSel, bCheckPassword))
		{
			CreateNewTaskList(FALSE);
			RefreshTabOrder();
		}

		if (!m_bClosing)
		{
			ResizeDlg();
		}
	}

	return TRUE;
}

void CToDoListWnd::OnCloseTasklist()
{
	int nSel = GetSelToDoCtrl();
	CFilteredToDoCtrl& tdc = GetToDoCtrl(nSel);

	// make sure there are no edits pending
	tdc.Flush(TRUE);

	// check if its a pristine tasklist and the last tasklist and
	// if so only close it if the default comments type has changed
	if (m_mgrToDoCtrls.IsPristine(nSel) && GetTDCCount() == 1)
	{
		return;
	}

	CloseToDoCtrl(nSel);

	// if empty then create a new dummy item
	if (!GetTDCCount())
	{
		CreateNewTaskList(FALSE);
	}
	else
	{
		ResizeDlg();
	}
}

BOOL CToDoListWnd::SelectToDoCtrl(LPCTSTR szFilePath, BOOL bCheckPassword, int nNotifyDueTasksBy)
{
	int nCtrl = m_mgrToDoCtrls.FindToDoCtrl(szFilePath);

	if (nCtrl != -1)
	{
		SelectToDoCtrl(nCtrl, bCheckPassword, nNotifyDueTasksBy);
		return TRUE;
	}

	return FALSE;
}

int CToDoListWnd::GetSelToDoCtrl() const
{
	if (m_tabCtrl.GetSafeHwnd())
	{
		return m_tabCtrl.GetCurSel();
	}
	else
	{
		return -1;
	}
}

BOOL CToDoListWnd::VerifyTaskListOpen(int nIndex, BOOL bWantNotifyDueTasks)
{
	if (!m_mgrToDoCtrls.IsLoaded(nIndex))
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl(nIndex);

		if (TDCO_SUCCESS == OpenTaskList(&tdc, tdc.GetFilePath()))
		{
			// make sure hidden windows stay hidden
			if (nIndex != GetSelToDoCtrl())
			{
				tdc.ShowWindow(SW_HIDE);
			}

			m_mgrToDoCtrls.SetLoaded(nIndex);
			m_mgrToDoCtrls.UpdateTabItemText(nIndex);

			if (bWantNotifyDueTasks)
			{
				DoDueTaskNotification(&tdc, Prefs().GetNotifyDueByOnLoad());
			}

			return TRUE;
		}

		return FALSE;
	}

	return TRUE;
}

BOOL CToDoListWnd::SelectToDoCtrl(int nIndex, BOOL bCheckPassword, int nNotifyDueTasksBy)
{
	ASSERT(nIndex >= 0);
	ASSERT(nIndex < GetTDCCount());

	// load and show the chosen item
	// we don't need to do a 'open' due task notification if the caller
	// has asked for a notification anyway
	if (!m_bClosing)
	{
		// if the tasklist is not loaded and we verify its loading
		// then we know that the password (if there is one) has been
		// verified and doesn't need checking again
		if (!m_mgrToDoCtrls.IsLoaded(nIndex))
		{
			if (!VerifyTaskListOpen(nIndex, nNotifyDueTasksBy == -1))
			{
				// TODO
				return FALSE;
			}
			else
			{
				bCheckPassword = FALSE;
			}
		}
	}

	// validate password first if necessary
	if (bCheckPassword && !VerifyToDoCtrlPassword(nIndex))
	{
		return FALSE;
	}

	int nCurSel = GetSelToDoCtrl(); // cache this

	// resize tdc first
	CFilteredToDoCtrl& tdc = GetToDoCtrl(nIndex);
	CRect rTDC;

	if (CalcToDoCtrlRect(rTDC))
	{
		tdc.MoveWindow(rTDC);
	}

	m_tabCtrl.SetCurSel(nIndex); // this changes the selected CToDoCtrl
	m_tabCtrl.UpdateWindow();

	if (!m_bClosing)
	{
		UpdateToDoCtrlPreferences();
	}

	const CPreferencesDlg& userPrefs = Prefs();

	tdc.EnableWindow(TRUE);
	tdc.SetFocusToTasks();
	tdc.ShowWindow(SW_SHOW);
	tdc.PauseTimeTracking(FALSE); // always
	tdc.SetMaximizeState(m_nMaxState);

	// if the tasklist is encrypted and todolist always prompts for password
	// then disable Flip3D and Aero Peek
	UpdateAeroFeatures();

	// show ui extension windows
	m_mgrUIExtensions.ShowAllExtensionsWindows((DWORD)tdc.GetSafeHwnd());

	// before hiding the previous selection
	if (nCurSel != -1 && nCurSel != nIndex)
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl(nCurSel);

		tdc.ShowWindow(SW_HIDE);
		tdc.EnableWindow(FALSE);
		tdc.PauseTimeTracking(!userPrefs.GetTrackNonActiveTasklists());

		// hide ui extension windows
		m_mgrUIExtensions.ShowAllExtensionsWindows((DWORD)tdc.GetSafeHwnd(), UIS_HIDE);
	}

	if (!m_bClosing)
	{
		if (userPrefs.GetReadonlyReloadOption() != RO_NO)
		{
			OnTimerReadOnlyStatus(nIndex);
		}

		if (userPrefs.GetTimestampReloadOption() != RO_NO)
		{
			OnTimerTimestampChange(nIndex);
		}

		if (userPrefs.GetEnableSourceControl())
		{
			OnTimerCheckoutStatus(nIndex);
		}

		UpdateCaption();
		UpdateStatusbar();

		// update the filter selection
		RefreshFilterControls();

		// and the menu icon manager
		InitMenuIconManager();

		// and current directory
		UpdateCwd();

		DoDueTaskNotification(nNotifyDueTasksBy);
	}

	return TRUE;
}

void CToDoListWnd::UpdateAeroFeatures()
{
	BOOL bEnable = (!m_bPasswordPrompting || !GetToDoCtrl().IsEncrypted());
}

void CToDoListWnd::UpdateToDoCtrlPreferences()
{
	// check if this has already been done since the last userPrefs change
	int nSel = GetSelToDoCtrl();

	if (m_mgrToDoCtrls.GetNeedsPreferenceUpdate(nSel))
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl(nSel);

		UpdateToDoCtrlPreferences(&tdc);

		// we do column visibility a bit different because
		// the manager knows whether the columns have been fiddled
		// with or not
		CTDCColumnArray aColumns;
		m_mgrToDoCtrls.RefreshColumns(nSel, aColumns);

		// and filter bar relies on this tdc's visible columns
		m_filterBar.SetVisibleFilters(aColumns);

		// reset flag
		m_mgrToDoCtrls.SetNeedsPreferenceUpdate(nSel, FALSE);
	}
}

void CToDoListWnd::UpdateToDoCtrlPreferences(CFilteredToDoCtrl* pCtrl)
{
	CPreferencesDlg& userPrefs = Prefs();
	CFilteredToDoCtrl& tdc = *pCtrl;

	CTDCStylesMap styles;

	styles[TDCS_COLORTEXTBYPRIORITY] = (userPrefs.GetTextColorOption() == COLOROPT_PRIORITY);
	styles[TDCS_COLORTEXTBYCATEGORY] = (userPrefs.GetTextColorOption() == COLOROPT_CATEGORY);
	styles[TDCS_COLORTEXTBYNONE] = (userPrefs.GetTextColorOption() == COLOROPT_NONE);
	styles[TDCS_SHOWINFOTIPS] = userPrefs.GetShowInfoTips();
	styles[TDCS_SHOWCOMMENTSINLIST] = userPrefs.GetShowComments();
	styles[TDCS_TREATSUBCOMPLETEDASDONE] = userPrefs.GetTreatSubCompletedAsDone();
	styles[TDCS_HIDEPERCENTFORDONETASKS] = userPrefs.GetHidePercentForDoneTasks();
	styles[TDCS_HIDESTARTDUEFORDONETASKS] = userPrefs.GetHideStartDueForDoneTasks();
	styles[TDCS_HIDEZEROTIMECOST] = userPrefs.GetHideZeroTimeCost();
	styles[TDCS_CONFIRMDELETE] = userPrefs.GetConfirmDelete();
	styles[TDCS_AVERAGEPERCENTSUBCOMPLETION] = userPrefs.GetAveragePercentSubCompletion();
	styles[TDCS_INCLUDEDONEINAVERAGECALC] = userPrefs.GetIncludeDoneInAverageCalc();
	styles[TDCS_USEEARLIESTDUEDATE] = userPrefs.GetUseEarliestDueDate();
	styles[TDCS_USEPERCENTDONEINTIMEEST] = userPrefs.GetUsePercentDoneInTimeEst();
	styles[TDCS_SHOWCTRLSASCOLUMNS] = userPrefs.GetShowCtrlsAsColumns();
	styles[TDCS_SHOWCOMMENTSALWAYS] = userPrefs.GetShowCommentsAlways();
	styles[TDCS_AUTOREPOSCTRLS] = userPrefs.GetAutoReposCtrls();
	styles[TDCS_SHOWPERCENTASPROGRESSBAR] = userPrefs.GetShowPercentAsProgressbar();
	styles[TDCS_STRIKETHOUGHDONETASKS] = userPrefs.GetStrikethroughDone();
	styles[TDCS_SHOWPATHINHEADER] = userPrefs.GetShowPathInHeader();
	styles[TDCS_FULLROWSELECTION] = userPrefs.GetFullRowSelection();
	styles[TDCS_TREECHECKBOXES] = userPrefs.GetTreeCheckboxes();
	styles[TDCS_TREETASKICONS] = userPrefs.GetTreeTaskIcons();
	styles[TDCS_SORTVISIBLETASKSONLY] = FALSE;//userPrefs.GetSortVisibleOnly();
	styles[TDCS_SHAREDCOMMENTSHEIGHT] = userPrefs.GetSharedCommentsHeight();
	styles[TDCS_TASKCOLORISBACKGROUND] = userPrefs.GetColorTaskBackground();
	styles[TDCS_COMMENTSUSETREEFONT] = userPrefs.GetCommentsUseTreeFont();
	styles[TDCS_SHOWDATESINISO] = userPrefs.GetDisplayDatesInISO();
	styles[TDCS_USEHIGHESTPRIORITY] = userPrefs.GetUseHighestPriority();
	styles[TDCS_USEHIGHESTRISK] = userPrefs.GetUseHighestRisk();
	styles[TDCS_DUEHAVEHIGHESTPRIORITY] = userPrefs.GetDueTasksHaveHighestPriority();
	styles[TDCS_DONEHAVELOWESTPRIORITY] = userPrefs.GetDoneTasksHaveLowestPriority();
	styles[TDCS_DONEHAVELOWESTRISK] = userPrefs.GetDoneTasksHaveLowestRisk();
	styles[TDCS_AUTOCALCTIMEESTIMATES] = userPrefs.GetAutoCalcTimeEstimates();
	styles[TDCS_SHOWWEEKDAYINDATES] = userPrefs.GetShowWeekdayInDates();
	styles[TDCS_ROUNDTIMEFRACTIONS] = userPrefs.GetRoundTimeFractions();
	styles[TDCS_SHOWNONFILEREFSASTEXT] = userPrefs.GetShowNonFilesAsText();
	styles[TDCS_INCLUDEDONEINPRIORITYCALC] = userPrefs.GetIncludeDoneInPriorityRiskCalc();
	styles[TDCS_INCLUDEDONEINRISKCALC] = userPrefs.GetIncludeDoneInPriorityRiskCalc();
	styles[TDCS_WEIGHTPERCENTCALCBYNUMSUB] = userPrefs.GetWeightPercentCompletionByNumSubtasks();
	styles[TDCS_RIGHTALIGNLABELS] = userPrefs.GetRightAlignLabels();
	styles[TDCS_SHOWPARENTSASFOLDERS] = userPrefs.GetShowParentsAsFolders();
	styles[TDCS_FOCUSTREEONENTER] = userPrefs.GetFocusTreeOnEnter();
	styles[TDCS_AUTOCALCPERCENTDONE] = userPrefs.GetAutoCalcPercentDone();
	styles[TDCS_TRACKSELECTEDTASKONLY] = !userPrefs.GetTrackNonSelectedTasks();
	styles[TDCS_HIDEPRIORITYNUMBER] = userPrefs.GetHidePriorityNumber();
	styles[TDCS_SHOWPROJECTNAME] = m_bShowProjectName;
	styles[TDCS_PAUSETIMETRACKINGONSCRNSAVER] = !userPrefs.GetTrackOnScreenSaver();
	styles[TDCS_SHOWFIRSTCOMMENTLINEINLIST] = userPrefs.GetDisplayFirstCommentLine();
	styles[TDCS_DISPLAYHMSTIMEFORMAT] = userPrefs.GetUseHMSTimeFormat();
	styles[TDCS_SORTDONETASKSATBOTTOM] = userPrefs.GetSortDoneTasksAtBottom();
	styles[TDCS_LOGTIMETRACKING] = userPrefs.GetLogTimeTracking();
	styles[TDCS_LOGTASKTIMESEPARATELY] = userPrefs.GetLogTaskTimeSeparately();
	styles[TDCS_WARNADDDELETEARCHIVE] = userPrefs.GetWarnAddDeleteArchive();
	styles[TDCS_SHOWSUBTASKCOMPLETION] = userPrefs.GetShowSubtaskCompletion();
	styles[TDCS_ALLOWPARENTTIMETRACKING] = userPrefs.GetAllowParentTimeTracking();
	styles[TDCS_AUTOADJUSTDEPENDENCYDATES] = userPrefs.GetAutoAdjustDependentsDates();
	styles[TDCS_RIGHTSIDECOLUMNS] = userPrefs.GetShowColumnsOnRight();
	styles[TDCS_HIDEDUETIMEFIELD] = userPrefs.GetHideDueTimeField();
	styles[TDCS_HIDESTARTTIMEFIELD] = userPrefs.GetHideStartTimeField();
	styles[TDCS_HIDEDONETIMEFIELD] = userPrefs.GetHideDoneTimeField();
	styles[TDCS_USES3RDPARTYSOURCECONTROL] = userPrefs.GetUsing3rdPartySourceControl();
	styles[TDCS_REFILTERONMODIFY] = userPrefs.GetReFilterOnModify();
	styles[TDCS_VERTCOMMENTS] = (userPrefs.GetCommentsPos() == PUIP_RIGHTCOMMENTS);
	styles[TDCS_NODUEDATEISDUETODAY] = userPrefs.GetNoDueDateIsDueToday();
	styles[TDCS_CALCREMAININGTIMEBYDUEDATE] = (userPrefs.GetTimeRemainingCalculation() == PTCP_REMAININGTTIMEISDUEDATE);
	styles[TDCS_CALCREMAININGTIMEBYSPENT] = (userPrefs.GetTimeRemainingCalculation() == PTCP_REMAININGTTIMEISSPENT);
	styles[TDCS_CALCREMAININGTIMEBYPERCENT] = (userPrefs.GetTimeRemainingCalculation() == PTCP_REMAININGTTIMEISPERCENTAGE);
	styles[TDCS_SHOWTREELISTBAR] = m_bShowTreeListBar;
	styles[TDCS_INCLUDEUSERINCHECKOUT] = userPrefs.GetIncludeUserNameInCheckout();

	// source control
	BOOL bSrcControl = m_mgrToDoCtrls.PathSupportsSourceControl(tdc.GetFilePath());

	styles[TDCS_ENABLESOURCECONTROL] = bSrcControl;
	styles[TDCS_CHECKOUTONLOAD] = bSrcControl && userPrefs.GetAutoCheckOut();

	// set the styles in one hit
	tdc.SetStyles(styles);

	// info tips
	tdc.SetMaxInfotipCommentsLength(userPrefs.GetMaxInfoTipCommentsLength());

	// update default task preferences
	TODOITEM tdi;

	tdi.sTitle = CEnString(IDS_TASK);
	tdi.color = userPrefs.GetDefaultColor();
	tdi.dateStart.m_dt = userPrefs.GetAutoDefaultStartDate() ? -1 : 0;
	tdi.sAllocBy = userPrefs.GetDefaultAllocBy();
	tdi.sStatus = userPrefs.GetDefaultStatus();
	tdi.dTimeEstimate = userPrefs.GetDefaultTimeEst(tdi.nTimeEstUnits);
	tdi.dTimeSpent = userPrefs.GetDefaultTimeSpent(tdi.nTimeSpentUnits);
	tdi.nTimeSpentUnits = tdi.nTimeEstUnits; // to match
	tdi.sCreatedBy = userPrefs.GetDefaultCreatedBy();
	tdi.dCost = userPrefs.GetDefaultCost();
	userPrefs.GetDefaultCategories(tdi.aCategories);
	userPrefs.GetDefaultAllocTo(tdi.aAllocTo);
	tdi.sCommentsTypeID = userPrefs.GetDefaultCommentsFormat();
	tdi.nPriority = userPrefs.GetDefaultPriority();
	tdi.nRisk = userPrefs.GetDefaultRisk();

	tdc.SetDefaultTaskAttributes(tdi);

	// default string lists
	CStringArray aItems;

	if (userPrefs.GetDefaultListItems(PTDP_CATEGORY, aItems))
	{
		aItems.Append(tdi.aCategories);
		tdc.SetDefaultCategoryNames(aItems);
	}

	if (userPrefs.GetDefaultListItems(PTDP_STATUS, aItems))
	{
		tdc.SetDefaultStatusNames(aItems);
	}

	if (userPrefs.GetDefaultListItems(PTDP_ALLOCBY, aItems))
	{
		tdc.SetDefaultAllocByNames(aItems);
	}

	if (userPrefs.GetDefaultListItems(PTDP_ALLOCTO, aItems))
	{
		aItems.Append(tdi.aAllocTo);
		tdc.SetDefaultAllocToNames(aItems);
	}

	CheckMinWidth();

	// fonts
	if (!m_fontTree.GetSafeHandle() || !m_fontComments.GetSafeHandle())
	{
		CString sFaceName;
		int nFontSize;

		if (!m_fontTree.GetSafeHandle() && userPrefs.GetTreeFont(sFaceName, nFontSize))
		{
			m_fontTree.Attach(GraphicsMisc::CreateFont(sFaceName, nFontSize));
		}

		if (!m_fontComments.GetSafeHandle() && userPrefs.GetCommentsFont(sFaceName, nFontSize))
		{
			m_fontComments.Attach(GraphicsMisc::CreateFont(sFaceName, nFontSize));
		}
	}

	tdc.SetTreeFont(m_fontTree);
	tdc.SetCommentsFont(m_fontComments);

	tdc.SetGridlineColor(userPrefs.GetGridlineColor());
	tdc.SetCompletedTaskColor(userPrefs.GetDoneTaskColor());
	tdc.SetAlternateLineColor(userPrefs.GetAlternateLineColor());
	tdc.SetFlaggedTaskColor(userPrefs.GetFlaggedTaskColor());

	tdc.SetMaxColumnWidth(userPrefs.GetMaxColumnWidth());

	// checkbox images
	tdc.SetCheckImageList(m_ilToDoCtrl);

	// colors
	tdc.SetPriorityColors(m_aPriorityColors);

	COLORREF crDue, crDueToday;
	userPrefs.GetDueTaskColors(crDue, crDueToday);

	tdc.SetDueTaskColors(crDue, crDueToday);

	CTDCColorMap mapColors;
	CCatColorArray aCatColors;
	int nColor = userPrefs.GetCategoryColors(aCatColors);

	while (nColor--)
	{
		CATCOLOR& cc = aCatColors[nColor];
		mapColors[cc.sCategory] = cc.color;
	}
	tdc.SetCategoryColors(mapColors);

	// drag drop
	tdc.SetSubtaskDragDropPos(userPrefs.GetNewSubtaskPos() == PUIP_TOP);

	tdc.Flush(); // clear any outstanding issues
}

void CToDoListWnd::UpdateTabSwitchTooltip()
{
	CToolTipCtrl* pTT = m_tabCtrl.GetToolTips();

	if (pTT)
	{
		// get the string for tab switching
		CString sShortcut = m_mgrShortcuts.GetShortcutTextByCmd(ID_VIEW_NEXT);

		if (sShortcut.IsEmpty())
		{
			sShortcut = m_mgrShortcuts.GetShortcutTextByCmd(ID_VIEW_PREV);
		}

		pTT->DelTool(&m_tabCtrl); // always

		if (!sShortcut.IsEmpty())
		{
			CEnString sTip(IDS_TABSWITCHTOOLTIP, sShortcut);
			pTT->AddTool(&m_tabCtrl, sTip);
		}
	}
}

void CToDoListWnd::OnSaveall()
{
	SaveAll(TDLS_INCLUDEUNSAVED | TDLS_FLUSH);
}

void CToDoListWnd::OnUpdateSaveall(CCmdUI* pCmdUI)
{
	int nCtrl = GetTDCCount();

	while (nCtrl--)
	{
		if (GetToDoCtrl(nCtrl).IsModified())
		{
			pCmdUI->Enable();
			return;
		}
	}

	// else nothing modified
	pCmdUI->Enable(FALSE);
}

void CToDoListWnd::OnCloseall()
{
	// save first
	TDC_FILE nSaveAll = SaveAll(TDLS_INCLUDEUNSAVED | TDLS_CLOSINGTASKLISTS | TDLS_FLUSH);

	if (nSaveAll != TDCO_SUCCESS)
	{
		return;
	}

	// remove tasklists
	int nCtrl = GetTDCCount();

	while (nCtrl--)
	{
		m_mgrToDoCtrls.RemoveToDoCtrl(nCtrl, TRUE);
	}

	// if empty then create a new dummy item
	if (!GetTDCCount())
	{
		CreateNewTaskList(FALSE);
	}
	else
	{
		ResizeDlg();
	}
}

void CToDoListWnd::OnUpdateCloseall(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetTDCCount());
}

void CToDoListWnd::OnExit()
{
	DoExit();
}

void CToDoListWnd::OnMinimizeToTray()
{
	MinimizeToTray();
}

void CToDoListWnd::DoExit()
{
	// save all first to ensure new tasklists get reloaded on startup
	TDC_FILE nSaveAll = SaveAll(TDLS_INCLUDEUNSAVED | TDLS_CLOSINGTASKLISTS | TDLS_FLUSH);

	if (nSaveAll != TDCO_SUCCESS)
	{
		return;   // user cancelled
	}

	SaveSettings(); // before we close the tasklists

	m_bClosing = TRUE;

	// hide thw window as soon as possible so users do not
	// see the machinations of closing down
	if (m_bFindShowing)
	{
		m_findDlg.ShowWindow(SW_HIDE);
	}

	ShowWindow(SW_HIDE);

	// remove tasklists
	int nCtrl = GetTDCCount();

	while (nCtrl--)
	{
		VERIFY(CloseToDoCtrl(nCtrl));   // shouldn't fail now
	}

	// if there are any still open then the user must have cancelled else destroy the window
	ASSERT(!GetTDCCount());

	if (!GetTDCCount())
	{
		DestroyWindow();
	}
	else
	{
		m_bClosing = FALSE;
	}
}

void CToDoListWnd::OnUpdateMovetask(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	pCmdUI->Enable(!tdc.IsReadOnly() && nSelCount == 1);
}

void CToDoListWnd::OnImportTasklist()
{
	CString sImportPath;
	BOOL bImportTaskList = FALSE;
	TDLID_IMPORTTO nImportTo = TDIT_NEWTASKLIST;
	int nImporter = -1;

	do
	{
		CTDLImportDialog dialog(m_mgrImportExport);

		if (dialog.DoModal() == IDOK)
		{
			// check file can be opened
			bImportTaskList = dialog.WantImportTasklist();
			nImportTo = dialog.GetImportTo();
			nImporter = dialog.GetImporterIndex();

			if (dialog.GetImportFromClipboard())
			{
				sImportPath = FileMisc::GetTempFileName(_T("ToDoList.import"), _T("txt"));
				FileMisc::SaveFile(sImportPath, dialog.GetImportClipboardText());
			}
			else
			{
				sImportPath = dialog.GetImportFilePath();
			}

			// check file accessibility
			if (FileMisc::CanOpenFile(sImportPath, TRUE))
			{
				break;
			}

			// else
			MessageBox(CEnString(IDS_IMPORTFILE_CANTOPEN, sImportPath), IDS_IMPORTTASKLIST_TITLE);
			sImportPath.Empty();
		}
		else // cancel
		{
			return;
		}
	}
	while (sImportPath.IsEmpty());

	// load/import tasks
	CTaskFile tasks;
	BOOL bCancel = FALSE;
	CWaitCursor wait;

	// do the import
	if (bImportTaskList)
	{
		if (!tasks.Load(sImportPath))
		{
			CEnString sMessage(IDS_INVALIDTASKLIST, sImportPath);
			AfxMessageBox(sMessage, MB_OK);
			return;
		}
	}
	else
	{
		bCancel = !m_mgrImportExport.ImportTaskList(sImportPath, &tasks, nImporter);
	}

	if (bCancel)
	{
		return;
	}

	if (!tasks.GetTaskCount())
	{
		// notify user
		AfxMessageBox(IDS_NOTASKSIMPORTED);
	}
	else
	{
		if (nImportTo == TDIT_NEWTASKLIST)
		{
			VERIFY(CreateNewTaskList(FALSE));
		}

		CFilteredToDoCtrl& tdc = GetToDoCtrl();
		TDC_INSERTWHERE nWhere = (nImportTo == TDIT_SELECTEDTASK) ? TDC_INSERTATTOPOFSELTASK : TDC_INSERTATTOP;

		VERIFY(tdc.InsertTasks(tasks, nWhere));
		m_mgrToDoCtrls.RefreshModifiedStatus(GetSelToDoCtrl());
		UpdateCaption();
	}
}

void CToDoListWnd::OnSetPriority(UINT nCmdID)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	if (!tdc.IsReadOnly() && tdc.HasSelection())
	{
		if (nCmdID == ID_EDIT_SETPRIORITYNONE)
		{
			tdc.SetSelectedTaskPriority(-2);
		}
		else
		{
			tdc.SetSelectedTaskPriority(nCmdID - ID_EDIT_SETPRIORITY0);
		}
	}
}

void CToDoListWnd::OnUpdateSetPriority(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	pCmdUI->Enable(!tdc.IsReadOnly() && nSelCount);

	int nPriority = pCmdUI->m_nID - ID_EDIT_SETPRIORITY0;

	if (pCmdUI->m_nID == ID_EDIT_SETPRIORITYNONE)
	{
		nPriority = -2;
	}

	pCmdUI->SetCheck(nPriority == tdc.GetSelectedTaskPriority());
}

void CToDoListWnd::OnEditSetfileref()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	if (!tdc.IsReadOnly() && tdc.HasSelection())
	{
		CString sFileRef = tdc.GetSelectedTaskFileRef();

		CFileDialog dialog(TRUE, NULL, sFileRef, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, CEnString(IDS_ALLFILEFILTER));
		CEnString sTitle(IDS_SETFILEREF_TITLE);

		dialog.m_ofn.lpstrTitle = (LPCTSTR)sTitle;

		if (dialog.DoModal() == IDOK)
		{
			tdc.SetSelectedTaskFileRef(dialog.GetPathName());
		}
	}
}

void CToDoListWnd::OnUpdateEditSetfileref(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	pCmdUI->Enable(!tdc.IsReadOnly() && tdc.HasSelection());
}

void CToDoListWnd::OnEditOpenfileref()
{
	GetToDoCtrl().GotoSelectedTaskFileRef();
}

void CToDoListWnd::OnUpdateEditOpenfileref(CCmdUI* pCmdUI)
{
	CEnString sFileRef = GetToDoCtrl().GetSelectedTaskFileRef();

	if (sFileRef.IsEmpty())
	{
		pCmdUI->Enable(FALSE);
	}
	else
	{
		pCmdUI->Enable(TRUE);

		// restrict file length to 250 pixels
		CClientDC dc(this);
		sFileRef.FormatDC(&dc, 250, ES_PATH);
		pCmdUI->SetText(sFileRef);
	}
}

LRESULT CToDoListWnd::OnPreferencesDefaultListChange(WPARAM wp, LPARAM lp)
{
	// decode params
	int nList = LOWORD(wp);
	BOOL bAdd = HIWORD(wp);
	LPCTSTR szItem = (LPCTSTR)lp;

	switch (nList)
	{
	case PTDP_ALLOCBY:
		break;

	case PTDP_ALLOCTO:
		break;

	case PTDP_STATUS:
		break;

	case PTDP_CATEGORY:
		break;
	}

	return 0L;
}

LRESULT CToDoListWnd::OnPreferencesTestTool(WPARAM /*wp*/, LPARAM lp)
{
	USERTOOL* pTool = (USERTOOL*)lp;

	if (pTool)
	{
		CToolsHelper th(Prefs().GetEnableTDLExtension(), ID_TOOLS_USERTOOL1);
		CFilteredToDoCtrl& tdc = GetToDoCtrl();

		USERTOOLARGS args;
		args.sTasklist = tdc.GetFilePath();
		args.dwTaskID = tdc.GetSelectedTaskID();
		args.sTaskTitle = tdc.GetSelectedTaskTitle();
		args.sTaskExtID = tdc.GetSelectedTaskExtID();
		args.sTaskComments = tdc.GetSelectedTaskComments();
		args.sTaskFileLink = tdc.GetSelectedTaskFileRef();
		args.sTaskAllocBy = tdc.GetSelectedTaskAllocBy();

		CStringArray aAllocTo;

		if (tdc.GetSelectedTaskAllocTo(aAllocTo))
		{
			args.sTaskAllocTo = Misc::FormatArray(aAllocTo);
		}

		th.TestTool(*pTool, args, &Prefs());
	}

	return 0;
}

LRESULT CToDoListWnd::OnPreferencesClearMRU(WPARAM /*wp*/, LPARAM /*lp*/)
{
	m_mruList.RemoveAll();

	return 0;
}

void CToDoListWnd::PrepareSortMenu(CMenu* pMenu)
{
	if (!Prefs().GetShowEditMenuAsColumns())
	{
		return;
	}

	const CToDoCtrl& tdc = GetToDoCtrl();

	int nCountLastSep = 0;

	for (int nItem = 0; nItem < (int)pMenu->GetMenuItemCount(); nItem++)
	{
		BOOL bDelete = FALSE;
		BOOL bIsSeparator = FALSE;

		UINT nMenuID = pMenu->GetMenuItemID(nItem);

		switch (nMenuID)
		{
		case ID_SORT_BYCOST:
			bDelete = !tdc.IsColumnShowing(TDCC_COST);
			break;
		case ID_SORT_BYFLAG:
			bDelete = !tdc.IsColumnShowing(TDCC_FLAG);
			break;
		case ID_SORT_BYDONEDATE:
			bDelete = !tdc.IsColumnShowing(TDCC_DONEDATE);
			break;
		case ID_SORT_BYPRIORITY:
			bDelete = !tdc.IsColumnShowing(TDCC_PRIORITY);
			break;
		case ID_SORT_BYPERCENT:
			bDelete = !tdc.IsColumnShowing(TDCC_PERCENT);
			break;
		case ID_SORT_BYSTARTDATE:
			bDelete = !tdc.IsColumnShowing(TDCC_STARTDATE);
			break;
		case ID_SORT_BYDUEDATE:
			bDelete = !tdc.IsColumnShowing(TDCC_DUEDATE);
			break;
		case ID_SORT_BYTIMEEST:
			bDelete = !tdc.IsColumnShowing(TDCC_TIMEEST);
			break;
		case ID_SORT_BYID:
			bDelete = !tdc.IsColumnShowing(TDCC_ID);
			break;
		case ID_SORT_BYDONE:
			bDelete = !tdc.IsColumnShowing(TDCC_DONE);
			break;
		case ID_SORT_BYALLOCBY:
			bDelete = !tdc.IsColumnShowing(TDCC_ALLOCBY);
			break;
		case ID_SORT_BYSTATUS:
			bDelete = !tdc.IsColumnShowing(TDCC_STATUS);
			break;
		case ID_SORT_BYCATEGORY:
			bDelete = !tdc.IsColumnShowing(TDCC_CATEGORY);
			break;
		case ID_SORT_BYTIMESPENT:
			bDelete = !tdc.IsColumnShowing(TDCC_TIMESPENT);
			break;
		case ID_SORT_BYALLOCTO:
			bDelete = !tdc.IsColumnShowing(TDCC_ALLOCTO);
			break;
		case ID_SORT_BYCREATIONDATE:
			bDelete = !tdc.IsColumnShowing(TDCC_CREATIONDATE);
			break;
		case ID_SORT_BYCREATEDBY:
			bDelete = !tdc.IsColumnShowing(TDCC_CREATEDBY);
			break;
		case ID_SORT_BYMODIFYDATE:
			bDelete = !tdc.IsColumnShowing(TDCC_LASTMOD);
			break;
		case ID_SORT_BYRISK:
			bDelete = !tdc.IsColumnShowing(TDCC_RISK);
			break;
		case ID_SORT_BYEXTERNALID:
			bDelete = !tdc.IsColumnShowing(TDCC_EXTERNALID);
			break;
		case ID_SORT_BYVERSION:
			bDelete = !tdc.IsColumnShowing(TDCC_VERSION);
			break;
		case ID_SORT_BYRECURRENCE:
			bDelete = !tdc.IsColumnShowing(TDCC_RECURRENCE);
			break;
		case ID_SORT_BYREMAINING:
			bDelete = !tdc.IsColumnShowing(TDCC_REMAINING);
			break;
		case ID_SORT_BYRECENTEDIT:
			bDelete = !tdc.IsColumnShowing(TDCC_RECENTEDIT);
			break;
		case ID_SORT_BYICON:
			bDelete = !tdc.IsColumnShowing(TDCC_ICON);
			break;
		case ID_SORT_BYFILEREF:
			bDelete = !tdc.IsColumnShowing(TDCC_FILEREF);
			break;
		case ID_SORT_BYTIMETRACKING:
			bDelete = !tdc.IsColumnShowing(TDCC_TRACKTIME);
			break;

		case ID_SEPARATOR:
			bIsSeparator = TRUE;
			bDelete = (nCountLastSep == 0);
			nCountLastSep = 0;
			break;

		default:
			bDelete = FALSE;
			break;
		}

		// delete the item else increment the count since the last separator
		if (bDelete)
		{
			pMenu->DeleteMenu(nItem, MF_BYPOSITION);
			nItem--;
		}
		else if (!bIsSeparator)
		{
			nCountLastSep++;
		}
	}
}

void CToDoListWnd::PrepareSourceControlMenu(CMenu* pMenu)
{
	if (Prefs().GetEnableSourceControl())
	{
		return;
	}

	int nCountLastSep = 0;

	for (int nItem = 0; nItem < (int)pMenu->GetMenuItemCount(); nItem++)
	{
		BOOL bDelete = FALSE;
		BOOL bIsSeparator = FALSE;

		UINT nMenuID = pMenu->GetMenuItemID(nItem);

		switch (nMenuID)
		{
		case -1: // its a popup so recursively handle it
			{
				CMenu* pPopup = pMenu->GetSubMenu(nItem);
				PrepareEditMenu(pPopup);

				// if the popup is now empty remove it too
				bDelete = !pPopup->GetMenuItemCount();
			}
			break;

		case ID_TOOLS_CHECKIN:
		case ID_TOOLS_CHECKOUT:
			bDelete = TRUE;
			break;

		case ID_SEPARATOR:
			bIsSeparator = TRUE;
			bDelete = (nCountLastSep == 0);
			nCountLastSep = 0;
			break;

		default:
			bDelete = FALSE;
			break;
		}

		// delete the item else increment the count since the last separator
		if (bDelete)
		{
			pMenu->DeleteMenu(nItem, MF_BYPOSITION);
			nItem--;
		}
		else if (!bIsSeparator)
		{
			nCountLastSep++;
		}
	}
}

void CToDoListWnd::PrepareEditMenu(CMenu* pMenu)
{
	if (!Prefs().GetShowEditMenuAsColumns())
	{
		return;
	}

	const CToDoCtrl& tdc = GetToDoCtrl();

	int nCountLastSep = 0;

	for (int nItem = 0; nItem < (int)pMenu->GetMenuItemCount(); nItem++)
	{
		BOOL bDelete = FALSE;
		BOOL bIsSeparator = FALSE;

		UINT nMenuID = pMenu->GetMenuItemID(nItem);

		switch (nMenuID)
		{
		case -1: // its a popup so recursively handle it
			{
				CMenu* pPopup = pMenu->GetSubMenu(nItem);

				if (pPopup)
				{
					PrepareEditMenu(pPopup);

					// if the popup is now empty remove it too
					bDelete = !pPopup->GetMenuItemCount();
				}
			}
			break;

		case ID_EDIT_TASKCOLOR:
		case ID_EDIT_CLEARTASKCOLOR:
			bDelete = (Prefs().GetTextColorOption() != COLOROPT_DEFAULT);
			break;

		case ID_EDIT_DECTASKPRIORITY:
		case ID_EDIT_INCTASKPRIORITY:
		case ID_EDIT_SETPRIORITYNONE:
		case ID_EDIT_SETPRIORITY0:
		case ID_EDIT_SETPRIORITY1:
		case ID_EDIT_SETPRIORITY2:
		case ID_EDIT_SETPRIORITY3:
		case ID_EDIT_SETPRIORITY4:
		case ID_EDIT_SETPRIORITY5:
		case ID_EDIT_SETPRIORITY6:
		case ID_EDIT_SETPRIORITY7:
		case ID_EDIT_SETPRIORITY8:
		case ID_EDIT_SETPRIORITY9:
		case ID_EDIT_SETPRIORITY10:
			bDelete = !tdc.IsColumnShowing(TDCC_PRIORITY);
			break;

		case ID_EDIT_OFFSETDATES:
			bDelete = !(tdc.IsColumnShowing(TDCC_STARTDATE) ||
				tdc.IsColumnShowing(TDCC_DUEDATE) ||
				tdc.IsColumnShowing(TDCC_DONEDATE));
			break;

		case ID_EDIT_CLOCK_TASK:
			bDelete = !(tdc.IsColumnShowing(TDCC_TRACKTIME) ||
				tdc.IsColumnShowing(TDCC_TIMESPENT));
			break;

		case ID_SHOWTIMELOGFILE:
			bDelete = !((tdc.IsColumnShowing(TDCC_TRACKTIME) ||
				tdc.IsColumnShowing(TDCC_TIMESPENT)) &&
				Prefs().GetLogTimeTracking());
			break;

		case ID_EDIT_DECTASKPERCENTDONE:
			bDelete = !tdc.IsColumnShowing(TDCC_PERCENT);
			break;
		case ID_EDIT_INCTASKPERCENTDONE:
			bDelete = !tdc.IsColumnShowing(TDCC_PERCENT);
			break;
		case ID_EDIT_OPENFILEREF:
			bDelete = !tdc.IsColumnShowing(TDCC_FILEREF);
			break;
		case ID_EDIT_SETFILEREF:
			bDelete = !tdc.IsColumnShowing(TDCC_FILEREF);
			break;
		case ID_EDIT_FLAGTASK:
			bDelete = !tdc.IsColumnShowing(TDCC_FLAG);
			break;

		case ID_EDIT_SETTASKICON:
		case ID_EDIT_CLEARTASKICON:
			bDelete = !(tdc.IsColumnShowing(TDCC_ICON) || Prefs().GetTreeTaskIcons());
			break;

		case ID_SEPARATOR:
			bIsSeparator = TRUE;
			bDelete = (nCountLastSep == 0);
			nCountLastSep = 0;
			break;

		default:
			bDelete = FALSE;
			break;
		}

		// delete the item else increment the count since the last separator
		if (bDelete)
		{
			pMenu->DeleteMenu(nItem, MF_BYPOSITION);
			nItem--;
		}
		else if (!bIsSeparator)
		{
			nCountLastSep++;
		}
	}

	// make sure last item is not a separator
	int nLastItem = (int)pMenu->GetMenuItemCount() - 1;

	if (pMenu->GetMenuItemID(nLastItem) == 0)
	{
		pMenu->DeleteMenu(nLastItem, MF_BYPOSITION);
	}
}

void CToDoListWnd::OnViewNext()
{
	if (GetTDCCount() < 2)
	{
		return;
	}

	int nNext = GetSelToDoCtrl() + 1;

	if (nNext >= GetTDCCount())
	{
		nNext = 0;
	}

	SelectToDoCtrl(nNext, TRUE, Prefs().GetNotifyDueByOnSwitch());
}

void CToDoListWnd::OnUpdateViewNext(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetTDCCount() > 1);
}

void CToDoListWnd::OnViewPrev()
{
	if (GetTDCCount() < 2)
	{
		return;
	}

	int nPrev = GetSelToDoCtrl() - 1;

	if (nPrev < 0)
	{
		nPrev = GetTDCCount() - 1;
	}

	SelectToDoCtrl(nPrev, TRUE, Prefs().GetNotifyDueByOnSwitch());
}

void CToDoListWnd::OnUpdateViewPrev(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetTDCCount() > 1);
}

void CToDoListWnd::OnSysCommand(UINT nID, LPARAM lParam)
{
	// we don't minimize if we're going to be hiding to then system tray
	if (nID == SC_MINIMIZE)
	{
		int nSysTrayOption = Prefs().GetSysTrayOption();

		if (nSysTrayOption == STO_ONMINIMIZE || nSysTrayOption == STO_ONMINCLOSE)
		{
			MinimizeToTray();
			return; // eat it
		}
	}
	else if (nID == SC_HOTKEY)
	{
		Show(FALSE);
	}

	// don't allow closing whilst reloading tasklists
	else if (nID == SC_CLOSE && m_bReloading)
	{
		return;
	}

	// else
	CFrameWnd::OnSysCommand(nID, lParam);
}

void CToDoListWnd::OnUpdateImport(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!GetToDoCtrl().IsReadOnly());
}

UINT CToDoListWnd::MapNewTaskPos(int nPos, BOOL bSubtask)
{
	if (!bSubtask) // task
	{
		switch (nPos)
		{
		case PUIP_TOP:
			return ID_NEWTASK_ATTOP;
		case PUIP_BOTTOM:
			return ID_NEWTASK_ATBOTTOM;
		case PUIP_BELOW:
			return ID_NEWTASK_AFTERSELECTEDTASK;

		case PUIP_ABOVE:
		default:
			return ID_NEWTASK_BEFORESELECTEDTASK;
		}
	}
	else // subtask
	{
		if (nPos == PUIP_BOTTOM)
		{
			return ID_NEWSUBTASK_ATBOTTOM;
		}
		else
		{
			return ID_NEWSUBTASK_ATTOP;
		}
	}
}

UINT CToDoListWnd::GetNewTaskCmdID()
{
	return MapNewTaskPos(Prefs().GetNewTaskPos(), FALSE);
}

UINT CToDoListWnd::GetNewSubtaskCmdID()
{
	return MapNewTaskPos(Prefs().GetNewSubtaskPos(), TRUE);
}

void CToDoListWnd::OnNewtask()
{
	// convert to users choice
	SendMessage(WM_COMMAND, GetNewTaskCmdID());
}

void CToDoListWnd::OnUpdateNewtask(CCmdUI* pCmdUI)
{
	switch (GetNewTaskCmdID())
	{
	case ID_NEWTASK_ATTOPSELECTED:
		OnUpdateNewtaskAttopSelected(pCmdUI);
		break;

	case ID_NEWTASK_ATBOTTOMSELECTED:
		OnUpdateNewtaskAtbottomSelected(pCmdUI);
		break;

	case ID_NEWTASK_AFTERSELECTEDTASK:
		OnUpdateNewtaskAfterselectedtask(pCmdUI);
		break;

	case ID_NEWTASK_BEFORESELECTEDTASK:
		OnUpdateNewtaskBeforeselectedtask(pCmdUI);
		break;

	case ID_NEWTASK_ATTOP:
		OnUpdateNewtaskAttop(pCmdUI);
		break;

	case ID_NEWTASK_ATBOTTOM:
		OnUpdateNewtaskAtbottom(pCmdUI);
		break;
	}
}

void CToDoListWnd::OnNewsubtask()
{
	// convert to users choice
	SendMessage(WM_COMMAND, GetNewSubtaskCmdID());
}

void CToDoListWnd::OnUpdateNewsubtask(CCmdUI* pCmdUI)
{
	switch (GetNewSubtaskCmdID())
	{
	case ID_NEWSUBTASK_ATTOP:
		OnUpdateNewsubtaskAttop(pCmdUI);
		break;

	case ID_NEWSUBTASK_ATBOTTOM:
		OnUpdateNewsubtaskAtBottom(pCmdUI);
		break;
	}
}

void CToDoListWnd::OnToolsCheckout()
{
	int nSel = GetSelToDoCtrl();

	// sanity check
	if (!m_mgrToDoCtrls.CanCheckOut(nSel))
	{
		return;
	}

	CAutoFlag af(m_bSaving, TRUE);

	CString sCheckedOutTo;
	CFilteredToDoCtrl& tdc = GetToDoCtrl(nSel);

	if (tdc.CheckOut(sCheckedOutTo))
	{
		m_mgrToDoCtrls.UpdateToDoCtrlReadOnlyUIState(nSel);
		m_mgrToDoCtrls.SetLastCheckoutStatus(nSel, TRUE);
		m_mgrToDoCtrls.RefreshLastModified(nSel);

		UpdateCaption();

		// update menu icon mgr
		m_mgrMenuIcons.ChangeImageID(ID_TOOLS_CHECKOUT, ID_TOOLS_CHECKIN);
	}
	else // failed
	{
		m_mgrToDoCtrls.SetLastCheckoutStatus(nSel, FALSE);

		// notify user
		CString sMessage;

		if (!sCheckedOutTo.IsEmpty())
		{
			sMessage.Format(IDS_CHECKEDOUTBYOTHER, tdc.GetFilePath(), sCheckedOutTo);
		}
		else
			// if sCheckedOutTo is empty then the error is most likely because
			// the file has been deleted or cannot be opened for editing
		{
			sMessage.Format(IDS_CHECKOUTFAILED, tdc.GetFilePath());
		}

		MessageBox(sMessage, IDS_CHECKOUT_TITLE, MB_OK | MB_ICONEXCLAMATION);
	}
}

void CToDoListWnd::OnUpdateToolsCheckout(CCmdUI* pCmdUI)
{
	int nSel = GetSelToDoCtrl();

	pCmdUI->Enable(m_mgrToDoCtrls.CanCheckOut(nSel));
}

void CToDoListWnd::OnToolsToggleCheckin()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	if (tdc.IsCheckedOut())
	{
		OnToolsCheckin();
	}
	else
	{
		OnToolsCheckout();
	}
}

void CToDoListWnd::OnUpdateToolsToggleCheckin(CCmdUI* pCmdUI)
{
	int nSel = GetSelToDoCtrl();
	BOOL bEnable = m_mgrToDoCtrls.CanCheckInOut(nSel);

	pCmdUI->Enable(bEnable);
	pCmdUI->SetCheck(bEnable && GetToDoCtrl().IsCheckedOut() ? 1 : 0);
}

void CToDoListWnd::OnToolsCheckin()
{
	int nSel = GetSelToDoCtrl();

	// sanity check
	if (!m_mgrToDoCtrls.CanCheckIn(nSel))
	{
		return;
	}

	CAutoFlag af(m_bSaving, TRUE);
	CFilteredToDoCtrl& tdc = GetToDoCtrl(nSel);

	ASSERT(!tdc.GetFilePath().IsEmpty());

	tdc.Flush();

	// save modifications
	TDC_FILE nSave = TDCO_SUCCESS;

	if (tdc.IsModified())
	{
		if (Prefs().GetConfirmSaveOnExit())
		{
			CString sName = m_mgrToDoCtrls.GetFriendlyProjectName(nSel);
			CEnString sMessage(IDS_SAVEBEFORECHECKIN, sName);

			int nRet = MessageBox(sMessage, IDS_CHECKIN_TITLE, MB_YESNOCANCEL);

			switch (nRet)
			{
			case IDYES:
				nSave = SaveTaskList(nSel);
				break;

			case IDNO:
				ReloadTaskList(nSel, FALSE);
				break;

			case IDCANCEL:
				return;
			}
		}
		else
		{
			SaveTaskList(nSel);
		}
	}

	if (nSave == TDCO_SUCCESS && tdc.CheckIn())
	{
		m_mgrToDoCtrls.UpdateToDoCtrlReadOnlyUIState(nSel);
		m_mgrToDoCtrls.RefreshLastModified(nSel);
		UpdateCaption();

		// update menu icon mgr
		m_mgrMenuIcons.ChangeImageID(ID_TOOLS_CHECKIN, ID_TOOLS_CHECKOUT);
	}

	m_mgrToDoCtrls.SetLastCheckoutStatus(nSel, TRUE); // so we won't try to immediately check it out again
}

void CToDoListWnd::OnUpdateToolsCheckin(CCmdUI* pCmdUI)
{
	int nSel = GetSelToDoCtrl();

	pCmdUI->Enable(m_mgrToDoCtrls.CanCheckIn(nSel));
}

void CToDoListWnd::OnExport()
{
	const CPreferencesDlg& userPrefs = Prefs();

	int nTDCCount = GetTDCCount(), nSelTDC = GetSelToDoCtrl();
	ASSERT(nTDCCount >= 1);

	BOOL bShowSubtaskCheckbox = (nTDCCount > 1) || (GetToDoCtrl().GetView() == FTCV_TASKTREE);

	CTDLExportDlg dialog(m_mgrImportExport, nTDCCount == 1, bShowSubtaskCheckbox,
		userPrefs.GetExportVisibleColsOnly(),
		m_mgrToDoCtrls.GetFilePath(nSelTDC, FALSE),
		userPrefs.GetAutoExportFolderPath());

	if (dialog.DoModal() != IDOK)
	{
		return;
	}

	int nFormat = dialog.GetExportFormat();
	BOOL bHtmlComments = (nFormat == EXPTOHTML);
	CString sExportPath = dialog.GetExportPath();

	// export
	CWaitCursor cursor;

	if (nTDCCount == 1 || !dialog.GetExportAllTasklists())
	{
		CTaskFile tasks;

		// Note: don't need to verify password if encrypted tasklist is active
		GetTasks(GetToDoCtrl(), bHtmlComments, FALSE, dialog.GetTaskSelection(), tasks);

		// add project name as report title
		CString sTitle = m_mgrToDoCtrls.GetFriendlyProjectName(nSelTDC);
		tasks.SetReportAttributes(sTitle);

		if (m_mgrImportExport.ExportTaskList(&tasks, sExportPath, nFormat, FALSE))
		{
			// and preview
			if (userPrefs.GetPreviewExport())
			{
				ShellExecute(*this, NULL, sExportPath, NULL, NULL, SW_SHOWNORMAL);
			}
		}
	}
	else // multiple tasklists
	{
		if (dialog.GetExportOneFile())
		{
			CMultiTaskFile taskFiles;

			for (int nCtrl = 0; nCtrl < nTDCCount; nCtrl++)
			{
				// verify password
				if (nCtrl != nSelTDC && !VerifyToDoCtrlPassword(nCtrl))
				{
					continue;
				}

				GetToDoCtrl(nCtrl).LockWindowUpdate();

				// make sure it's loaded
				if (VerifyTaskListOpen(nCtrl, FALSE))
				{
					CTaskFile& tasks = taskFiles.GetTaskFile(nCtrl);
					tasks.Reset();

					GetTasks(GetToDoCtrl(nCtrl), bHtmlComments, FALSE, dialog.GetTaskSelection(), tasks);

					// add project name as report title
					CString sTitle = m_mgrToDoCtrls.GetFriendlyProjectName(nCtrl);
					tasks.SetReportAttributes(sTitle);
				}
				GetToDoCtrl(nCtrl).UnlockWindowUpdate();
			}

			ResizeDlg();

			if (m_mgrImportExport.ExportTaskLists(&taskFiles, sExportPath, nFormat, FALSE))
			{
				// and preview
				if (userPrefs.GetPreviewExport())
				{
					ShellExecute(*this, NULL, sExportPath, NULL, NULL, SW_SHOWNORMAL);
				}
			}
		}
		else // separate files
		{
			CString sExt = m_mgrImportExport.GetExporterFileExtension(nFormat);

			for (int nCtrl = 0; nCtrl < nTDCCount; nCtrl++)
			{
				// verify password
				if (nCtrl != nSelTDC && !VerifyToDoCtrlPassword(nCtrl))
				{
					continue;
				}

				GetToDoCtrl(nCtrl).LockWindowUpdate();

				// make sure it's loaded
				if (VerifyTaskListOpen(nCtrl, FALSE))
				{
					CTaskFile tasks;
					GetTasks(GetToDoCtrl(nCtrl), bHtmlComments, FALSE, dialog.GetTaskSelection(), tasks);

					// add project name as report title
					CString sTitle = m_mgrToDoCtrls.GetFriendlyProjectName(nCtrl);
					tasks.SetReportAttributes(sTitle);

					// build filepath if required (if exporter has an extension)
					CString sFilePath;

					if (!sExt.IsEmpty())
					{
						CString sPath = m_mgrToDoCtrls.GetFilePath(nCtrl);
						CString sFName;

						// if the file has not been saved before we use the tab text
						// and prompt the user to confirm
						if (sPath.IsEmpty())
						{
							sPath = m_mgrToDoCtrls.GetFilePath(nCtrl, FALSE);
							FileMisc::MakePath(sFilePath, NULL, sExportPath, sPath, sExt);

							CFileDialog dlgFile(FALSE, sExt, sFilePath, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
								m_mgrImportExport.GetExporterFileFilter(nFormat));

							if (dlgFile.DoModal() == IDOK)
							{
								sFilePath = dlgFile.GetPathName();
							}
							else
							{
								continue;   // next tasklist
							}
						}
						else
						{
							FileMisc::SplitPath(sPath, NULL, NULL, &sFName);
							FileMisc::MakePath(sFilePath, NULL, sExportPath, sFName, sExt);
						}
					}

					m_mgrImportExport.ExportTaskList(&tasks, sFilePath, nFormat, FALSE);
				}
				GetToDoCtrl(nCtrl).UnlockWindowUpdate();
			}
		}
	}
}

int CToDoListWnd::GetTasks(CFilteredToDoCtrl& tdc, TSD_TASKS nWhatTasks, CTaskFile& tasks) const
{
	TDCGETTASKS filter(TDCGT_ALL);

	return GetTasks(tdc, FALSE, FALSE, nWhatTasks, filter, tasks);
}

int CToDoListWnd::GetTasks(CFilteredToDoCtrl& tdc, BOOL bHtmlComments, BOOL bTransform, TSD_TASKS nWhatTasks,
	TDCGETTASKS& filter, CTaskFile& tasks) const
{
	// preferences
	const CPreferencesDlg& userPrefs = Prefs();

	// project name
	tasks.SetProjectName(ATL::CT2A(tdc.GetFriendlyProjectName()));
	tasks.SetCharSet(userPrefs.GetHtmlCharSet());

	// export flags
	filter.dwFlags |= TDCGTF_FILENAME;

	if (userPrefs.GetDisplayDatesInISO())
	{
		filter.dwFlags |= TDCGTF_ISODATES;
	}

	if (userPrefs.GetExportParentTitleCommentsOnly())
	{
		filter.dwFlags |= TDCGTF_PARENTTITLECOMMENTSONLY;
	}

	if (bHtmlComments)
	{
		filter.dwFlags |= TDCGTF_HTMLCOMMENTS;
	}

	if (bTransform)
	{
		filter.dwFlags |= TDCGTF_TRANSFORM;
	}

	// get the tasks
	tdc.Flush();

	switch (nWhatTasks)
	{
	case TSDT_ALL:
		{
			// if there's a filter present then we toggle it off
			// grab the tasks and then toggle it back on
			BOOL bNeedToggle = (tdc.HasFilter() || tdc.HasCustomFilter());

			if (bNeedToggle)
			{
				::LockWindowUpdate(tdc.GetSafeHwnd());
				tdc.ToggleFilter();
			}

			tdc.GetTasks(tasks, filter);

			if (bNeedToggle)
			{
				tdc.ToggleFilter();
				::LockWindowUpdate(NULL);
			}
		}
		break;

	case TSDT_FILTERED:
		// if no filter is present then this just gets the whole lot
		tdc.GetFilteredTasks(tasks, filter);
		break;

	case TSDT_SELECTED:
		tdc.GetSelectedTasks(tasks, filter);
		break;
	}

	return tasks.GetTaskCount();
}

int CToDoListWnd::GetTasks(CFilteredToDoCtrl& tdc, BOOL bHtmlComments, BOOL bTransform, const CTaskSelectionDlg&
	taskSel, CTaskFile& tasks) const
{
	DWORD dwFlags = 0;
	TSD_TASKS nWhatTasks = taskSel.GetWantWhatTasks();

	if (taskSel.GetWantSelectedTasks() && !taskSel.GetWantSelectedSubtasks())
	{
		dwFlags |= TDCGTF_NOTSUBTASKS;
	}

	TDC_GETTASKS nFilter = TDCGT_ALL;

	// build filter
	if (taskSel.GetWantCompletedTasks() && !taskSel.GetWantInCompleteTasks())
	{
		nFilter = TDCGT_DONE;
	}
	else if (!taskSel.GetWantCompletedTasks() && taskSel.GetWantInCompleteTasks())
	{
		nFilter = TDCGT_NOTDONE;
	}

	TDCGETTASKS filter(nFilter, dwFlags);

	// attributes to export
	switch (taskSel.GetAttributeOption())
	{
	case TSDA_ALL:
		break;

	case TSDA_VISIBLE:
		{
			CTDCColumnArray aCols;
			GetToDoCtrl().GetVisibleColumns(aCols);

			MapColumnsToAttributes(aCols, filter.aAttribs);
			filter.dwFlags |= TDCGTF_CUSTOMATTRIB;
		}
		break;

	case TSDA_CUSTOM:
		taskSel.GetCustomAttributes(filter.aAttribs);
		filter.dwFlags |= TDCGTF_CUSTOMATTRIB;
		break;
	}

	// get the tasks
	return GetTasks(tdc, bHtmlComments, bTransform, nWhatTasks, filter, tasks);
}

void CToDoListWnd::OnUpdateExport(CCmdUI* pCmdUI)
{
	// make sure at least one control has items
	int nCtrl = GetTDCCount();

	while (nCtrl--)
	{
		if (GetToDoCtrl().GetTaskCount())
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}

	// else
	pCmdUI->Enable(FALSE);
}

void CToDoListWnd::OnToolsTransformactivetasklist()
{
	// pass the project name as the title field
	CString sTitle = GetToDoCtrl().GetProjectName();

	BOOL bShowSubtaskCheckbox = (GetToDoCtrl().GetView() == FTCV_TASKTREE);
	CTDLTransformDialog dialog(sTitle, bShowSubtaskCheckbox);

	if (dialog.DoModal() != IDOK)
	{
		return;
	}

	CString sStylesheet = dialog.GetStylesheet();
	sTitle = dialog.GetTitle();

	// output path
	CString sOutputPath(GetToDoCtrl().GetFilePath());
	{
		if (!sOutputPath.IsEmpty())
		{
			FileMisc::ReplaceExtension(sOutputPath, _T("html"));
		}

		CEnString sTitle(IDS_SAVETRANSFORM_TITLE), sFilter(IDS_TRANSFORMFILEFILTER);
		CFileDialog dialog(FALSE, _T("html"), sOutputPath, OFN_OVERWRITEPROMPT, sFilter, this);

		dialog.m_ofn.lpstrTitle = (LPCTSTR)sTitle;

		if (dialog.DoModal() != IDOK)
		{
			return;   // user elected not to proceed
		}

		sOutputPath = dialog.GetPathName();
	}

	// export
	CWaitCursor cursor;
	CTaskFile tasks;
	CString sOutput;

	GetTasks(GetToDoCtrl(), TRUE, TRUE, dialog.GetTaskSelection(), tasks);

	// add title and date
	COleDateTime date;

	if (dialog.GetWantDate())
	{
		date = COleDateTime::GetCurrentTime();
	}

	tasks.SetReportAttributes(sTitle, date);

	if (tasks.Transform(sStylesheet, sOutput, Prefs().GetHtmlCharSet()))
	{
		CFile output;

		if (output.Open(sOutputPath, CFile::modeCreate | CFile::modeWrite))
		{
			output.Write((void*)(LPCTSTR)sOutput, sOutput.GetLength());
			output.Close();

			// and preview
			if (Prefs().GetPreviewExport())
			{
				ShellExecute(*this, NULL, sOutputPath, NULL, NULL, SW_SHOWNORMAL);
			}
		}
	}
}

void CToDoListWnd::OnNexttopleveltask()
{
	GetToDoCtrl().GotoNextTopLevelTask(TDCG_NEXT);
}

void CToDoListWnd::OnPrevtopleveltask()
{
	GetToDoCtrl().GotoNextTopLevelTask(TDCG_PREV);
}

void CToDoListWnd::OnUpdateNexttopleveltask(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().CanGotoNextTopLevelTask(TDCG_NEXT));
}

void CToDoListWnd::OnUpdatePrevtopleveltask(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().CanGotoNextTopLevelTask(TDCG_PREV));
}

void CToDoListWnd::OnGotoNexttask()
{
	GetToDoCtrl().GotoNextTask(TDCG_NEXT);
}

void CToDoListWnd::OnGotoPrevtask()
{
	GetToDoCtrl().GotoNextTask(TDCG_PREV);
}

void CToDoListWnd::OnUpdateGotoPrevtask(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().CanGotoNextTask(TDCG_PREV));
}

void CToDoListWnd::OnUpdateGotoNexttask(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().CanGotoNextTask(TDCG_NEXT));
}
//------------------------------------------------------------------------

void CToDoListWnd::OnFindTasks()
{
	if (!m_findDlg.GetSafeHwnd())
	{
		VERIFY(m_findDlg.Initialize(this));

		if (CThemed::IsThemeActive())
		{
			m_findDlg.SetUITheme(m_theme);
		}
	}

	if (IsWindowVisible())
	{
		// remove results from encrypted tasklists but not from the
		// active tasklist
		if (!m_findDlg.IsWindowVisible())
		{
			int nSelTDC = GetSelToDoCtrl();
			int nTDC = GetTDCCount();

			while (nTDC--)
			{
				const CFilteredToDoCtrl& tdc = GetToDoCtrl(nTDC);

				if (nTDC != nSelTDC && tdc.IsEncrypted())
				{
					m_findDlg.DeleteResults(&tdc);
				}
			}
		}
		m_findDlg.Show();
	}

	m_bFindShowing = TRUE;
}

LRESULT CToDoListWnd::OnFindDlgClose(WPARAM /*wp*/, LPARAM /*lp*/)
{
	m_bFindShowing = FALSE;
	GetToDoCtrl().SetFocusToTasks();

	return 0L;
}

LRESULT CToDoListWnd::OnFindDlgFind(WPARAM /*wp*/, LPARAM /*lp*/)
{
	// set up the search
	BOOL bSearchAll = m_findDlg.GetSearchAllTasklists();
	int nSelTaskList = GetSelToDoCtrl();

	int nFrom = bSearchAll ? 0 : nSelTaskList;
	int nTo = bSearchAll ? GetTDCCount() - 1 : nSelTaskList;

	// search params
	SEARCHPARAMS params;

	if (m_findDlg.GetSearchParams(params))
	{
		int nSel = GetSelToDoCtrl();
		int bFirst = TRUE;

		for (int nCtrl = nFrom; nCtrl <= nTo; nCtrl++)
		{
			// load or verify password unless tasklist is already active
			if (nCtrl != nSel)
			{
				// load if necessary (in which case the password will have been checked)
				if (!m_mgrToDoCtrls.IsLoaded(nCtrl))
				{
					if (!VerifyTaskListOpen(nCtrl, FALSE))
					{
						continue;
					}
				}
				else if (!VerifyToDoCtrlPassword(nCtrl))
				{
					continue;
				}
			}

			CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtrl);
			CResultArray aResults;
			CHoldRedraw hr(m_bFindShowing ? m_findDlg : NULL);

			if (tdc.FindTasks(params, aResults))
			{
				// use tasklist title from tabctrl
				CString sTitle = m_mgrToDoCtrls.GetTabItemText(nCtrl);

				m_findDlg.AddHeaderRow(sTitle, !bFirst);

				for (int nResult = 0; nResult < aResults.GetSize(); nResult++)
				{
					AddFindResult(aResults[nResult], &tdc);
				}

				bFirst = FALSE;
			}
		}
	}

	// auto-select single results
	if (m_findDlg.GetResultCount() == 1 && m_findDlg.GetAutoSelectSingles())
	{
		CFTDResultsArray results;

		m_findDlg.GetAllResults(results);
		ASSERT(results.GetSize() == 1);

		if (OnFindSelectResult(0, (LPARAM)&results[0]))
		{
			m_findDlg.Show(FALSE);
		}
	}
	else
	{
		m_findDlg.SetActiveWindow();
	}

	return 0;
}

void CToDoListWnd::AddFindResult(const SEARCHRESULT& result, const CFilteredToDoCtrl* pTDC)
{
	CString sTitle = pTDC->GetTaskTitle(result.dwID);
	CString sPath;

	m_findDlg.AddResult(result, sTitle, sPath, pTDC);
}

LRESULT CToDoListWnd::OnFindSelectResult(WPARAM /*wp*/, LPARAM lp)
{
	// extract Task ID
	FTDRESULT* pResult = (FTDRESULT*)lp;
	ASSERT(pResult->dwTaskID);

	int nCtrl = m_mgrToDoCtrls.FindToDoCtrl(pResult->pTDC);
	ASSERT(nCtrl != -1);

	if (m_tabCtrl.GetCurSel() != nCtrl)
	{
		if (!SelectToDoCtrl(nCtrl, TRUE))
		{
			return 0L;
		}
	}

	// we can't use pResult->pTDC because it's const
	CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtrl);
	tdc.SetFocusToTasks();

	if (tdc.GetSelectedTaskID() != pResult->dwTaskID)
	{
		tdc.SelectTask(pResult->dwTaskID);
		Invalidate();
		UpdateWindow();
	}

	return 1L;
}

LRESULT CToDoListWnd::OnFindSelectAll(WPARAM /*wp*/, LPARAM /*lp*/)
{
	if (!m_findDlg.GetResultCount())
	{
		return 0;
	}

	CWaitCursor cursor;

	for (int nTDC = 0; nTDC < GetTDCCount(); nTDC++)
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl(nTDC);
		tdc.DeselectAll();

		// collate the taskIDs
		CDWordArray aTaskIDs;
		m_findDlg.GetResultIDs(&tdc, aTaskIDs);

		// select them in one hit
		if (aTaskIDs.GetSize())
		{
			tdc.MultiSelectItems(aTaskIDs, TSHS_SELECT, (nTDC == GetSelToDoCtrl()));
		}
	}

	return 0;
}

LRESULT CToDoListWnd::OnFindApplyAsFilter(WPARAM /*wp*/, LPARAM lp)
{
	SEARCHPARAMS filter;
	m_findDlg.GetSearchParams(filter);

	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.SetCustomFilter(filter, (LPCTSTR)lp);
	tdc.SetFocusToTasks();

	m_filterBar.SetCustomFilter(TRUE, (LPCTSTR)lp);

	return 0;
}

//------------------------------------------------------------------------

LRESULT CToDoListWnd::OnDropFile(WPARAM /*wParam*/, LPARAM lParam)
{
	LPCTSTR szFile = (LPCTSTR)lParam;
	TDC_FILE nRes = OpenTaskList(szFile);

	HandleLoadTasklistError(nRes, szFile);

	return 0;
}

void CToDoListWnd::OnViewMovetasklistright()
{
	m_mgrToDoCtrls.MoveToDoCtrl(GetSelToDoCtrl(), 1);
}

void CToDoListWnd::OnUpdateViewMovetasklistright(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!Prefs().GetKeepTabsOrdered() &&
		m_mgrToDoCtrls.CanMoveToDoCtrl(GetSelToDoCtrl(), 1));
}

void CToDoListWnd::OnViewMovetasklistleft()
{
	m_mgrToDoCtrls.MoveToDoCtrl(GetSelToDoCtrl(), -1);
}

void CToDoListWnd::OnUpdateViewMovetasklistleft(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!Prefs().GetKeepTabsOrdered() &&
		m_mgrToDoCtrls.CanMoveToDoCtrl(GetSelToDoCtrl(), -1));
}

void CToDoListWnd::OnToolsShowtasksDue(UINT nCmdID)
{
	int nDueBy = PFP_DUETODAY;
	UINT nIDDueBy = IDS_NODUETODAY;

	switch (nCmdID)
	{
	case ID_TOOLS_SHOWTASKS_DUETODAY:
		break; // done

	case ID_TOOLS_SHOWTASKS_DUETOMORROW:
		nIDDueBy = IDS_NODUETOMORROW;
		nDueBy = PFP_DUETOMORROW;
		break;

	case ID_TOOLS_SHOWTASKS_DUEENDTHISWEEK:
		nIDDueBy = IDS_NODUETHISWEEK;
		nDueBy = PFP_DUETHISWEEK;
		break;

	case ID_TOOLS_SHOWTASKS_DUEENDNEXTWEEK:
		nIDDueBy = IDS_NODUENEXTWEEK;
		nDueBy = PFP_DUENEXTWEEK;
		break;

	case ID_TOOLS_SHOWTASKS_DUEENDTHISMONTH:
		nIDDueBy = IDS_NODUETHISMONTH;
		nDueBy = PFP_DUETHISMONTH;
		break;

	case ID_TOOLS_SHOWTASKS_DUEENDNEXTMONTH:
		nIDDueBy = IDS_NODUENEXTMONTH;
		nDueBy = PFP_DUENEXTMONTH;
		break;

	default:
		ASSERT(0);
		return;
	}

	if (!DoDueTaskNotification(nDueBy))
	{
		CEnString sMessage(nIDDueBy, m_mgrToDoCtrls.GetFriendlyProjectName(GetSelToDoCtrl()));
		AfxMessageBox(sMessage);
	}
}

void CToDoListWnd::ResetPrefs()
{
	delete m_pPrefs;
	m_pPrefs = new CPreferencesDlg(&m_mgrShortcuts, IDR_MAINFRAME, &m_mgrContent, &m_mgrImportExport);

	// update
	m_mgrToDoCtrls.SetPrefs(m_pPrefs);

	// grab current colors
	Prefs().GetPriorityColors(m_aPriorityColors);

	m_filterBar.SetPriorityColors(m_aPriorityColors);
}

CPreferencesDlg& CToDoListWnd::Prefs() const
{
	ASSERT(m_pPrefs);
	return *m_pPrefs;
}

void CToDoListWnd::OnNewtaskAttop()
{
	VERIFY(NewTask(CEnString(IDS_TASK), TDC_INSERTATTOP));
}

void CToDoListWnd::OnUpdateNewtaskAttop(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	pCmdUI->Enable(!tdc.IsReadOnly());
}

void CToDoListWnd::OnNewtaskAtbottom()
{
	VERIFY(NewTask(CEnString(IDS_TASK), TDC_INSERTATBOTTOM));
}

void CToDoListWnd::OnUpdateNewtaskAtbottom(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	pCmdUI->Enable(!tdc.IsReadOnly());
}

void CToDoListWnd::OnSpellcheckcomments()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.SpellcheckSelectedTask(FALSE);
}

void CToDoListWnd::OnUpdateSpellcheckcomments(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	pCmdUI->Enable(tdc.CanSpellcheckSelectedTaskComments());
}

void CToDoListWnd::OnSpellchecktitle()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.SpellcheckSelectedTask(TRUE);
}

void CToDoListWnd::OnUpdateSpellchecktitle(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	pCmdUI->Enable(!tdc.GetSelectedTaskTitle().IsEmpty());
}

void CToDoListWnd::OnFileEncrypt()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	if (!tdc.IsReadOnly())
	{
		BOOL bWasEncrypted = tdc.IsEncrypted();
		CString sPassword = tdc.GetPassword();

		// if the tasklist is already encrypted then verify password
		// before allowing change
		if (!bWasEncrypted || VerifyToDoCtrlPassword())
		{
			tdc.EnableEncryption(!tdc.IsEncrypted());
		}

		// make sure we disable encryption on the archive too
		if (bWasEncrypted)
		{
			CString sArchive = m_mgrToDoCtrls.GetArchivePath(GetSelToDoCtrl());

			if (FileMisc::FileExists(sArchive))
			{
				CTaskFile archive(sPassword);

				if (archive.Load(sArchive))
				{
					archive.SetPassword(NULL); // remove password
					archive.Save(sArchive);
				}
			}
		}
	}

	UpdateAeroFeatures();
}

void CToDoListWnd::OnUpdateFileEncrypt(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	pCmdUI->Enable(tdc.CanEncrypt() && !tdc.IsReadOnly());
	pCmdUI->SetCheck(tdc.IsEncrypted() ? 1 : 0);
}

void CToDoListWnd::OnFileResetversion()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	if (!tdc.IsReadOnly())
	{
		tdc.ResetFileVersion();
		tdc.SetModified();

		UpdateStatusbar();
		UpdateCaption();
	}
}

void CToDoListWnd::OnUpdateFileResetversion(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	pCmdUI->Enable(!tdc.IsReadOnly());
}

void CToDoListWnd::OnSpellcheckTasklist()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.Spellcheck();
}

void CToDoListWnd::OnUpdateSpellcheckTasklist(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	pCmdUI->Enable(tdc.GetTaskCount());
}

TDC_FILE CToDoListWnd::SaveAll(DWORD dwFlags)
{
	int nCtrl = GetTDCCount();
	TDC_FILE nSaveAll = TDCO_SUCCESS;

	while (nCtrl--)
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl(nCtrl);

		// bypass unsaved tasklists
		BOOL bIncUnsaved = Misc::HasFlag(dwFlags, TDLS_INCLUDEUNSAVED);

		if (!bIncUnsaved && tdc.GetFilePath().IsEmpty())
		{
			continue;
		}

		if (Misc::HasFlag(dwFlags, TDLS_FLUSH))
		{
			tdc.Flush();
		}

		TDC_FILE nSave = ConfirmSaveTaskList(nCtrl, dwFlags);

		if (nSave == TDCO_CANCELLED)
		{
			return TDCO_CANCELLED;   // user cancelled
		}

		// else cache any failure w/o overwriting previous
		if (nSaveAll == TDCO_SUCCESS)
		{
			nSaveAll = nSave;
		}

		m_mgrToDoCtrls.UpdateTabItemText(nCtrl);
	}

	UpdateCaption();
	return nSaveAll;
}

void CToDoListWnd::OnEditTimeTrackTask()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.TimeTrackSelectedTask();
}

void CToDoListWnd::OnUpdateEditTimeTrackTask(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	pCmdUI->Enable(tdc.CanTimeTrackSelectedTask());
	pCmdUI->SetCheck(tdc.IsSelectedTaskBeingTimeTracked() ? 1 : 0);
}

void CToDoListWnd::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (nIDCtl == IDC_TABCONTROL)
	{
		TDCM_DUESTATUS nStatus = m_mgrToDoCtrls.GetDueItemStatus(lpDrawItemStruct->itemID);

		if (nStatus == TDCM_PAST || nStatus == TDCM_TODAY)
		{
			// determine appropriate due colour
			COLORREF crDue, crDueToday;

			GetToDoCtrl(lpDrawItemStruct->itemID).GetDueTaskColors(crDue, crDueToday);

			COLORREF crTag = (nStatus == TDCM_PAST) ? crDue : crDueToday;

			if (crTag != (COLORREF)-1)
			{
				CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
				const CRect& rect = lpDrawItemStruct->rcItem;

				// draw a little tag in the top left corner
				for (int nHPos = 0; nHPos < 6; nHPos++)
				{
					for (int nVPos = 0; nVPos < 6 - nHPos; nVPos++)
					{
						pDC->SetPixelV(rect.left + nHPos, rect.top + nVPos, crTag);
					}
				}

				// draw a black line between the two
				pDC->SelectStockObject(BLACK_PEN);
				pDC->MoveTo(rect.left, rect.top + 6);
				pDC->LineTo(rect.left + 7, rect.top - 1);
			}
		}
		return;
	}
	else if (nIDCtl == 0 && lpDrawItemStruct->itemID == ID_CLOSE)
	{
		if (m_menubar.DrawMDIButton(lpDrawItemStruct))
		{
			return;
		}
	}

	CFrameWnd::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CToDoListWnd::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (nIDCtl == 0 && lpMeasureItemStruct->itemID == ID_CLOSE)
	{
		if (m_menubar.MeasureMDIButton(lpMeasureItemStruct))
		{
			return;
		}
	}

	CFrameWnd::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CToDoListWnd::OnViewNextSel()
{
	GetToDoCtrl().SelectNextTasksInHistory();
}

void CToDoListWnd::OnUpdateViewNextSel(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().CanSelectNextTasksInHistory());
}

void CToDoListWnd::OnViewPrevSel()
{
	GetToDoCtrl().SelectPrevTasksInHistory();
}

void CToDoListWnd::OnUpdateViewPrevSel(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().CanSelectPrevTasksInHistory());
}

void CToDoListWnd::OnSplitTaskIntoPieces(UINT nCmdID)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nNumPieces = 2 + (nCmdID - ID_NEWTASK_SPLITTASKINTO_TWO);

	tdc.SplitSelectedTask(nNumPieces);
}

void CToDoListWnd::OnUpdateSplitTaskIntoPieces(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	pCmdUI->Enable(tdc.CanSplitSelectedTask());
}

void CToDoListWnd::OnViewExpandtask()
{
	GetToDoCtrl().ExpandSelectedTask(TRUE);
}

void CToDoListWnd::OnUpdateViewExpandtask(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().CanExpandSelectedTask(TRUE));
}

void CToDoListWnd::OnViewCollapsetask()
{
	GetToDoCtrl().ExpandSelectedTask(FALSE);
}

void CToDoListWnd::OnUpdateViewCollapsetask(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().CanExpandSelectedTask(FALSE));
}

void CToDoListWnd::OnViewCollapseall()
{
	GetToDoCtrl().ExpandAllTasks(FALSE);
}

void CToDoListWnd::OnViewExpandall()
{
	GetToDoCtrl().ExpandAllTasks(TRUE);
}

void CToDoListWnd::OnUpdateViewExpandall(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().GetTaskCount());
}

void CToDoListWnd::OnUpdateViewCollapseall(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().GetTaskCount());
}


void CToDoListWnd::OnViewToggletaskexpanded()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	if (tdc.CanExpandSelectedTask(TRUE))
	{
		tdc.ExpandSelectedTask(TRUE);
	}
	else
	{
		tdc.ExpandSelectedTask(FALSE);
	}
}

void CToDoListWnd::OnUpdateViewToggletaskexpanded(CCmdUI* pCmdUI)
{
	const CFilteredToDoCtrl& tdc = GetToDoCtrl();

	pCmdUI->Enable(tdc.CanExpandSelectedTask(TRUE) || tdc.CanExpandSelectedTask(FALSE));
}

void CToDoListWnd::OnWindow(UINT nCmdID)
{
	int nTDC = (int)(nCmdID - ID_WINDOW1);

	if (nTDC < GetTDCCount())
	{
		SelectToDoCtrl(nTDC, (nTDC != GetSelToDoCtrl()), Prefs().GetNotifyDueByOnSwitch());
	}
}

void CToDoListWnd::OnUpdateWindow(CCmdUI* pCmdUI)
{
	if (pCmdUI->m_pMenu)
	{
		ASSERT(ID_WINDOW1 == pCmdUI->m_nID);
		const UINT MAXWINDOWS = 16;
		int nWnd;

		// delete existing tool entries first
		for (nWnd = 0; nWnd < MAXWINDOWS; nWnd++)
		{
			pCmdUI->m_pMenu->DeleteMenu(ID_WINDOW1 + nWnd, MF_BYCOMMAND);
		}

		int nSel = GetSelToDoCtrl();
		int nPos = 0, nTDCCount = GetTDCCount();
		ASSERT(nTDCCount);

		nTDCCount = min(nTDCCount, MAXWINDOWS);

		for (nWnd = 0; nWnd < nTDCCount; nWnd++)
		{
			CFilteredToDoCtrl& tdc = GetToDoCtrl(nWnd);

			CString sMenuItem;
			sMenuItem.Format(_T("&%d %s"), (nPos + 1) % 10, tdc.GetFriendlyProjectName());

			UINT nFlags = MF_BYPOSITION | MF_STRING | (nSel == nWnd ? MF_CHECKED : MF_UNCHECKED);
			pCmdUI->m_pMenu->InsertMenu(pCmdUI->m_nIndex++, nFlags, ID_WINDOW1 + nWnd, sMenuItem);

			nPos++;
		}

		// update end menu count
		pCmdUI->m_nIndex--; // point to last menu added
		pCmdUI->m_nIndexMax = pCmdUI->m_pMenu->GetMenuItemCount();

		pCmdUI->m_bEnableChanged = TRUE;    // all the added items are enabled
	}
}

#if _MSC_VER >= 1400
void CToDoListWnd::OnActivateApp(BOOL bActive, DWORD dwThreadID)
#else
void CToDoListWnd::OnActivateApp(BOOL bActive, HTASK hTask)
#endif
{
	// don't activate when in the middle of loading
	if (m_bReloading && !bActive)
	{
		return;
	}

#if _MSC_VER >= 1400
	CFrameWnd::OnActivateApp(bActive, dwThreadID);
#else
	CFrameWnd::OnActivateApp(bActive, hTask);
#endif

	// don't do any further processing if closing
	if (m_bClosing)
	{
		return;
	}

	if (!bActive)
	{
		// save focus to restore when we next get activated
		m_hwndLastFocus = ::GetFocus();

		// save tasklists if required
		if (Prefs().GetAutoSaveOnSwitchApp())
		{
			SaveAll(TDLS_FLUSH | TDLS_AUTOSAVE);
		}
	}
	else
	{
		if (GetTDCCount() && (!m_hwndLastFocus || Prefs().GetAutoFocusTasklist()))
		{
			PostMessage(WM_APPRESTOREFOCUS, 0L, (LPARAM)GetToDoCtrl().GetSafeHwnd());
		}

		else if (m_hwndLastFocus)
		{
			// delay the restoration of focus else it gets lost
			PostMessage(WM_APPRESTOREFOCUS, 0L, (LPARAM)m_hwndLastFocus);
		}

		UpdateCwd();
	}
}

LRESULT CToDoListWnd::OnAppRestoreFocus(WPARAM /*wp*/, LPARAM lp)
{
	HWND hWnd = (HWND)lp;

	if (GetTDCCount() && hWnd == GetToDoCtrl().GetSafeHwnd())
	{
		GetToDoCtrl().SetFocusToTasks();
	}
	else
	{
		return (LRESULT)::SetFocus(hWnd);
	}

	return 0L;
}

void CToDoListWnd::UpdateCwd()
{
	// set cwd to active tasklist
	if (GetTDCCount())
	{
		CString sFolder = FileMisc::GetFolderFromFilePath(m_mgrToDoCtrls.GetFilePath(GetSelToDoCtrl()));

		if (FileMisc::FolderExists(sFolder))
		{
			SetCurrentDirectory(sFolder);
		}
	}
}

BOOL CToDoListWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UpdateWindow();

	return CFrameWnd::OnCommand(wParam, lParam);
}

void CToDoListWnd::OnEnable(BOOL bEnable)
{
	CFrameWnd::OnEnable(bEnable);

	// save current focus because modal window is being shown
	if (!bEnable)
	{
		m_hwndLastFocus = ::GetFocus();
	}

	// then restore it when we are enabled
	else if (m_hwndLastFocus)
	{
		UpdateWindow();
		PostMessage(WM_APPRESTOREFOCUS, 0L, (LPARAM)m_hwndLastFocus);
	}
}

void CToDoListWnd::OnViewSorttasklisttabs()
{
	int nSel = m_mgrToDoCtrls.SortToDoCtrlsByName();
	SelectToDoCtrl(nSel, FALSE);
}

void CToDoListWnd::OnUpdateViewSorttasklisttabs(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetTDCCount() > 1 && !Prefs().GetKeepTabsOrdered());
}

void CToDoListWnd::OnEditInctaskpercentdone()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.IncrementSelectedTaskPercentDone(5);
}

void CToDoListWnd::OnUpdateEditInctaskpercentdone(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	pCmdUI->Enable(nSelCount && !tdc.IsReadOnly());
}

void CToDoListWnd::OnEditDectaskpercentdone()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.IncrementSelectedTaskPercentDone(-5);
}

void CToDoListWnd::OnUpdateEditDectaskpercentdone(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	pCmdUI->Enable(nSelCount && !tdc.IsReadOnly());
}

void CToDoListWnd::OnEditDectaskpriority()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.IncrementSelectedTaskPriority(-1);
}

void CToDoListWnd::OnUpdateEditDectaskpriority(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	pCmdUI->Enable(nSelCount && !tdc.IsReadOnly());
}

void CToDoListWnd::OnEditInctaskpriority()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.IncrementSelectedTaskPriority(1);
}

void CToDoListWnd::OnUpdateEditInctaskpriority(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	pCmdUI->Enable(nSelCount && !tdc.IsReadOnly());
}

void CToDoListWnd::OnEditFlagtask()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.SetSelectedTaskFlag(!tdc.IsSelectedTaskFlagged());
}

void CToDoListWnd::OnUpdateEditFlagtask(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	pCmdUI->Enable(nSelCount && !tdc.IsReadOnly());
	pCmdUI->SetCheck(tdc.IsSelectedTaskFlagged() ? 1 : 0);
}

void CToDoListWnd::OnFileOpenarchive()
{
	CString sArchivePath = m_mgrToDoCtrls.GetArchivePath(GetSelToDoCtrl());
	BOOL bArchiveExists = FileMisc::FileExists(sArchivePath);

	if (bArchiveExists)
	{
		OpenTaskList(sArchivePath, NULL, FALSE);
	}
}

void CToDoListWnd::OnUpdateFileOpenarchive(CCmdUI* pCmdUI)
{
	CString sArchivePath = m_mgrToDoCtrls.GetArchivePath(GetSelToDoCtrl());
	BOOL bArchiveExists = FileMisc::FileExists(sArchivePath);

	pCmdUI->Enable(bArchiveExists);
}

LRESULT CToDoListWnd::OnSelchangeFilter(WPARAM /*wp*/, LPARAM lp)
{
	FTDCFILTER* pFilter = (FTDCFILTER*)lp;
	ASSERT(pFilter);

	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	FTDCFILTER nPrevFilter;
	tdc.GetFilter(nPrevFilter);

	if (pFilter->nFilter == FT_CUSTOM)
	{
		// did the user just reselect 'Custom'
		if (nPrevFilter.nFilter == FT_CUSTOM)
		{
			tdc.RefreshFilter();
		}
		else
		{
			tdc.RestoreCustomFilter();
		}

		m_filterBar.RefreshFilterControls(tdc);
	}
	else
	{
		m_filterBar.SetCustomFilter(FALSE);
		tdc.SetFilter(*pFilter);
	}

	UpdateStatusbar();

	// update UI plugins
	RefreshUIExtensions(FALSE);

	return 0L;
}

void CToDoListWnd::OnViewShowfilterbar()
{
	m_bShowFilterBar = !m_bShowFilterBar;

	m_filterBar.ShowWindow(m_bShowFilterBar ? SW_SHOW : SW_HIDE);
	ResizeDlg();
}

void CToDoListWnd::OnUpdateViewShowfilterbar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowFilterBar ? 1 : 0);
}

void CToDoListWnd::OnViewClearfilter()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	if (tdc.HasFilter() || tdc.HasCustomFilter())
	{
		tdc.ClearFilter();

		// reenable the filter controls
		m_filterBar.RemoveCustomFilter();
		m_filterBar.RefreshFilterControls(tdc);

		RefreshFilterControls();
		UpdateStatusbar();

		// update UI plugins
		RefreshUIExtensions(FALSE);
	}
}

void CToDoListWnd::OnUpdateViewClearfilter(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	pCmdUI->Enable(tdc.HasFilter() || tdc.HasCustomFilter());
}

void CToDoListWnd::OnViewTogglefilter()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	tdc.ToggleFilter();

	RefreshFilterControls();
	UpdateStatusbar();

	// reenable the filter controls
	m_filterBar.SetCustomFilter(tdc.HasCustomFilter(), tdc.GetCustomFilterName());

	// update UI plugins
	RefreshUIExtensions(FALSE);
}

void CToDoListWnd::OnUpdateViewTogglefilter(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	pCmdUI->Enable(tdc.HasFilter() || tdc.HasLastFilter() || tdc.HasCustomFilter());
}

void CToDoListWnd::OnViewFilter()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	CTDLFilterDlg dialog(Prefs().GetMultiSelFilters());

	if (dialog.DoModal(tdc, m_aPriorityColors) == IDOK)
	{
		FTDCFILTER filter, filterTDC;

		dialog.GetFilter(filter);
		tdc.GetFilter(filterTDC);

		// then set the filter
		if (filter.nFilter == FT_CUSTOM) // must mean that we have a current custom filter
		{
			tdc.RefreshFilter();
		}
		else
		{
			m_filterBar.SetFilter(filter, tdc.GetView());
			tdc.SetFilter(filter);
		}

		UpdateStatusbar();

		// update UI plugins
		RefreshUIExtensions(FALSE);
	}
}

void CToDoListWnd::OnUpdateViewFilter(CCmdUI* pCmdUI)
{
	UNREFERENCED_PARAMETER(pCmdUI);
	//	pCmdUI->Enable(!m_bShowFilterBar);
}

void CToDoListWnd::OnViewRefreshfilter()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	FTDCFILTER filterTDC, filter;

	tdc.GetFilter(filterTDC);
	m_filterBar.GetFilter(filter);

	// if the filter has changed then set the new one else
	// refresh the current one
	if (filterTDC == filter)
	{
		tdc.RefreshFilter();
	}
	else
	{
		tdc.SetFilter(filter);

		if (Prefs().GetExpandTasksOnLoad())
		{
			tdc.ExpandAllTasks();
		}
	}

	UpdateStatusbar();

	// update UI plugins
	RefreshUIExtensions(FALSE);
}

void CToDoListWnd::OnUpdateViewRefreshfilter(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	FTDCFILTER filterTDC, filter;

	tdc.GetFilter(filterTDC);
	m_filterBar.GetFilter(filter);

	pCmdUI->Enable(tdc.HasFilter() || (filter != filterTDC) || tdc.HasCustomFilter());
}

void CToDoListWnd::OnTabctrlPreferences()
{
	DoPreferences(PREFPAGE_UI);
}

void CToDoListWnd::OnTasklistSelectColumns()
{
	CTDCColumnArray aColumns;
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSel = GetSelToDoCtrl();

	tdc.GetVisibleColumns(aColumns);

	CTDLColumnSelectionDlg dialog(aColumns, TRUE);

	if (dialog.DoModal() == IDOK)
	{
		dialog.GetVisibleColumns(aColumns);

		tdc.SetVisibleColumns(aColumns);
		m_filterBar.SetVisibleFilters(aColumns);

		if (dialog.GetApplyActiveTasklist())
		{
			m_mgrToDoCtrls.SetHasOwnColumns(nSel, TRUE);
		}
		else
		{
			// update preferences
			Prefs().SetVisibleColumns(aColumns);

			// and flag other tasklists as requiring updates
			m_mgrToDoCtrls.SetAllNeedPreferenceUpdate(TRUE, nSel);
			m_mgrToDoCtrls.SetHasOwnColumns(nSel, FALSE);
		}

		// reload the menu if we dynamically alter it
		if (Prefs().GetShowEditMenuAsColumns())
		{
			LoadMenubar();
		}

		ResizeDlg();
	}
}

void CToDoListWnd::OnViewProjectname()
{
	m_bShowProjectName = !m_bShowProjectName;

	// mark all tasklists as needing update
	m_mgrToDoCtrls.SetAllNeedPreferenceUpdate(TRUE);

	// then update active tasklist
	GetToDoCtrl().SetStyle(TDCS_SHOWPROJECTNAME, m_bShowProjectName);
	m_mgrToDoCtrls.SetNeedsPreferenceUpdate(GetSelToDoCtrl(), FALSE);
}

void CToDoListWnd::OnUpdateViewProjectname(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowProjectName ? 1 : 0);
}

void CToDoListWnd::OnEditOffsetdates()
{
	COffsetDatesDlg dialog;

	if (dialog.DoModal() == IDOK)
	{
		ODD_UNITS nUnits;
		int nAmount = dialog.GetOffsetAmount(nUnits);

		if (!nAmount)
		{
			return;
		}

		DWORD dwWhat = dialog.GetOffsetWhat();
		BOOL bSubtasks = dialog.GetOffsetSubtasks();

		// translate units
		int nTDCUnits = (nUnits == ODDU_DAYS) ? TDITU_DAYS :
			((nUnits == ODDU_WEEKS) ? TDITU_WEEKS : TDITU_MONTHS);

		// do the offsets
		CFilteredToDoCtrl& tdc = GetToDoCtrl();

		if (dwWhat & ODD_STARTDATE)
		{
			tdc.OffsetSelectedTaskDate(TDCD_START, nAmount, nTDCUnits, bSubtasks);
		}

		if (dwWhat & ODD_DUEDATE)
		{
			tdc.OffsetSelectedTaskDate(TDCD_DUE, nAmount, nTDCUnits, bSubtasks);
		}

		if (dwWhat & ODD_DONEDATE)
		{
			tdc.OffsetSelectedTaskDate(TDCD_DONE, nAmount, nTDCUnits, bSubtasks);
		}
	}
}

void CToDoListWnd::OnUpdateEditOffsetdates(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	pCmdUI->Enable(nSelCount && !tdc.IsReadOnly());
}

void CToDoListWnd::OnPrintpreview()
{
	DoPrint(TRUE);
}

void CToDoListWnd::OnShowTimelogfile()
{
	CString sLogPath = GetToDoCtrl().GetSelectedTaskTimeLogPath();

	if (!sLogPath.IsEmpty())
	{
		ShellExecute(*this, NULL, sLogPath, NULL, NULL, SW_HIDE);
	}
}

void CToDoListWnd::OnUpdateShowTimelogfile(CCmdUI* pCmdUI)
{
	const CPreferencesDlg& userPrefs = Prefs();
	int nTasks = GetToDoCtrl().GetSelectedCount();
	BOOL bEnable = FALSE;

	if (Prefs().GetLogTimeTracking() &&
		(nTasks == 1 || !userPrefs.GetLogTaskTimeSeparately()))
	{
		CString sLogPath = GetToDoCtrl().GetSelectedTaskTimeLogPath();
		bEnable = FileMisc::FileExists(sLogPath);
	}

	pCmdUI->Enable(bEnable);
}

void CToDoListWnd::OnAddtimetologfile()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	DWORD dwTaskID = tdc.GetSelectedTaskID();
	CString sTitle = tdc.GetSelectedTaskTitle();

	CTDLAddLoggedTimeDlg dialog(dwTaskID, sTitle);

	if (dialog.DoModal() == IDOK)
	{
		tdc.AddTimeToTaskLogFile(dwTaskID, dialog.GetLoggedTime(), dialog.GetWhen(), dialog.GetAddToTimeSpent());
	}
}

void CToDoListWnd::OnUpdateAddtimetologfile(CCmdUI* pCmdUI)
{
	const CPreferencesDlg& userPrefs = Prefs();
	int nTasks = GetToDoCtrl().GetSelectedCount();
	BOOL bEnable = FALSE;

	if (Prefs().GetLogTimeTracking() &&
		(nTasks == 1 || !userPrefs.GetLogTaskTimeSeparately()))
	{
		CString sLogPath = GetToDoCtrl().GetSelectedTaskTimeLogPath();
		bEnable = FileMisc::FileExists(sLogPath);
	}

	pCmdUI->Enable(bEnable);
}

LRESULT CToDoListWnd::OnToDoCtrlDoTaskLink(WPARAM wParam, LPARAM lParam)
{
	CString sFile((LPCTSTR)lParam);

	if (!sFile.IsEmpty())
	{
		// build the full path to the file
		CString sFolder = FileMisc::GetFolderFromFilePath(m_mgrToDoCtrls.GetFilePath(GetSelToDoCtrl()));
		FileMisc::MakeFullPath(sFile, sFolder);

		TDC_FILE nRet = OpenTaskList(sFile);

		if (nRet == TDCO_SUCCESS)
		{
			if (wParam > 0)
			{
				GetToDoCtrl().SelectTask(wParam);
			}

			return TRUE;
		}
		else
		{
			HandleLoadTasklistError(nRet, sFile);
		}
	}
	else if (wParam > 0) // select in current tasklist
	{
		GetToDoCtrl().SelectTask(wParam);
	}

	return 0L;
}

LRESULT CToDoListWnd::OnToDoCtrlTaskIsDone(WPARAM wParam, LPARAM lParam)
{
	ASSERT(lParam);
	CString sFile((LPCTSTR)lParam);

	if (!sFile.IsEmpty())
	{
		// build the full path to the file
		if (::PathIsRelative(sFile))
		{
			// append it to the folder containing the active tasklist
			CString sPathName = m_mgrToDoCtrls.GetFilePath(GetSelToDoCtrl());
			CString sDrive, sFolder;

			FileMisc::SplitPath(sPathName, &sDrive, &sFolder);
			FileMisc::MakePath(sFile, sDrive, sFolder, sFile);
		}
		// else its a full path already

		int nTDC = m_mgrToDoCtrls.FindToDoCtrl(sFile);

		if (nTDC != -1) // already loaded
		{
			return GetToDoCtrl(nTDC).IsTaskDone(wParam);
		}
		else
		{
			// we must load the tasklist ourselves
			CTaskFile tasks;

			if (tasks.Load(sFile))
			{
				HTASKITEM ht = tasks.FindTask(wParam);
				return ht ? tasks.IsTaskDone(ht) : FALSE;
			}
		}
	}

	return 0L;
}

LRESULT CToDoListWnd::OnPowerBroadcast(WPARAM wp, LPARAM /*lp*/)
{
	const CPreferencesDlg& userPrefs = Prefs();

	switch (wp)
	{
	case PBT_APMSUSPEND:
	case PBT_APMSTANDBY:
	case PBT_APMQUERYSUSPEND:
	case PBT_APMQUERYSTANDBY:
		// Terminate all timers
		SetTimer(TIMER_DUEITEMS, FALSE);
		SetTimer(TIMER_READONLYSTATUS, FALSE);
		SetTimer(TIMER_TIMESTAMPCHANGE, FALSE);
		SetTimer(TIMER_CHECKOUTSTATUS, FALSE);
		SetTimer(TIMER_AUTOSAVE, FALSE);
		SetTimer(TIMER_TIMETRACKING, FALSE);
		break;

	case PBT_APMQUERYSUSPENDFAILED:
	case PBT_APMQUERYSTANDBYFAILED:
	case PBT_APMRESUMECRITICAL:
	case PBT_APMRESUMESUSPEND:
	case PBT_APMRESUMESTANDBY:
		// reset time tracking as required
		if (!userPrefs.GetTrackHibernated())
		{
			int nCtrl = GetTDCCount();

			while (nCtrl--)
			{
				GetToDoCtrl(nCtrl).ResetTimeTracking();
			}
		}

		// restart timers
		SetTimer(TIMER_DUEITEMS, TRUE);
		SetTimer(TIMER_READONLYSTATUS, userPrefs.GetReadonlyReloadOption() != RO_NO);
		SetTimer(TIMER_TIMESTAMPCHANGE, userPrefs.GetTimestampReloadOption() != RO_NO);
		SetTimer(TIMER_AUTOSAVE, userPrefs.GetAutoSaveFrequency());
		SetTimer(TIMER_CHECKOUTSTATUS, userPrefs.GetCheckoutOnCheckin() || userPrefs.GetCheckinOnNoEditTime());
		SetTimer(TIMER_TIMETRACKING, userPrefs.GetUseSysTray());

		// check for updates
		if (Prefs().GetAutoCheckForUpdates())
		{
			CheckForUpdates(FALSE);
		}
		break;
	}

	return TRUE; // allow
}

LRESULT CToDoListWnd::OnGetFont(WPARAM /*wp*/, LPARAM /*lp*/)
{
	return (LRESULT)m_fontMain.GetSafeHandle();
}

void CToDoListWnd::OnViewStatusBar()
{
	m_bShowStatusBar = !m_bShowStatusBar;
	m_statusBar.ShowWindow(m_bShowStatusBar ? SW_SHOW : SW_HIDE);

	SendMessage(WM_SIZE, SIZE_RESTORED, 0L);

	if (m_bShowStatusBar)
	{
		UpdateStatusbar();
	}
	else
	{
		UpdateCaption();
	}
}

void CToDoListWnd::OnUpdateViewStatusBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowStatusBar ? 1 : 0);
}

BOOL CToDoListWnd::OnQueryOpen()
{
	if (CFrameWnd::OnQueryOpen())
	{
		// fail if the active tasklist is encrypted because we have to verify the password
		// and we're not allowed to display a dialog in this message handler
		if (!m_bQueryOpenAllow && GetToDoCtrl().IsEncrypted())
		{
			PostMessage(WM_TDL_RESTORE);
			return FALSE;
		}

		// all others
		return TRUE;
	}

	return FALSE;
}

LRESULT CToDoListWnd::OnToDoListRestore(WPARAM /*wp*/, LPARAM /*lp*/)
{
	ASSERT(IsIconic() && GetToDoCtrl().IsEncrypted());  // sanity check

	if (IsIconic())
	{
		if (VerifyToDoCtrlPassword())
		{
			CAutoFlag af(m_bQueryOpenAllow, TRUE);
			ShowWindow(SW_RESTORE);
		}
	}

	return 0L;
}

void CToDoListWnd::OnCopyTaskasRef()
{
	CopySelectedTasksToClipboard(CT_ASREF);
}

void CToDoListWnd::OnUpdateCopyTaskasRef(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().GetSelectedCount() == 1);
}

void CToDoListWnd::OnCopyTaskasDependency()
{
	CopySelectedTasksToClipboard(CT_ASDEPENDS);
}

void CToDoListWnd::OnUpdateCopyTaskasDependency(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().GetSelectedCount() == 1);
}

void CToDoListWnd::OnCopyTaskasRefFull()
{
	CopySelectedTasksToClipboard(CT_ASREFFULL);
}

void CToDoListWnd::OnUpdateCopyTaskasRefFull(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().GetSelectedCount() == 1);
}

void CToDoListWnd::OnCopyTaskasDependencyFull()
{
	CopySelectedTasksToClipboard(CT_ASDEPENDSFULL);
}

void CToDoListWnd::OnUpdateCopyTaskasDependencyFull(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().GetSelectedCount() == 1);
}

void CToDoListWnd::OnCopyTaskasPath()
{
	CopySelectedTasksToClipboard(CT_ASPATH);
}

void CToDoListWnd::OnUpdateCopyTaskasPath(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().GetSelectedCount() == 1);
}

BOOL CToDoListWnd::PreCreateWindow(CREATESTRUCT& cs)
{
	if (CFrameWnd::PreCreateWindow(cs))
	{
		cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
		return TRUE;
	}

	// else
	return FALSE;
}

void CToDoListWnd::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos)
{
	CFrameWnd::OnWindowPosChanging(lpwndpos);
}

void CToDoListWnd::OnToolsCheckforupdates()
{
	CheckForUpdates(TRUE);
}

void CToDoListWnd::OnEditInsertdatetime()
{
	COleDateTime date = COleDateTime::GetCurrentTime();
	const CPreferencesDlg& userPrefs = Prefs();

	DWORD dwFmt = DHFD_TIME;

	if (userPrefs.GetShowWeekdayInDates())
	{
		dwFmt |= DHFD_DOW;
	}

	if (userPrefs.GetDisplayDatesInISO())
	{
		dwFmt |= DHFD_ISO;
	}

	CString sDate = CDateHelper::FormatDate(date, dwFmt);
	GetToDoCtrl().PasteText(sDate);
}

void CToDoListWnd::OnEditInsertdate()
{
	COleDateTime date = COleDateTime::GetCurrentTime();
	const CPreferencesDlg& userPrefs = Prefs();

	DWORD dwFmt = 0;

	if (userPrefs.GetShowWeekdayInDates())
	{
		dwFmt |= DHFD_DOW;
	}

	if (userPrefs.GetDisplayDatesInISO())
	{
		dwFmt |= DHFD_ISO;
	}

	CString sDate = CDateHelper::FormatDate(date, dwFmt);
	GetToDoCtrl().PasteText(sDate);
}

void CToDoListWnd::OnEditInserttime()
{
	COleDateTime date = COleDateTime::GetCurrentTime();
	const CPreferencesDlg& userPrefs = Prefs();

	CString sTime;

	if (userPrefs.GetDisplayDatesInISO())
	{
		sTime = CTimeHelper::FormatISOTime(date.GetHour(), date.GetMinute(), date.GetSecond(), TRUE);
	}
	else
	{
		sTime = CTimeHelper::Format24HourTime(date.GetHour(), date.GetMinute(), date.GetSecond(), TRUE);
	}

	GetToDoCtrl().PasteText(sTime);
}

void CToDoListWnd::OnUpdateEditInsertdatetime(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().CanPasteText());
}

void CToDoListWnd::OnUpdateEditInserttime(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().CanPasteText());
}

void CToDoListWnd::OnUpdateEditInsertdate(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().CanPasteText());
}

void CToDoListWnd::OnSysColorChange()
{
	CFrameWnd::OnSysColorChange();

	InitMenuIconManager();

	SetUITheme(m_sThemeFile);
}

void CToDoListWnd::OnUpdateSBSelectionCount(CCmdUI* /*pCmdUI*/)
{
	if (GetTDCCount())
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl();
		CString sText;

		// number of selected tasks
		int nSelCount = tdc.GetSelectedCount();

		if (nSelCount == 1)
		{
			sText.Format(ID_SB_SELCOUNTONE, tdc.GetSelectedTaskID());
		}
		else
		{
			sText.Format(ID_SB_SELCOUNT, nSelCount);
		}

		m_statusBar.SetPaneText(m_statusBar.CommandToIndex(ID_SB_SELCOUNT), sText);

		// time estimate
		const CPreferencesDlg& userPrefs = Prefs();
		int nUnits;
		double dTime;
		CString sValue;

		userPrefs.GetDefaultTimeEst(nUnits);

		dTime = tdc.CalcSelectedTaskTimeEstimate(nUnits);

		if (userPrefs.GetUseHMSTimeFormat())
		{
			sValue = CTimeHelper().FormatTimeHMS(dTime, nUnits);
		}
		else
		{
			sValue = CTimeHelper().FormatTime(dTime, nUnits, 2);
		}

		sText.Format(ID_SB_SELTIMEEST, sValue);
		m_statusBar.SetPaneText(m_statusBar.CommandToIndex(ID_SB_SELTIMEEST), sText);

		// time spent
		userPrefs.GetDefaultTimeSpent(nUnits);

		dTime = tdc.CalcSelectedTaskTimeSpent(nUnits);

		if (userPrefs.GetUseHMSTimeFormat())
		{
			sValue = CTimeHelper().FormatTimeHMS(dTime, nUnits);
		}
		else
		{
			sValue = CTimeHelper().FormatTime(dTime, nUnits, 2);
		}

		sText.Format(ID_SB_SELTIMESPENT, sValue);
		m_statusBar.SetPaneText(m_statusBar.CommandToIndex(ID_SB_SELTIMESPENT), sText);

		// cost
		sText.Format(ID_SB_SELCOST, Misc::Format(tdc.CalcSelectedTaskCost()));
		m_statusBar.SetPaneText(m_statusBar.CommandToIndex(ID_SB_SELCOST), sText);

		// set tray tip too
		UpdateTooltip();
	}
}

void CToDoListWnd::OnUpdateSBTaskCount(CCmdUI* /*pCmdUI*/)
{
	if (GetTDCCount())
	{
		CFilteredToDoCtrl& tdc = GetToDoCtrl();
		CString sText;
		UINT nTooltipID;

		UINT nVisibleTasks;
		UINT nTotalTasks = tdc.GetTaskCount(&nVisibleTasks);

		sText.Format(IDS_SB_TASKCOUNT, nVisibleTasks, nTotalTasks);
		nTooltipID = IDS_SB_TASKCOUNT_TIP;

		int nIndex = m_statusBar.CommandToIndex(ID_SB_TASKCOUNT);
		m_statusBar.SetPaneText(nIndex, sText);
		m_statusBar.SetPaneTooltipIndex(nIndex, nTooltipID);
	}
}

void CToDoListWnd::OnEditSelectall()
{
	GetToDoCtrl().SelectAll();
}

void CToDoListWnd::OnUpdateEditSelectall(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetToDoCtrl().GetTaskCount());
}

void CToDoListWnd::OnCloseallbutthis()
{
	int nThis = GetSelToDoCtrl();
	int nCtrl = GetTDCCount();

	// remove tasklists
	while (nCtrl--)
	{
		if (nCtrl != nThis)
		{
			if (ConfirmSaveTaskList(nCtrl, TDLS_CLOSINGTASKLISTS) != TDCO_SUCCESS)
			{
				continue;   // user cancelled
			}

			m_mgrToDoCtrls.RemoveToDoCtrl(nCtrl, TRUE);
		}
	}
}

void CToDoListWnd::OnUpdateCloseallbutthis(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetTDCCount() > 1);
}

void CToDoListWnd::DoSendTasks(TD_SENDWHAT nWhat, TD_SENDAS nSendAs)
{
	CTDLSendTasksDlg dialog(nWhat, nSendAs);

	if (dialog.DoModal() == IDOK)
	{
		TD_SENDWHAT nSendWhat;
		TD_SENDAS nSendAs;

		dialog.GetSendOptions(nSendWhat, nSendAs);

		CString sSubject, sAttachment, sBody;
		const CFilteredToDoCtrl& tdc = GetToDoCtrl();

		sSubject = tdc.GetFriendlyProjectName();

		switch (nSendWhat)
		{
		case TDSW_ACTIVETASKLIST:
			// make sure it's saved if sending the whole tasklist
			if (ConfirmSaveTaskList(GetSelToDoCtrl()) != TDCO_SUCCESS)
			{
				return;
			}

			sAttachment = tdc.GetFilePath();
			break;

		case TDSW_SELECTEDTASKS:
			{
				CTaskFile tasks;
				tdc.GetSelectedTasks(tasks);

				switch (nSendAs)
				{
				case TDSA_TASKLIST:
					{
						CString sFilename, sExt;
						FileMisc::SplitPath(tdc.GetFilePath(), NULL, NULL, &sFilename, &sExt);

						sAttachment = FileMisc::GetTempFileName(sFilename, sExt);

						if (!tasks.Save(sAttachment))
						{
							// TODO
							return;
						}

					}
					break;

				case TDSA_BODYTEXT:
					sBody = m_mgrImportExport.ExportTaskListToText(&tasks);
					break;
				}
			}
			break;
		}

		CSendFileTo().SendMail(*this, sSubject, sBody, sAttachment);
	}
}

void CToDoListWnd::OnSendtasks()
{
	DoSendTasks(TDSW_ACTIVETASKLIST, TDSA_TASKLIST);
}

void CToDoListWnd::OnEditUndo()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.UndoLastAction(TRUE);
	UpdateStatusbar();
}

void CToDoListWnd::OnUpdateEditUndo(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	pCmdUI->Enable(tdc.CanUndoLastAction(TRUE));
}

void CToDoListWnd::OnEditRedo()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	tdc.UndoLastAction(FALSE);
	UpdateStatusbar();
}

void CToDoListWnd::OnUpdateEditRedo(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	pCmdUI->Enable(tdc.CanUndoLastAction(FALSE));
}

void CToDoListWnd::OnViewToggleTreeandList()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	if (tdc.GetView() == FTCV_TASKLIST)
	{
		tdc.SetView(FTCV_TASKTREE);
	}
	else
	{
		tdc.SetView(FTCV_TASKLIST);
	}

	tdc.SetFocusToTasks();
}

void CToDoListWnd::OnUpdateViewToggleTreeandList(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nMaxState != TDCMS_MAXCOMMENTS);
}

void CToDoListWnd::OnViewToggletasksandcomments()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	if (!tdc.TasksHaveFocus())
	{
		tdc.SetFocusToTasks();
	}
	else
	{
		tdc.SetFocusToComments();
	}
}

void CToDoListWnd::OnUpdateViewToggletasksandcomments(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(m_nMaxState == TDCMS_NORMAL ||
		(m_nMaxState == TDCMS_MAXTASKLIST && Prefs().GetShowCommentsAlways()));
}

void CToDoListWnd::OnQuickFind()
{
	m_cbQuickFind.SetFocus();
}

void CToDoListWnd::OnQuickFindNext()
{
	if (!m_sQuickFind.IsEmpty())
	{
		if (!GetToDoCtrl().SelectTask(m_sQuickFind, TDC_SELECTNEXT))
		{
			GetToDoCtrl().SelectTask(m_sQuickFind, TDC_SELECTFIRST);   // return to start
		}
	}
}

void CToDoListWnd::OnUpdateQuickFindNext(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_sQuickFind.IsEmpty());
}

LRESULT CToDoListWnd::OnQuickFindItemAdded(WPARAM /*wp*/, LPARAM /*lp*/)
{
	// keep only the last 20 items
	int nItem = m_cbQuickFind.GetCount();

	while (nItem > 20)
	{
		nItem--;
		m_cbQuickFind.DeleteString(nItem);
	}

	return 0L;
}

void CToDoListWnd::OnQuickFindPrev()
{
	if (!m_sQuickFind.IsEmpty())
	{
		if (!GetToDoCtrl().SelectTask(m_sQuickFind, TDC_SELECTPREV))
		{
			GetToDoCtrl().SelectTask(m_sQuickFind, TDC_SELECTLAST);   // return to end
		}
	}
}

void CToDoListWnd::OnUpdateQuickFindPrev(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!m_sQuickFind.IsEmpty());
}

void CToDoListWnd::OnMove(int x, int y)
{
	CFrameWnd::OnMove(x, y);
}

void CToDoListWnd::OnLoadFromWeb()
{
	CString sRemotePath, sLocalPath = FileMisc::GetTempFolder();
	CRemoteFile rf;

	DWORD dwOptions = RMO_ALLOWDIALOG | RMO_CREATEDOWNLOADDIR | RMO_USETEMPFILE | RMO_MULTISELECT;

	if (rf.GetFile(sRemotePath, sLocalPath, dwOptions, CEnString(IDS_TDLFILEFILTER)) == RMERR_SUCCESS)
	{
		CStringArray aRemoteFiles, aLocalFiles;

		int nFiles = Misc::Split(sLocalPath, _T(';'), aLocalFiles);
		VERIFY(nFiles == Misc::Split(sRemotePath, _T(';'), aRemoteFiles));

		for (int nFile = 0; nFile < nFiles; nFile++)
		{
			sLocalPath = aLocalFiles[nFile];
			CString sUrl = rf.GetServer() + aRemoteFiles[nFile];

			TDC_FILE nErr = OpenTaskList(sLocalPath, sUrl);

			int nTaskList = m_mgrToDoCtrls.FindToDoCtrl(sLocalPath);

			if (nErr == TDCO_SUCCESS)
			{
				ASSERT(nTaskList != -1);

				if (nTaskList != -1)
				{
					m_mgrToDoCtrls.SetWebDetails(nTaskList, rf.GetServer(), aRemoteFiles[nFile], rf.GetUsername(),
						rf.GetPassword());
					UpdateStatusbar();
				}
			}
			else
			{
				if (GetTDCCount() > 1 && nTaskList != -1)
				{
					CloseToDoCtrl(nTaskList);
				}

				HandleLoadTasklistError(nErr, sUrl);
			}

			// delete temp download file
			::DeleteFile(sLocalPath);
		}
	}
}

void CToDoListWnd::OnSaveToWeb()
{
	CString sRemotePath, sLocalPath = m_mgrToDoCtrls.GetFilePath(GetSelToDoCtrl());

	// if not yet saved then save to temp filepath
	if (sLocalPath.IsEmpty())
	{
		sLocalPath = FileMisc::GetTempFileName(_T("rmf"));
	}

	// do local save
	int nTaskList = GetSelToDoCtrl();
	TDC_FILE nErr = SaveTaskList(nTaskList, sLocalPath, TRUE);

	if (nErr == TDCO_SUCCESS)
	{
		CRemoteFile rf;
		DWORD dwOptions = RMO_NOCANCELPROGRESS | RMO_ALLOWDIALOG;

		if (rf.SetFile(sLocalPath, sRemotePath, dwOptions) == RMERR_SUCCESS)
		{
			m_mgrToDoCtrls.SetWebDetails(nTaskList, rf.GetServer(), sRemotePath, rf.GetUsername(), rf.GetPassword());
			UpdateStatusbar();
		}
	}

	// clean up temp file
	if (FileMisc::IsTempFile(sLocalPath))
	{
		::DeleteFile(sLocalPath);
	}
}

void CToDoListWnd::OnUpdateSaveToWeb(CCmdUI* pCmdUI)
{
	// disable if already on the web
	pCmdUI->Enable(!m_mgrToDoCtrls.IsWebTaskList(GetSelToDoCtrl()));
}

void CToDoListWnd::OnEditSettaskicon()
{
	GetToDoCtrl().EditSelectedTaskIcon();
}

void CToDoListWnd::OnUpdateEditSettaskicon(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	pCmdUI->Enable(nSelCount && !tdc.IsReadOnly());
}

LRESULT CToDoListWnd::OnToDoCtrlReminder(WPARAM /*wp*/, LPARAM lp)
{
	AF_NOREENTRANT_RET(0L) // macro helper

	// ignore if we are showing a dialog
	if (!IsWindowEnabled())
	{
		return 0L;
	}

	Show(FALSE);

	TDCREMINDER* pReminder = (TDCREMINDER*)lp;
	CTDLShowReminderDlg dialog;

	int nRet = dialog.DoModal(*pReminder);

	switch (nRet)
	{
	case IDSNOOZE:
		{
			double dSnooze = dialog.GetSnoozeMinutes();
			double dNow = COleDateTime::GetCurrentTime().m_dt;

			if (pReminder->bRelative)
			{
				if (pReminder->nRelativeFromWhen == TDCR_DUEDATE)
				{
					// in case the user didn't handle the notification immediately we need
					// to soak up any additional elapsed time in the snooze
					COleDateTime dDue = pReminder->pTDC->GetTaskDate(pReminder->dwTaskID, TDCD_DUE);

					pReminder->dDaysSnooze = dNow - dDue.m_dt + pReminder->dRelativeDaysLeadIn;

					// then we add the user's snooze
					pReminder->dDaysSnooze += (dSnooze / (24.0 * 60.0));
				}
				else // from start
				{
					// in case the user didn't handle the notification immediately we need
					// to soak up any additional elapsed time in the snooze
					COleDateTime dStart = pReminder->pTDC->GetTaskDate(pReminder->dwTaskID, TDCD_START);

					pReminder->dDaysSnooze = dNow - dStart.m_dt + pReminder->dRelativeDaysLeadIn;

					// then we add the user's snooze
					pReminder->dDaysSnooze += (dSnooze / (24.0 * 60.0));
				}
			}
			else // absolute
			{
				// in case the user didn't handle the notification immediately we need
				// to soak up any additional elapsed time in the snooze
				pReminder->dDaysSnooze = dNow - pReminder->dtAbsolute;

				// then we add the user's snooze
				pReminder->dDaysSnooze += (dSnooze / (24.0 * 60.0));
			}
		}
		return 0L; // don't delete (default)

	case IDGOTOTASK:
		{
			int nTDC = m_mgrToDoCtrls.FindToDoCtrl(pReminder->pTDC);
			ASSERT(nTDC != -1);

			SelectToDoCtrl(nTDC, TRUE);
			GetToDoCtrl().SelectTask(pReminder->dwTaskID);
		}
		// fall thru

	case IDCANCEL:
	default:
		// delete unless it's a recurring task in which case we
		// disable it so that it can later be copied when the
		// recurring task is completed
		if (GetToDoCtrl().IsTaskRecurring(pReminder->dwTaskID))
		{
			pReminder->bEnabled = FALSE;
			return 0L; // don't delete
		}
		// else
		return 1L; // delete
	}
}

LRESULT CToDoListWnd::OnToDoCtrlTaskHasReminder(WPARAM wParam, LPARAM lParam)
{
	return (m_reminders.FindReminder(wParam, (CFilteredToDoCtrl*)lParam) != -1);
}

LRESULT CToDoListWnd::OnDoubleClkReminderCol(WPARAM /*wp*/, LPARAM /*lp*/)
{
	OnEditSetReminder();
	return 0L;
}

void CToDoListWnd::OnEditSetReminder()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	CTDLSetReminderDlg dialog;

	CString sTitle = tdc.GetSelectedTaskTitle();
	DWORD dwTaskID = tdc.GetSelectedTaskID();
	TDCREMINDER rem;

	int nRem = m_reminders.FindReminder(dwTaskID, &tdc);
	static BOOL sbRelative = FALSE;

	if (nRem != -1)
	{
		m_reminders.GetReminder(nRem, rem);
	}
	else
	{
		rem.dwTaskID = dwTaskID;
		rem.pTDC = &tdc;
		rem.bRelative = sbRelative;
		rem.dRelativeDaysLeadIn = 0.25 / 24; // 15 minutes
		rem.dtAbsolute = COleDateTime::GetCurrentTime();

		if (tdc.SelectedTaskHasDate(TDCD_DUE))
		{
			rem.nRelativeFromWhen = TDCR_DUEDATE;
		}
		else
		{
			rem.nRelativeFromWhen = TDCR_STARTDATE;
		}
	}

	if (dialog.DoModal(rem) == IDOK)
	{
		// cache relative state for next time
		sbRelative = rem.bRelative;

		m_reminders.SetReminder(rem);
		tdc.RedrawReminders();
	}
}

void CToDoListWnd::OnUpdateEditSetReminder(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	BOOL bEnable = FALSE;

	// must be only one task selected and it must not be completed
	if (tdc.GetSelectedCount() == 1)
	{
		DWORD dwID = tdc.GetSelectedTaskID();

		if (!tdc.IsTaskDone(dwID, TDCCHECKALL))
		{
			bEnable = TRUE;
		}
	}

	pCmdUI->Enable(bEnable);
}

void CToDoListWnd::OnEditClearReminder()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	DWORD dwTaskID = tdc.GetSelectedTaskID();

	m_reminders.RemoveReminder(dwTaskID, &tdc);
	tdc.RedrawReminders();
}

void CToDoListWnd::OnUpdateEditClearReminder(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	DWORD dwTaskID = tdc.GetSelectedTaskID();

	int nRem = m_reminders.FindReminder(dwTaskID, &tdc);

	pCmdUI->Enable(nRem != -1);
}

void CToDoListWnd::OnEditCleartaskicon()
{
	GetToDoCtrl().ClearSelectedTaskIcon();
}

void CToDoListWnd::OnUpdateEditCleartaskicon(CCmdUI* pCmdUI)
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();
	int nSelCount = tdc.GetSelectedCount();

	pCmdUI->Enable(nSelCount && !tdc.IsReadOnly() && tdc.GetSelectedTaskIconIndex() != -1);
}

void CToDoListWnd::OnSortMulti()
{
	TDSORTCOLUMNS sort;
	CTDCColumnArray aColumns;

	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	tdc.GetSortBy(sort);
	tdc.GetVisibleColumns(aColumns);

	CTDLMultiSortDlg dialog(sort, aColumns);

	if (dialog.DoModal() == IDOK)
	{
		dialog.GetSortBy(sort);
		tdc.MultiSort(sort);
	}
}

void CToDoListWnd::OnUpdateSortMulti(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(GetToDoCtrl().IsMultiSorting() ? 1 : 0);
}

void CToDoListWnd::OnToolsRemovefromsourcecontrol()
{
	CFilteredToDoCtrl& tdc = GetToDoCtrl();

	if (tdc.RemoveFromSourceControl())
	{
		int nCtrl = GetSelToDoCtrl();

		m_mgrToDoCtrls.UpdateToDoCtrlReadOnlyUIState(nCtrl);
		m_mgrToDoCtrls.UpdateTabItemText(nCtrl);
		m_mgrToDoCtrls.SetModifiedStatus(nCtrl, FALSE);
		m_mgrToDoCtrls.RefreshLastModified(nCtrl);
		m_mgrToDoCtrls.RefreshReadOnlyStatus(nCtrl);
		m_mgrToDoCtrls.RefreshPathType(nCtrl);
	}
}

void CToDoListWnd::OnUpdateToolsRemovefromsourcecontrol(CCmdUI* pCmdUI)
{
	int nCtrl = GetSelToDoCtrl();

	BOOL bEnable = m_mgrToDoCtrls.IsSourceControlled(nCtrl);
	bEnable &= m_mgrToDoCtrls.PathSupportsSourceControl(nCtrl);

	if (bEnable)
	{
		// make sure no-one has the file checked out
		CFilteredToDoCtrl& tdc = GetToDoCtrl();
		bEnable &= !tdc.IsCheckedOut();
	}

	pCmdUI->Enable(bEnable);
}


void CToDoListWnd::OnViewShowTasklistTabbar()
{
	m_bShowTasklistBar = !m_bShowTasklistBar;
	ResizeDlg();
}

void CToDoListWnd::OnUpdateViewShowTasklistTabbar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowTasklistBar ? 1 : 0);
}

void CToDoListWnd::OnViewShowTreeListTabbar()
{
	m_bShowTreeListBar = !m_bShowTreeListBar;

	GetToDoCtrl().SetStyle(TDCS_SHOWTREELISTBAR, m_bShowTreeListBar);

	// refresh all the other tasklists
	m_mgrToDoCtrls.SetAllNeedPreferenceUpdate(TRUE, GetSelToDoCtrl());
}

void CToDoListWnd::OnUpdateViewShowTreeListTabbar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bShowTreeListBar ? 1 : 0);
}
