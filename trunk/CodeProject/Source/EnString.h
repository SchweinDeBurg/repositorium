// EnString.h: interface for the CEnString class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENSTRING_H__BBF8AAC0_9B2C_11D5_95AD_444553540000__INCLUDED_)
#define AFX_ENSTRING_H__BBF8AAC0_9B2C_11D5_95AD_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum { ES_END, ES_WORD, ES_PATH, ES_START };

class CEnString : public CString  
{
public:
	CEnString();
	CEnString(LPCTSTR lpszFormat, ... );
	CEnString(UINT nFormatID, LPCTSTR szText = NULL);
	CEnString(UINT nFormatID, int nData);
	CEnString(const CString& str);

	~CEnString(); // non-virtual else it messes up LPCTSTR operator

	CSize FormatDC(CDC* pDC, int nWidth, int nStyle = ES_END);
	CSize Draw(CDC* pDC, LPRECT lpRect, int nStyle = ES_END);

protected:
	CSize FormatDCEx(CDC* pDC, int nWidth, int nStyle = ES_END);

};

#endif // !defined(AFX_ENSTRING_H__BBF8AAC0_9B2C_11D5_95AD_444553540000__INCLUDED_)
