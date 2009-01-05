// AfxGadgets library.
// Copyright (c) 2004-2009 by Elijah Zarezky,
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

// ddxv.cpp - implementation of the extended DDX/DDV routines

//////////////////////////////////////////////////////////////////////////////////////////////
// PCH includes

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// other includes

#include "ddxv.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// debugging support

#if defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// _DEBUG

//////////////////////////////////////////////////////////////////////////////////////////////
// helper routines

void FailMinMaxWithFormat(CDataExchange* pDX, LONGLONG minVal, LONGLONG maxVal, LPCTSTR pszFormat, UINT idsPrompt)
{
	ASSERT(pDX != NULL);
	ASSERT(pszFormat != NULL);

	if (!pDX->m_bSaveAndValidate)
	{
		TRACE(traceAppMsg, 0, "Warning: initial dialog data is out of range.\n");
		return;
	}

	TCHAR szMin[64] = { 0 };
	TCHAR szMax[64] = { 0 };
#if (_MSC_VER < 1500)
	_stprintf(szMin, pszFormat, minVal);
	_stprintf(szMax, pszFormat, maxVal);
#else
	_stprintf_s(szMin, _countof(szMin), pszFormat, minVal);
	_stprintf_s(szMax, _countof(szMax), pszFormat, maxVal);
#endif   // _MSC_VER
	CString strPrompt;
	AfxFormatString2(strPrompt, idsPrompt, szMin, szMax);
	AfxMessageBox(strPrompt, MB_ICONEXCLAMATION, idsPrompt);
	strPrompt.Empty();
	pDX->Fail();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// extra Dialog Data Exchange routines

void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, WORD& value)
{
	HWND hEditWnd = pDX->PrepareEditCtrl(nIDC);
   ASSERT(hEditWnd != NULL);

	TCHAR szText[64] = { 0 };

	if (pDX->m_bSaveAndValidate)
	{
		::GetWindowText(hEditWnd, szText, _countof(szText));
		if (_stscanf(szText, _T("%hu"), &value) != 1)
		{
			AfxMessageBox(AFX_IDP_PARSE_UINT);
			pDX->Fail();
		}
	}
	else {
#if (_MSC_VER < 1500)
		_stprintf(szText, _T("%hu"), value);
#else
		_stprintf_s(szText, _countof(szText), _T("%hu"), value);
#endif   // _MSC_VER
		AfxSetWindowText(hEditWnd, szText);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
// extra Dialog Data Validation routines

void AFXAPI DDV_MinMaxWord(CDataExchange* pDX, WORD value, WORD minVal, WORD maxVal)
{
	ASSERT(minVal <= maxVal);

	if (value < minVal || value > maxVal)
	{
		FailMinMaxWithFormat(pDX, minVal, maxVal, _T("%I64u"), AFX_IDP_PARSE_INT_RANGE);
	}
}

// end of file
