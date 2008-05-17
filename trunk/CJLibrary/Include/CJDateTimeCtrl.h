// CJDateTimeCtrl.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
// Copied from MFC, Visual C++ 6, to extend functionality to Visual C++ 5 
// users, with some ideas taken from Luis Barreira's article 'Classes for new 
// IE4 controls' http://www.codeguru.com/controls/ie4_controls_classes.shtml
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 8/31/99 1:11a $
 * $Revision: 7 $
 * $Archive: /CodeJockey/Include/CJDateTimeCtrl.h $
 *
 * $History: CJDateTimeCtrl.h $
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 7/31/99    Time: 4:27p
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:17p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:04a
 * Created in $/CodeJockey/Include
 * Copied from MFC, Visual C++ 6 and vc5 Tech. Preview. Added to extend
 * functionality to Visual C++ 5 users.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJDATETIMECTRL_H__
#define __CJDATETIMECTRL_H__


class CCJMonthCalCtrl;

// A CCJDateTimeCtrl object encapsulates the functionality of a date and time picker 
// control. The date and time picker control (DTP control) provides a simple interface 
// to exchange date and time information with a user. This interface contains fields, 
// each of which displays a part of the date and time information stored in the control. 
// The user can change the information stored in the control by changing the content of 
// the string in a given field. The user can move from field to field using the mouse or 
// the keyboard.
class _CJX_EXT_CLASS CCJDateTimeCtrl : public CWnd
{
	DECLARE_DYNAMIC(CCJDateTimeCtrl)

// Construction / destruction
public:

	// Constructs a CDateTimeCtrl object.
	//
	CCJDateTimeCtrl();

	// Destroys a CDateTimeCtrl object.
	//
	virtual ~CCJDateTimeCtrl();

// Initialization
public:

	// Creates the date and time picker control and attaches it to the 
	// CDateTimeCtrl object. Returns nonzero if creation was successful; 
	// otherwise 0.
	//
	BOOL Create(
		// Specifies the combination of date time control styles. SeeDate 
		// and Time Picker Control Styles in the Platform SDK for more information 
		//about date and time picker styles.
		DWORD dwStyle,
		// A reference to aRECT structure, which is the position and size of the 
		// date and time picker control.
		const RECT& rect,
		// A pointer to a CWnd object that is the parent window of the date and 
		// time picker control. It must not be NULL.
		CWnd* pParentWnd,
		// Specifies the date and time picker control's control ID.
		UINT nID);
	
// Attributes
public:

	// Retrieves the color for a given portion of the month calendar within the 
	// date and time picker control. Returns a COLORREF value that represents the 
	// color setting for the specified portion of the month calendar control if 
	// successful. The function returns -1 if unsuccessful. 
	//
	COLORREF GetMonthCalColor(
		// An int value specifying which color area of the month calendar to 
		// retrieve. For a list of values, see the iColor parameter for 
		// SetMonthCalColor.
		int iColor) const;

	// Sets the color for a given portion of the month calendar within a date 
	// and time picker control. Returns a COLORREF value that represents the 
	// previous color setting for the specified portion of the month calendar 
	// control if successful. Otherwise, the message returns -1. 
	//
	COLORREF SetMonthCalColor(
		// int value specifying which area of the month calendar control to set. 
		// This value can be one of the following.
		//
		// Value				Meaning  
		// MCSC_BACKGROUND		Set the background color displayed between months.  
		// MCSC_MONTHBK			Set the background color displayed within a month.  
		// MCSC_TEXT			Set the color used to display text within a month.  
		// MCSC_TITLEBK			Set the background color displayed in the calendar's title.  
		// MCSC_TITLETEXT		Set the color used to display text within the calendar's title.  
		// MCSC_TRAILINGTEXT	Set the color used to display header and trailing-day text. Header and trailing days are the days from the previous and following months that appear on the current calendar.  
		int iColor,
		// A COLORREF value representing the color that will be set for the specified 
		// area of the month calendar.
		COLORREF ref);

	// Sets the display of a date and time picker control based on a given format string.
	// Returns nonzero if successful; otherwise 0.
	//
	BOOL SetFormat(
		// A pointer to a zero-terminated format string that defines the desired display. 
		// Setting this parameter to NULL will reset the control to the default format 
		// string for the current style.
		LPCTSTR pstrFormat);

