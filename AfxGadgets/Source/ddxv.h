// AfxGadgets library.
// Copyright (c) 2004-2008 by Elijah Zarezky,
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

// ddxv.h - interface of the extended DDX/DDV routines

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

#if !defined(__ddxv_h)
#define __ddxv_h

#if !defined(_INC_LIMITS)
#pragma message(__FILE__ " : put <limits.h> in your PCH to speed up compilation")
#include <limits.h>
#endif   // _INC_LIMITS

//////////////////////////////////////////////////////////////////////////////////////////////
// helper routines

void FailMinMaxWithFormat(CDataExchange* pDX, LONGLONG minVal, LONGLONG maxVal, LPCTSTR pszFormat, UINT idsPrompt);

//////////////////////////////////////////////////////////////////////////////////////////////
// extra Dialog Data Exchange routines

void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, WORD& value);

//////////////////////////////////////////////////////////////////////////////////////////////
// extra Dialog Data Validation routines

void AFXAPI DDV_MinMaxWord(CDataExchange* pDX, WORD value, WORD minVal, WORD maxVal);

//////////////////////////////////////////////////////////////////////////////////////////////
// combined Dialog Data Exchange/Validation routines (with numeric range validation)

template
<
	typename _Int_t,
	typename _TestInt_t,
	_Int_t _MinRange,
	_Int_t _MaxRange,
	void(AFXAPI* _DDV_MinMax)(CDataExchange*, _Int_t, _Int_t, _Int_t)
>
void DDXV_Generic(CDataExchange* pDX, int nIDC, _Int_t& value, _Int_t minVal, _Int_t maxVal, LPCTSTR pszFormat, UINT idsPrompt)
{
	if (minVal == maxVal)
	{
		// no user-defined range validation
		minVal = _MinRange;
		maxVal = _MaxRange;
	}

	ASSERT(minVal < maxVal);

	if (pDX->m_bSaveAndValidate)
	{
		_TestInt_t testVal = value;
		DDX_Text(pDX, nIDC, testVal);
		_Int_t minLimit = minVal > _MinRange ? minVal : _MinRange;
		_Int_t maxLimit = maxVal < _MaxRange ? maxVal : _MaxRange;
		if (testVal < minLimit || testVal > maxLimit)
		{
			FailMinMaxWithFormat(pDX, minLimit, maxLimit, pszFormat, idsPrompt);
		}
		else {
			value = static_cast<_Int_t>(testVal);
		}
	}
	else {
		DDX_Text(pDX, nIDC, value);
		(*_DDV_MinMax)(pDX, value, minVal, maxVal);
	}
}

inline void DDXV_Short(CDataExchange* pDX, int nIDC, short& value, short minVal = 0, short maxVal = 0)
{
	DDXV_Generic<short, LONGLONG, SHRT_MIN, SHRT_MAX, &DDV_MinMaxShort>
	(pDX, nIDC, value, minVal, maxVal, _T("%I64d"), AFX_IDP_PARSE_INT_RANGE);
}

inline void DDXV_Word(CDataExchange* pDX, int nIDC, WORD& value, WORD minVal = 0, WORD maxVal = 0)
{
	DDXV_Generic<WORD, ULONGLONG, 0, USHRT_MAX, &DDV_MinMaxWord>
	(pDX, nIDC, value, minVal, maxVal, _T("%I64u"), AFX_IDP_PARSE_INT_RANGE);
}

inline void DDXV_Int(CDataExchange* pDX, int nIDC, int& value, int minVal = 0, int maxVal = 0)
{
	DDXV_Generic<int, LONGLONG, INT_MIN, INT_MAX, &DDV_MinMaxInt>
	(pDX, nIDC, value, minVal, maxVal, _T("%I64d"), AFX_IDP_PARSE_INT_RANGE);
}

inline void DDXV_UInt(CDataExchange* pDX, int nIDC, UINT& value, UINT minVal = 0, UINT maxVal = 0)
{
	DDXV_Generic<UINT, ULONGLONG, 0, UINT_MAX, &DDV_MinMaxUInt>
	(pDX, nIDC, value, minVal, maxVal, _T("%I64u"), AFX_IDP_PARSE_INT_RANGE);
}

inline void DDXV_Long(CDataExchange* pDX, int nIDC, long& value, long minVal = 0, long maxVal = 0)
{
	DDXV_Generic<long, LONGLONG, LONG_MIN, LONG_MAX, &DDV_MinMaxLong>
	(pDX, nIDC, value, minVal, maxVal, _T("%I64d"), AFX_IDP_PARSE_INT_RANGE);
}

inline void DDXV_DWord(CDataExchange* pDX, int nIDC, DWORD& value, DWORD minVal = 0, DWORD maxVal = 0)
{
	DDXV_Generic<DWORD, ULONGLONG, 0, ULONG_MAX, &DDV_MinMaxDWord>
	(pDX, nIDC, value, minVal, maxVal, _T("%I64u"), AFX_IDP_PARSE_INT_RANGE);
}

#endif   // __ddxv_h

// end of file
