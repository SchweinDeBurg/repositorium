//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
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
// - merged with ToDoList version 6.1.3 sources
//*****************************************************************************

#if !defined(AFX_CALENDARFRAMEWND_H__47616F96_0B5B_4F86_97A2_93B9DC796EB4__INCLUDED_)
#define AFX_CALENDARFRAMEWND_H__47616F96_0B5B_4F86_97A2_93B9DC796EB4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CalendarFrameWnd.h : header file
//

#include "../../Common/IUIExtension.h"
#include "../../Common/BigCalendarCtrl.h"
#include "../../Common/MiniCalendarCtrl.h"
#include "../../../CodeProject/Source/StatusBarACT.h"
#include "../../Common/CalendarDefines.h"

class CCalendarData;

/////////////////////////////////////////////////////////////////////////////
// CCalendarFrameWnd frame

class CCalendarFrameWnd : public CFrameWnd, public IUIExtensionWindow
{
	DECLARE_DYNAMIC(CCalendarFrameWnd)

public:
	CCalendarFrameWnd();           // protected constructor used by dynamic creation

// IUIExtensionWindow
	virtual BOOL Create(HWND hParent, BOOL bShow, LPSIZE pSize);
	virtual BOOL Show(BOOL bShow);
	virtual BOOL IsShowing() const;

	virtual void Update(const ITaskList* pTasks, DWORD dwFlags);
	virtual void Release();

	DWORD GetCompletedTasksStyle() const;
	int  GetNumWeeksToDisplay() const;
	int	 GetNumDaysToDisplay() const;
	BOOL IsWeekendsHidden() const;
	BOOL IsDateHidden(const COleDateTime& _dt) const;

//protected member functions
protected:
	int     GetStatusBarHeight() const;
	BOOL    InitStatusBar();
	void    UpdateStatusBar();

	void    ResizeControls(int _nxFrame, int _nyFrame);
	void    UpdateTitleBarText();

//protected member variables
protected:
	CBigCalendarCtrl    m_BigCalendar;
	CMiniCalendarCtrl   m_MiniCalendar;
	CStatusBarACT       m_StatusBar;
	CCalendarData*      m_pCalendarData;
	HWND                m_hParentOfFrame;

	DWORD               m_dwStyleCompletedTasks;
	int                 m_nNumVisibleWeeks;
	BOOL                m_bShowMiniCalendar;
	BOOL                m_bShowStatusBar;
	BOOL                m_bShowWeekends;
	BOOL                m_bWindowMaximized;
	CRect               m_rcWindowSize;
	CString             m_strTasklistName;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCalendarFrameWnd)
protected:
	virtual void PostNcDestroy();
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CCalendarFrameWnd();

	// Generated message map functions
	//{{AFX_MSG(CCalendarFrameWnd)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnViewMiniCalendar();
	afx_msg void OnUpdateViewMiniCalendar(CCmdUI* pCmdUI);
	afx_msg void OnViewStatusBar();
	afx_msg void OnUpdateViewStatusBar(CCmdUI* pCmdUI);
	afx_msg void OnViewWeekends();
	afx_msg void OnUpdateViewWeekends(CCmdUI* pCmdUI);
	afx_msg void OnViewCompletedTasks();
	afx_msg void OnUpdateViewCompletedTasks(CCmdUI* pCmdUI);
	afx_msg void OnViewNumWeeks1();
	afx_msg void OnUpdateViewNumWeeks1(CCmdUI* pCmdUI);
	afx_msg void OnViewNumWeeks2();
	afx_msg void OnUpdateViewNumWeeks2(CCmdUI* pCmdUI);
	afx_msg void OnViewNumWeeks3();
	afx_msg void OnUpdateViewNumWeeks3(CCmdUI* pCmdUI);
	afx_msg void OnViewNumWeeks4();
	afx_msg void OnUpdateViewNumWeeks4(CCmdUI* pCmdUI);
	afx_msg void OnViewNumWeeks5();
	afx_msg void OnUpdateViewNumWeeks5(CCmdUI* pCmdUI);
	afx_msg void OnViewNumWeeks6();
	afx_msg void OnUpdateViewNumWeeks6(CCmdUI* pCmdUI);
	afx_msg void OnViewNumWeeks7();
	afx_msg void OnUpdateViewNumWeeks7(CCmdUI* pCmdUI);
	afx_msg void OnViewNumWeeks8();
	afx_msg void OnUpdateViewNumWeeks8(CCmdUI* pCmdUI);
	afx_msg void OnViewNumWeeks9();
	afx_msg void OnUpdateViewNumWeeks9(CCmdUI* pCmdUI);
	afx_msg void OnGoToToday();
	afx_msg void OnUpdateGoToToday(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CALENDARFRAMEWND_H__47616F96_0B5B_4F86_97A2_93B9DC796EB4__INCLUDED_)