	// Retrieves the date and time picker's child month calendar control.
	// Returns a pointer to a CMonthCalCtrl object, or NULL if unsuccessful or if 
	// the window is not visible.
	//
	CCJMonthCalCtrl* GetMonthCalCtrl() const;

	// Retrieves the font currently used by the date and time picker control's child 
	// month calendar control. Returns a pointer to a CFont object, or NULL if unsuccessful.
	//
	CFont* GetMonthCalFont() const;

	// Sets the font to be used by the date and time picker control's child month 
	// calendar control.
	void SetMonthCalFont(
		// Handle to the font that will be set. 
		HFONT hFont,
		// Specifies whether the control should be redrawn immediately upon setting the 
		// font. Setting this parameter to TRUE causes the control to redraw itself. 
		BOOL bRedraw = TRUE);

	// Sets the minimum and maximum allowable system times for a date and time 
	// picker control. Returns nonzero if successful; otherwise 0. 
	//
	BOOL SetRange(
		// A pointer to a COleDateTime object containing the 
		// earliest time allowed in the CDateTimeCtrl object.
		const COleDateTime* pMinRange,
		// A pointer to a COleDateTime object containing the 
		// latest time allowed in the CDateTimeCtrl object. 
		const COleDateTime* pMaxRange);

	// Sets the minimum and maximum allowable system times for a date and time 
	// picker control. Returns nonzero if successful; otherwise 0. 
	//
	BOOL SetRange(
		// A pointer to a CTime object containing the 
		// earliest time allowed in the CDateTimeCtrl object.
		const CTime* pMinRange,
		// A pointer to a CTime object containing the 
		// latest time allowed in the CDateTimeCtrl object. 
		const CTime* pMaxRange);

	// Retrieves the current minimum and maximum allowable system times for a date 
	// and time picker control. Returns a DWORD value containing flags that indicate 
	// which ranges are set.
	//
	DWORD GetRange(
		// A pointer to a COleDateTime object containing the 
		// earliest time allowed in the CDateTimeCtrl object.
		COleDateTime* pMinRange,
		// A pointer to a COleDateTime object containing the 
		// latest time allowed in the CDateTimeCtrl object. 
		COleDateTime* pMaxRange) const;

	// Retrieves the current minimum and maximum allowable system times for a date 
	// and time picker control. Returns a DWORD value containing flags that indicate 
	// which ranges are set.
	//
	DWORD GetRange(
		// A pointer to a CTime object containing the 
		// earliest time allowed in the CDateTimeCtrl object.
		CTime* pMinRange,
		// A pointer to a CTime object containing the 
		// latest time allowed in the CDateTimeCtrl object. 
		CTime* pMaxRange) const;
	
// Operations
public:

	// Sets the time in a date and time picker control. Returns nonzero if successful; 
	// otherwise 0. 
	//
	BOOL SetTime(
		// A pointer to a CTime object containing the time to which the control will be set.
		const CTime* pTimeNew);

	// Sets the time in a date and time picker control. Returns nonzero if successful; 
	// otherwise 0. 
	//
	BOOL SetTime(
		// A reference to a COleDateTime object containing the to which the control 
		// will be set.
		const COleDateTime& timeNew);

	// Sets the time in a date and time picker control. Returns nonzero if successful; 
	// otherwise 0. 
	//
	BOOL SetTime(
		// A pointer to a SYSTEMTIME structure containing the time to which the control 
		// will be set.
		LPSYSTEMTIME pTimeNew = NULL);

	// Retrieves the currently selected time from a date and time picker control and 
	// places it in a specified SYSTEMTIME structure. Returns a DWORD value equal to 
	// the dwFlag member set in theNMDATETIMECHANGE structure
	//
	DWORD GetTime(
		// a reference to a CTime object that will receive the system time information
		CTime& timeDest) const;

	// Retrieves the currently selected time from a date and time picker control and 
	// places it in a specified SYSTEMTIME structure. Returns nonzero if the time is 
	// successfully written to the COleDateTime object; otherwise 0.
	//
	BOOL GetTime(
		// a reference to a COleDateTime object that will receive the system 
		// time information
		COleDateTime& timeDest) const;

	// Retrieves the currently selected time from a date and time picker control and 
	// places it in a specified SYSTEMTIME structure. Returns a DWORD value equal to 
	// the dwFlag member set in theNMDATETIMECHANGE structure
	//
	DWORD GetTime(
		// A pointer to theSYSTEMTIME structure to receive the system time information. 
		// Must not be NULL.
		LPSYSTEMTIME pTimeDest) const;
};

