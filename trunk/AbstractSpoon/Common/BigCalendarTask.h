// Written for the ToDoList (http://www.codeproject.com/KB/applications/todolist2.aspx)
// Design and latest version - http://www.codeproject.com/KB/applications/TDL_Calendar.aspx
// by demon.code.monkey@googlemail.com

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - improved compatibility with the MFC version 9.0
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

#ifndef _BIGCALENDARTASK_H_
#define _BIGCALENDARTASK_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../CodeProject/Source/TransparentListBox.h"

class CBigCalendarCtrl;


class CBigCalendarTask : public CTransparentListBox
{
public:
	CBigCalendarTask(CBigCalendarCtrl* _pParent = NULL, DWORD _dwStyleCompletedTasks = 0);
	virtual ~CBigCalendarTask();

	int  AddItem(const CString& _strText, DWORD _dwTaskID, BOOL _bStartTask, BOOL _bCompleteTask);
	void ResetContent();

protected:
	int	 OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	BOOL OnToolTipText(UINT id, NMHDR* pTTTStruct, LRESULT* pResult);

	//{{AFX_MSG(CBigCalendarTask)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
protected:
	virtual void PreSubclassWindow();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	CBigCalendarCtrl*   m_pParent;

private:
	CMap <DWORD, DWORD&, BOOL, BOOL&> m_mapTaskIDToStartTask;
	CMap <DWORD, DWORD&, BOOL, BOOL&> m_mapTaskIDToCompleteTask;
};

#endif//_BIGCALENDARTASK_H_
