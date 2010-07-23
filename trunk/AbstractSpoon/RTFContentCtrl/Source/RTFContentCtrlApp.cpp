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
// - added AbstractSpoon Software copyright notice and licenese information
// - adjusted #include's paths
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

IContentControl* CRTFContentCtrlApp::CreateCtrl(unsigned short nCtrlID, unsigned long nStyle,
      long nLeft, long nTop, long nWidth, long nHeight, HWND hwndParent)
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
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//	delete this;
}

void CRTFContentCtrlApp::SetIniLocation(bool bRegistry, const char* szIniPathName)
{
	if (bRegistry)
	{
		m_pszRegistryKey = ATL::CA2T(_strdup(szIniPathName));
	}
	else
	{
		m_pszProfileName = ATL::CA2T(_strdup(szIniPathName));
	}
}

int CRTFContentCtrlApp::ConvertToHtml(const unsigned char* pContent,
                                      int nLength, char*& szHtml)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (nLength == 0)
	{
		return 0;   // nothing to convert
	}

	// we may have to decompress it first
	unsigned char* pDecompressed = NULL;

	if (strncmp((const char*)pContent, ATL::CT2A(RTFTAG), LENTAG) != 0)
	{
		int nLenDecompressed = 0;

		if (CRTFContentControl::Decompress(pContent, nLength, pDecompressed, nLenDecompressed))
		{
			pContent = pDecompressed;
			nLength = nLenDecompressed;
		}
		else
		{
			return 0;
		}
	}

	CString sRtf((LPCSTR)pContent, nLength), sHtml;

	// scan the string looking for anything that smells like a
	// multi-byte character, because CRTF_HTMLConverter can't
	// handle that at the moment
	if (!CRTF_HTMLConverter::HasMultiByteChars(sRtf))
	{
		/*
			// check the code page to see if it represents a multi-byte
			// char set because we can't handle that at present
			int nCodePage = CRTF_HTMLConverter::GetCodePage(sRtf);

			switch (nCodePage)
			{
			case 874:	// Thai
			case 932:	// Japanese
			case 936:	// Simplified Chinese
			case 949:	// Korean
			case 950:	// Traditional Chinese:
				return 0;
			}
		*/

		// scan the string looking for anything that smells like a
		// multi-byte character, because CRTF_HTMLConverter can't
		// handle that at the moment
		if (!Misc::IsMultibyteString(sRtf))
		{
			if (CRTF_HTMLConverter::Convert(sRtf, sHtml, FALSE))
			{
				nLength = sHtml.GetLength();
				szHtml = new char[nLength + 1];

				memcpy(szHtml, sHtml, nLength);
				szHtml[nLength] = 0;
			}
			else
			{
				nLength = 0;   // reuse
			}
		}
		else
		{
			nLength = 0;
		}
	}
	else
	{
		nLength = 0;
	}

	delete [] pDecompressed;

	return nLength;
}