// A CCJMonthCalCtrl object encapsulates the functionality of a month calendar control. 
// The month calendar control provides the user with a simple calendar interface, from 
// which the user can select a date.
class _CJX_EXT_CLASS CCJMonthCalCtrl : public CWnd
{
  DECLARE_DYNAMIC(CCJMonthCalCtrl)
	  

// Construction / destruction
public:

	// Constructs a CCJMonthCalCtrl object.
	//
	CCJMonthCalCtrl();

	// Destroys a CCJMonthCalCtrl object.
	//
	virtual ~CCJMonthCalCtrl();

// Initialization
public:

	// Creates a month calendar control and attaches it to the CMonthCalCtrl object.
	// Returns nonzero if initialization was successful; otherwise 0.
	//
	BOOL Create(
		// Specifies the combination of Windows styles applied to the month 
		// calendar control. SeeMonth Calendar Control Styles in the Platform 
		// SDK for more information about the styles.
		DWORD dwStyle,
		// A reference to aRECT structure. Contains the position and size of 
		// the month calendar control
		const RECT& rect,
		// A pointer to a CWnd object that is the parent window of the month
		// calendar control. It must not be NULL.
		CWnd* pParentWnd,
		// Specifies the month calendar control’s control ID.
		UINT nID);

	// Creates a month calendar control and attaches it to the CMonthCalCtrl object.
	// Returns nonzero if initialization was successful; otherwise 0.
	//
	BOOL Create(
		// Specifies the combination of Windows styles applied to the month 
		// calendar control. SeeMonth Calendar Control Styles in the Platform 
		// SDK for more information about the styles.
		DWORD dwStyle,
		// A reference to aPOINT structure that identifies the location of 
		// the month calendar control
		const POINT& pt,
		// A pointer to a CWnd object that is the parent window of the month
		// calendar control. It must not be NULL.
		CWnd* pParentWnd,
		// Specifies the month calendar control’s control ID.
		UINT nID);
	
//Attributes
public:

	// Retrieves the minimum size required to show a full month in a month calendar 
	// control. Returns nonzero and lpRect receives the applicable bounding information.
	// If unsuccessful, the member function returns 0.
	//
	BOOL GetMinReqRect(
		// A pointer to aRECT structure that will receive bounding rectangle information. 
		// This parameter must be a valid address and cannot be NULL. 
		RECT* pRect) const;

	// Sets the scroll rate for a month calendar control. Returns the previous scroll 
	// rate. If the scroll rate has not been previously set, the return value is 0. 
	//
	int SetMonthDelta(
		// The number of months to be set as the control's scroll rate. If this value 
		// is zero, the month delta is reset to the default, which is the number of 
		// months displayed in the control. 
		int iDelta);

	// Retrieves the scroll rate for a month calendar control. Returns The scroll rate 
	// for the month calendar control. The scroll rate is the number of months that the 
	// control moves its display when the user clicks a scroll button once. 
	//
	int GetMonthDelta() const;

	// Sets the day of week to be displayed in the leftmost column of the calendar. 
	// Returns nonzero if the previous first day of the week is set to a value other 
	// than that of LOCALE_IFIRSTDAYOFWEEK, which is the day indicated in the control 
	// panel setting. Otherwise, this function returns 0.
	BOOL SetFirstDayOfWeek(
		// An integer value representing which day is to be set as the first day of 
		// the week. This value must be one of the day numbers. See GetFirstDayOfWeek 
		// for a description of the day numbers.
		int iDay,
		// A pointer to an integer indicating the first day of the week previously set.
		int* lpnOld = NULL);

	// Gets the first day of the week to be displayed in the leftmost column of the 
	// calendar. Returns an integer value that represents the first day of the week. 
	// The days of the week are represented as integers, as follows.
	// 
	// Value	Day of the Week 
	// 0		Monday  
	// 1		Tuesday  
	// 2		Wednesday  
	// 3		Thursday  
	// 4		Friday  
	// 5		Saturday  
	// 6		Sunday  
	//
	int GetFirstDayOfWeek(
		// A pointer to a BOOL value. If the value is non-zero, the control's setting 
		// does not match the setting in the control panel. 
		BOOL* pbLocal = NULL) const;

