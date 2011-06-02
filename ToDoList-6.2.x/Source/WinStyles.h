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
// - taken out from the original ToDoList package for better sharing
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
//*****************************************************************************

#if !defined(AFX_STYLES_H__46AFC4E3_CC43_4E69_B5D3_2B6D1F501D0E__INCLUDED_)
#define AFX_STYLES_H__46AFC4E3_CC43_4E69_B5D3_2B6D1F501D0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <richedit.h>

struct Style
{
	LPCTSTR szStyle; // style name
	DWORD dwStyle; // style value
};

#define STYLE(s) _T(#s), (DWORD)s

static Style ClassStyles[] =
{
	{ STYLE(CS_VREDRAW) }, //       0x0001
	{ STYLE(CS_HREDRAW) }, //       0x0002
	{ STYLE(CS_DBLCLKS) }, //       0x0008
	{ STYLE(CS_OWNDC) }, //         0x0020
	{ STYLE(CS_CLASSDC) }, //       0x0040
	{ STYLE(CS_PARENTDC) }, //      0x0080
	{ STYLE(CS_NOCLOSE) }, //       0x0200
	{ STYLE(CS_SAVEBITS) }, //      0x0800
	{ STYLE(CS_BYTEALIGNCLIENT) },//0x1000
	{ STYLE(CS_BYTEALIGNWINDOW) },//0x2000
	{ STYLE(CS_GLOBALCLASS) }, //   0x4000
	{ STYLE(CS_IME) } //            0x00010000
};

///////

static Style WindowStyles[] =
{
	{ STYLE(WS_OVERLAPPED) }, //    0x00000000L
	{ STYLE(WS_POPUP) }, //         0x80000000L
	{ STYLE(WS_CHILD) }, //         0x40000000L
	{ STYLE(WS_MINIMIZE) }, //      0x20000000L
	{ STYLE(WS_VISIBLE) }, //       0x10000000L
	{ STYLE(WS_DISABLED) }, //      0x08000000L
	{ STYLE(WS_CLIPSIBLINGS) }, //  0x04000000L
	{ STYLE(WS_CLIPCHILDREN) }, //  0x02000000L
	{ STYLE(WS_MAXIMIZE) }, //      0x01000000L
	{ STYLE(WS_CAPTION) }, //       0x00C00000L
	{ STYLE(WS_BORDER) }, //        0x00800000L
	{ STYLE(WS_DLGFRAME) }, //      0x00400000L
	{ STYLE(WS_VSCROLL) }, //       0x00200000L
	{ STYLE(WS_HSCROLL) }, //       0x00100000L
	{ STYLE(WS_SYSMENU) }, //       0x00080000L
	{ STYLE(WS_THICKFRAME) }, //    0x00040000L
	{ STYLE(WS_GROUP) }, //         0x00020000L
	{ STYLE(WS_TABSTOP) }, //       0x00010000L
	{ STYLE(WS_MINIMIZEBOX) },//    0x00020000L
	{ STYLE(WS_MAXIMIZEBOX) }// 0x00010000L
	//{ STYLE(WS_TILED) }, // duplicate
	//{ STYLE(WS_ICONIC) }, // duplicate
	//{ STYLE(WS_SIZEBOX) }, // duplicate
	//{ STYLE(WS_TILEDWINDOW) }, // duplicate
	//{ STYLE(WS_OVERLAPPEDWINDOW) }, // aggregate style
	//{ STYLE(WS_POPUPWINDOW) }, // aggregate style
	//{ STYLE(WS_CHILDWINDOW) }, // duplicate
};

const int NUMWNDSTYLES = sizeof(WindowStyles) / sizeof(Style);

static UINT LookupWndStyle(LPCTSTR szStyle)
{
	int nStyle = NUMWNDSTYLES;

	while (nStyle--)
	{
		if (lstrcmp(szStyle, WindowStyles[nStyle].szStyle) == 0)
		{
			return WindowStyles[nStyle].dwStyle;
		}
	}

	return 0;
}

////////

static Style WindowExStyles[] =
{
	{ STYLE(WS_EX_DLGMODALFRAME) }, //  0x00000001L
	{ STYLE(WS_EX_NOPARENTNOTIFY) }, // 0x00000004L
	{ STYLE(WS_EX_TOPMOST) }, //        0x00000008L
	{ STYLE(WS_EX_ACCEPTFILES) }, //    0x00000010L
	{ STYLE(WS_EX_TRANSPARENT) }, //    0x00000020L
	{ STYLE(WS_EX_MDICHILD) }, //       0x00000040L
	{ STYLE(WS_EX_TOOLWINDOW) }, //     0x00000080L
	{ STYLE(WS_EX_WINDOWEDGE) }, //     0x00000100L
	{ STYLE(WS_EX_CLIENTEDGE) }, //     0x00000200L
	{ STYLE(WS_EX_CONTEXTHELP) }, //    0x00000400L
	{ STYLE(WS_EX_RIGHT) },  //         0x00001000L
	{ STYLE(WS_EX_LEFT) }, //           0x00000000L
	{ STYLE(WS_EX_RTLREADING) }, //     0x00002000L
	{ STYLE(WS_EX_LTRREADING) }, //     0x00000000L
	{ STYLE(WS_EX_LEFTSCROLLBAR) }, //  0x00004000L
	{ STYLE(WS_EX_RIGHTSCROLLBAR) }, // 0x00000000L
	{ STYLE(WS_EX_CONTROLPARENT) }, //  0x00010000L
	{ STYLE(WS_EX_STATICEDGE) }, //     0x00020000L
	{ STYLE(WS_EX_APPWINDOW) } //       0x00040000L
	//{ STYLE(WS_EX_OVERLAPPEDWINDOW) }, // aggregate style
	//{ STYLE(WS_EX_PALETTEWINDOW) }, // aggregate style
};

const int NUMWNDEXSTYLES = sizeof(WindowExStyles) / sizeof(Style);

static UINT LookupWndExStyle(LPCTSTR szStyle)
{
	int nStyle = NUMWNDEXSTYLES;

	while (nStyle--)
	{
		if (lstrcmp(szStyle, WindowExStyles[nStyle].szStyle) == 0)
		{
			return WindowExStyles[nStyle].dwStyle;
		}
	}

	return 0;
}

//////

