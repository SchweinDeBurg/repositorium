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
// - merged with ToDoList version 6.1.2-6.2.2 sources
//*****************************************************************************

#if !defined(AFX_PREFERENCESUITASKLISTCOLORSPAGE_H__9612D6FB_2A00_46DA_99A4_1AC6270F060D__INCLUDED_)
#define AFX_PREFERENCESUITASKLISTCOLORSPAGE_H__9612D6FB_2A00_46DA_99A4_1AC6270F060D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// PreferencesUITasklistColorsPage.h : header file
//

#include "../../Common/FileEdit.h"
#include "../../Common/ColorButton.h"
#include "../../../CodeProject/Source/FontComboBox.h"
#include "../../../CodeProject/Source/AutoComboBox.h"
#include "../../Common/PreferencesBase.h"
#include "../../Common/GroupLine.h"

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CPreferencesUITasklistColorsPage dialog

enum { COLOROPT_CATEGORY, COLOROPT_PRIORITY, COLOROPT_DEFAULT, COLOROPT_NONE };

struct CATCOLOR
{
	CString sCategory;
	COLORREF color;
};

const UINT WM_PUITCP_CATEGORYCHANGE = ::RegisterWindowMessage(_T("WM_PUITCP_CATEGORYCHANGE"));

typedef CArray<CATCOLOR, CATCOLOR&> CCatColorArray;

class CPreferencesUITasklistColorsPage : public CPreferencesPageBase
{
	DECLARE_DYNCREATE(CPreferencesUITasklistColorsPage)

// Construction
public:
	CPreferencesUITasklistColorsPage();
	~CPreferencesUITasklistColorsPage();

	BOOL GetColorPriority() const
	{
		return m_bColorPriority;
	}
	int GetTextColorOption() const
	{
		return m_nTextColorOption;
	}
	BOOL GetHidePriorityNumber() const
	{
		return m_bHidePriorityNumber;
	}
	int GetPriorityColors(CDWordArray& aColors) const;
	int GetCategoryColors(CCatColorArray& aColors) const;
	BOOL GetTreeFont(CString& sFaceName, int& nPointSize) const;
	BOOL GetCommentsFont(CString& sFaceName, int& nPointSize) const;
	COLORREF GetGridlineColor() const
	{
		return m_bSpecifyGridColor ? m_crGridlines : -1;
	}
	COLORREF GetDoneTaskColor() const
	{
		return m_bSpecifyDoneColor ? m_crDone : -1;
	}
	COLORREF GetAlternateLineColor() const
	{
		return m_bAlternateLineColor ? m_crAltLine : -1;
	}
	void GetDueTaskColors(COLORREF& crDue, COLORREF& crDueToday) const;
	BOOL GetColorTaskBackground() const
	{
		return m_bColorTaskBackground;
	}
	BOOL GetCommentsUseTreeFont() const
	{
		return m_bSpecifyTreeFont && m_bCommentsUseTreeFont;
	}
	COLORREF GetFlaggedTaskColor() const
	{
		return m_bSpecifyFlaggedColor ? m_crFlagged : -1;
	}

	void DeleteCategory(LPCTSTR szCategory);
	void AddCategory(LPCTSTR szCategory);

protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesUITasklistColorsPage)
	enum { IDD = IDD_PREFUITASKLISTCOLORS_PAGE };
	CAutoComboBox   m_cbCategories;
	BOOL    m_bColorTaskBackground;
	BOOL    m_bCommentsUseTreeFont;
	BOOL    m_bHLSColorGradient;
	BOOL    m_bHidePriorityNumber;
	BOOL    m_bAlternateLineColor;
	int     m_nTextColorOption;
	CString m_sSelCategory;
	BOOL    m_bSpecifyDueColor;
	//}}AFX_DATA
	BOOL    m_bSpecifyDueTodayColor;
	BOOL    m_bSpecifyGridColor;
	BOOL    m_bSpecifyDoneColor;
	BOOL    m_bSpecifyFlaggedColor;
	CColorButton    m_btFilteredColor;
	CColorButton    m_btCatColor;
	CColorButton    m_btDoneColor;
	CColorButton    m_btGridlineColor;
	CColorButton    m_btDueColor;
	CColorButton    m_btDueTodayColor;
	CColorButton    m_btFlaggedColor;
	CComboBox   m_cbTreeFontSize, m_cbCommentsFontSize;
	CFontComboBox   m_cbTreeFonts, m_cbCommentsFonts;
	BOOL    m_bSpecifyTreeFont;
	BOOL    m_bSpecifyCommentsFont;
	CColorButton    m_btSetColor;
	CColorButton    m_btLowColor;
	CColorButton    m_btHighColor;
	BOOL    m_bColorPriority;
	int     m_bGradientPriorityColors;
	int     m_nSelPriorityColor;
	BOOL    m_bShowTimeColumn;
	CDWordArray m_aPriorityColors;
	CCatColorArray m_aCategoryColors;
	COLORREF m_crLow, m_crHigh;
	CString m_sTreeFont;
	int     m_nTreeFontSize;
	CString m_sCommentsFont;
	int     m_nCommentsFontSize;
	COLORREF m_crGridlines, m_crDone;
	CColorButton    m_btAltLineColor;
	COLORREF m_crAltLine;
	COLORREF m_crDue, m_crDueToday;
	CGroupLineManager m_mgrGroupLines;
	COLORREF m_crFlagged;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesUITasklistColorsPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesUITasklistColorsPage)
	afx_msg void OnAltlinecolor();
	afx_msg void OnSpecifyAlternatelinecolor();
	afx_msg void OnChangeTextColorOption();
	afx_msg void OnSetcategorycolor();
	afx_msg void OnEditCategorycolors();
	afx_msg void OnSelchangeCategorycolors();
	afx_msg void OnCommentsusetreefont();
	afx_msg void OnSpecifycommentsfont();
	afx_msg void OnDuetaskcolor();
	afx_msg void OnSetduetaskcolor();
	//}}AFX_MSG
	virtual BOOL OnInitDialog();
	afx_msg void OnDuetodaytaskcolor();
	afx_msg void OnSetduetodaytaskcolor();
	afx_msg void OnSpecifytreefont();
	afx_msg void OnSetgridlinecolor();
	afx_msg void OnSpecifygridlinecolor();
	afx_msg void OnSetdonecolor();
	afx_msg void OnSpecifydonecolor();
	afx_msg void OnLowprioritycolor();
	afx_msg void OnHighprioritycolor();
	afx_msg void OnSetprioritycolor();
	afx_msg void OnChangePriorityColorOption();
	afx_msg void OnColorPriority();
	afx_msg void OnSelchangePrioritycolors();
	afx_msg LRESULT OnCategoryAdded(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCategoryDeleted(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSpecifyflaggedcolor();
	afx_msg void OnSetflaggedcolor();

	DECLARE_MESSAGE_MAP()

protected:
	int FindCategoryColor(LPCTSTR szCategory);

	virtual void LoadPreferences(const CPreferences& prefs);
	virtual void SavePreferences(CPreferences& prefs);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESUITASKLISTPAGE_H__9612D6FB_2A00_46DA_99A4_1AC6270F060D__INCLUDED_)