	// Gets the color of a specified area of a month calendar control. Returns a COLORREF
	// value specifying the color associated with the portion of the month calendar 
	// control, if successful. Otherwise, this member function returns -1.
	//
	COLORREF GetColor(
		// The region of the month calendar control from which the color is retrieved. 
		// For a list of values, see the nRegion parameter of  SetColor.
		int nRegion) const;

	// Sets the color of a specified area of a month calendar control. Returns a COLORREF 
	// value that represents the previous color setting for the specified portion of the 
	// month calendar control, if successful. Otherwise this message returns -1. 
	//
	COLORREF SetColor(
		// An integer value specifying which month calendar color to set. This value can be one of the following.
		//
		// Value				Meaning  
		// MCSC_BACKGROUND		The background color displayed between months.  
		// MCSC_MONTHBK			The background color displayed within the month.  
		// MCSC_TEXT			The color used to display text within a month.  
		// MCSC_TITLEBK			The background color displayed in the calendar's title.  
		// MCSC_TITLETEXT		The color used to display text within the calendar's title.  
		// MCSC_TRAILINGTEXT	The color used to display header and trailing-day text. Header and trailing days are the days from the previous and following months that appear on the current calendar.  
		//
		int nRegion,
		// A COLORREF value for the new color setting for the specified portion of the
		// month calendar control
		COLORREF ref);

// Operations
public:

	// Repaints the month calendar control to its minimum, one-month size. Returns 
	// nonzero if the month calendar control is sized to its minimum; otherwise 0. 
	//
	BOOL SizeMinReq(
		// Specifies whether the control is to be repainted. By default, TRUE.
		// If FALSE, no repainting occurs.
		BOOL bRepaint = TRUE);

	// Sets the calendar control for the current day.
	//
	void SetToday(
		// A reference to a COleDateTime object that contains the current date.
		const COleDateTime& refDateTime);

	// Sets the calendar control for the current day.
	//
	void SetToday(
		// A pointer to a CTime object containing the current date information.
		const CTime* pDateTime);

	// Sets the calendar control for the current day.
	//
	void SetToday(
		// A pointer to aSYSTEMTIME structure that contains the current date information. 
		const LPSYSTEMTIME pDateTime);

	// Retrieves the date information for the date specified as "today" for a month 
	// calendar control. Returns nonzero if successful; otherwise 0.
	//
	BOOL GetToday(
		// A reference to a CTime object indicating the current day.
		CTime& refTime) const;

	// Retrieves the date information for the date specified as "today" for a month 
	// calendar control. Returns nonzero if successful; otherwise 0.
	//
	BOOL GetToday(
		// A reference to a COleDateTime object indicating the current day.
		COleDateTime& refDateTime) const;

	// Retrieves the date information for the date specified as "today" for a month 
	// calendar control. Returns nonzero if successful; otherwise 0.
	//
	BOOL GetToday(
		// A pointer to aSYSTEMTIME structure that will receive the date information.
		// This parameter must be a valid address and cannot be NULL.
		LPSYSTEMTIME pDateTime) const;

	// Retrieves the system time as indicated by the currently-selected date. Returns
	// nonzero if successful; otherwize 0.
	//
	BOOL GetCurSel(
		// A pointer to aSYSTEMTIME structure that will receive the currently-selected
		// date information. This parameter must be a valid address and cannot be NULL.
		LPSYSTEMTIME pDateTime) const;

	// Retrieves the system time as indicated by the currently-selected date. Returns
	// nonzero if successful; otherwize 0.
	//
	BOOL GetCurSel(
		// A reference to a CTime object. Receives the current time.
		CTime& refDateTime) const;

	// Retrieves the system time as indicated by the currently-selected date. Returns
	// nonzero if successful; otherwize 0.
	//
	BOOL GetCurSel(
		// A reference to a COleDateTime object. Receives the current time.
		COleDateTime& refDateTime) const;

	// Sets the currently-selected date for a month calendar control. Returns
	// nonzero if successful; otherwise 0. 
	//
	BOOL SetCurSel(
		// Pointer to aSYSTEMTIME structure that contains the date to be set
		// as the current selection. 
		const LPSYSTEMTIME pDateTime);

