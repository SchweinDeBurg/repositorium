// AfxGadgets library.
// Copyright (c) 2003-2005 by Elijah Zarezky,
// All rights reserved.

// StringConv.h - Unicode <-> ANSI translation helpers

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
	int nNumChars = mbstowcs(NULL, mbsSource, 0) + 1;
	m_wcsResult = nNumChars > BUF_SIZE ? new WCHAR[nNumChars] : m_szBuffer;
	mbstowcs(m_wcsResult, mbsSource, nNumChars);
}

inline _A2W::~_A2W(void)
{
	if (m_wcsResult != m_szBuffer) {
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
	int nNumChars = wcstombs(NULL, wcsSource, 0) + 1;
	m_mbsResult = nNumChars > BUF_SIZE ? new CHAR[nNumChars] : m_szBuffer;
	wcstombs(m_mbsResult, wcsSource, nNumChars);
}

inline _W2A::~_W2A(void)
{
	if (m_mbsResult != m_szBuffer) {
		delete[] m_mbsResult;
	}
}

inline _W2A::operator LPCSTR(void) const
{
	return (m_mbsResult);
}

#if defined(_UNICODE)

typedef _W2A _T2A;
typedef _A2W _A2T;

inline LPCWSTR _T2W(LPCTSTR pszSource)
{
	return (pszSource);
}

inline LPCTSTR _W2T(LPCWSTR pszSource)
{
	return (pszSource);
}

#else

inline LPCSTR _T2A(LPCTSTR pszSource)
{
	return (pszSource);
}

inline LPCTSTR _A2T(LPCSTR pszSource)
{
	return (pszSource);
}

typedef _A2W _T2W;
typedef _W2A _W2T;

#endif	// _UNICODE

#endif	// __StringConv_h

// end of file
