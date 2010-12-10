// Written for the ToDoList (http://www.codeproject.com/KB/applications/todolist2.aspx)
// Design and latest version - http://www.codeproject.com/KB/applications/TDL_Calendar.aspx
// by demon.code.monkey@googlemail.com

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - taken out from the original TDL_Calendar package for better sharing
// - adjusted #include's paths
// - slightly reformatted source code
//*****************************************************************************

#ifndef _CALENDARDEFINES_H_
#define _CALENDARDEFINES_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define CALENDAR_DLL_NAME   _T("CalendarExt.dll")

#define CALENDAR_MSG_SELECTDATE         23356
#define CALENDAR_MSG_MOUSEWHEEL_UP      23357
#define CALENDAR_MSG_MOUSEWHEEL_DOWN    23358

enum //visual style of completed tasks
{
	COMPLETEDTASKS_HIDDEN           = 0x0001,
	COMPLETEDTASKS_STRIKETHRU       = 0x0002,
	COMPLETEDTASKS_GREY             = 0x0004,
};

#endif//_CALENDARDEFINES_H_