	// Sets the currently-selected date for a month calendar control. Returns
	// nonzero if successful; otherwise 0. 
	//
	BOOL SetCurSel(
		// A reference to a CTime object indicating the currently-selected
		// month calendar control. 
		const CTime& refDateTime);

	// Sets the currently-selected date for a month calendar control. Returns
	// nonzero if successful; otherwise 0. 
	//
	BOOL SetCurSel(
		// A reference to a COleDateTime indicating the currently-selected month
		// calendar control. 
		const COleDateTime& refDateTime);

	// Sets the display for days in a month calendar control. Returns nonzero 
	// if successful; otherwise 0. 
	//
	BOOL SetDayState(
		// Value indicating how many elements are in the array that pStates
		// points to.
		int nMonths,
		// A pointer to aMONTHDAYSTATE array of values that define how the month
		// calendar control will draw each day in its display. The MONTHDAYSTATE
		// data type is a bit field, where each bit (1 through 31) represents the
		// state of a day in a month. If a bit is on, the corresponding day will be
		// displayed in bold; otherwise it will be displayed with no emphasis. 
		LPMONTHDAYSTATE pStates);

	// Sets the maximum number of days that can be selected in a month 
	// calendar control. Returns nonzero if successful; otherwise 0. 
	//
	BOOL SetMaxSelCount(
		// 
		int nMax);

	// Retrieves the current maximum number of days that can be selected 
	// in a month calendar control. Returns an integer value that represents 
	// the total number of days that can be selected for the control.
	//
	int GetMaxSelCount() const;

	// Sets the minimum and maximum allowable dates for a month calendar control.
	// Returns nonzero if successful; otherwise 0. 
	//
	BOOL SetRange(
		// A pointer to a COleDateTime object containing the date at the lowest
		// end of the range. 
		const COleDateTime* pMinRange,
		// A pointer to a COleDateTime object containing the date at the highest
		// end of the range.
		const COleDateTime* pMaxRange);

	// Sets the minimum and maximum allowable dates for a month calendar control.
	// Returns nonzero if successful; otherwise 0. 
	//
	BOOL SetRange(
		// A pointer to a CTime object containing the date at the lowest 
		// end of the range. 
		const CTime* pMinRange,
		// A pointer to a CTime object containing the date at the highest
		// end of the range.
		const CTime* pMaxRange);

	// Sets the minimum and maximum allowable dates for a month calendar control.
	// Returns nonzero if successful; otherwise 0. 
	//
	BOOL SetRange(
		// A pointer to a SYSTEMTIME structure containing the date at the lowest 
		// end of the range. 
		const LPSYSTEMTIME pMinRange,
		// A pointer to a SYSTEMTIME structure containing the date at the highest
		// end of the range.
		const LPSYSTEMTIME pMaxRange);

	// Retrieves the current minimum and maximum dates set in a month calendar control.
	// Returns A DWORD that can be zero (no limits are set) or a combination of the 
	// following values that specify limit information. 
	// 
	// Value		Meaning  
	// GDTR_MAX		A maximum limit is set for the control;
	//				pMaxRange is valid and contains the applicable date information.  
	// GDTR_MIN		A minimum limit is set for the control;
	//				pMinRange is valid and contains the applicable date information.  
	//
	DWORD GetRange(
		// A pointer to a COleDateTime object containing the date at the lowest end 
		// of the range. 
		COleDateTime* pMinRange,
		// A pointer to a COleDateTime object containing the date at the highest end 
		// of the range.
		COleDateTime* pMaxRange) const;

	// Retrieves the current minimum and maximum dates set in a month calendar control.
	// Returns A DWORD that can be zero (no limits are set) or a combination of the 
	// following values that specify limit information. 
	// 
	// Value		Meaning  
	// GDTR_MAX		A maximum limit is set for the control;
	//				pMaxRange is valid and contains the applicable date information.  
	// GDTR_MIN		A minimum limit is set for the control;
	//				pMinRange is valid and contains the applicable date information.  
	//
	DWORD GetRange(
		// A pointer to a CTime object containing the date at the lowest end of the range. 
		CTime* pMinRange,
		// A pointer to a CTime object containing the date at the highest end of the range.
		CTime* pMaxRange) const;

