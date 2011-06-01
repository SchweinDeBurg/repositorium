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
// - added AbstractSpoon Software copyright notice and license information
// - adjusted #include's paths
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-switches
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
// - merged with ToDoList versions 6.1.2-6.1.6 sources
//*****************************************************************************

// ToDoListWnd.h : header file
//

#if !defined(AFX_TODOLISTWND_H__13051D32_D372_4205_BA71_05FAC2159F1C__INCLUDED_)
#define AFX_TODOLISTWND_H__13051D32_D372_4205_BA71_05FAC2159F1C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FilteredToDoCtrl.h"
#include "PreferencesDlg.h"
#include "TDLFindTasksDlg.h"
#include "ToDoCtrlMgr.h"
#include "TDLImportExportMgr.h"
#include "TDLContentMgr.h"
#include "TDLfilterbar.h"
#include "TDLSendTasksDlg.h"
#include "TaskSelectionDlg.h"
#include "ToDoCtrlReminders.h"

#include "../../../CodeProject/Source/TrayIcon.h"
#include "../../../CodeProject/Source/ToolBarHelper.h"
#include "../../../CodeProject/Source/FileMisc.h"
#include "../../Common/ShortcutManager.h"
#include "../../../CodeProject/Source/DriveInfo.h"
#include "../../../CodeProject/Source/EnToolBar.h"
#include "../../../CodeProject/Source/TabCtrlEx.h"
#include "../../Common/EnRecentFileList.h"
#include "../../../CodeProject/Source/MenuEx.h"
#include "../../../CodeProject/Source/DialogHelper.h"
#include "../../Common/TabbedComboBox.h"
#include "../../../CodeProject/Source/DeferWndMove.h"
#include "../../Common/EnBrowserCtrl.h"
#include "../../Common/UIExtensionMgr.h"
#include "../../../CodeProject/Source/MenuIconMgr.h"
#include "../../../CodeProject/Source/AutoComboBox.h"
#include "../../Common/BrowserDlg.h"
#include "../../Common/UITheme.h"

#include "../../../CodeProject/Source/StatusbarACT.h"

/////////////////////////////////////////////////////////////////////////////
// CToDoListWnd dialog

const UINT WM_TDL_SHOWWINDOW = ::RegisterWindowMessage(_T("WM_TDL_SHOWWINDOW"));
const UINT WM_TDL_GETVERSION = ::RegisterWindowMessage(_T("WM_TDL_GETVERSION"));
const UINT WM_TDL_ISCLOSING = ::RegisterWindowMessage(_T("WM_TDL_ISCLOSING"));
const UINT WM_TDL_REFRESHPREFS = ::RegisterWindowMessage(_T("WM_TDL_REFRESHPREFS"));
const UINT WM_TDL_RESTORE = ::RegisterWindowMessage(_T("WM_TDL_RESTORE"));

enum FIND_WHAT;

class CToDoListWnd : public CFrameWnd, public CDialogHelper
{
// Construction
public:
	CToDoListWnd(); // standard constructor
	~CToDoListWnd();

	static int GetVersion();
	BOOL Create(const TDCSTARTUP& startup);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToDoListWnd)
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
	virtual void OnOK() {}
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);

