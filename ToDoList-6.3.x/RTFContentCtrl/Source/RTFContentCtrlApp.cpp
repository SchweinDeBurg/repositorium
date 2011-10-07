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
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
// - merged with ToDoList version 6.1.2-6.2.2 sources
//*****************************************************************************

// RTFContentCtrlApp.cpp : Defines the initialization routines for the DLL.
//

#include "StdAfx.h"
#include "RTFContentCtrlApp.h"
#include "RTFContentControl.h"
#include "../../../CodeProject/Source/FileMisc.h"

#include "RTF2HTMLConverter.h"

#include "../../../CodeProject/Source/FileMisc.h"
#include "../../../CodeProject/Source/Misc.h"

#include "../../../CodeProject/Source/Compression.h"
#include "../../../zlib/Source/zlib.h"

#include "../../../CodeProject/Source/TextFileDocument.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// from r2h.h
typedef int (*PFNCONVERTRTF2HTML)(const char*, const char*, unsigned long, const char*);

static CRTFContentCtrlApp theApp;

DLL_DECLSPEC IContent* CreateContentInterface()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return &theApp;
}

DLL_DECLSPEC int GetInterfaceVersion()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return ICONTENTCTRL_VERSION;
}

CRTFContentCtrlApp::CRTFContentCtrlApp()
{
}

const char* CRTFContentCtrlApp::GetTypeID() const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	static CStringA sID;

#if defined(UNICODE) || defined(_UNICODE)
	CString strTemp;
	Misc::GuidToString(RTF_TYPEID, strTemp);
	sID = ATL::CT2A(strTemp);
#else
	Misc::GuidToString(RTF_TYPEID, sID);
#endif   // UNICODE || _UNICODE

	return sID;
}

const char* CRTFContentCtrlApp::GetTypeDescription() const
{
	return "Rich Text";
}

IContentControl* CRTFContentCtrlApp::CreateCtrl(unsigned short nCtrlID, unsigned long nStyle, long nLeft, long nTop,
	long nWidth, long nHeight, HWND hwndParent)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// load localized resources
	HINSTANCE hResDll = LoadLibrary(_T("RTFContentCtrlLOC.dll"));

	if (hResDll)
	{
		AfxSetResourceHandle(hResDll);
	}

	CRTFContentControl* pControl = new CRTFContentControl;

	if (pControl)
	{
		CRect rect(nLeft, nTop, nLeft + nWidth, nTop + nHeight);

		if (pControl->Create(nStyle, rect, CWnd::FromHandle(hwndParent), nCtrlID, TRUE))
		{
			return pControl;
		}
	}

	delete pControl;
	return NULL;
}

void CRTFContentCtrlApp::Release()
{
}

int CRTFContentCtrlApp::ConvertToHtml(const unsigned char* pContent, int nLength, const TCHAR* szCharSet,
	char*& szHtml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (nLength == 0)
	{
		return 0;   // nothing to convert
	}

	// we may have to decompress it first
	unsigned char* pDecompressed = NULL;

	if (strncmp((const char*)pContent, RTFTAG, LENTAG) != 0)
	{
		int nLenDecompressed = 0;

		if (Compression::Decompress(pContent, nLength, pDecompressed, nLenDecompressed))
		{
			pContent = pDecompressed;
			nLength = nLenDecompressed;
		}
		else
		{
			return 0;
		}
	}

	// copy rtf to CString
	CString sRtf((LPCSTR)pContent, nLength), sHtml;
	nLength = 0; // reuse for resultant html length

	// try loading our new converter
	CString sRtf2HtmlPath = FileMisc::GetAppFolder() + _T("\\Rtf2HtmlBridge.dll");
	static HMODULE hMod = LoadLibrary(sRtf2HtmlPath);

	if (hMod)
	{
		typedef int (*PFNCONVERTRTF2HTML)(LPCTSTR /*rtfFile*/, LPCTSTR /*htmlFile*/, LPCTSTR /*arg1*/,
			LPCTSTR /*arg2*/, LPCTSTR /*arg3*/, LPCTSTR /*arg4*/, LPCTSTR /*arg5*/, LPCTSTR /*arg6*/,
			LPCTSTR /*arg7*/, LPCTSTR /*arg8*/, LPCTSTR /*arg9*/);

		PFNCONVERTRTF2HTML fnRtf2Html = (PFNCONVERTRTF2HTML)GetProcAddress(hMod, "fnRtf2Html");

		if (fnRtf2Html)
		{
			CString sTempRtf = FileMisc::GetTempFileName(_T("Rtf2Html"), _T("rtf"));
			FileMisc::SaveFile(sTempRtf, sRtf, sRtf.GetLength());

			// arguments
			// create a unique image folder every time this is called for this session
			static int nImgCount = 1;
			CString sImgDir, sCharSet;

			sCharSet.Format(_T("/CS:%s"), szCharSet);

			// fully qualify image folder path
			sImgDir.Format(_T("/ID:%simages%d"), FileMisc::GetTempFolder(), nImgCount);
			nImgCount++;

			CString sTempHtml = FileMisc::GetTempFileName(_T("Rtf2Html"), _T("html"));

			try
			{
				nLength = 0;
				int nRet = fnRtf2Html(sTempRtf, FileMisc::GetTempFolder(), sImgDir, _T("/IT:png"), _T("/DS:content"),
					sCharSet, _T(""), _T(""), _T(""), _T(""), _T(""));
				if (nRet)
				{
					CTextFileRead file(sTempHtml);
					file.Read(sHtml);
					nLength = sHtml.GetLength();
				}
			}
			catch (...)
			{
			}

			// cleanup
			DeleteFile(sTempRtf);
			DeleteFile(sTempHtml);
		}
	}

	if (nLength)
	{
		szHtml = new char[nLength + 1];

		memcpy(szHtml, sHtml, nLength);
		szHtml[nLength] = 0;
	}

	delete[] pDecompressed;

	return nLength;
}