	// Retrieves the current minimum and maximum dates set in a month calendar control.
	// Returns A DWORD that can be zero (no limits are set) or a combination of the 
	// following values that specify limit information. 
	// 
	// Value		Meaning  
	// GDTR_MAX		A maximum limit is set for the control;
	//				pMaxRange is valid and contains the applicable date information.  
	// GDTR_MIN		A minimum limit is set for the control;
	//				pMinRange is valid and contains the applicable date information.  
	//
	DWORD GetRange(
		// A pointer to a SYSTEMTIME structure containing the date at the 
		// lowest end of the range. 
		LPSYSTEMTIME pMinRange,
		// A pointer to a COleDateTime object, a CTime object, orSYSTEMTIME structure 
		// containing the date at the highest end of the range.
		LPSYSTEMTIME pMaxRange) const;

	// Retrieves date information representing the high and low limits of a month 
	// calendar control's display. Returns An integer that represents the range, 
	// in months, spanned by the two limits indicated by refMinRange and 
	// refMaxRange.
	//
	int GetMonthRange(
		// A reference to a COleDateTime object containing the minimum date allowed.
		COleDateTime& refMinRange,
		// A reference to a COleDateTime object containing the maximum date allowed.
		COleDateTime& refMaxRange,
		// Value specifying the scope of the range limits to be retrieved. This value must be one of the following.
		// 
		// Value			Meaning  
		// GMR_DAYSTATE		Include preceding and trailing months of visible range that are only partially displayed.  
		// GMR_VISIBLE		Include only those months that are entirely displayed.  
		DWORD dwFlags) const;

	// Retrieves date information representing the high and low limits of a month 
	// calendar control's display. Returns An integer that represents the range, 
	// in months, spanned by the two limits indicated by refMinRange and 
	// refMaxRange.
	//
	int GetMonthRange(
		// A reference to a CTime object containing the minimum date allowed.
		CTime& refMinRange,
		// A reference to a CTime object containing the maximum date allowed.
		CTime& refMaxRange,
		// Value specifying the scope of the range limits to be retrieved. This value must be one of the following.
		// 
		// Value			Meaning  
		// GMR_DAYSTATE		Include preceding and trailing months of visible range that are only partially displayed.  
		// GMR_VISIBLE		Include only those months that are entirely displayed.  
		DWORD dwFlags) const;

	// Retrieves date information representing the high and low limits of a month 
	// calendar control's display. Returns An integer that represents the range, 
	// in months, spanned by the two limits indicated by pMinRange and pMaxRange.
	//
	int GetMonthRange(
		// A pointer to aSYSTEMTIME structure containing the date at the lowest 
		// end of the range.
		LPSYSTEMTIME pMinRange,
		// A pointer to a SYSTEMTIME structure containing the date at the highest 
		// end of the range.
		LPSYSTEMTIME pMaxRange,
		// Value specifying the scope of the range limits to be retrieved. This value must be one of the following.
		// 
		// Value			Meaning  
		// GMR_DAYSTATE		Include preceding and trailing months of visible range that are only partially displayed.  
		// GMR_VISIBLE		Include only those months that are entirely displayed.  
		DWORD dwFlags) const;

	// Sets the selection for a month calendar control to a given date range.
	// Returns nonzero if successful; otherwise 0. 
	//
	BOOL SetSelRange(
		// A pointer to a COleDateTime object containing the date at the lowest 
		// end of the range. 
		const COleDateTime& pMinRange,
		// A pointer to a COleDateTime object containing the date at the highest 
		// end of the range.
		const COleDateTime& pMaxRange);

	// Sets the selection for a month calendar control to a given date range.
	// Returns nonzero if successful; otherwise 0. 
	//
	BOOL SetSelRange(
		// A pointer to a CTime object containing the date at the lowest 
		// end of the range. 
		const CTime& pMinRange,
		// A pointer to a CTime object containing the date at the highest
		// end of the range.
		const CTime& pMaxRange);

	// Sets the selection for a month calendar control to a given date range.
	// Returns nonzero if successful; otherwise 0. 
	//
	BOOL SetSelRange(
		// A pointer to a SYSTEMTIME structure containing the date at the lowest 
		// end of the range. 
		const LPSYSTEMTIME pMinRange,
		// A pointer to a SYSTEMTIME structure containing the date at the highest
		// end of the range.
		const LPSYSTEMTIME pMaxRange);

	// Retrieves date information representing the upper and lower limits of 
	// the date range currently selected by the user. Returns nonzero if successful; 
	// otherwise 0.  
	//
	BOOL GetSelRange(
		// A reference to a COleDateTime object containing the minimum date allowed.
		COleDateTime& refMinRange,
		// A reference to a COleDateTime object containing the maximum date allowed.
		COleDateTime& refMaxRange) const;

