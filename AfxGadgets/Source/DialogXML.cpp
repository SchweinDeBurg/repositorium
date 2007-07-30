// AfxGadgets library.
// Copyright (c) 2004-2007 by Elijah Zarezky,
// All rights reserved.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// DialogXML.cpp - implementation of the CDialogXML class

#include "stdafx.h"
#include "DialogXML.h"
#include "PugXMLplus.h"
#include "StringConv.h"

#if !defined(_INC_SHLWAPI)
#pragma message(__FILE__ " : put <shlwapi.h> in your PCH to speed up compilation")
#include <shlwapi.h>
#endif	// _INC_SHLWAPI

// PSDK header for Win95/98/NT4 builds
#if (_WIN32_WINDOWS < 0x0490) && !defined(_DBGHELP_)
#pragma message(__FILE__ " : put <dbghelp.h> in your PCH to speed up compilation")
#include <dbghelp.h>
#endif	// _WIN32_WINDOWS && _DBGHELP_

class CFindReplaceDialog;
#if (_MFC_VER < 0x0700)
#include <../src/afximpl.h>
#else
#include <../src/mfc/afximpl.h>
#endif	// _MFC_VER

#if defined(__INTEL_COMPILER)
// remark #279: controlling expression is constant
#pragma warning(disable: 279)
// remark #797: a class-qualified name is not allowed
#pragma warning(disable: 797)
// remark #981: operands are evaluated in unspecified order
#pragma warning(disable: 981)
// remark #1418: external definition with no prior declaration
#pragma warning(disable: 1418)
#endif	// __INTEL_COMPILER

#if defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// _DEBUG

// object model
IMPLEMENT_DYNAMIC(CDialogXML, CDialog)

// message map
BEGIN_MESSAGE_MAP(CDialogXML, CDialog)
END_MESSAGE_MAP()

CDialogXML::CDialogXML(LPCTSTR pszDialogName, CWnd* pParentWnd):
CDialog(),
m_strDialogName(pszDialogName)
{
	ASSERT(!m_strDialogName.IsEmpty());
	m_pParentWnd = pParentWnd;
}

CDialogXML::~CDialogXML(void)
{
	if (m_lpDialogTemplate != NULL)
	{
		free(const_cast<DLGTEMPLATE*>(m_lpDialogTemplate));
		m_lpDialogTemplate = NULL;
	}
}

const DLGTEMPLATE* CDialogXML::CreateDlgTemplate(LPCTSTR pszFileXML)
{
	CMemFile fileMem;

	ASSERT(AfxIsValidString(pszFileXML));
	ASSERT(::PathFileExists(pszFileXML));

	std::auto_ptr<CPugXmlParser> pParser(new CPugXmlParser());

	// try to parse source XML file
	if (pParser->ParseFile(pszFileXML))
	{
		// let's go ahead and fuck this iceberg!
		fileMem.Attach(NULL, 0, 1024);

		// obtain needed XML nodes
		CPugXmlBranch branchRoot = pParser->GetRoot();
		ASSERT(!branchRoot.IsNull());
		CPugXmlBranch branchDialog = branchRoot.FindByPath(_T("./Dialog"));
		ASSERT(!branchDialog.IsNull());

		// kind of dilaog template
		BOOL fExtended = GetAttribute_BOOL(branchDialog, _T("Extended"));
		if (!fExtended)
		{
			DlgTemplateHelper(fileMem, branchDialog);
		}
		else {
			DlgTemplateExHelper(fileMem, branchDialog);
		}

		// return memory address of the created template
		return (reinterpret_cast<DLGTEMPLATE*>(fileMem.Detach()));
	}
	else {
		// error parsing source XML file
		return (NULL);
	}
}