// Implementation
protected:
	HICON m_hIcon;
	CImageList m_ilTabCtrl, m_ilToDoCtrl;
	CEnToolBar m_toolbar;
	CTrayIcon m_ti;
	CEnRecentFileList m_mruList;
	CTabCtrlEx m_tabCtrl;
	CShortcutManager m_mgrShortcuts;
	CTaskListDropTarget m_dropTarget;
	CPreferencesDlg* m_pPrefs;
	CTDLFindTasksDlg m_findDlg;
	CTDLImportExportMgr m_mgrImportExport;
	CToDoCtrlMgr m_mgrToDoCtrls;
	CFont m_fontTree, m_fontComments; // shared by all tasklists
	CMenuEx m_menubar;
	CTDLContentMgr m_mgrContent;
	CEnBrowserCtrl m_IE;
	CTDLFilterBar m_filterBar;
	CStatusBarACT m_statusBar;
	HWND m_hwndLastFocus;
	CMenuIconMgr m_mgrMenuIcons;
	CUIExtensionMgr m_mgrUIExtensions;
	CAutoComboBox m_cbQuickFind;
	CString m_sQuickFind;
	CWndPromptManager m_mgrPrompts;
	CBrowserDlg m_dlgNotifyDue;
	CToDoCtrlReminders m_reminders;
	UITHEME m_theme;
	CString m_sThemeFile;
	TDC_MAXSTATE m_nMaxState, m_nPrevMaxState;

	TDCSTARTUP m_startupOptions;

	CDWordArray m_aPriorityColors;
	CFont m_fontMain;
	int m_nLastSelItem; // just for flicker-free todoctrl switching

	BOOL m_bVisible;
	BOOL m_bShowFilterBar, m_bShowProjectName;
	BOOL m_bShowStatusBar, m_bShowToolbar;
	BOOL m_bInNewTask;
	BOOL m_bSaving;
	BOOL m_bInTimer;
	BOOL m_bClosing;
	BOOL m_bFindShowing;
	BOOL m_bQueryOpenAllow;
	BOOL m_bPasswordPrompting;
	BOOL m_bReloading; // on startup
	BOOL m_bStartHidden;
	BOOL m_bLogging;

	// Generated message map functions
	//{{AFX_MSG(CToDoListWnd)
	afx_msg void OnViewExpandtask();
	afx_msg void OnUpdateViewExpandtask(CCmdUI* pCmdUI);
	afx_msg void OnViewCollapsetask();
	afx_msg void OnUpdateViewCollapsetask(CCmdUI* pCmdUI);
	afx_msg void OnViewCollapseall();
	afx_msg void OnViewExpandall();
	afx_msg void OnUpdateViewExpandall(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewCollapseall(CCmdUI* pCmdUI);
	afx_msg void OnUpdateWindow(CCmdUI* pCmdUI);
#if _MSC_VER >= 1400
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
#else
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#endif
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnNewtask();
	afx_msg void OnNewsubtask();
	afx_msg void OnUpdateNewsubtask(CCmdUI* pCmdUI);
	afx_msg void OnToolsTransformactivetasklist();
	afx_msg void OnViewSorttasklisttabs();
	afx_msg void OnUpdateViewSorttasklisttabs(CCmdUI* pCmdUI);
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnEditInctaskpercentdone();
	afx_msg void OnUpdateEditInctaskpercentdone(CCmdUI* pCmdUI);
	afx_msg void OnEditDectaskpercentdone();
	afx_msg void OnUpdateEditDectaskpercentdone(CCmdUI* pCmdUI);
	afx_msg void OnEditDectaskpriority();
	afx_msg void OnUpdateEditDectaskpriority(CCmdUI* pCmdUI);
	afx_msg void OnEditInctaskpriority();
	afx_msg void OnUpdateEditInctaskpriority(CCmdUI* pCmdUI);
	afx_msg void OnEditFlagtask();
	afx_msg void OnUpdateEditFlagtask(CCmdUI* pCmdUI);
	afx_msg void OnFileOpenarchive();
	afx_msg void OnUpdateFileOpenarchive(CCmdUI* pCmdUI);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnViewShowfilterbar();
	afx_msg void OnUpdateViewShowfilterbar(CCmdUI* pCmdUI);
	afx_msg void OnViewClearfilter();
	afx_msg void OnUpdateViewClearfilter(CCmdUI* pCmdUI);
	afx_msg void OnViewTogglefilter();
	afx_msg void OnUpdateViewTogglefilter(CCmdUI* pCmdUI);
	afx_msg void OnViewFilter();
	afx_msg void OnUpdateViewFilter(CCmdUI* pCmdUI);
	afx_msg void OnTabctrlPreferences();
	afx_msg void OnTasklistSelectColumns();
	afx_msg void OnViewProjectname();
	afx_msg void OnUpdateViewProjectname(CCmdUI* pCmdUI);
	afx_msg void OnEditOffsetdates();
	afx_msg void OnUpdateEditOffsetdates(CCmdUI* pCmdUI);
	afx_msg void OnPrintpreview();
	afx_msg void OnShowTimelogfile();
	afx_msg void OnUpdateShowTimelogfile(CCmdUI* pCmdUI);
	afx_msg void OnViewStatusBar();
	afx_msg void OnUpdateViewStatusBar(CCmdUI* pCmdUI);
	afx_msg BOOL OnQueryOpen();
	afx_msg void OnCopyTaskasRef();
	afx_msg void OnUpdateCopyTaskasRef(CCmdUI* pCmdUI);
	afx_msg void OnCopyTaskasDependency();
	afx_msg void OnUpdateCopyTaskasDependency(CCmdUI* pCmdUI);
	afx_msg void OnCopyTaskasRefFull();
	afx_msg void OnUpdateCopyTaskasRefFull(CCmdUI* pCmdUI);
	afx_msg void OnCopyTaskasDependencyFull();
	afx_msg void OnUpdateCopyTaskasDependencyFull(CCmdUI* pCmdUI);
	afx_msg void OnCopyTaskasPath();
	afx_msg void OnUpdateCopyTaskasPath(CCmdUI* pCmdUI);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnToolsCheckforupdates();
	afx_msg void OnShowKeyboardshortcuts();
	afx_msg void OnEditInsertdatetime();
	afx_msg void OnUpdateEditInsertdatetime(CCmdUI* pCmdUI);
	afx_msg void OnSysColorChange();
	afx_msg void OnEditCleartaskcolor();
	afx_msg void OnUpdateEditCleartaskcolor(CCmdUI* pCmdUI);
	afx_msg void OnEditSelectall();
	afx_msg void OnUpdateEditSelectall(CCmdUI* pCmdUI);
	afx_msg void OnCloseallbutthis();
	afx_msg void OnUpdateCloseallbutthis(CCmdUI* pCmdUI);
	afx_msg void OnSendtasks();
	afx_msg void OnEditInsertdate();
	afx_msg void OnUpdateEditInsertdate(CCmdUI* pCmdUI);
	afx_msg void OnGotoNexttask();
	afx_msg void OnGotoPrevtask();
	afx_msg void OnUpdateGotoPrevtask(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGotoNexttask(CCmdUI* pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnEditRedo();
	afx_msg void OnUpdateEditRedo(CCmdUI* pCmdUI);
	afx_msg void OnViewToggleTreeandList();
	afx_msg void OnUpdateViewToggleTreeandList(CCmdUI* pCmdUI);
	afx_msg void OnViewToggletasksandcomments();
	afx_msg void OnUpdateViewToggletasksandcomments(CCmdUI* pCmdUI);
	afx_msg void OnQuickFind();
	afx_msg void OnQuickFindNext();
	afx_msg void OnUpdateQuickFindNext(CCmdUI* pCmdUI);
	afx_msg void OnQuickFindPrev();
	afx_msg void OnUpdateQuickFindPrev(CCmdUI* pCmdUI);
	afx_msg void OnLoadFromWeb();
	afx_msg void OnSaveToWeb();
	afx_msg void OnUpdateSaveToWeb(CCmdUI* pCmdUI);
	afx_msg void OnEditSettaskicon();
	afx_msg void OnUpdateEditSettaskicon(CCmdUI* pCmdUI);
	afx_msg void OnEditSetReminder();
	afx_msg void OnUpdateEditSetReminder(CCmdUI* pCmdUI);
	afx_msg void OnEditClearReminder();
	afx_msg void OnUpdateEditClearReminder(CCmdUI* pCmdUI);
	afx_msg void OnEditCleartaskicon();
	afx_msg void OnUpdateEditCleartaskicon(CCmdUI* pCmdUI);
	afx_msg void OnSortMulti();
	afx_msg void OnUpdateSortMulti(CCmdUI* pCmdUI);
	afx_msg void OnArchiveSelectedTasks();
	afx_msg void OnUpdateArchiveSelectedCompletedTasks(CCmdUI* pCmdUI);
	afx_msg void OnAddtimetologfile();
	afx_msg void OnUpdateAddtimetologfile(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnToolsRemovefromsourcecontrol();
	afx_msg void OnUpdateToolsRemovefromsourcecontrol(CCmdUI* pCmdUI);
	afx_msg void OnViewRefreshfilter();
	afx_msg void OnUpdateViewRefreshfilter(CCmdUI* pCmdUI);
	afx_msg LRESULT OnSelchangeFilter(WPARAM wp, LPARAM lp);
	afx_msg void OnEditChangeQuickFind();
	afx_msg void OnSelChangeQuickFind();
	afx_msg void OnWindow(UINT nCmdID);
	afx_msg void OnUserUIExtension(UINT nCmdID);
	afx_msg void OnUpdateUserUIExtension(CCmdUI* pCmdUI);
	afx_msg void OnViewNext();
	afx_msg void OnUpdateViewNext(CCmdUI* pCmdUI);
	afx_msg void OnViewPrev();
	afx_msg void OnUpdateViewPrev(CCmdUI* pCmdUI);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnUpdateImport(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNewtask(CCmdUI* pCmdUI);
	afx_msg void OnToolsCheckout();
	afx_msg void OnUpdateToolsCheckout(CCmdUI* pCmdUI);
	afx_msg void OnToolsCheckin();
	afx_msg void OnUpdateToolsCheckin(CCmdUI* pCmdUI);
	afx_msg void OnExport();
	afx_msg void OnUpdateExport(CCmdUI* pCmdUI);
	afx_msg void OnNexttopleveltask();
	afx_msg void OnPrevtopleveltask();
	afx_msg void OnUpdateNexttopleveltask(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePrevtopleveltask(CCmdUI* pCmdUI);
	afx_msg void OnFindTasks();
	afx_msg void OnViewMovetasklistright();
	afx_msg void OnUpdateViewMovetasklistright(CCmdUI* pCmdUI);
	afx_msg void OnViewMovetasklistleft();
	afx_msg void OnUpdateViewMovetasklistleft(CCmdUI* pCmdUI);
	afx_msg void OnViewToolbar();
	afx_msg void OnUpdateViewToolbar(CCmdUI* pCmdUI);
	afx_msg void OnSort();
	afx_msg void OnUpdateSort(CCmdUI* pCmdUI);
	afx_msg void OnNewtaskAttop();
	afx_msg void OnUpdateNewtaskAttop(CCmdUI* pCmdUI);
	afx_msg void OnNewtaskAtbottom();
	afx_msg void OnUpdateNewtaskAtbottom(CCmdUI* pCmdUI);
	afx_msg void OnSpellcheckcomments();
	afx_msg void OnUpdateSpellcheckcomments(CCmdUI* pCmdUI);
	afx_msg void OnSpellchecktitle();
	afx_msg void OnUpdateSpellchecktitle(CCmdUI* pCmdUI);
	afx_msg void OnFileEncrypt();
	afx_msg void OnUpdateFileEncrypt(CCmdUI* pCmdUI);
	afx_msg void OnFileResetversion();
	afx_msg void OnUpdateFileResetversion(CCmdUI* pCmdUI);
	afx_msg void OnSpellcheckTasklist();
	afx_msg void OnUpdateSpellcheckTasklist(CCmdUI* pCmdUI);
	afx_msg void OnEditPasteAfter();
	afx_msg void OnUpdateEditPasteAfter(CCmdUI* pCmdUI);
	afx_msg void OnEditTimeTrackTask();
	afx_msg void OnUpdateEditTimeTrackTask(CCmdUI* pCmdUI);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnViewNextSel();
	afx_msg void OnUpdateViewNextSel(CCmdUI* pCmdUI);
	afx_msg void OnViewPrevSel();
	afx_msg void OnUpdateViewPrevSel(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnToolsToggleCheckin();
	afx_msg void OnUpdateToolsToggleCheckin(CCmdUI* pCmdUI);
	afx_msg void OnSplitTaskIntoPieces(UINT nCmdID);
	afx_msg void OnUpdateSplitTaskIntoPieces(CCmdUI* pCmdUI);
	afx_msg void OnToolsShowtasksDue(UINT nCmdID);
	afx_msg void OnDeleteTask();
	afx_msg void OnDeleteAllTasks();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSave();
	afx_msg void OnLoad();
	afx_msg void OnNew();
	afx_msg void OnUpdateDeletetask(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTaskcolor(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTaskdone(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDeletealltasks(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNew(CCmdUI* pCmdUI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNewtaskAttopSelected();
	afx_msg void OnNewtaskAtbottomSelected();
	afx_msg void OnNewtaskAfterselectedtask();
	afx_msg void OnNewtaskBeforeselectedtask();
	afx_msg void OnNewsubtaskAtbottom();
	afx_msg void OnNewsubtaskAttop();
	afx_msg void OnEditTaskcolor();
	afx_msg void OnEditTaskdone();
	afx_msg void OnEditTasktext();
	afx_msg void OnMovetaskdown();
	afx_msg void OnUpdateMovetaskdown(CCmdUI* pCmdUI);
	afx_msg void OnMovetaskup();
	afx_msg void OnUpdateMovetaskup(CCmdUI* pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnTrayiconClose();
	afx_msg void OnUpdateNewsubtaskAtBottom(CCmdUI* pCmdUI);
	afx_msg void OnSaveas();
	afx_msg void OnUpdateSaveas(CCmdUI* pCmdUI);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnTrayiconShow();
	afx_msg void OnTrayiconShowDueTasks(UINT nCmdID);
	afx_msg BOOL OnQueryEndSession();
	afx_msg void OnUpdateRecentFileMenu(CCmdUI* pCmdUI);
	afx_msg void OnAbout();
	afx_msg void OnPreferences();
	afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
	afx_msg void OnEditCopy();
	afx_msg void OnEditPasteSub();
	afx_msg void OnEditCopyastext();
	afx_msg void OnEditCopyashtml();
	afx_msg void OnUpdateEditPasteSub(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNewtaskAttopSelected(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNewtaskAtbottomSelected(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNewtaskAfterselectedtask(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNewtaskBeforeselectedtask(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNewsubtaskAttop(CCmdUI* pCmdUI);
	afx_msg void OnMaximizeTasklist();
	afx_msg void OnUpdateMaximizeTasklist(CCmdUI* pCmdUI);
	afx_msg void OnMaximizeComments();
	afx_msg void OnUpdateMaximizeComments(CCmdUI* pCmdUI);
	afx_msg void OnReload();
	afx_msg void OnUpdateReload(CCmdUI* pCmdUI);
	afx_msg void OnArchiveCompletedtasks();
	afx_msg void OnUpdateArchiveCompletedtasks(CCmdUI* pCmdUI);
	afx_msg void OnPrint();
	afx_msg void OnUpdatePrint(CCmdUI* pCmdUI);
	afx_msg void OnMovetaskright();
	afx_msg void OnUpdateMovetaskright(CCmdUI* pCmdUI);
	afx_msg void OnMovetaskleft();
	afx_msg void OnUpdateMovetaskleft(CCmdUI* pCmdUI);
	afx_msg void OnSelchangeTabcontrol(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangingTabcontrol(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMBtnClickTabcontrol(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCloseTasklist();
	afx_msg void OnSaveall();
	afx_msg void OnUpdateSaveall(CCmdUI* pCmdUI);
	afx_msg void OnCloseall();
	afx_msg void OnUpdateCloseall(CCmdUI* pCmdUI);
	afx_msg void OnExit();
	afx_msg void OnUpdateMovetask(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnImportTasklist();
	afx_msg void OnSortBy(UINT nCmdID);
	afx_msg void OnUpdateSortBy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditTasktext(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnTrayiconCreatetask();
	afx_msg void OnSetPriority(UINT nCmdUI);
	afx_msg void OnUpdateSetPriority(CCmdUI* pCmdUI);
	afx_msg void OnEditSetfileref();
	afx_msg void OnMinimizeToTray();
	afx_msg void OnUpdateEditSetfileref(CCmdUI* pCmdUI);
	afx_msg void OnEditOpenfileref();
	afx_msg void OnUpdateEditOpenfileref(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUserTool(CCmdUI* pCmdUI);
	afx_msg void OnUserTool(UINT nCmdID);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnTrayIconClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTrayIconDblClk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTrayIconRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNeedTooltipText(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnOpenRecentFile(UINT nID);
	afx_msg void OnUpdateSBSelectionCount(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSBTaskCount(CCmdUI* pCmdUI);
	afx_msg LRESULT OnGetIcon(WPARAM bLargeIcon, LPARAM /*not used*/);
	afx_msg LRESULT OnToDoCtrlNotifySort(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnToDoCtrlNotifyMod(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnToDoCtrlNotifyMinWidthChange(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnToDoCtrlNotifyTimeTrack(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnToDoCtrlDoTaskLink(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnToDoCtrlTaskIsDone(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnToDoCtrlTaskHasReminder(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnToDoCtrlNotifyViewChange(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnToDoCtrlNotifyRecreateRecurringTask(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnToDoListShowWindow(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnToDoCtrlNotifyListChange(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnToDoListGetVersion(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnToDoListIsClosing(WPARAM /*wp*/, LPARAM /*lp*/)
	{
		return m_bClosing;
	}
	afx_msg LRESULT OnToDoListRefreshPrefs(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnToDoListRestore(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnFindDlgFind(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnFindSelectResult(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnFindSelectAll(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnFindApplyAsFilter(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnFindDlgClose(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnDropFile(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPreferencesTestTool(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnPreferencesDefaultListChange(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnPreferencesClearMRU(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnPostOnCreate(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnWebUpdateWizard(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnAddToolbarTools(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnHotkey(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnPowerBroadcast(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnGetFont(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnQuickFindItemAdded(WPARAM wp, LPARAM lp);
	afx_msg void OnMove(int x, int y);
	afx_msg LRESULT OnToDoCtrlReminder(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnAppRestoreFocus(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnDoubleClkReminderCol(WPARAM wp, LPARAM lp);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);

#ifdef _DEBUG
	afx_msg void OnDebugQueryEndSession()
	{
		SendMessage(WM_QUERYENDSESSION);
	}
#endif
	DECLARE_MESSAGE_MAP()

	// OnTimer helpers
	void OnTimerReadOnlyStatus(int nCtrl = -1);
	void OnTimerTimestampChange(int nCtrl = -1);
	void OnTimerAutoSave();
	void OnTimerCheckoutStatus(int nCtrl = -1);
	void OnTimerDueItems(int nCtrl = -1);
	void OnTimerTimeTracking();

	virtual void LoadSettings();
	virtual void SaveSettings();
	CRect OffsetCtrl(UINT uCtrlID, int cx = 0, int cy = 0);
	void RestorePosition();
	void RestoreVisibility();

	// tdc not const because we need to flush it first
	BOOL Export2Html(CFilteredToDoCtrl& tdc, LPCTSTR szFilePath, TSD_TASKS nWhatTasks, LPCTSTR szStylesheet = NULL) const;
	BOOL Export2Html(const CTaskFile& tasks, LPCTSTR szFilePath, LPCTSTR szStylesheet = NULL) const;

	TDC_FILE DelayOpenTaskList(LPCTSTR szFilePath); // 0 = failed, 1 = success, -1 = cancelled
	TDC_FILE OpenTaskList(LPCTSTR szFilePath, LPCTSTR szDisplayPath = NULL, BOOL bNotifyDueTasks = TRUE); // 0 = failed, 1 = success, -1 = cancelled
	TDC_FILE OpenTaskList(CFilteredToDoCtrl* pCtrl, LPCTSTR szFilePath = NULL);
	void ReloadTaskList(int nIndex, BOOL bNotifyDueTasks);
	BOOL VerifyTaskListOpen(int nIndex, BOOL bWantNotifyDueTasks);

	BOOL ImportFile(LPCTSTR szFilePath);

	void InitShortcutManager();
	void InitMenuIconManager();
	void InitUIFont();
	BOOL LoadMenubar();
	BOOL InitCheckboxImageList();
	BOOL InitToolbar();
	BOOL InitStatusbar();
	void SetUITheme(const CString& sThemeFile);

	BOOL ProcessStartupOptions(const TDCSTARTUP& startup/*, BOOL bPreviousInstance*/);
	BOOL NewTask(LPCTSTR szTitle, TDC_INSERTWHERE nInsertWhere, /*BOOL bSelect = TRUE, */BOOL bEdit = TRUE);
	TDC_SORTBY GetSortBy(UINT nSortID);
	UINT GetSortID(TDC_SORTBY nSortBy);
	void CheckMinWidth();
	void MinimizeToTray();
	void Show(BOOL bAllowToggle);
	BOOL DoDueTaskNotification(const CFilteredToDoCtrl* pCtrl, int nDueBy);
	BOOL DoDueTaskNotification(int nDueBy); // works on active tasklist
	void DoExit();
	void RefreshPauseTimeTracking();
	void SetTimer(UINT nTimerID, BOOL bOn);
	void RefreshTabOrder();
	void UpdateGlobalHotkey();
	LPCTSTR GetFileFilter();
	LPCTSTR GetDefaultFileExt();
	void DoPreferences(int nInitPage = -1);
	void DoPrint(BOOL bPreview = FALSE);
	void HandleLoadTasklistError(TDC_FILE nErr, LPCTSTR szTasklist);
	void CheckForUpdates(BOOL bManual);
	void UpdateCwd();
	BOOL WantTabBarVisible() const
	{
		return GetTDCCount() > 1 || !Prefs().GetAutoHideTabbar();
	}
	void ShowFindDialog(BOOL bShow = TRUE);
	void RefreshUIExtensions(BOOL bEdit);
	void UpdateAeroFeatures();

	enum { CT_ASHTML, CT_ASTEXT, CT_ASREF, CT_ASDEPENDS, CT_ASREFFULL, CT_ASDEPENDSFULL, CT_ASPATH };
	void CopySelectedTasksToClipboard(int nAsFormat);

	void RefreshFilterControls();
	void RefreshFilterLabelAlignment();

	void ResizeDlg(int cx = 0, int cy = 0, BOOL bMaximized = FALSE);
	BOOL CalcToDoCtrlRect(CRect& rect, int cx = 0, int cy = 0, BOOL bMaximized = FALSE);
	int ReposTabBar(CDeferWndMove& dwm, const CPoint& ptOrg, int nWidth, BOOL bCalcOnly = FALSE);

	void PrepareEditMenu(CMenu* pMenu);
	void PrepareSortMenu(CMenu* pMenu);
	void PrepareSourceControlMenu(CMenu* pMenu);

	void AddFindResult(const SEARCHRESULT& result, const CFilteredToDoCtrl* pTDC);

	void PrepareToolbar(int nOption);
	void SetToolbarOption(int nOption);
	void AppendTools2Toolbar(BOOL bAppend = TRUE);

	CFilteredToDoCtrl& GetToDoCtrl();
	CFilteredToDoCtrl& GetToDoCtrl(int nIndex);
	const CFilteredToDoCtrl& GetToDoCtrl() const;
	const CFilteredToDoCtrl& GetToDoCtrl(int nIndex) const;
	CFilteredToDoCtrl* NewToDoCtrl();
	BOOL CreateToDoCtrl(CFilteredToDoCtrl* pCtrl);
	int AddToDoCtrl(CFilteredToDoCtrl* pCtrl, BOOL bResizeDlg = TRUE);
	inline int GetTDCCount() const
	{
		return m_mgrToDoCtrls.GetCount();
	}
	BOOL SelectToDoCtrl(LPCTSTR szFilePath, BOOL bCheckPassword, int nNotifyDueTasksBy = -1);
	BOOL SelectToDoCtrl(int nIndex, BOOL bCheckPassword, int nNotifyDueTasksBy = -1);
	int GetSelToDoCtrl() const;
	BOOL CreateNewTaskList(BOOL bAddDefTask);
	BOOL VerifyToDoCtrlPassword() const;
	BOOL VerifyToDoCtrlPassword(int nIndex) const;

	// caller must flush todoctrls if required before calling these
	BOOL CloseToDoCtrl(int nIndex);
	TDC_FILE ConfirmSaveTaskList(int nIndex, DWORD dwFlags = 0);
	TDC_FILE SaveTaskList(int nIndex, LPCTSTR szFilePath = NULL, BOOL bAuto = FALSE); // returns FALSE only if the user was prompted to save and cancelled
	TDC_FILE SaveAll(DWORD dwFlags); // returns FALSE only if the user was prompted to save and cancelled
	BOOL DoBackup(int nIndex);

	void UpdateTooltip();
	void UpdateCaption();
	void UpdateStatusbar();
	void UpdateTabSwitchTooltip();

	void UpdateToDoCtrlPreferences(CFilteredToDoCtrl* pCtrl);
	void UpdateToDoCtrlPreferences();
	CPreferencesDlg& Prefs() const;
	void ResetPrefs();

	UINT MapNewTaskPos(int nPos, BOOL bSubtask);
	UINT GetNewTaskCmdID();
	UINT GetNewSubtaskCmdID();

	int MessageBox(UINT nIDText, UINT nIDCaption, UINT nType = MB_OK);
	int MessageBox(LPCTSTR szText, UINT nIDCaption, UINT nType = MB_OK);
	int MessageBox(LPCTSTR szText, LPCTSTR szCaption, UINT nType = MB_OK)
	{
		return CFrameWnd::MessageBox(szText, szCaption, nType);
	}

	// helpers
	int GetTasks(CFilteredToDoCtrl& tdc, BOOL bHtmlComments, BOOL bTransform,
		const CTaskSelectionDlg& taskSel, CTaskFile& tasks) const;
	int GetTasks(CFilteredToDoCtrl& tdc, BOOL bHtmlComments, BOOL bTransform,
		TSD_TASKS nWhatTasks, TDCGETTASKS& filter, CTaskFile& tasks) const;
	int GetTasks(CFilteredToDoCtrl& tdc, TSD_TASKS nWhatTasks, CTaskFile& tasks) const;

	void DoSendTasks(TD_SENDWHAT nWhat, TD_SENDAS nSendAs);
	
	TDC_ARCHIVE GetAutoArchiveOptions(LPCTSTR szFilePath, CString& sArchivePath, BOOL& bRemoveFlagged) const;

	static void PrepareOpenFilePath(CString& sFilePath);
	static void EnableTDLProtocol(BOOL bEnable);

	static void SetupUIStrings();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TODOLISTWND_H__13051D32_D372_4205_BA71_05FAC2159F1C__INCLUDED_)