	// Retrieves date information representing the upper and lower limits of 
	// the date range currently selected by the user. Returns nonzero if successful; 
	// otherwise 0.  
	//
	BOOL GetSelRange(
		// A reference to a CTime object containing the minimum date allowed.
		CTime& refMinRange,
		// A reference to a CTime object containing the maximum date allowed.
		CTime& refMaxRange) const;

	// Retrieves date information representing the upper and lower limits of 
	// the date range currently selected by the user. Returns nonzero if successful; 
	// otherwise 0.  
	//
	BOOL GetSelRange(
		// A pointer to aSYSTEMTIME structure containing the date at the lowest 
		// end of the range.
		LPSYSTEMTIME pMinRange,
		// A pointer to a SYSTEMTIME structure containing the date at the highest 
		// end of the range.
		LPSYSTEMTIME pMaxRange) const;

	// Determines which portion of a month calendar control is at a given point on 
	// the screen. Returns A DWORD value. Equal to the uHit member of the MCHITTESTINFO
	// structure.
	//
	DWORD HitTest(
		// A pointer to aMCHITTESTINFO structure containing hit testing points for the 
		// month calendar control.
		PMCHITTESTINFO pMCHitTest);
};

/////////////////////////////////////////////////////////////////////////////
// Standard Dialog Data Exchange routines
/////////////////////////////////////////////////////////////////////////////

void AFXAPI DDX_CJDateTimeCtrl(CDataExchange* pDX, int nIDC, COleDateTime& value);
void AFXAPI DDX_CJDateTimeCtrl(CDataExchange* pDX, int nIDC, CTime& value);
void AFXAPI DDX_CJMonthCalCtrl(CDataExchange* pDX, int nIDC, COleDateTime& value);
void AFXAPI DDX_CJMonthCalCtrl(CDataExchange* pDX, int nIDC, CTime& value);

/////////////////////////////////////////////////////////////////////////////
// Standard Dialog Data Validation routines
/////////////////////////////////////////////////////////////////////////////

void AFXAPI DDV_CJMinMaxDateTime(CDataExchange* pDX, CTime& refValue, const CTime* pMinRange, const CTime* pMaxRange);
void AFXAPI DDV_CJMinMaxDateTime(CDataExchange* pDX,  COleDateTime& refValue, const COleDateTime* pMinRange, const COleDateTime* pMaxRange);
void AFXAPI DDV_CJMinMaxMonth(CDataExchange* pDX, CTime& refValue, const CTime* pMinRange, const CTime* pMaxRange);
void AFXAPI DDV_CJMinMaxMonth(CDataExchange* pDX, COleDateTime& refValue, const COleDateTime* pMinRange, const COleDateTime* pMaxRange);

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

//CJDateTimeCtrl
_CJXLIB_INLINE CCJDateTimeCtrl::CCJDateTimeCtrl()
	{ }
_CJXLIB_INLINE CFont* CCJDateTimeCtrl::GetMonthCalFont() const
	{ ASSERT(::IsWindow(m_hWnd)); return CFont::FromHandle((HFONT) ::SendMessage(m_hWnd, DTM_GETMCFONT, 0, 0)); }
_CJXLIB_INLINE CCJMonthCalCtrl* CCJDateTimeCtrl::GetMonthCalCtrl() const
	{ ASSERT(::IsWindow(m_hWnd)); return (CCJMonthCalCtrl*) CWnd::FromHandle((HWND) ::SendMessage(m_hWnd, DTM_GETMONTHCAL, 0, 0)); }
_CJXLIB_INLINE void CCJDateTimeCtrl::SetMonthCalFont(HFONT hFont, BOOL bRedraw /* = TRUE */)
	{ ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, DTM_SETMCFONT, (WPARAM) hFont, MAKELONG(bRedraw, 0)); }
_CJXLIB_INLINE COLORREF CCJDateTimeCtrl::SetMonthCalColor(int iColor, COLORREF ref)
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, DTM_SETMCCOLOR, (WPARAM) iColor, (LPARAM) ref); }
_CJXLIB_INLINE DWORD CCJDateTimeCtrl::GetTime(LPSYSTEMTIME pTimeDest) const
	{ ASSERT(::IsWindow(m_hWnd)); ASSERT(pTimeDest != NULL); return (DWORD) ::SendMessage(m_hWnd, DTM_GETSYSTEMTIME, 0, (LPARAM) pTimeDest); }