int CDialogXML::DoModal(void)
{
	using CFile::modeCreate;
	using CFile::modeWrite;
	using CFile::shareExclusive;

	CString strFileXML;

	// precondition
	ASSERT(m_lpszTemplateName == NULL);
	ASSERT(m_lpDialogTemplate == NULL);

	// get the path and ensure that it exists
	GetXMLpath(strFileXML);
	::PathAddBackslash(strFileXML.GetBuffer(_MAX_PATH));
	strFileXML.ReleaseBuffer();
	if (!::PathFileExists(strFileXML))
	{
		TRACE(_T("Warning: folder %s doesn\'t exists, trying to create.\n"), strFileXML);
#if (_WIN32_WINDOWS < 0x0490)
		if (!::MakeSureDirectoryPathExists(_T2A(strFileXML)))
		{
#else
		if (::SHCreateDirectoryEx(NULL, strFileXML, NULL) != ERROR_SUCCESS) {
#endif	// _WIN32_WINDOWS
			TRACE(_T("Error: unable to create folder %s\n"), strFileXML);
			return (-1);
		}
	}

	// construct full name of the XML-file...
	strFileXML += m_strDialogName + _T(".xml");

	// ...and if this file doesn't exists...
	if (!::PathFileExists(strFileXML))
	{
		// ...then try to create it from the corresonding resource
		TRACE(_T("Warning: file %s doesn\'t exists, trying to create.\n"), strFileXML);
		static const TCHAR szResType[] = _T("DIALOG_XML");
		HINSTANCE hInstRes = AfxFindResourceHandle(m_strDialogName, szResType);
		if (hInstRes != NULL)
		{
			HRSRC hResInfo = ::FindResource(hInstRes, m_strDialogName, szResType);
			ASSERT(hResInfo != NULL);
			HGLOBAL hResData = ::LoadResource(hInstRes, hResInfo);
			ASSERT(hResData != NULL);
			void* pvResData = ::LockResource(hResData);
			ASSERT(pvResData != NULL);
			DWORD cbSize = ::SizeofResource(hInstRes, hResInfo);
			try
			{
				CFile fileXML(strFileXML, modeCreate | modeWrite | shareExclusive);
				fileXML.Write(pvResData, cbSize);
				fileXML.Flush();
				fileXML.Close();
			}
			catch (CFileException* pXcpt)
			{
				// oops!
				::UnlockResource(hResData);
				::FreeResource(hResData);
				pXcpt->ReportError(MB_ICONSTOP | MB_OK);
				pXcpt->Delete();
				return (-1);
			}
			::UnlockResource(hResData);
			::FreeResource(hResData);
			TRACE(_T("File %s was successfully created.\n"), strFileXML);
		}
		else {
			TRACE(_T("Error: resource \"%s\" of type \"%s\" not found.\n"), m_strDialogName, szResType);
			return (-1);
		}
	}

	// try to build the dialog template
	if ((m_lpDialogTemplate = CreateDlgTemplate(strFileXML)) != NULL)
	{
		m_lpDialogInit = NULL;
		return (CDialog::DoModal());
	}
	else {
		TRACE(_T("Error: unable to create dialog template -\n"));
		TRACE(_T("\tprobably %s is not a valid XML file.\n"), strFileXML);
		return (-1);
	}

}

void CDialogXML::GetXMLpath(CString& strDest)
{
	ASSERT(*(AfxGetApp()->m_pszRegistryKey) != 0);

	LPTSTR pszPath = strDest.GetBuffer(_MAX_PATH);
	::SHGetSpecialFolderPath(NULL, pszPath, CSIDL_APPDATA, TRUE);
	::PathAddBackslash(pszPath);
	::lstrcat(pszPath, AfxGetApp()->m_pszRegistryKey);
	::PathAddBackslash(pszPath);
	::lstrcat(pszPath, AfxGetAppName());
	::PathAddBackslash(pszPath);
	strDest.ReleaseBuffer();
}

void CDialogXML::SetStylesParser(LPCTSTR pszClassName, PFN_StylesParserProc pfnParser)
{
	m_mapParsers.SetAt(pszClassName, pfnParser);
}

BOOL CDialogXML::RemoveStylesParser(LPCTSTR pszClassName)
{
	return (m_mapParsers.RemoveKey(pszClassName));
}

DWORD CDialogXML::ParseStyles(WINDOW_STYLE awsDict[], LPCTSTR pszStylesStr)
{
	ASSERT(awsDict != NULL);
	ASSERT(AfxIsValidString(pszStylesStr));

	DWORD fdwStyle = 0;
	LPTSTR pszTemp = ::StrDup(pszStylesStr);
	static const TCHAR szSeps[] = _T("\x20\t,;");
	LPTSTR pszCurStyle = _tcstok(pszTemp, szSeps);
	while (pszCurStyle != NULL)
	{
		for (int i = 0; awsDict[i].pszName != NULL; ++i)
		{
			if (::lstrcmpi(pszCurStyle, awsDict[i].pszName) == 0)
			{
				fdwStyle |= awsDict[i].fdwValue;
			}
		}
		pszCurStyle = _tcstok(NULL, szSeps);
	}
	::LocalFree(pszTemp);
	return (fdwStyle);
}

DWORD CDialogXML::Parse_Window(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("Overlapped"), WS_OVERLAPPED },
		{ _T("Popup"), WS_POPUP },
		{ _T("Child"), WS_CHILD },
		{ _T("Minimize"), WS_MINIMIZE },
		{ _T("Visible"), WS_VISIBLE },
		{ _T("Disabled"), WS_DISABLED },
		{ _T("ClipSiblings"), WS_CLIPSIBLINGS },
		{ _T("ClipChildren"), WS_CLIPCHILDREN },
		{ _T("Maximize"), WS_MAXIMIZE },
		{ _T("Caption"), WS_CAPTION },
		{ _T("Border"), WS_BORDER },
		{ _T("DlgFrame"), WS_DLGFRAME },
		{ _T("VScroll"), WS_VSCROLL },
		{ _T("HScroll"), WS_HSCROLL },
		{ _T("SysMenu"), WS_SYSMENU },
		{ _T("ThickFrame"), WS_THICKFRAME },
		{ _T("Group"), WS_GROUP },
		{ _T("TabStop"), WS_TABSTOP },
		{ _T("MinimizeBox"), WS_MINIMIZEBOX },
		{ _T("MaximizeBox"), WS_MAXIMIZEBOX },
		{ NULL, 0 }
	};
	return (ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_WindowEx(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("DlgModalFrame"), WS_EX_DLGMODALFRAME },
		{ _T("NoParentNotify"), WS_EX_NOPARENTNOTIFY },
		{ _T("Topmost"), WS_EX_TOPMOST },
		{ _T("AcceptFiles"), WS_EX_ACCEPTFILES },
		{ _T("Transparent"), WS_EX_TRANSPARENT },
		{ _T("MDIChild"), WS_EX_MDICHILD },
		{ _T("ToolWindow"), WS_EX_TOOLWINDOW },
		{ _T("WindowEdge"), WS_EX_WINDOWEDGE },
		{ _T("ClientEdge"), WS_EX_CLIENTEDGE },
		{ _T("ContextHelp"), WS_EX_CONTEXTHELP },
		{ _T("Right"), WS_EX_RIGHT },
		{ _T("Left"), WS_EX_LEFT },
		{ _T("RTLReading"), WS_EX_RTLREADING },
		{ _T("LTRReading"), WS_EX_LTRREADING },
		{ _T("LeftScrollBar"), WS_EX_LEFTSCROLLBAR },
		{ _T("RightScrollBar"), WS_EX_RIGHTSCROLLBAR },
		{ _T("ControlParent"), WS_EX_CONTROLPARENT },
		{ _T("StaticEdge"), WS_EX_STATICEDGE },
		{ _T("AppWindow"), WS_EX_APPWINDOW },
#if (_WIN32_WINNT >= 0x0500)
		{ _T("Layered"), WS_EX_LAYERED },
		{ _T("NoInheritLayout"), WS_EX_NOINHERITLAYOUT },
		{ _T("LayoutRTL"), WS_EX_LAYOUTRTL },
		{ _T("NoActivate"), WS_EX_NOACTIVATE },
#endif
#if (_WIN32_WINNT >= 0x0501)
		{ _T("Composited"), WS_EX_COMPOSITED },
#endif
		{ NULL, 0 }
	};
	return (ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_Dialog(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("AbsAlign"), DS_ABSALIGN },
		{ _T("SysModal"), DS_SYSMODAL },
		{ _T("LocalEdit"), DS_LOCALEDIT },
		{ _T("SetFont"), DS_SETFONT },
		{ _T("ModalFrame"), DS_MODALFRAME },
		{ _T("NoIdleMsg"), DS_NOIDLEMSG },
		{ _T("SetForeground"), DS_SETFOREGROUND },
		{ _T("3DLook"), DS_3DLOOK },
		{ _T("FixedSys"), DS_FIXEDSYS },
		{ _T("NoFailCreate"), DS_NOFAILCREATE },
		{ _T("Control"), DS_CONTROL },
		{ _T("Center"), DS_CENTER },
		{ _T("CenterMouse"), DS_CENTERMOUSE },
		{ _T("ContextHelp"), DS_CONTEXTHELP },
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_Button(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("PushButton"), BS_PUSHBUTTON },
		{ _T("DefPushButton"), BS_DEFPUSHBUTTON },
		{ _T("CheckBox"), BS_CHECKBOX },
		{ _T("AutoCheckBox"), BS_AUTOCHECKBOX },
		{ _T("RadioButton"), BS_RADIOBUTTON },
		{ _T("3State"), BS_3STATE },
		{ _T("Auto3State"), BS_AUTO3STATE },
		{ _T("GroupBox"), BS_GROUPBOX },
		{ _T("UserButton"), BS_USERBUTTON },
		{ _T("AutoRadioButton"), BS_AUTORADIOBUTTON },
		{ _T("OwnerDraw"), BS_OWNERDRAW },
		{ _T("Text"), BS_TEXT },
		{ _T("LeftText"), BS_LEFTTEXT },
		{ _T("Icon"), BS_ICON },
		{ _T("Bitmap"), BS_BITMAP },
		{ _T("Left"), BS_LEFT },
		{ _T("Right"), BS_RIGHT },
		{ _T("Center"), BS_CENTER },
		{ _T("Top"), BS_TOP },
		{ _T("Bottom"), BS_BOTTOM },
		{ _T("VCenter"), BS_VCENTER },
		{ _T("PushLike"), BS_PUSHLIKE },
		{ _T("MultiLine"), BS_MULTILINE },
		{ _T("Notify"), BS_NOTIFY },
		{ _T("Flat"), BS_FLAT },
		{ _T("RightButton"), BS_RIGHTBUTTON },
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_Edit(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("Left"), ES_LEFT },
		{ _T("Center"), ES_CENTER },
		{ _T("Right"), ES_RIGHT },
		{ _T("MultiLine"), ES_MULTILINE },
		{ _T("UpperCase"), ES_UPPERCASE },
		{ _T("LowerCase"), ES_LOWERCASE },
		{ _T("Password"), ES_PASSWORD },
		{ _T("AutoVScroll"), ES_AUTOVSCROLL },
		{ _T("AutoHScroll"), ES_AUTOHSCROLL },
		{ _T("NoHideSel"), ES_NOHIDESEL },
		{ _T("OEMConvert"), ES_OEMCONVERT },
		{ _T("ReadOnly"), ES_READONLY },
		{ _T("WantReturn"), ES_WANTRETURN },
		{ _T("Number"), ES_NUMBER },
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_Static(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("Left"), SS_LEFT },
		{ _T("Center"), SS_CENTER },
		{ _T("Right"), SS_RIGHT },
		{ _T("Icon"), SS_ICON },
		{ _T("BlackRect"), SS_BLACKRECT },
		{ _T("GrayRect"), SS_GRAYRECT },
		{ _T("WhiteRect"), SS_WHITERECT },
		{ _T("BlackFrame"), SS_BLACKFRAME },
		{ _T("GrayFrame"), SS_GRAYFRAME },
		{ _T("WhiteFrame"), SS_WHITEFRAME },
		{ _T("UserItem"), SS_USERITEM },
		{ _T("Simple"), SS_SIMPLE },
		{ _T("LeftNoWordWrap"), SS_LEFTNOWORDWRAP },
		{ _T("OwnerDraw"), SS_OWNERDRAW },
		{ _T("Bitmap"), SS_BITMAP },
		{ _T("EnhMetaFile"), SS_ENHMETAFILE },
		{ _T("EtchedHorz"), SS_ETCHEDHORZ },
		{ _T("EtchedVert"), SS_ETCHEDVERT },
		{ _T("EtchedFrame"), SS_ETCHEDFRAME },
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_ListBox(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("Notify"), LBS_NOTIFY },
		{ _T("Sort"), LBS_SORT },
		{ _T("NoRedraw"), LBS_NOREDRAW },
		{ _T("MultipleSel"), LBS_MULTIPLESEL },
		{ _T("OwnerDrawFixed"), LBS_OWNERDRAWFIXED },
		{ _T("OwnerDrawVariable"), LBS_OWNERDRAWVARIABLE },
		{ _T("HasStrings"), LBS_HASSTRINGS },
		{ _T("UseTabStops"), LBS_USETABSTOPS },
		{ _T("NoIntegralHeight"), LBS_NOINTEGRALHEIGHT },
		{ _T("MultiColumn"), LBS_MULTICOLUMN },
		{ _T("WantKeyboardInput"), LBS_WANTKEYBOARDINPUT },
		{ _T("ExtendedSel"), LBS_EXTENDEDSEL },
		{ _T("DisableNoScroll"), LBS_DISABLENOSCROLL },
		{ _T("NoData"), LBS_NODATA },
		{ _T("NoSel"), LBS_NOSEL },
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_ScrollBar(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("Horz"), SBS_HORZ },
		{ _T("Vert"), SBS_VERT },
		{ _T("TopAlign"), SBS_TOPALIGN },
		{ _T("LeftAlign"), SBS_LEFTALIGN },
		{ _T("BottomAlign"), SBS_BOTTOMALIGN },
		{ _T("RightAlign"), SBS_RIGHTALIGN },
		{ _T("SizeBoxTopLeftAlign"), SBS_SIZEBOXTOPLEFTALIGN },
		{ _T("SizeBoxBottomRightAlign"), SBS_SIZEBOXBOTTOMRIGHTALIGN },
		{ _T("SizeBox"), SBS_SIZEBOX },
		{ _T("SizeGrip"), SBS_SIZEGRIP },
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_ComboBox(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("Simple"), CBS_SIMPLE },
		{ _T("DropDown"), CBS_DROPDOWN },
		{ _T("DropDownList"), CBS_DROPDOWNLIST },
		{ _T("OwnerDrawFixed"), CBS_OWNERDRAWFIXED },
		{ _T("OwnerDrawVariable"), CBS_OWNERDRAWVARIABLE },
		{ _T("AutoHScroll"), CBS_AUTOHSCROLL },
		{ _T("OEMConvert"), CBS_OEMCONVERT },
		{ _T("Sort"), CBS_SORT },
		{ _T("HasStrings"), CBS_HASSTRINGS },
		{ _T("NoIntegralHeight"), CBS_NOINTEGRALHEIGHT },
		{ _T("DisableNoScroll"), CBS_DISABLENOSCROLL },
		{ _T("UpperCase"), CBS_UPPERCASE },
		{ _T("LowerCase"), CBS_LOWERCASE },
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

#if defined(_INC_COMMCTRL)

DWORD CDialogXML::Parse_Header(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("Horz"), HDS_HORZ },
		{ _T("Buttons"), HDS_BUTTONS },
		{ _T("HotTrack"), HDS_HOTTRACK },
		{ _T("Hidden"), HDS_HIDDEN },
		{ _T("DragDrop"), HDS_DRAGDROP },
		{ _T("FullDrag"), HDS_FULLDRAG },
		{ _T("FilterBar"), HDS_FILTERBAR },
#if (_WIN32_WINNT >= 0x0501)
		{ _T("Flat"), HDS_FLAT },
#endif
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_Toolbar(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("ToolTips"), TBSTYLE_TOOLTIPS },
		{ _T("Wrapable"), TBSTYLE_WRAPABLE },
		{ _T("AltDrag"), TBSTYLE_ALTDRAG },
		{ _T("Flat"), TBSTYLE_FLAT },
		{ _T("List"), TBSTYLE_LIST },
		{ _T("CustomErase"), TBSTYLE_CUSTOMERASE },
		{ _T("RegisterDrop"), TBSTYLE_REGISTERDROP },
		{ _T("Transparent"), TBSTYLE_TRANSPARENT },
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_ReBar(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("ToolTips"), RBS_TOOLTIPS },
		{ _T("VarHeight"), RBS_VARHEIGHT },
		{ _T("BandBorders"), RBS_BANDBORDERS },
		{ _T("FixedOrder"), RBS_FIXEDORDER },
		{ _T("RegisterDrop"), RBS_REGISTERDROP },
		{ _T("AutoSize"), RBS_AUTOSIZE },
		{ _T("VerticalGripper"), RBS_VERTICALGRIPPER },
		{ _T("DblClkToggle"), RBS_DBLCLKTOGGLE },
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_ToolTip(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("AlwaysTip"), TTS_ALWAYSTIP },
		{ _T("NoPrefix"), TTS_NOPREFIX },
		{ _T("NoAnimate"), TTS_NOANIMATE },
		{ _T("NoFade"), TTS_NOFADE },
		{ _T("Balloon"), TTS_BALLOON },
		{ _T("Close"), TTS_CLOSE },
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_StatusBar(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("SizeGrip"), SBARS_SIZEGRIP },
		{ _T("ToolTips"), SBARS_TOOLTIPS },
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_TrackBar(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("AutoTicks"), TBS_AUTOTICKS },
		{ _T("Vert"), TBS_VERT },
		{ _T("Horz"), TBS_HORZ },
		{ _T("Top"), TBS_TOP },
		{ _T("Bottom"), TBS_BOTTOM },
		{ _T("Left"), TBS_LEFT },
		{ _T("Right"), TBS_RIGHT },
		{ _T("Both"), TBS_BOTH },
		{ _T("NoTicks"), TBS_NOTICKS },
		{ _T("EnableSelRange"), TBS_ENABLESELRANGE },
		{ _T("FixedLength"), TBS_FIXEDLENGTH },
		{ _T("NoThumb"), TBS_NOTHUMB },
		{ _T("ToolTips"), TBS_TOOLTIPS },
		{ _T("Reversed"), TBS_REVERSED },
		{ _T("DownIsLeft"), TBS_DOWNISLEFT },
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_UpDown(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("Wrap"), UDS_WRAP },
		{ _T("SetBuddyInt"), UDS_SETBUDDYINT },
		{ _T("AlignRight"), UDS_ALIGNRIGHT },
		{ _T("AlignLeft"), UDS_ALIGNLEFT },
		{ _T("AutoBuddy"), UDS_AUTOBUDDY },
		{ _T("ArrowKeys"), UDS_ARROWKEYS },
		{ _T("Horz"), UDS_HORZ },
		{ _T("NoThousands"), UDS_NOTHOUSANDS },
		{ _T("HotTrack"), UDS_HOTTRACK },
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_ProgressBar(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("Smooth"), PBS_SMOOTH },
		{ _T("Vertical"), PBS_VERTICAL },
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_HotKey(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_ListView(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("Icon"), LVS_ICON },
		{ _T("Report"), LVS_REPORT },
		{ _T("SmallIcon"), LVS_SMALLICON },
		{ _T("List"), LVS_LIST },
		{ _T("SingleSel"), LVS_SINGLESEL },
		{ _T("ShowSelAlways"), LVS_SHOWSELALWAYS },
		{ _T("SortAscending"), LVS_SORTASCENDING },
		{ _T("SortDescending"), LVS_SORTDESCENDING },
		{ _T("ShareImageLists"), LVS_SHAREIMAGELISTS },
		{ _T("NoLabelWrap"), LVS_NOLABELWRAP },
		{ _T("AutoArrange"), LVS_AUTOARRANGE },
		{ _T("EditLabels"), LVS_EDITLABELS },
		{ _T("OwnerData"), LVS_OWNERDATA },
		{ _T("NoScroll"), LVS_NOSCROLL },
		{ _T("AlignTop"), LVS_ALIGNTOP },
		{ _T("AlignLeft"), LVS_ALIGNLEFT },
		{ _T("OwnerDrawFixed"), LVS_OWNERDRAWFIXED },
		{ _T("NoColumnHeader"), LVS_NOCOLUMNHEADER },
		{ _T("NoSortHeader"), LVS_NOSORTHEADER },
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_TreeView(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("HasButtons"), TVS_HASBUTTONS },
		{ _T("HasLines"), TVS_HASLINES },
		{ _T("LinesAtRoot"), TVS_LINESATROOT },
		{ _T("EditLabels"), TVS_EDITLABELS },
		{ _T("DisableDragDrop"), TVS_DISABLEDRAGDROP },
		{ _T("ShowSelAlways"), TVS_SHOWSELALWAYS },
		{ _T("RTLReading"), TVS_RTLREADING },
		{ _T("NoToolTips"), TVS_NOTOOLTIPS },
		{ _T("CheckBoxes"), TVS_CHECKBOXES },
		{ _T("TrackSelect"), TVS_TRACKSELECT },
		{ _T("SingleExpand"), TVS_SINGLEEXPAND },
		{ _T("InfoTip"), TVS_INFOTIP },
		{ _T("FullRowSelect"), TVS_FULLROWSELECT },
		{ _T("NoScroll"), TVS_NOSCROLL },
		{ _T("NonEvenHeight"), TVS_NONEVENHEIGHT },
		{ _T("NoHScroll"), TVS_NOHSCROLL },
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_TabControl(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("ScrollOpposite"), TCS_SCROLLOPPOSITE },
		{ _T("Bottom"), TCS_BOTTOM },
		{ _T("Right"), TCS_RIGHT },
		{ _T("MultiSelect"), TCS_MULTISELECT },
		{ _T("FlatButtons"), TCS_FLATBUTTONS },
		{ _T("ForceIconLeft"), TCS_FORCEICONLEFT },
		{ _T("ForceLabelLeft"), TCS_FORCELABELLEFT },
		{ _T("HotTrack"), TCS_HOTTRACK },
		{ _T("Vertical"), TCS_VERTICAL },
		{ _T("Tabs"), TCS_TABS },
		{ _T("Buttons"), TCS_BUTTONS },
		{ _T("SingleLine"), TCS_SINGLELINE },
		{ _T("MultiLine"), TCS_MULTILINE },
		{ _T("RightJustify"), TCS_RIGHTJUSTIFY },
		{ _T("FixedWidth"), TCS_FIXEDWIDTH },
		{ _T("RaggedRight"), TCS_RAGGEDRIGHT },
		{ _T("FocusOnButtonDown"), TCS_FOCUSONBUTTONDOWN },
		{ _T("OwnerDrawFixed"), TCS_OWNERDRAWFIXED },
		{ _T("ToolTips"), TCS_TOOLTIPS },
		{ _T("FocusNever"), TCS_FOCUSNEVER },
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_Animate(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("Center"), ACS_CENTER },
		{ _T("Transparent"), ACS_TRANSPARENT },
		{ _T("AutoPlay"), ACS_AUTOPLAY },
		{ _T("Timer"), ACS_TIMER },
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_MonthCal(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("DayState"), MCS_DAYSTATE },
		{ _T("MultiSelect"), MCS_MULTISELECT },
		{ _T("WeekNumbers"), MCS_WEEKNUMBERS },
		{ _T("NoTodayCircle"), MCS_NOTODAYCIRCLE },
		{ _T("NoToday"), MCS_NOTODAY },
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_DateTimePick(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("UpDown"), DTS_UPDOWN },
		{ _T("ShowNone"), DTS_SHOWNONE },
		{ _T("ShortDateFormat"), DTS_SHORTDATEFORMAT },
		{ _T("LongDateFormat"), DTS_LONGDATEFORMAT },
		{ _T("ShortDateCenturyFormat"), DTS_SHORTDATECENTURYFORMAT },
		{ _T("TimeFormat"), DTS_TIMEFORMAT },
		{ _T("AppCanParse"), DTS_APPCANPARSE },
		{ _T("RightAlign"), DTS_RIGHTALIGN },
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

DWORD CDialogXML::Parse_Pager(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("Vert"), PGS_VERT },
		{ _T("Horz"), PGS_HORZ },
		{ _T("AutoScroll"), PGS_AUTOSCROLL },
		{ _T("DragNDrop"), PGS_DRAGNDROP },
		{ NULL, 0 }
	};
	return (Parse_Window(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

#endif	// _INC_COMMCTRL

#if defined(_RICHEDIT_)

DWORD CDialogXML::Parse_RichEdit(LPCTSTR pszStylesStr)
{
	static WINDOW_STYLE awsDict[] =
	{
		{ _T("SaveSel"), ES_SAVESEL },
		{ _T("Sunken"), ES_SUNKEN },
		{ _T("DisableNoScroll"), ES_DISABLENOSCROLL },
		{ _T("SelectionBar"), ES_SELECTIONBAR },
		{ _T("NoOLEDragDrop"), ES_NOOLEDRAGDROP },
		{ _T("Vertical"), ES_VERTICAL },
		{ _T("NoIME"), ES_NOIME },
		{ _T("SelfIME"), ES_SELFIME },
		{ NULL, 0 }
	};
	return (Parse_Edit(pszStylesStr) | ParseStyles(awsDict, pszStylesStr));
}

#endif	// _RICHEDIT_

using CDialogXML::PFN_StylesParserProc;
CMap<CString, LPCTSTR, PFN_StylesParserProc, PFN_StylesParserProc> CDialogXML::m_mapParsers;

void CDialogXML::InitParsersMap(void)
{
	// common styles parsers
	SetStylesParser(_T("Window"), &Parse_Window);
	SetStylesParser(_T("WindowEx"), &Parse_WindowEx);

	// dialog styles parser
	SetStylesParser(_T("Dialog"), &Parse_Dialog);

	// control-specific styles parsers
	SetStylesParser(_T("Button"), &Parse_Button);
	SetStylesParser(_T("Edit"), &Parse_Edit);
	SetStylesParser(_T("Static"), &Parse_Static);
	SetStylesParser(_T("ListBox"), &Parse_ListBox);
	SetStylesParser(_T("ScrollBar"), &Parse_ScrollBar);
	SetStylesParser(_T("ComboBox"), &Parse_ComboBox);
#if defined(_INC_COMMCTRL)
	SetStylesParser(_T("SysHeader32"), &Parse_Header);
	SetStylesParser(_T("ToolbarWindow32"), &Parse_Toolbar);
	SetStylesParser(_T("ReBarWindow32"), &Parse_ReBar);
	SetStylesParser(_T("tooltips_class32"), &Parse_ToolTip);
	SetStylesParser(_T("msctls_statusbar32"), &Parse_StatusBar);
	SetStylesParser(_T("msctls_trackbar32"), &Parse_TrackBar);
	SetStylesParser(_T("msctls_updown32"), &Parse_UpDown);
	SetStylesParser(_T("msctls_progress32"), &Parse_ProgressBar);
	SetStylesParser(_T("msctls_hotkey32"), &Parse_HotKey);
	SetStylesParser(_T("SysListView32"), &Parse_ListView);
	SetStylesParser(_T("SysTreeView32"), &Parse_TreeView);
	SetStylesParser(_T("SysTabControl32"), &Parse_TabControl);
	SetStylesParser(_T("SysAnimate32"), &Parse_Animate);
	SetStylesParser(_T("SysMonthCal32"), &Parse_MonthCal);
	SetStylesParser(_T("SysDateTimePick32"), &Parse_DateTimePick);
	SetStylesParser(_T("SysPager"), &Parse_Pager);
#endif	// _INC_COMMCTRL
#if defined(_RICHEDIT_)
	SetStylesParser(_T("RichEdit"), &Parse_RichEdit);
	SetStylesParser(_T("RichEdit20A"), &Parse_RichEdit);
	SetStylesParser(_T("RichEdit20W"), &Parse_RichEdit);
#endif	// _RICHEDIT_
}

void CDialogXML::DlgTemplateHelper(CMemFile& fileMem, CPugXmlBranch& branchDialog)
{
	DLGTEMPLATE dlgTemplate;
	PFN_StylesParserProc pfnParser = NULL;		// to avoid stupid warning C4701
	UINT cbRemains;
	DLGITEMTEMPLATE itemTemplate;

	BYTE abAligner[4] = { 0 };

	// obtain needed XML nodes
	CPugXmlBranch branchFont = branchDialog.FindByPath(_T("./Font"));
	ASSERT(!branchFont.IsNull());
	CPugXmlBranch branchControls = branchDialog.FindByPath(_T("./Controls"));
	ASSERT(!branchControls.IsNull());

	// build DLGTEMPLATE structure
	dlgTemplate.style = GetAttribute_DWORD(branchDialog, _T("Style"));
	dlgTemplate.dwExtendedStyle = GetAttribute_DWORD(branchDialog, _T("ExStyle"));
	dlgTemplate.cdit = GetAttribute_WORD(branchControls, _T("Count"));
	dlgTemplate.x = GetAttribute_short(branchDialog, _T("Left"));
	dlgTemplate.y = GetAttribute_short(branchDialog, _T("Top"));
	dlgTemplate.cx = GetAttribute_short(branchDialog, _T("Width"));
	dlgTemplate.cy = GetAttribute_short(branchDialog, _T("Height"));

	// try to parse string-coded dialog's styles
	LPCTSTR pszStylesStr = branchDialog.GetAttribute(_T("Styles"));
	if (*pszStylesStr != 0)
	{
		pfnParser = m_mapParsers[_T("Dialog")];
		ASSERT(pfnParser != NULL);
		dlgTemplate.style |= (*pfnParser)(pszStylesStr);
	}

	// try to parse string-coded extended styles
	LPCTSTR pszExStylesStr = branchDialog.GetAttribute(_T("ExStyles"));
	if (*pszExStylesStr != 0)
	{
		pfnParser = m_mapParsers[_T("WindowEx")];
		ASSERT(pfnParser != NULL);
		dlgTemplate.dwExtendedStyle |= (*pfnParser)(pszExStylesStr);
	}

	// write DLGTEMPLATE into the memory
	fileMem.Write(&dlgTemplate, sizeof(dlgTemplate));

	// menu - not supported yet
	WCHAR awcMenu[1] = { 0 };
	fileMem.Write(awcMenu, sizeof(awcMenu));

	// custom window class - not supported yet
	WCHAR awcClass[1] = { 0 };
	fileMem.Write(awcClass, sizeof(awcClass));

	// dialog's caption
	_T2W pwszCaption(branchDialog.GetAttribute(_T("Caption")));
	fileMem.Write(pwszCaption, (::lstrlenW(pwszCaption) + 1) * sizeof(WCHAR));

	if ((dlgTemplate.style & DS_SETFONT) != 0)
	{
		// font size and typeface
		WORD wFontSize = GetAttribute_WORD(branchFont, _T("PointSize"));
		fileMem.Write(&wFontSize, sizeof(wFontSize));
		_T2W pwszFontFace(branchFont.GetAttribute(_T("TypeFace")));
		fileMem.Write(pwszFontFace, (::lstrlenW(pwszFontFace) + 1) * sizeof(WCHAR));
	}

	// align to the DWORD boundary
	ASSERT(sizeof(DWORD) == 4);
#if (_MFC_VER < 0x0700)
	if ((cbRemains = (4 - fileMem.GetLength() % 4) & 3) > 0)
#else
	if ((cbRemains = (4 - static_cast<UINT>(fileMem.GetLength() % 4)) & 3) > 0)
#endif
	{
		fileMem.Write(abAligner, cbRemains);
	}

	// build controls
	for (int i = 0; i < dlgTemplate.cdit; ++i)
	{
		// obtain control's XML node
		CPugXmlBranch branchControl = branchControls.GetChildAt(i);
		ASSERT(!branchControl.IsNull());

		// build DLGITEMTEMPLATE structure
		itemTemplate.style = GetAttribute_DWORD(branchControl, _T("Style"));
		itemTemplate.dwExtendedStyle = GetAttribute_DWORD(branchControl, _T("ExStyle"));
		itemTemplate.x = GetAttribute_short(branchControl, _T("Left"));
		itemTemplate.y = GetAttribute_short(branchControl, _T("Top"));
		itemTemplate.cx = GetAttribute_short(branchControl, _T("Width"));
		itemTemplate.cy = GetAttribute_short(branchControl, _T("Height"));
		itemTemplate.id = GetAttribute_WORD(branchControl, _T("ID"));

		// cache "Class" attribute
		XMLATTR* pAttrClass = branchControl.MapStringToAttributePtr(_T("Class"));
		ASSERT(pAttrClass != NULL);

		// try to parse string-coded control's styles
		if (m_mapParsers.Lookup(pAttrClass->value, pfnParser))
		{
			LPCTSTR pszStylesStr = branchControl.GetAttribute(_T("Styles"));
			if (*pszStylesStr != 0)
			{
				itemTemplate.style |= (*pfnParser)(pszStylesStr);
			}
		}

		// try to parse string-coded extended styles
		LPCTSTR pszExStylesStr = branchControl.GetAttribute(_T("ExStyles"));
		if (*pszExStylesStr != 0)
		{
			pfnParser = m_mapParsers[_T("WindowEx")];
			ASSERT(pfnParser != NULL);
			itemTemplate.dwExtendedStyle |= (*pfnParser)(pszExStylesStr);
		}

		// write DLGITEMTEMPLATE into the memory
		fileMem.Write(&itemTemplate, sizeof(itemTemplate));

		// control's window class
		_T2W pwszClass(pAttrClass->value);
		fileMem.Write(pwszClass, (::lstrlenW(pwszClass) + 1) * sizeof(WCHAR));

		// initial control's text
		_T2W pwszCaption(branchControl.GetAttribute(_T("Caption")));
		WCHAR* pwChr = const_cast<WCHAR*>(static_cast<LPCWSTR>(pwszCaption));
		while ((pwChr = ::StrChrW(pwChr, L'|')) != NULL) *pwChr++ = L'\n';
		fileMem.Write(pwszCaption, (::lstrlenW(pwszCaption) + 1) * sizeof(WCHAR));

		// creation data - not used
		WORD wData[1] = { 0 };
		fileMem.Write(wData, sizeof(wData));

		// align to the DWORD boundary
		ASSERT(sizeof(DWORD) == 4);
#if (_MFC_VER < 0x0700)
		if ((cbRemains = (4 - fileMem.GetLength() % 4) & 3) > 0)
#else
		if ((cbRemains = (4 - static_cast<UINT>(fileMem.GetLength() % 4)) & 3) > 0)
#endif
		{
			fileMem.Write(abAligner, cbRemains);
		}
	}
}

void CDialogXML::DlgTemplateExHelper(CMemFile& fileMem, CPugXmlBranch& branchDialog)
{
	DLGTEMPLATEEX dlgTemplateEx;
	PFN_StylesParserProc pfnParser = NULL;		// to avoid stupid warning C4701
	UINT cbRemains;
	DLGITEMTEMPLATEEX itemTemplateEx;

	BYTE abAligner[4] = { 0 };

	// obtain needed XML nodes
	CPugXmlBranch branchFont = branchDialog.FindByPath(_T("./Font"));
	ASSERT(!branchFont.IsNull());
	CPugXmlBranch branchControls = branchDialog.FindByPath(_T("./Controls"));
	ASSERT(!branchControls.IsNull());

	// build DLGTEMPLATEEX structure
	dlgTemplateEx.dlgVer = 1;
	dlgTemplateEx.signature = 0xFFFF;
	dlgTemplateEx.helpID = GetAttribute_DWORD(branchDialog, _T("HelpID"));
	dlgTemplateEx.exStyle = GetAttribute_DWORD(branchDialog, _T("ExStyle"));
	dlgTemplateEx.style = GetAttribute_DWORD(branchDialog, _T("Style"));
	dlgTemplateEx.cDlgItems = GetAttribute_WORD(branchControls, _T("Count"));
	dlgTemplateEx.x = GetAttribute_short(branchDialog, _T("Left"));
	dlgTemplateEx.y = GetAttribute_short(branchDialog, _T("Top"));
	dlgTemplateEx.cx = GetAttribute_short(branchDialog, _T("Width"));
	dlgTemplateEx.cy = GetAttribute_short(branchDialog, _T("Height"));

	// try to parse string-coded dialog's styles
	LPCTSTR pszStylesStr = branchDialog.GetAttribute(_T("Styles"));
	if (*pszStylesStr != 0)
	{
		pfnParser = m_mapParsers[_T("Dialog")];
		ASSERT(pfnParser != NULL);
		dlgTemplateEx.style |= (*pfnParser)(pszStylesStr);
	}

	// try to parse string-coded extended styles
	LPCTSTR pszExStylesStr = branchDialog.GetAttribute(_T("ExStyles"));
	if (*pszExStylesStr != 0)
	{
		pfnParser = m_mapParsers[_T("WindowEx")];
		ASSERT(pfnParser != NULL);
		dlgTemplateEx.exStyle |= (*pfnParser)(pszExStylesStr);
	}

	// write dialog's template into the memory
	fileMem.Write(&dlgTemplateEx, sizeof(dlgTemplateEx));

	// menu - not supported yet
	WCHAR awcMenu[1] = { 0 };
	fileMem.Write(awcMenu, sizeof(awcMenu));

	// custom window class - not supported yet
	WCHAR awcClass[1] = { 0 };
	fileMem.Write(awcClass, sizeof(awcClass));

	// dialog's caption
	_T2W pwszCaption(branchDialog.GetAttribute(_T("Caption")));
	fileMem.Write(pwszCaption, (::lstrlenW(pwszCaption) + 1) * sizeof(WCHAR));

	if ((dlgTemplateEx.style & DS_SETFONT) != 0)
	{
		// font attributes
		WORD wFontSize = GetAttribute_WORD(branchFont, _T("PointSize"));
		fileMem.Write(&wFontSize, sizeof(wFontSize));
		WORD wFontWeight = GetAttribute_WORD(branchFont, _T("Weight"), FW_NORMAL);
		fileMem.Write(&wFontWeight, sizeof(wFontWeight));
		BYTE bFontItalic = GetAttribute_BYTE(branchFont, _T("Italic"));
		fileMem.Write(&bFontItalic, sizeof(bFontItalic));
		BYTE bFontCharset = GetAttribute_BYTE(branchFont, _T("CharSet"), ANSI_CHARSET);
		fileMem.Write(&bFontCharset, sizeof(bFontCharset));
		_T2W pwszFontFace(branchFont.GetAttribute(_T("TypeFace")));
		fileMem.Write(pwszFontFace, (::lstrlenW(pwszFontFace) + 1) * sizeof(WCHAR));
	}

	// align to the DWORD boundary
	ASSERT(sizeof(DWORD) == 4);
#if (_MFC_VER < 0x0700)
	if ((cbRemains = (4 - fileMem.GetLength() % 4) & 3) > 0)
#else
	if ((cbRemains = (4 - static_cast<UINT>(fileMem.GetLength() % 4)) & 3) > 0)
#endif
	{
		fileMem.Write(abAligner, cbRemains);
	}

	// build controls
	for (int i = 0; i < dlgTemplateEx.cDlgItems; ++i)
	{
		// obtain control's XML node
		CPugXmlBranch branchControl = branchControls.GetChildAt(i);
		ASSERT(!branchControl.IsNull());

		// build DLGITEMTEMPLATEEX structure
		itemTemplateEx.helpID = GetAttribute_DWORD(branchControl, _T("HelpID"));
		itemTemplateEx.exStyle = GetAttribute_DWORD(branchControl, _T("ExStyle"));
		itemTemplateEx.style = GetAttribute_DWORD(branchControl, _T("Style"));
		itemTemplateEx.x = GetAttribute_short(branchControl, _T("Left"));
		itemTemplateEx.y = GetAttribute_short(branchControl, _T("Top"));
		itemTemplateEx.cx = GetAttribute_short(branchControl, _T("Width"));
		itemTemplateEx.cy = GetAttribute_short(branchControl, _T("Height"));
		itemTemplateEx.id = GetAttribute_WORD(branchControl, _T("ID"));

		// cache "Class" attribute
		XMLATTR* pAttrClass = branchControl.MapStringToAttributePtr(_T("Class"));
		ASSERT(pAttrClass != NULL);

		// try to parse string-coded control's styles
		if (m_mapParsers.Lookup(pAttrClass->value, pfnParser)) {
			LPCTSTR pszStylesStr = branchControl.GetAttribute(_T("Styles"));
			if (*pszStylesStr != 0) {
				itemTemplateEx.style |= (*pfnParser)(pszStylesStr);
			}
		}

		// try to parse string-coded extended styles
		LPCTSTR pszExStylesStr = branchControl.GetAttribute(_T("ExStyles"));
		if (*pszExStylesStr != 0) {
			pfnParser = m_mapParsers[_T("WindowEx")];
			ASSERT(pfnParser != NULL);
			itemTemplateEx.exStyle |= (*pfnParser)(pszExStylesStr);
		}

		// write DLGITEMTEMPLATEEX into the memory
		fileMem.Write(&itemTemplateEx, sizeof(itemTemplateEx));

		// control's window class
		_T2W pwszClass(pAttrClass->value);
		fileMem.Write(pwszClass, (::lstrlenW(pwszClass) + 1) * sizeof(WCHAR));

		// initial control's text
		_T2W pwszCaption(branchControl.GetAttribute(_T("Caption")));
		WCHAR* pwChr = const_cast<WCHAR*>(static_cast<LPCWSTR>(pwszCaption));
		while ((pwChr = ::StrChrW(pwChr, L'|')) != NULL) *pwChr++ = L'\n';
		fileMem.Write(pwszCaption, (::lstrlenW(pwszCaption) + 1) * sizeof(WCHAR));

		// creation data - not used
		WORD wData[1] = { 0 };
		fileMem.Write(wData, sizeof(wData));

		// align to the DWORD boundary
		ASSERT(sizeof(DWORD) == 4);
#if (_MFC_VER < 0x0700)
		if ((cbRemains = (4 - fileMem.GetLength() % 4) & 3) > 0)
#else
		if ((cbRemains = (4 - static_cast<UINT>(fileMem.GetLength() % 4)) & 3) > 0)
#endif
		{
			fileMem.Write(abAligner, cbRemains);
		}
	}
}

#if defined(_DEBUG)

void CDialogXML::AssertValid(void) const
{
	// first perform inherited validity check...
	CDialog::AssertValid();

	// ...and then verify our own state as well
}

void CDialogXML::Dump(CDumpContext& dumpCtx) const
{
	try {
		// first invoke inherited dumper...
		CDialog::Dump(dumpCtx);

		// ...and then dump own unique members
		dumpCtx << "m_strDialogName = " << m_strDialogName << "\n";
		dumpCtx.SetDepth(1);
		dumpCtx << "m_mapParsers = " << m_mapParsers;
	}
	catch (CFileException* pXcpt)
	{
		pXcpt->ReportError();
		pXcpt->Delete();
	}
}

#endif	// _DEBUG

struct INIT_PARSERS_MAP
{
	INIT_PARSERS_MAP(void)
	{
		CDialogXML::InitParsersMap();
	}
} g_initParsersMap;

// import libraries
#pragma comment(lib, "shlwapi.lib")

// import library for Win95/98/NT4 builds
#if (_WIN32_WINDOWS < 0x0490)
#pragma comment(lib, "dbghelp.lib")
#endif	// _WIN32_WINDOWS

// end of file
