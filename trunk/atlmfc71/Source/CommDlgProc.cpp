// atlmfc71 library.
// Copyright (c) 2005 by Elijah Zarezky,
// All rights reserved.
// General copyright (c) 1992-2002 Microsoft Corporation.

// CommDlgProc.cpp - implementation of the _AfxCommDlgProc() hook procedure

#include "stdafx.h"
#include "FileDialogEx.h"

#if !defined(_DLGSH_INCLUDED_)
#pragma message(__FILE__ " : put <dlgs.h> in your PCH to speed up compilation")
#include <dlgs.h>
#endif	// _DLGSH_INCLUDED_

#include <../src/afximpl.h>

#if defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// _DEBUG

// extracted from "dlgcomm.cpp" v7.1

UINT _afxMsgLBSELCHANGE = 0;
UINT _afxMsgSHAREVI = 0;
UINT _afxMsgFILEOK = 0;
UINT _afxMsgCOLOROK = 0;
UINT _afxMsgHELP = 0;
UINT _afxMsgSETRGB = 0;

UINT_PTR CALLBACK
_AfxCommDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (hWnd == NULL)
		return 0;

	// If a m_pAlternateWndInit is set, this is the first time we've
	// seend the hWnd given to us in the callback. We'll need to
	// subclass it and reset the m_pAlternateWndInit flag.

	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	if (pThreadState->m_pAlternateWndInit != NULL && CWnd::FromHandlePermanent(hWnd) == NULL)
	{
		ASSERT_KINDOF(CFileDialogEx, pThreadState->m_pAlternateWndInit);
		pThreadState->m_pAlternateWndInit->SubclassWindow(hWnd);
		pThreadState->m_pAlternateWndInit = NULL;
	}

	if (message == WM_INITDIALOG)
	{
		_afxMsgLBSELCHANGE = ::RegisterWindowMessage(LBSELCHSTRING);
		_afxMsgSHAREVI = ::RegisterWindowMessage(SHAREVISTRING);
		_afxMsgFILEOK = ::RegisterWindowMessage(FILEOKSTRING);
		_afxMsgCOLOROK = ::RegisterWindowMessage(COLOROKSTRING);
		_afxMsgHELP = ::RegisterWindowMessage(HELPMSGSTRING);
		_afxMsgSETRGB = ::RegisterWindowMessage(SETRGBSTRING);
		return (UINT)AfxDlgProc(hWnd, message, wParam, lParam);
	}

	if (message == _afxMsgHELP ||
	   (message == WM_COMMAND && LOWORD(wParam) == pshHelp))
	{
		// just translate the message into the AFX standard help command.
		SendMessage(hWnd, WM_COMMAND, ID_HELP, 0);
		return 1;
	}

	if (message < 0xC000)
	{
		// not a ::RegisterWindowMessage message
		return 0;
	}

	// assume it is already wired up to a permanent one
	CDialog* pDlg = (CDialog*)CWnd::FromHandlePermanent(hWnd);
	ASSERT(pDlg != NULL);
	ASSERT_KINDOF(CDialog, pDlg);

	if (pDlg->IsKindOf(RUNTIME_CLASS(CFileDialogEx)))
	{
		// If we're exploring then we are not interested in the Registered messages
		if (((CFileDialogEx*)pDlg)->m_ofn.Flags & OFN_EXPLORER)
			return 0;
	}

	// RegisterWindowMessage - does not copy to lastState buffer, so
	// CWnd::GetCurrentMessage and CWnd::Default will NOT work
	// while in these handlers

	// Dispatch special commdlg messages through our virtual callbacks
	if (message == _afxMsgSHAREVI)
	{
		ASSERT_KINDOF(CFileDialogEx, pDlg);
		return ((CFileDialogEx*)pDlg)->OnShareViolation((LPCTSTR)lParam);
	}
	else if (message == _afxMsgFILEOK)
	{
		ASSERT_KINDOF(CFileDialogEx, pDlg);

		((CFileDialogEx*)pDlg)->m_pofnTemp = (OPENFILENAME*)lParam;
		BOOL bResult = ((CFileDialogEx*)pDlg)->OnFileNameOK();
		((CFileDialogEx*)pDlg)->m_pofnTemp = NULL;

		return bResult;
	}
	else if (message == _afxMsgLBSELCHANGE)
	{
		ASSERT_KINDOF(CFileDialogEx, pDlg);
		((CFileDialogEx*)pDlg)->OnLBSelChangedNotify((UINT)wParam, LOWORD(lParam),
				HIWORD(lParam));
		return 0;
	}
	else if (message == _afxMsgCOLOROK)
	{
		ASSERT_KINDOF(CColorDialog, pDlg);
		return ((CColorDialog*)pDlg)->OnColorOK();
	}
	else if (message == _afxMsgSETRGB)
	{
		// nothing to do here, since this is a SendMessage
		return 0;
	}
	return 0;
}

// end of file