static Style HitTests[] =
{
	{ STYLE(HTERROR) }, //             (-2)
	{ STYLE(HTTRANSPARENT) }, //       (-1)
	{ STYLE(HTNOWHERE) }, //           0
	{ STYLE(HTCLIENT) }, //            1
	{ STYLE(HTCAPTION) }, //           2
	{ STYLE(HTSYSMENU) }, //           3
	{ STYLE(HTGROWBOX) }, //           4
	{ STYLE(HTSIZE) }, //              HTGROWBOX
	{ STYLE(HTMENU) }, //              5
	{ STYLE(HTHSCROLL) }, //           6
	{ STYLE(HTVSCROLL) }, //           7
	{ STYLE(HTMINBUTTON) }, //         8
	{ STYLE(HTMAXBUTTON) }, //         9
	{ STYLE(HTLEFT) }, //              10
	{ STYLE(HTRIGHT) }, //             11
	{ STYLE(HTTOP) }, //               12
	{ STYLE(HTTOPLEFT) }, //           13
	{ STYLE(HTTOPRIGHT) }, //          14
	{ STYLE(HTBOTTOM) }, //            15
	{ STYLE(HTBOTTOMLEFT) }, //        16
	{ STYLE(HTBOTTOMRIGHT) }, //       17
	{ STYLE(HTBORDER) }, //            18
	{ STYLE(HTREDUCE) }, //            HTMINBUTTON
	{ STYLE(HTZOOM) }, //              HTMAXBUTTON
	{ STYLE(HTSIZEFIRST) }, //         HTLEFT
	{ STYLE(HTSIZELAST) }, //          HTBOTTOMRIGHT
	{ STYLE(HTOBJECT) }, //            19
	{ STYLE(HTCLOSE) }, //             20
	{ STYLE(HTHELP) }  //              21
};

// controls
struct Control
{
	LPCTSTR szControlClass; // window class name
	LPCTSTR szBaseClass; // avoids duplicating styles for richedit and comboboxex
	DWORD dwExStyleMsg; // msg to send to control to retrieve Ex styles
};

static Control Controls[] =
{
	{ _T("Button"), _T(""), 0 },
	{ _T("Static"), _T(""), 0 },
	{ _T("Edit"), _T(""), 0 },
	{ _T("ComboBox"), _T(""), 0 },
	{ _T("ComboLBox"), _T("ListBox"), 0 }, // drop list for the combo box
	{ _T("ListBox"), _T(""), 0 },
	{ _T("Scrollbar"), _T(""), 0 },
	{ _T("toolbarwindow32"), _T("CommonControl"), TB_GETEXTENDEDSTYLE },
	{ _T("msctls_updown32"), _T(""), 0 },
	{ _T("msctls_progress32"), _T(""), 0 },
	{ _T("msctls_trackbar32"), _T(""), 0 },
	{ _T("msctls_hotkey32"), _T(""), 0 },
	{ _T("SysListView32"), _T(""), LVM_GETEXTENDEDLISTVIEWSTYLE },
	{ _T("SysTreeView32"), _T(""), 0 },
	{ _T("SysTabControl32"), _T(""), TCM_GETEXTENDEDSTYLE },
	{ _T("SysAnimate32"), _T(""), 0 },
	{ _T("Richedit"), _T("Edit"), EM_GETEVENTMASK },	// event mask is like extended styles
	{ _T("SysDateTimePick32"), _T(""), 0 },
	{ _T("SysMonthCal32"), _T(""), 0 },
	{ _T("SysIPAddress32"), _T(""), 0 },
	{ _T("SysPager32"), _T(""), 0 },
	{ _T("ComboBoxEx32"), _T("ComboBox"), CBEM_GETEXTENDEDSTYLE },
	{ _T("msctls_statusbar32"), _T("CommonControl"), 0 },
	{ _T("ReBarWindow32"), _T(""), 0 },
	{ _T("SysHeader32"), _T(""), 0 },
	{ _T("DialogBox"), _T(""), 0 },
	{ _T("MDIClient"), _T(""), 0 },
	{ _T("tooltips_class32"), _T(""), 0 }
};

struct CtrlStyle
{
	LPCTSTR szControlClass; // window class name
	LPCTSTR szStyle; // style name
	DWORD dwStyle; // style value
	DWORD dwMask; // required masked
	BOOL bExStyle; // is it an _EX_ style
};

#define NOMASK 0x0L

#ifndef HDS_VERT
#define HDS_VERT 0x0001
#endif

////////

