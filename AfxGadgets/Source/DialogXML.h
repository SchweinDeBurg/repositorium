// AfxGadgets library.
// Copyright (c) 2005 by Elijah Zarezky,
// All rights reserved.

// DialogXML.h - interface of the CDialogXML class

#if !defined(__DialogXML_h)
#define __DialogXML_h

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

class CDialogXML: public CDialog
{
	DECLARE_DYNAMIC(CDialogXML)
	DECLARE_MESSAGE_MAP()

// construction/destruction
public:
	CDialogXML(LPCTSTR pszDialogName, CWnd* pParentWnd = NULL);
	virtual ~CDialogXML(void);

// initilization
public:
	static const DLGTEMPLATE* CreateDlgTemplate(LPCTSTR pszFileXML);

// overridables
public:
	virtual int DoModal(void);
	virtual void GetXMLpath(CString& strDest);

// styles parsing
public:
	typedef DWORD(__cdecl* PFN_StylesParserProc)(LPCTSTR);
	static void SetStylesParser(LPCTSTR pszClassName, PFN_StylesParserProc pfnParser);
	static BOOL RemoveStylesParser(LPCTSTR pszClassName);
	struct WINDOW_STYLE { LPCTSTR pszName; DWORD fdwValue; };
	static DWORD ParseStyles(WINDOW_STYLE awsDict[], LPCTSTR pszStylesStr);
	// predefined parsers
	static DWORD Parse_Window(LPCTSTR pszStylesStr);
	static DWORD Parse_WindowEx(LPCTSTR pszStylesStr);
	static DWORD Parse_Dialog(LPCTSTR pszStylesStr);
	static DWORD Parse_Button(LPCTSTR pszStylesStr);
	static DWORD Parse_Edit(LPCTSTR pszStylesStr);
	static DWORD Parse_Static(LPCTSTR pszStylesStr);
	static DWORD Parse_ListBox(LPCTSTR pszStylesStr);
	static DWORD Parse_ScrollBar(LPCTSTR pszStylesStr);
	static DWORD Parse_ComboBox(LPCTSTR pszStylesStr);
#if defined(_INC_COMMCTRL)
	static DWORD Parse_Header(LPCTSTR pszStylesStr);
	static DWORD Parse_Toolbar(LPCTSTR pszStylesStr);
	static DWORD Parse_ReBar(LPCTSTR pszStylesStr);
	static DWORD Parse_ToolTip(LPCTSTR pszStylesStr);
	static DWORD Parse_StatusBar(LPCTSTR pszStylesStr);
	static DWORD Parse_TrackBar(LPCTSTR pszStylesStr);
	static DWORD Parse_UpDown(LPCTSTR pszStylesStr);
	static DWORD Parse_ProgressBar(LPCTSTR pszStylesStr);
	static DWORD Parse_HotKey(LPCTSTR pszStylesStr);
	static DWORD Parse_ListView(LPCTSTR pszStylesStr);
	static DWORD Parse_TreeView(LPCTSTR pszStylesStr);
	static DWORD Parse_TabControl(LPCTSTR pszStylesStr);
	static DWORD Parse_Animate(LPCTSTR pszStylesStr);
	static DWORD Parse_MonthCal(LPCTSTR pszStylesStr);
	static DWORD Parse_DateTimePick(LPCTSTR pszStylesStr);
	static DWORD Parse_Pager(LPCTSTR pszStylesStr);
#endif	// _INC_COMMCTRL
#if defined(_RICHEDIT_)
	static DWORD Parse_RichEdit(LPCTSTR pszStylesStr);
#endif	// _RICHEDIT_

// attributes
public:
	CString m_strDialogName;
	static CMap<CString, LPCTSTR, PFN_StylesParserProc, PFN_StylesParserProc> m_mapParsers;
	static void InitParsersMap(void);

// implementation helpers
private:
	static void DlgTemplateHelper(CMemFile& fileMem, CPugXmlBranch& branchDialog);
	static void DlgTemplateExHelper(CMemFile& fileMem, CPugXmlBranch& branchDialog);

// diagnostic services
#if defined(_DEBUG)
public:
	virtual void AssertValid(void) const;
	virtual void Dump(CDumpContext& dumpCtx) const;
#endif
};

#endif	// __DialogXML_h

// end of file