_CJXLIB_INLINE COLORREF CCJDateTimeCtrl::GetMonthCalColor(int iColor) const
	{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, DTM_GETMCCOLOR, (WPARAM) iColor, 0); }
_CJXLIB_INLINE BOOL CCJDateTimeCtrl::SetFormat(LPCTSTR pstrFormat)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, DTM_SETFORMAT, 0, (LPARAM) pstrFormat); }

//CCJMonthCalCtrl
_CJXLIB_INLINE CCJMonthCalCtrl::CCJMonthCalCtrl()
	{ }
_CJXLIB_INLINE DWORD CCJMonthCalCtrl::HitTest(PMCHITTESTINFO pMCHitTest)
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, MCM_HITTEST, 0, (LPARAM) pMCHitTest); }
_CJXLIB_INLINE BOOL CCJMonthCalCtrl::GetMinReqRect(RECT* pRect) const
	{ ASSERT(m_hWnd != NULL); return (BOOL) ::SendMessage(m_hWnd, MCM_GETMINREQRECT, 0, (LPARAM) pRect); }
_CJXLIB_INLINE int CCJMonthCalCtrl::SetMonthDelta(int iDelta)
	{ ASSERT(m_hWnd != NULL); return (int) ::SendMessage(m_hWnd, MCM_SETMONTHDELTA, (WPARAM) iDelta, 0); }
_CJXLIB_INLINE int CCJMonthCalCtrl::GetMonthDelta() const
	{ ASSERT(m_hWnd != NULL); return (int) ::SendMessage(m_hWnd, MCM_GETMONTHDELTA, 0, 0); }
_CJXLIB_INLINE COLORREF CCJMonthCalCtrl::GetColor(int nRegion) const
	{ ASSERT(m_hWnd != NULL); return (COLORREF) ::SendMessage(m_hWnd, MCM_GETCOLOR, (WPARAM) nRegion, 0); }
_CJXLIB_INLINE COLORREF CCJMonthCalCtrl::SetColor(int nRegion, COLORREF ref)
	{ ASSERT(m_hWnd != NULL); return (COLORREF) ::SendMessage(m_hWnd, MCM_SETCOLOR, (WPARAM) nRegion, (LPARAM) ref); }
_CJXLIB_INLINE BOOL CCJMonthCalCtrl::SetMaxSelCount(int nMax)
	{ ASSERT(m_hWnd != NULL); return (BOOL) ::SendMessage(m_hWnd, MCM_SETMAXSELCOUNT, nMax, 0); }
_CJXLIB_INLINE int CCJMonthCalCtrl::GetMaxSelCount() const
	{ ASSERT(m_hWnd != NULL); return (int) ::SendMessage(m_hWnd, MCM_GETMAXSELCOUNT, 0, 0); }
_CJXLIB_INLINE void CCJMonthCalCtrl::SetToday(const LPSYSTEMTIME pDateTime)
	{ ASSERT(m_hWnd != NULL); ::SendMessage(m_hWnd, MCM_SETTODAY, 0, (LPARAM) pDateTime); }
_CJXLIB_INLINE BOOL CCJMonthCalCtrl::GetToday(LPSYSTEMTIME pDateTime) const
	{ ASSERT(m_hWnd != NULL); return (BOOL) ::SendMessage(m_hWnd, MCM_GETTODAY, 0, (LPARAM) pDateTime); }
_CJXLIB_INLINE BOOL CCJMonthCalCtrl::SetCurSel(const LPSYSTEMTIME pDateTime)
	{ ASSERT(m_hWnd != NULL); return (BOOL) ::SendMessage(m_hWnd, MCM_SETCURSEL, 0, (LPARAM) pDateTime); }
_CJXLIB_INLINE BOOL CCJMonthCalCtrl::GetCurSel(LPSYSTEMTIME pDateTime) const
	{ ASSERT(m_hWnd != NULL); return (BOOL) ::SendMessage(m_hWnd, MCM_GETCURSEL, 0, (LPARAM) pDateTime); }

#endif //__CJDATETIMECTRL_H__

/////////////////////////////////////////////////////////////////////////////
