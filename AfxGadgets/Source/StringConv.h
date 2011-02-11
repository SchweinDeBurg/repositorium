// AfxGadgets library.
// Copyright (c) 2004-2011 by Elijah Zarezky,
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

// StringConv.h - Unicode <-> ANSI translation helpers

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif   // _MSC_VER

#if !defined(__StringConv_h)
#define __StringConv_h

class _A2W
{
// construction/destruction
public:
	_A2W(LPCSTR mbsSource);
	~_A2W(void);

// type convertsion
public:
	operator LPCWSTR(void) const;

// attributes
private:
	enum { BUF_SIZE = 16 };
	WCHAR m_szBuffer[BUF_SIZE];
	LPWSTR m_wcsResult;
};

inline _A2W::_A2W(LPCSTR mbsSource):
m_wcsResult(NULL)
{
	size_t cNumChars = mbstowcs(NULL, mbsSource, 0) + 1;
	m_wcsResult = cNumChars > BUF_SIZE ? new WCHAR[cNumChars] : m_szBuffer;
	mbstowcs(m_wcsResult, mbsSource, cNumChars);
}

inline _A2W::~_A2W(void)
{
	if (m_wcsResult != m_szBuffer)
	{
		delete[] m_wcsResult;
	}
}

inline _A2W::operator LPCWSTR(void) const
{
	return (m_wcsResult);
}

class _W2A
{
// construction/destruction
public:
	_W2A(LPCWSTR wcsSource);
	~_W2A(void);

// type convertsion
public:
	operator LPCSTR(void) const;

// attributes
private:
	enum { BUF_SIZE = 32 };
	CHAR m_szBuffer[BUF_SIZE];
	LPSTR m_mbsResult;
};

inline _W2A::_W2A(LPCWSTR wcsSource):
m_mbsResult(NULL)
{
	size_t cNumChars = wcstombs(NULL, wcsSource, 0) + 1;
	m_mbsResult = cNumChars > BUF_SIZE ? new CHAR[cNumChars] : m_szBuffer;
	wcstombs(m_mbsResult, wcsSource, cNumChars);
}

inline _W2A::~_W2A(void)
{
	if (m_mbsResult != m_szBuffer)
	{
		delete[] m_mbsResult;
	}
}

inline _W2A::operator LPCSTR(void) const
{
	return (m_mbsResult);
}

#if defined(_UNICODE)

class _W2W
{
// construction/destruction
public:
	_W2W(LPCWSTR wcsSource);
	~_W2W(void);

// type convertsion
public:
	operator LPCWSTR(void) const;

// attributes
private:
	enum { BUF_SIZE = 16 };
	WCHAR m_szBuffer[BUF_SIZE];
	LPWSTR m_wcsResult;
};

inline _W2W::_W2W(LPCWSTR wcsSource):
m_wcsResult(NULL)
{
	int nNumChars = ::lstrlenW(wcsSource) + 1;
	m_wcsResult = nNumChars > BUF_SIZE ? new WCHAR[nNumChars] : m_szBuffer;
	::lstrcpyW(m_wcsResult, wcsSource);
}

inline _W2W::~_W2W(void)
{
	if (m_wcsResult != m_szBuffer)
	{
		delete[] m_wcsResult;
	}
}

inline _W2W::operator LPCWSTR(void) const
{
	return (m_wcsResult);
}

typedef _W2A _T2A;
typedef _A2W _A2T;

typedef _W2W _T2W;
typedef _W2W _W2T;

#else

class _A2A
{
// construction/destruction
public:
	_A2A(LPCSTR mbsSource);
	~_A2A(void);

// type convertsion
public:
	operator LPCSTR(void) const;

// attributes
private:
	enum { BUF_SIZE = 32 };
	CHAR m_szBuffer[BUF_SIZE];
	LPSTR m_mbsResult;
};

inline _A2A::_A2A(LPCSTR mbsSource):
m_mbsResult(NULL)
{
	int nNumChars = ::lstrlenA(mbsSource) + 1;
	m_mbsResult = nNumChars > BUF_SIZE ? new CHAR[nNumChars] : m_szBuffer;
	::lstrcpyA(m_mbsResult, mbsSource);
}

inline _A2A::~_A2A(void)
{
	if (m_mbsResult != m_szBuffer)
	{
		delete[] m_mbsResult;
	}
}

inline _A2A::operator LPCSTR(void) const
{
	return (m_mbsResult);
}

typedef _A2A _T2A;
typedef _A2A _A2T;

typedef _A2W _T2W;
typedef _W2A _W2T;

#endif   // _UNICODE

#endif   // __StringConv_h

// end of file
