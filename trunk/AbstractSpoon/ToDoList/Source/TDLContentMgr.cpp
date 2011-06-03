// Copyright (C) 2003-2011 AbstractSpoon Software.
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

// TDLContentMgr.cpp: implementation of the CTDLContentMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TDLContentMgr.h"
#include "ToDoCommentsCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

class CDefaultContent : public IContent
{
	const char* GetTypeID() const
	{
		static LPCSTR sID = "PLAIN_TEXT";
		return sID;
	}
	const char* GetTypeDescription() const
	{
		static LPCSTR sDesc = "Simple Text";
		return sDesc;
	}

	IContentControl* CreateCtrl(unsigned short nCtrlID, unsigned long nStyle,
		long nLeft, long nTop, long nWidth, long nHeight, HWND hwndParent)
	{
		CToDoCommentsCtrl* pComments = new CToDoCommentsCtrl;

		nStyle |= ES_MULTILINE | ES_WANTRETURN | WS_VSCROLL;
		CRect rect(nLeft, nTop, nLeft + nWidth, nTop + nHeight);

		if (pComments->Create(nStyle, rect, CWnd::FromHandle(hwndParent), nCtrlID))
		{
			return pComments;
		}

		// else
		delete pComments;
		return NULL;
	}

	void Release()
	{
		delete this;
	}

	int ConvertToHtml(const unsigned char* /*pContent*/, int /*nLength*/, const TCHAR* /*szCharset*/,
		char*& /*szHtml*/)
	{
		return 0;   // not supported
	}
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTDLContentMgr::CTDLContentMgr()
{
}

CTDLContentMgr::~CTDLContentMgr()
{
}

BOOL CTDLContentMgr::Initialize()
{
	BOOL bWasInitialized = m_bInitialized;

	if (CContentMgr::Initialize())
	{
		if (!bWasInitialized)
		{
			m_aContent.InsertAt(0, new CDefaultContent);
		}

		return TRUE;
	}

	return FALSE;
}