static CtrlStyle ControlStyles[] =
{
	{ _T("Button"), STYLE(BS_PUSHBUTTON), 0x0F, 0 },//        0x00000000L
	{ _T("Button"), STYLE(BS_DEFPUSHBUTTON), 0x0F, 0 },//     0x00000001L
	{ _T("Button"), STYLE(BS_CHECKBOX), 0x0F, 0 },//          0x00000002L
	{ _T("Button"), STYLE(BS_AUTOCHECKBOX), 0x0F, 0 },//      0x00000003L
	{ _T("Button"), STYLE(BS_RADIOBUTTON), 0x0F, 0 },//       0x00000004L
	{ _T("Button"), STYLE(BS_3STATE), 0x0F, 0 },//            0x00000005L
	{ _T("Button"), STYLE(BS_AUTO3STATE), 0x0F, 0 },//        0x00000006L
	{ _T("Button"), STYLE(BS_GROUPBOX), 0x0F, 0 },//          0x00000007L
	{ _T("Button"), STYLE(BS_USERBUTTON), 0x0F, 0 },//        0x00000008L
	{ _T("Button"), STYLE(BS_AUTORADIOBUTTON), 0x0F, 0 },//   0x00000009L
	{ _T("Button"), STYLE(BS_OWNERDRAW), 0x0F, 0 },//         0x0000000BL
	{ _T("Button"), STYLE(BS_LEFTTEXT), NOMASK, 0 },//        0x00000020L
	{ _T("Button"), STYLE(BS_TEXT), NOMASK, 0 }, //           0x00000000L
	{ _T("Button"), STYLE(BS_ICON), NOMASK, 0 }, //           0x00000040L
	{ _T("Button"), STYLE(BS_BITMAP), NOMASK, 0 },//          0x00000080L
	{ _T("Button"), STYLE(BS_LEFT), NOMASK, 0 }, //           0x00000100L
	{ _T("Button"), STYLE(BS_RIGHT), NOMASK, 0 },//           0x00000200L
	{ _T("Button"), STYLE(BS_CENTER), NOMASK, 0 },//          0x00000300L
	{ _T("Button"), STYLE(BS_TOP), NOMASK, 0 }, //            0x00000400L
	{ _T("Button"), STYLE(BS_BOTTOM), NOMASK, 0 },//          0x00000800L
	{ _T("Button"), STYLE(BS_VCENTER), NOMASK, 0 },//         0x00000C00L
	{ _T("Button"), STYLE(BS_PUSHLIKE), NOMASK, 0 },//        0x00001000L
	{ _T("Button"), STYLE(BS_MULTILINE), NOMASK, 0 },//       0x00002000L
	{ _T("Button"), STYLE(BS_NOTIFY), NOMASK, 0 },//          0x00004000L
	{ _T("Button"), STYLE(BS_FLAT), NOMASK, 0 }, //           0x00008000L
	{ _T("Button"), STYLE(BS_RIGHTBUTTON), NOMASK, 0 },//     BS_LEFTTEXT

	{ _T("Static"), STYLE(SS_LEFT), 0x1FL, 0 },//             0x00000000L
	{ _T("Static"), STYLE(SS_CENTER), 0x1FL, 0 },//           0x00000001L
	{ _T("Static"), STYLE(SS_RIGHT), 0x1FL, 0 },//            0x00000002L
	{ _T("Static"), STYLE(SS_ICON), 0x1FL, 0 },//             0x00000003L
	{ _T("Static"), STYLE(SS_BLACKRECT), 0x1FL, 0 },//        0x00000004L
	{ _T("Static"), STYLE(SS_GRAYRECT), 0x1FL, 0 },//         0x00000005L
	{ _T("Static"), STYLE(SS_WHITERECT), 0x1FL, 0 },//        0x00000006L
	{ _T("Static"), STYLE(SS_BLACKFRAME), 0x1FL, 0 },//       0x00000007L
	{ _T("Static"), STYLE(SS_GRAYFRAME), 0x1FL, 0 },//        0x00000008L
	{ _T("Static"), STYLE(SS_WHITEFRAME), 0x1FL, 0 },//       0x00000009L
	{ _T("Static"), STYLE(SS_USERITEM), 0x1FL, 0 },//         0x0000000AL
	{ _T("Static"), STYLE(SS_SIMPLE), 0x1FL, 0 },//           0x0000000BL
	{ _T("Static"), STYLE(SS_LEFTNOWORDWRAP), 0x1FL, 0 },//   0x0000000CL
	{ _T("Static"), STYLE(SS_OWNERDRAW), 0x1FL, 0 },//        0x0000000DL
	{ _T("Static"), STYLE(SS_BITMAP), 0x1FL, 0 },//           0x0000000EL
	{ _T("Static"), STYLE(SS_ENHMETAFILE), 0x1FL, 0 },//      0x0000000FL
	{ _T("Static"), STYLE(SS_ETCHEDHORZ), 0x1FL, 0 },//       0x00000010L
	{ _T("Static"), STYLE(SS_ETCHEDVERT), 0x1FL, 0 },//       0x00000011L
	{ _T("Static"), STYLE(SS_ETCHEDFRAME), 0x1FL, 0 },//      0x00000012L
	{ _T("Static"), STYLE(SS_NOPREFIX), NOMASK, 0 },//        0x00000080L
	{ _T("Static"), STYLE(SS_NOTIFY), NOMASK, 0 },//          0x00000100L
	{ _T("Static"), STYLE(SS_CENTERIMAGE), NOMASK, 0 },//     0x00000200L
	{ _T("Static"), STYLE(SS_RIGHTJUST), NOMASK, 0 },//       0x00000400L
	{ _T("Static"), STYLE(SS_REALSIZEIMAGE), NOMASK, 0 },//   0x00000800L
	{ _T("Static"), STYLE(SS_SUNKEN), NOMASK, 0 },//          0x00001000L
	{ _T("Static"), STYLE(SS_ENDELLIPSIS), 0xC000L, 0 },//    0x00004000L
	{ _T("Static"), STYLE(SS_PATHELLIPSIS), 0xC000L, 0 },//   0x00008000L
	{ _T("Static"), STYLE(SS_WORDELLIPSIS), 0xC000L, 0 },//   0x0000C000L

	{ _T("Edit"), STYLE(ES_LEFT), 0x2L, 0 },//                0x0000L
	{ _T("Edit"), STYLE(ES_CENTER), 0x2L, 0 },//              0x0001L
	{ _T("Edit"), STYLE(ES_RIGHT), 0x2L, 0 },//               0x0002L
	{ _T("Edit"), STYLE(ES_MULTILINE), NOMASK, 0 },//         0x0004L
	{ _T("Edit"), STYLE(ES_UPPERCASE), NOMASK, 0 },//         0x0008L
	{ _T("Edit"), STYLE(ES_LOWERCASE), NOMASK, 0 },//         0x0010L
	{ _T("Edit"), STYLE(ES_PASSWORD), NOMASK, 0 },//          0x0020L
	{ _T("Edit"), STYLE(ES_AUTOVSCROLL), NOMASK, 0 },//       0x0040L
	{ _T("Edit"), STYLE(ES_AUTOHSCROLL), NOMASK, 0 },//       0x0080L
	{ _T("Edit"), STYLE(ES_NOHIDESEL), NOMASK, 0 },//         0x0100L
	{ _T("Edit"), STYLE(ES_OEMCONVERT), NOMASK, 0 },//        0x0400L
	{ _T("Edit"), STYLE(ES_READONLY), NOMASK, 0 },//          0x0800L
	{ _T("Edit"), STYLE(ES_WANTRETURN), NOMASK, 0 },//        0x1000L
	{ _T("Edit"), STYLE(ES_NUMBER), NOMASK, 0 },//            0x2000L

	{ _T("ComboBox"), STYLE(CBS_SIMPLE), 0xFL, 0 },//               0x0001L
	{ _T("ComboBox"), STYLE(CBS_DROPDOWN), 0xFL, 0 },//             0x0002L
	{ _T("ComboBox"), STYLE(CBS_DROPDOWNLIST), 0xFL, 0 },//         0x0003L
	{ _T("ComboBox"), STYLE(CBS_OWNERDRAWFIXED), NOMASK, 0 },//     0x0010L
	{ _T("ComboBox"), STYLE(CBS_OWNERDRAWVARIABLE), NOMASK, 0 },//  0x0020L
	{ _T("ComboBox"), STYLE(CBS_AUTOHSCROLL), NOMASK, 0 },//        0x0040L
	{ _T("ComboBox"), STYLE(CBS_OEMCONVERT), NOMASK, 0 },//         0x0080L
	{ _T("ComboBox"), STYLE(CBS_SORT), NOMASK, 0 },//               0x0100L
	{ _T("ComboBox"), STYLE(CBS_HASSTRINGS), NOMASK, 0 },//         0x0200L
	{ _T("ComboBox"), STYLE(CBS_NOINTEGRALHEIGHT), NOMASK, 0 },//   0x0400L
	{ _T("ComboBox"), STYLE(CBS_DISABLENOSCROLL), NOMASK, 0 },//    0x0800L
	{ _T("ComboBox"), STYLE(CBS_UPPERCASE), NOMASK, 0 },//          0x2000L
	{ _T("ComboBox"), STYLE(CBS_LOWERCASE), NOMASK, 0 },//          0x4000L

	{ _T("ListBox"), STYLE(LBS_NOTIFY), NOMASK, 0 },//             0x0001L
	{ _T("ListBox"), STYLE(LBS_SORT), NOMASK, 0 },//               0x0002L
	{ _T("ListBox"), STYLE(LBS_NOREDRAW), NOMASK, 0 },//           0x0004L
	{ _T("ListBox"), STYLE(LBS_MULTIPLESEL), NOMASK, 0 },//        0x0008L
	{ _T("ListBox"), STYLE(LBS_OWNERDRAWFIXED), NOMASK, 0 },//     0x0010L
	{ _T("ListBox"), STYLE(LBS_OWNERDRAWVARIABLE), NOMASK, 0 },//  0x0020L
	{ _T("ListBox"), STYLE(LBS_HASSTRINGS), NOMASK, 0 },//         0x0040L
	{ _T("ListBox"), STYLE(LBS_USETABSTOPS), NOMASK, 0 },//        0x0080L
	{ _T("ListBox"), STYLE(LBS_NOINTEGRALHEIGHT), NOMASK, 0 },//   0x0100L
	{ _T("ListBox"), STYLE(LBS_MULTICOLUMN), NOMASK, 0 },//        0x0200L
	{ _T("ListBox"), STYLE(LBS_WANTKEYBOARDINPUT), NOMASK, 0 },//  0x0400L
	{ _T("ListBox"), STYLE(LBS_EXTENDEDSEL), NOMASK, 0 },//        0x0800L
	{ _T("ListBox"), STYLE(LBS_DISABLENOSCROLL), NOMASK, 0 },//    0x1000L
	{ _T("ListBox"), STYLE(LBS_NODATA), NOMASK, 0 },//             0x2000L
	{ _T("ListBox"), STYLE(LBS_NOSEL), NOMASK, 0 },//              0x4000L
	{ _T("ListBox"), STYLE(LBS_STANDARD), NOMASK, 0 },//           (LBS_NOTIFY | LBS_SORT | WS_VSCROLL | WS_BORDER)

	{ _T("Scrollbar"), STYLE(SBS_HORZ), 0x1L, 0 },//                       0x0000L
	{ _T("Scrollbar"), STYLE(SBS_VERT), 0x1L, 0 },//                       0x0001L
	{ _T("Scrollbar"), STYLE(SBS_TOPALIGN), NOMASK, 0 },//                 0x0002L
	{ _T("Scrollbar"), STYLE(SBS_LEFTALIGN), NOMASK, 0 },//                0x0002L
	{ _T("Scrollbar"), STYLE(SBS_BOTTOMALIGN), NOMASK, 0 },//              0x0004L
	{ _T("Scrollbar"), STYLE(SBS_RIGHTALIGN), NOMASK, 0 },//               0x0004L
	{ _T("Scrollbar"), STYLE(SBS_SIZEBOXTOPLEFTALIGN), NOMASK, 0 },//      0x0002L
	{ _T("Scrollbar"), STYLE(SBS_SIZEBOXBOTTOMRIGHTALIGN), NOMASK, 0 },//  0x0004L
	{ _T("Scrollbar"), STYLE(SBS_SIZEBOX), NOMASK, 0 },//                  0x0008L
	{ _T("Scrollbar"), STYLE(SBS_SIZEGRIP), NOMASK, 0 },//                 0x0010L

	/*{ _T("toolbarwindow32"), STYLE(TBSTYLE_BUTTON), 0x1L, 0 },//             0x0000
	{ _T("toolbarwindow32"), STYLE(TBSTYLE_SEP), 0x1L, 0 },//                0x0001
	{ _T("toolbarwindow32"), STYLE(TBSTYLE_CHECK), NOMASK, 0 },//            0x0002
	{ _T("toolbarwindow32"), STYLE(TBSTYLE_GROUP), NOMASK, 0 },//            0x0004
	{ _T("toolbarwindow32"), STYLE(TBSTYLE_CHECKGROUP), NOMASK, 0 },//       (TBSTYLE_GROUP | TBSTYLE_CHECK)
	{ _T("toolbarwindow32"), STYLE(TBSTYLE_DROPDOWN), NOMASK, 0 },//         0x0008*/
	{ _T("toolbarwindow32"), STYLE(TBSTYLE_AUTOSIZE), NOMASK, 0 },//         0x0010 // automatically calculate the cx of the button
	{ _T("toolbarwindow32"), STYLE(TBSTYLE_NOPREFIX), NOMASK, 0 },//         0x0020 // if this button should not have accel prefix
	{ _T("toolbarwindow32"), STYLE(TBSTYLE_TOOLTIPS), NOMASK, 0 },//         0x0100
	{ _T("toolbarwindow32"), STYLE(TBSTYLE_WRAPABLE), NOMASK, 0 },//         0x0200
	{ _T("toolbarwindow32"), STYLE(TBSTYLE_ALTDRAG), NOMASK, 0 },//          0x0400
	{ _T("toolbarwindow32"), STYLE(TBSTYLE_FLAT), NOMASK, 0 },//             0x0800
	{ _T("toolbarwindow32"), STYLE(TBSTYLE_LIST), NOMASK, 0 },//             0x1000
	{ _T("toolbarwindow32"), STYLE(TBSTYLE_CUSTOMERASE), NOMASK, 0 },//      0x2000
	{ _T("toolbarwindow32"), STYLE(TBSTYLE_REGISTERDROP), NOMASK, 0 },//     0x4000
	{ _T("toolbarwindow32"), STYLE(TBSTYLE_TRANSPARENT), NOMASK, 0 },//      0x8000
	{ _T("toolbarwindow32"), STYLE(TBSTYLE_EX_DRAWDDARROWS), NOMASK, 1 },//  0x00000001

	{ _T("CommonControl"), STYLE(CCS_TOP), NOMASK, 0 },//                 0x00000001L
	{ _T("CommonControl"), STYLE(CCS_NOMOVEY), NOMASK, 0 },//             0x00000002L
	{ _T("CommonControl"), STYLE(CCS_BOTTOM), NOMASK, 0 },//              0x00000003L
	{ _T("CommonControl"), STYLE(CCS_NORESIZE), NOMASK, 0 },//            0x00000004L
	{ _T("CommonControl"), STYLE(CCS_NOPARENTALIGN), NOMASK, 0 },//       0x00000008L
	{ _T("CommonControl"), STYLE(CCS_ADJUSTABLE), NOMASK, 0 },//          0x00000020L
	{ _T("CommonControl"), STYLE(CCS_NODIVIDER), NOMASK, 0 },//           0x00000040L
	{ _T("CommonControl"), STYLE(CCS_VERT), NOMASK, 0 },//                0x00000080L

	{ _T("msctls_updown32"), STYLE(UDS_WRAP), NOMASK, 0 },//                 0x0001
	{ _T("msctls_updown32"), STYLE(UDS_SETBUDDYINT), NOMASK, 0 },//          0x0002
	{ _T("msctls_updown32"), STYLE(UDS_ALIGNRIGHT), NOMASK, 0 },//           0x0004
	{ _T("msctls_updown32"), STYLE(UDS_ALIGNLEFT), NOMASK, 0 },//            0x0008
	{ _T("msctls_updown32"), STYLE(UDS_AUTOBUDDY), NOMASK, 0 },//            0x0010
	{ _T("msctls_updown32"), STYLE(UDS_ARROWKEYS), NOMASK, 0 },//            0x0020
	{ _T("msctls_updown32"), STYLE(UDS_HORZ), NOMASK, 0 },//                 0x0040
	{ _T("msctls_updown32"), STYLE(UDS_NOTHOUSANDS), NOMASK, 0 },//          0x0080
	{ _T("msctls_updown32"), STYLE(UDS_HOTTRACK), NOMASK, 0 },//             0x0100

	{ _T("msctls_progress32"), STYLE(PBS_SMOOTH), NOMASK, 0 },//               0x01
	{ _T("msctls_progress32"), STYLE(PBS_VERTICAL), NOMASK, 0 },//             0x04

	{ _T("msctls_trackbar32"), STYLE(TBS_AUTOTICKS), NOMASK, 0 },//            0x0001
	{ _T("msctls_trackbar32"), STYLE(TBS_VERT), NOMASK, 0 },//                 0x0002
	{ _T("msctls_trackbar32"), STYLE(TBS_HORZ), NOMASK, 0 },//                 0x0000
	{ _T("msctls_trackbar32"), STYLE(TBS_TOP), NOMASK, 0 },//                  0x0004
	{ _T("msctls_trackbar32"), STYLE(TBS_BOTTOM), NOMASK, 0 },//               0x0000
	{ _T("msctls_trackbar32"), STYLE(TBS_LEFT), NOMASK, 0 },//                 0x0004
	{ _T("msctls_trackbar32"), STYLE(TBS_RIGHT), NOMASK, 0 },//                0x0000
	{ _T("msctls_trackbar32"), STYLE(TBS_BOTH), NOMASK, 0 },//                 0x0008
	{ _T("msctls_trackbar32"), STYLE(TBS_NOTICKS), NOMASK, 0 },//              0x0010
	{ _T("msctls_trackbar32"), STYLE(TBS_ENABLESELRANGE), NOMASK, 0 },//       0x0020
	{ _T("msctls_trackbar32"), STYLE(TBS_FIXEDLENGTH), NOMASK, 0 },//          0x0040
	{ _T("msctls_trackbar32"), STYLE(TBS_NOTHUMB), NOMASK, 0 },//              0x0080
	{ _T("msctls_trackbar32"), STYLE(TBS_TOOLTIPS), NOMASK, 0 },//             0x0100

	//	{ _T("msctls_hotkey32"), // has no styles

	{ _T("SysListView32"), STYLE(LVS_ICON), LVS_TYPEMASK, 0 },//           0x0000
	{ _T("SysListView32"), STYLE(LVS_REPORT), LVS_TYPEMASK, 0 },//         0x0001
	{ _T("SysListView32"), STYLE(LVS_SMALLICON), LVS_TYPEMASK, 0 },//      0x0002
	{ _T("SysListView32"), STYLE(LVS_LIST), LVS_TYPEMASK, 0 },//           0x0003
	//	{ _T("SysListView32"), STYLE(LVS_TYPEMASK), NOMASK, 0 },//             0x0003
	{ _T("SysListView32"), STYLE(LVS_SINGLESEL), NOMASK, 0 },//            0x0004
	{ _T("SysListView32"), STYLE(LVS_SHOWSELALWAYS), NOMASK, 0 },//        0x0008
	{ _T("SysListView32"), STYLE(LVS_SORTASCENDING), NOMASK, 0 },//        0x0010
	{ _T("SysListView32"), STYLE(LVS_SORTDESCENDING), NOMASK, 0 },//       0x0020
	{ _T("SysListView32"), STYLE(LVS_SHAREIMAGELISTS), NOMASK, 0 },//      0x0040
	{ _T("SysListView32"), STYLE(LVS_NOLABELWRAP), NOMASK, 0 },//          0x0080
	{ _T("SysListView32"), STYLE(LVS_AUTOARRANGE), NOMASK, 0 },//          0x0100
	{ _T("SysListView32"), STYLE(LVS_EDITLABELS), NOMASK, 0 },//           0x0200
	{ _T("SysListView32"), STYLE(LVS_OWNERDATA), NOMASK, 0 },//            0x1000
	{ _T("SysListView32"), STYLE(LVS_NOSCROLL), NOMASK, 0 },//             0x2000
	//	{ _T("SysListView32"), STYLE(LVS_TYPESTYLEMASK), NOMASK, 0 },//        0xfc00
	{ _T("SysListView32"), STYLE(LVS_ALIGNTOP), LVS_ALIGNMASK, 0 },//      0x0000
	{ _T("SysListView32"), STYLE(LVS_ALIGNLEFT), LVS_ALIGNMASK, 0 },//     0x0800
	//	{ _T("SysListView32"), STYLE(LVS_ALIGNMASK), NOMASK, 0 },//            0x0c00
	{ _T("SysListView32"), STYLE(LVS_OWNERDRAWFIXED), NOMASK, 0 },//       0x0400
	{ _T("SysListView32"), STYLE(LVS_NOCOLUMNHEADER), NOMASK, 0 },//       0x4000
	{ _T("SysListView32"), STYLE(LVS_NOSORTHEADER), NOMASK, 0 },//         0x8000
	{ _T("SysListView32"), STYLE(LVS_EX_GRIDLINES), NOMASK, 1 },//         0x00000001
	{ _T("SysListView32"), STYLE(LVS_EX_SUBITEMIMAGES), NOMASK, 1 },//     0x00000002
	{ _T("SysListView32"), STYLE(LVS_EX_CHECKBOXES), NOMASK, 1 },//        0x00000004
	{ _T("SysListView32"), STYLE(LVS_EX_TRACKSELECT), NOMASK, 1 },//       0x00000008
	{ _T("SysListView32"), STYLE(LVS_EX_HEADERDRAGDROP), NOMASK, 1 },//    0x00000010
	{ _T("SysListView32"), STYLE(LVS_EX_FULLROWSELECT), NOMASK, 1 },//     0x00000020 // applies to report mode only
	{ _T("SysListView32"), STYLE(LVS_EX_ONECLICKACTIVATE), NOMASK, 1 },//  0x00000040
	{ _T("SysListView32"), STYLE(LVS_EX_TWOCLICKACTIVATE), NOMASK, 1 },//  0x00000080
	{ _T("SysListView32"), STYLE(LVS_EX_FLATSB), NOMASK, 1 },//            0x00000100
	{ _T("SysListView32"), STYLE(LVS_EX_REGIONAL), NOMASK, 1 },//          0x00000200
	{ _T("SysListView32"), STYLE(LVS_EX_INFOTIP), NOMASK, 1 },//           0x00000400 // listview does InfoTips for you
	{ _T("SysListView32"), STYLE(LVS_EX_UNDERLINEHOT), NOMASK, 1 },//      0x00000800
	{ _T("SysListView32"), STYLE(LVS_EX_UNDERLINECOLD), NOMASK, 1 },//     0x00001000
	{ _T("SysListView32"), STYLE(LVS_EX_MULTIWORKAREAS), NOMASK, 1 },//    0x00002000

	{ _T("SysTreeView32"), STYLE(TVS_HASBUTTONS), NOMASK, 0 },//           0x0001
	{ _T("SysTreeView32"), STYLE(TVS_HASLINES), NOMASK, 0 },//             0x0002
	{ _T("SysTreeView32"), STYLE(TVS_LINESATROOT), NOMASK, 0 },//          0x0004
	{ _T("SysTreeView32"), STYLE(TVS_EDITLABELS), NOMASK, 0 },//           0x0008
	{ _T("SysTreeView32"), STYLE(TVS_DISABLEDRAGDROP), NOMASK, 0 },//      0x0010
	{ _T("SysTreeView32"), STYLE(TVS_SHOWSELALWAYS), NOMASK, 0 },//        0x0020
	{ _T("SysTreeView32"), STYLE(TVS_RTLREADING), NOMASK, 0 },//           0x0040
	{ _T("SysTreeView32"), STYLE(TVS_NOTOOLTIPS), NOMASK, 0 },//           0x0080
	{ _T("SysTreeView32"), STYLE(TVS_CHECKBOXES), NOMASK, 0 },//           0x0100
	{ _T("SysTreeView32"), STYLE(TVS_TRACKSELECT), NOMASK, 0 },//          0x0200
	{ _T("SysTreeView32"), STYLE(TVS_SINGLEEXPAND), NOMASK, 0 },//         0x0400
	{ _T("SysTreeView32"), STYLE(TVS_INFOTIP), NOMASK, 0 },//              0x0800
	{ _T("SysTreeView32"), STYLE(TVS_FULLROWSELECT), NOMASK, 0 },//        0x1000
	{ _T("SysTreeView32"), STYLE(TVS_NOSCROLL), NOMASK, 0 },//             0x2000
	{ _T("SysTreeView32"), STYLE(TVS_NONEVENHEIGHT), NOMASK, 0 },//        0x4000

	{ _T("SysTabControl32"), STYLE(TCS_SCROLLOPPOSITE), NOMASK, 0 },//       0x0001   // assumes multiline tab
	{ _T("SysTabControl32"), STYLE(TCS_BOTTOM), NOMASK, 0 },//               0x0002
	{ _T("SysTabControl32"), STYLE(TCS_RIGHT), NOMASK, 0 },//                0x0002
	{ _T("SysTabControl32"), STYLE(TCS_MULTISELECT), NOMASK, 0 },//          0x0004  // allow multi-select in button mode
	{ _T("SysTabControl32"), STYLE(TCS_FLATBUTTONS), NOMASK, 0 },//          0x0008
	{ _T("SysTabControl32"), STYLE(TCS_FORCEICONLEFT), NOMASK, 0 },//        0x0010
	{ _T("SysTabControl32"), STYLE(TCS_FORCELABELLEFT), NOMASK, 0 },//       0x0020
	{ _T("SysTabControl32"), STYLE(TCS_HOTTRACK), NOMASK, 0 },//             0x0040
	{ _T("SysTabControl32"), STYLE(TCS_VERTICAL), NOMASK, 0 },//             0x0080
	{ _T("SysTabControl32"), STYLE(TCS_TABS), NOMASK, 0 },//                 0x0000
	{ _T("SysTabControl32"), STYLE(TCS_BUTTONS), NOMASK, 0 },//              0x0100
	{ _T("SysTabControl32"), STYLE(TCS_SINGLELINE), NOMASK, 0 },//           0x0000
	{ _T("SysTabControl32"), STYLE(TCS_MULTILINE), NOMASK, 0 },//            0x0200
	{ _T("SysTabControl32"), STYLE(TCS_RIGHTJUSTIFY), NOMASK, 0 },//         0x0000
	{ _T("SysTabControl32"), STYLE(TCS_FIXEDWIDTH), NOMASK, 0 },//           0x0400
	{ _T("SysTabControl32"), STYLE(TCS_RAGGEDRIGHT), NOMASK, 0 },//          0x0800
	{ _T("SysTabControl32"), STYLE(TCS_FOCUSONBUTTONDOWN), NOMASK, 0 },//    0x1000
	{ _T("SysTabControl32"), STYLE(TCS_OWNERDRAWFIXED), NOMASK, 0 },//       0x2000
	{ _T("SysTabControl32"), STYLE(TCS_TOOLTIPS), NOMASK, 0 },//             0x4000
	{ _T("SysTabControl32"), STYLE(TCS_FOCUSNEVER), NOMASK, 0 },//           0x8000
	{ _T("SysTabControl32"), STYLE(TCS_EX_FLATSEPARATORS), NOMASK, 1 },//   0x00000001
	{ _T("SysTabControl32"), STYLE(TCS_EX_REGISTERDROP), NOMASK, 1 },//     0x00000002

	{ _T("SysAnimate32"), STYLE(ACS_CENTER), NOMASK, 0 },//               0x0001
	{ _T("SysAnimate32"), STYLE(ACS_TRANSPARENT), NOMASK, 0 },//          0x0002
	{ _T("SysAnimate32"), STYLE(ACS_AUTOPLAY), NOMASK, 0 },//             0x0004
	{ _T("SysAnimate32"), STYLE(ACS_TIMER), NOMASK, 0 },//                0x0008  // don't use threads... use timers

	// uses styles from EDIT control
	{ _T("Richedit"), STYLE(ES_SAVESEL), NOMASK, 0 },//            0x00008000
	{ _T("Richedit"), STYLE(ES_SUNKEN), NOMASK, 0 },//             0x00004000
	{ _T("Richedit"), STYLE(ES_DISABLENOSCROLL), NOMASK, 0 },//    0x00002000
	{ _T("Richedit"), STYLE(ES_SELECTIONBAR), NOMASK, 0 },//       0x01000000
	{ _T("Richedit"), STYLE(ES_NOOLEDRAGDROP), NOMASK, 0 },//      0x00000008
	// event mask is like extended styles
	{ _T("Richedit"), STYLE(ENM_NONE), 0x1L, 1 },//                0x00000000
	{ _T("Richedit"), STYLE(ENM_CHANGE), NOMASK, 1 },//            0x00000001
	{ _T("Richedit"), STYLE(ENM_UPDATE), NOMASK, 1 },//            0x00000002
	{ _T("Richedit"), STYLE(ENM_SCROLL), NOMASK, 1 },//            0x00000004
	{ _T("Richedit"), STYLE(ENM_KEYEVENTS), NOMASK, 1 },//         0x00010000
	{ _T("Richedit"), STYLE(ENM_MOUSEEVENTS), NOMASK, 1 },//       0x00020000
	{ _T("Richedit"), STYLE(ENM_REQUESTRESIZE), NOMASK, 1 },//     0x00040000
	{ _T("Richedit"), STYLE(ENM_SELCHANGE), NOMASK, 1 },//         0x00080000
	{ _T("Richedit"), STYLE(ENM_DROPFILES), NOMASK, 1 },//         0x00100000
	{ _T("Richedit"), STYLE(ENM_PROTECTED), NOMASK, 1 },//         0x00200000
	{ _T("Richedit"), STYLE(ENM_CORRECTTEXT), NOMASK, 1 },//       0x00400000     /* PenWin specific */
	{ _T("Richedit"), STYLE(ENM_SCROLLEVENTS), NOMASK, 1 },//      0x00000008
	{ _T("Richedit"), STYLE(ENM_DRAGDROPDONE), NOMASK, 1 },//      0x00000010
	{ _T("Richedit"), STYLE(ENM_IMECHANGE), NOMASK, 1 },//         0x00800000     /* unused by RE2.0 */
	{ _T("Richedit"), STYLE(ENM_LANGCHANGE), NOMASK, 1 },//        0x01000000
	{ _T("Richedit"), STYLE(ENM_OBJECTPOSITIONS), NOMASK, 1 },//   0x02000000
	{ _T("Richedit"), STYLE(ENM_LINK), NOMASK, 1 },//              0x04000000

	{ _T("SysDateTimePick32"), STYLE(DTS_UPDOWN), NOMASK, 0 },//           0x0001 // use UPDOWN instead of MONTHCAL
	{ _T("SysDateTimePick32"), STYLE(DTS_SHOWNONE), NOMASK, 0 },//         0x0002 // allow a NONE selection
	{ _T("SysDateTimePick32"), STYLE(DTS_SHORTDATEFORMAT), 0x4L, 0 },//    0x0000 // use the short date format (app must forward WM_WININICHANGE messages)
	{ _T("SysDateTimePick32"), STYLE(DTS_LONGDATEFORMAT), 0x4L, 0 },//     0x0004 // use the long date format (app must forward WM_WININICHANGE messages)
	{ _T("SysDateTimePick32"), STYLE(DTS_TIMEFORMAT), NOMASK, 0 },//       0x0009 // use the time format (app must forward WM_WININICHANGE messages)
	{ _T("SysDateTimePick32"), STYLE(DTS_APPCANPARSE), NOMASK, 0 },//      0x0010 // allow user entered strings (app MUST respond to DTN_USERSTRING)
	{ _T("SysDateTimePick32"), STYLE(DTS_RIGHTALIGN), NOMASK, 0 },//       0x0020 // right-align popup instead of left-align it

	{ _T("SysMonthCal32"), STYLE(MCS_DAYSTATE), NOMASK, 0 },//         0x0001
	{ _T("SysMonthCal32"), STYLE(MCS_MULTISELECT), NOMASK, 0 },//      0x0002
	{ _T("SysMonthCal32"), STYLE(MCS_WEEKNUMBERS), NOMASK, 0 },//      0x0004
	{ _T("SysMonthCal32"), STYLE(MCS_NOTODAYCIRCLE), NOMASK, 0 },//    0x0008
	{ _T("SysMonthCal32"), STYLE(MCS_NOTODAY), NOMASK, 0 },//          0x0010

	//	{ _T("SysIPAddress32"), // no styles

	{ _T("SysPager32"), STYLE(PGS_VERT), 0x1L, 0 },//                   0x00000000
	{ _T("SysPager32"), STYLE(PGS_HORZ), 0x1L, 0 }, //                  0x00000001
	{ _T("SysPager32"), STYLE(PGS_AUTOSCROLL), NOMASK, 0 },//           0x00000002
	{ _T("SysPager32"), STYLE(PGS_DRAGNDROP), NOMASK, 0 },//            0x00000004

	// uses styles from combo box
	{ _T("ComboBoxEx32"), STYLE(CBES_EX_NOEDITIMAGE), NOMASK, 1 },//          0x00000001
	{ _T("ComboBoxEx32"), STYLE(CBES_EX_NOEDITIMAGEINDENT), NOMASK, 1 },//    0x00000002
	{ _T("ComboBoxEx32"), STYLE(CBES_EX_PATHWORDBREAKPROC), NOMASK, 1 },//    0x00000004
	{ _T("ComboBoxEx32"), STYLE(CBES_EX_NOSIZELIMIT), NOMASK, 1 },//          0x00000008
	{ _T("ComboBoxEx32"), STYLE(CBES_EX_CASESENSITIVE), NOMASK, 1 },//        0x00000010

	{ _T("msctls_statusbar32"), STYLE(SBARS_SIZEGRIP), NOMASK, 0 },//         0x0100

	{ _T("ReBarWindow32"), STYLE(RBBS_BREAK), NOMASK, 0 },//           0x00000001  // break to new line
	{ _T("ReBarWindow32"), STYLE(RBBS_FIXEDSIZE), NOMASK, 0 },//       0x00000002  // band can't be sized
	{ _T("ReBarWindow32"), STYLE(RBBS_CHILDEDGE), NOMASK, 0 },//       0x00000004  // edge around top & bottom of child window
	{ _T("ReBarWindow32"), STYLE(RBBS_HIDDEN), NOMASK, 0 },//          0x00000008  // don't show
	{ _T("ReBarWindow32"), STYLE(RBBS_NOVERT), NOMASK, 0 },//          0x00000010  // don't show when vertical
	{ _T("ReBarWindow32"), STYLE(RBBS_FIXEDBMP), NOMASK, 0 },//        0x00000020  // bitmap doesn't move during band resize
	{ _T("ReBarWindow32"), STYLE(RBBS_VARIABLEHEIGHT), NOMASK, 0 },//  0x00000040  // allow autosizing of this child vertically
	{ _T("ReBarWindow32"), STYLE(RBBS_GRIPPERALWAYS), NOMASK, 0 },//   0x00000080  // always show the gripper
	{ _T("ReBarWindow32"), STYLE(RBBS_NOGRIPPER), NOMASK, 0 },//       0x00000100  // never show the gripper

	{ _T("SysHeader32"), STYLE(HDS_HORZ), 0x1L, 0 },//                   0x0000
	{ _T("SysHeader32"), STYLE(HDS_VERT), 0x1L, 0 },//                   0x0001
	{ _T("SysHeader32"), STYLE(HDS_BUTTONS), NOMASK, 0 },//              0x0002
	{ _T("SysHeader32"), STYLE(HDS_HOTTRACK), NOMASK, 0 },//             0x0004
	{ _T("SysHeader32"), STYLE(HDS_HIDDEN), NOMASK, 0 },//               0x0008
	{ _T("SysHeader32"), STYLE(HDS_DRAGDROP), NOMASK, 0 },//             0x0040
	{ _T("SysHeader32"), STYLE(HDS_FULLDRAG), NOMASK, 0 },//             0x0080
	{ _T("SysHeader32"), STYLE(HDS_BUTTONS), NOMASK, 0 },//              0x0002

	{ _T("DialogBox"), STYLE(DS_ABSALIGN), NOMASK, 0 },//          0x01L
	{ _T("DialogBox"), STYLE(DS_SYSMODAL), NOMASK, 0 },//          0x02L
	{ _T("DialogBox"), STYLE(DS_LOCALEDIT), NOMASK, 0 },//         0x20L   /* Edit items get Local storage. */
	{ _T("DialogBox"), STYLE(DS_SETFONT), NOMASK, 0 },//           0x40L   /* User specified font for Dlg controls */
	{ _T("DialogBox"), STYLE(DS_MODALFRAME), NOMASK, 0 },//        0x80L   /* Can be combined with WS_CAPTION  */
	{ _T("DialogBox"), STYLE(DS_NOIDLEMSG), NOMASK, 0 },//         0x100L  /* WM_ENTERIDLE message will not be sent */
	{ _T("DialogBox"), STYLE(DS_SETFOREGROUND), NOMASK, 0 },//     0x200L  /* not in win3.1 */
	{ _T("DialogBox"), STYLE(DS_3DLOOK), NOMASK, 0 },//            0x0004L
	{ _T("DialogBox"), STYLE(DS_FIXEDSYS), NOMASK, 0 },//          0x0008L
	{ _T("DialogBox"), STYLE(DS_NOFAILCREATE), NOMASK, 0 },//      0x0010L
	{ _T("DialogBox"), STYLE(DS_CONTROL), NOMASK, 0 },//           0x0400L
	{ _T("DialogBox"), STYLE(DS_CENTER), NOMASK, 0 },//            0x0800L
	{ _T("DialogBox"), STYLE(DS_CENTERMOUSE), NOMASK, 0 },//       0x1000L
	{ _T("DialogBox"), STYLE(DS_CONTEXTHELP), NOMASK, 0 },//       0x2000L

	//	{ _T("MDIClient"), // no styles

#ifndef TTS_NOANIMATE

#define TTS_NOANIMATE           0x10
#define TTS_NOFADE              0x20
#define TTS_BALLOON             0x40

#endif

	{ _T("tooltips_class32"), STYLE(TTS_ALWAYSTIP), NOMASK, 0 }, //   0x01
	{ _T("tooltips_class32"), STYLE(TTS_NOPREFIX), NOMASK, 0 }, //    0x02
	{ _T("tooltips_class32"), STYLE(TTS_NOANIMATE), NOMASK, 0 }, //   0x10
	{ _T("tooltips_class32"), STYLE(TTS_NOFADE), NOMASK, 0 }, //      0x20
	{ _T("tooltips_class32"), STYLE(TTS_BALLOON), NOMASK, 0 }, //     0x40

};
#endif

const int NUMCTRLSTYLES = sizeof(ControlStyles) / sizeof(CtrlStyle);

static UINT LookupCtrlStyle(LPCTSTR szStyle)
{
	int nStyle = NUMCTRLSTYLES;

	while (nStyle--)
	{
		if (lstrcmp(szStyle, ControlStyles[nStyle].szStyle) == 0)
		{
			return ControlStyles[nStyle].dwStyle;
		}
	}

	return 0;
}

/////
