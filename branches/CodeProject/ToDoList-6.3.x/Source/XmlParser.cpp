//  XmlParser.cpp Version 1.0

#include "stdafx.h"	// use of MFC is optional, unless CXmlWinApp is being used
#include <windows.h>
#include <tchar.h>
#include "XmlParser.h"

#ifndef __noop
#if _MSC_VER < 1300
#define __noop ((void)0)
#endif
#endif

#undef TRACE
#define TRACE __noop

// if you want to see the TRACE output, 
// uncomment this line:
//#include "XTrace.h"

#pragma warning(disable : 4127)		// conditional expression is constant
#pragma warning(disable : 4996)		// disable bogus deprecation warning

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif	//_DEBUG

//=============================================================================
CString CXmlElement::GetValue(LPCTSTR attribute)
//=============================================================================
{
	CString strAttribute(attribute);
	CString ret = m_strAttributes;
	int pos = m_strAttributes.Find(strAttribute + _T("="));
	if (pos >= 0)
	{
		ret.Delete(0, pos + strAttribute.GetLength() + 2);
		int n = ret.Find(_T("\""));
		if (n >= 0)
			ret = ret.Left(n);
	}
	else 
	{
		ret = _T("");
	}
	
	// restore special chars
	ret.Replace(_T("&amp;"), _T("&"));
	ret.Replace(_T("&quot;"), _T("\""));
	ret.Replace(_T("&lt;"), _T("<"));
	ret.Replace(_T("&gt;"), _T(">"));

	return ret;
}

//=============================================================================
CXmlParser::CXmlParser()
//=============================================================================
{
	TRACE(_T("in CXmlParser::CXmlParser\n"));
	m_nLevel = -1;
}

//=============================================================================
CXmlParser::~CXmlParser()
//=============================================================================
{
	TRACE(_T("in CXmlParser::~CXmlParser\n"));
}

//=============================================================================
void CXmlParser::DeleteContents()
//=============================================================================
{
	TRACE(_T("in CXmlParser::DeleteContents\n"));
	// clean up any previous data
	CXmlListElement *pos = m_RootElement.m_ChildElements.GetFirst();
	while (pos != NULL && !pos->IsRoot())
	{
		CXmlListElement *posnext = pos->GetNext();
		pos->DeleteRemove();
		pos = posnext;
	}

	m_pCurElement = &m_RootElement;
	m_pCurElement->m_pParentElement = NULL;
	m_RootElement.m_strName = _T("");
	m_RootElement.m_strData = _T("");
	m_RootElement.m_strAttributes = _T("");
	m_RootElement.m_strFind = _T("");
	m_RootElement.m_posFind = NULL;
}

//=============================================================================
BOOL CXmlParser::Parse(LPCTSTR lpszString)
//=============================================================================
{
	TRACE(_T("in CXmlParser::Parse\n"));
	_ASSERTE(lpszString);

	// clean previous data
	DeleteContents();

	// save string
	m_strXml = lpszString;

	// remove end-of-line and tabs
	m_strXml.Replace(_T('\n'), _T(' '));
	m_strXml.Replace(_T('\r'), _T(' '));
	m_strXml.Replace(_T('\t'), _T(' '));

	BOOL bInsideTag = FALSE;

	CString strTag = _T("");
	CString strData = _T("");
	CString strResult = _T("");

	for (int i = 0; i < m_strXml.GetLength(); i++)
    {
        TCHAR ch = m_strXml[i];
		// begin of tag ?
		if (ch == _T('<'))
        {
			strTag += ch;

			// add data to element
			m_pCurElement->m_strData = strData;
			// trim spaces
			m_pCurElement->m_strData.TrimLeft();
			m_pCurElement->m_strData.TrimRight();

			// clear data
            strData = _T("");
            
			// processing tag...
			bInsideTag = TRUE;
            continue;        
        }

		// end of tag ?
        if (ch == _T('>'))
        {
            strTag += ch;
		    strResult = _T("");	//+++

			// determine type and name of the tag
			int nType = ValidateTag(strTag, strResult);
			TRACE(_T("strResult=<%s>\n"), strResult);

			// clear tag
            strTag = _T("");

			// skip errors/comments/declaration
			if (nType == -1)
			{
				continue;
			}

			// start or start-end tag -> add new element
			if (nType == 0 || nType == 2)
			{
				// currently processing root element ?
				if (m_RootElement.m_strName.IsEmpty())
				{
					strResult.TrimLeft();
					strResult.TrimRight();
					// split name and attributes
					int nPos = strResult.Find(' ');
					if (nPos != -1)
					{
						// set properties of root element
						m_RootElement.m_strName = strResult.Left(nPos);
						m_RootElement.m_strAttributes = strResult.Mid(nPos+1);
						// trim spaces
						m_RootElement.m_strAttributes.TrimLeft();
						m_RootElement.m_strAttributes.TrimRight();
					}
					else
					{
						m_RootElement.m_strName = strResult;
					}
					TRACE(_T("m_RootElement: adding <%s> - <%s>\n"), m_RootElement.m_strName, m_RootElement.m_strAttributes);
				}
				else
				{
					// create new element
					CXmlElement *pElement = new CXmlElement;

					pElement->m_pParentElement = m_pCurElement;
					
					// split name and attributes
					int nPos = strResult.Find(' ');
					if (nPos != -1)
					{
						// set properties of current element
						pElement->m_strName = strResult.Left(nPos);
						pElement->m_strAttributes = strResult.Mid(nPos+1);
						// trim spaces
						pElement->m_strAttributes.TrimLeft();
						pElement->m_strAttributes.TrimRight();
					}
					else
					{
						pElement->m_strName = strResult;
					}
					TRACE(_T("~~~~~ adding <%s> - <%s>\n"), pElement->m_strName, pElement->m_strAttributes);
					m_pCurElement->m_ChildElements.AddTail(pElement);
					m_pCurElement = pElement;
				}
			}

			// end or start-end tag -> finished with current tag
			if (nType == 1 || nType == 2)
			{
				// go back to parent level
				if (m_pCurElement->m_pParentElement != NULL)
					m_pCurElement = m_pCurElement->m_pParentElement;
			}

			// processing data...
			bInsideTag = FALSE;
            continue;
        }
        
		if (bInsideTag)
		{
			// append character to tag
            strTag += ch;
		}
        else
        {
			// append character to data
			strData += ch;
        }
    }
	return TRUE;
}

//=============================================================================
//
// Function name : ValidateTag
// Description   : Determine type and name of the tag.
//				   0 = start tag
//				   1 = end tag
//				   2 = start-end tag
//				   -1 = comments or declaration
//
int CXmlParser::ValidateTag(CString &strTag, CString &strResult)
//=============================================================================
{
	strResult = _T("");

	if (strTag.IsEmpty())
		return -1;

    TCHAR ch;
	TCHAR chPrevious = _T('0');
	
	int nResult = 0;
	int nCount = 0;

	// determine tag type
    while (nCount <strTag.GetLength())
    {
        // get next character
		ch = strTag[nCount++];

		// skip comments '<!' and declaration '<?'
        if ((chPrevious == _T('<') && ch == _T('!')) || 
			(chPrevious == _T('<') && ch == _T('?')))
		{
            return -1;
		}
        else
		// is it an end-tag '</' ?
        if (chPrevious ==_T('<') && ch == _T('/')) 
        {
            nResult = 1;
        }
        else
		// is it a start-end-tag '<..../>' ?
        if (chPrevious ==_T('/') && ch == _T('>')) 
        {
            nResult = 2;
			// remove last character
			strResult.Delete(strResult.GetLength()-1, 1);
        }
        else 
		if (ch != _T('<') && ch != _T('>'))
		{
			// add character
            strResult += ch;
        }
        chPrevious = ch;
    }
	return nResult;
}

//=============================================================================
CXmlElement * CXmlParser::GetFirstChild(CXmlElement *pElement) 
//=============================================================================
{
	CXmlElement *pResult = NULL;
	pElement->m_posFind = NULL;
	
	CXmlListElement *pFirst = pElement->m_ChildElements.GetFirst();
	if (pFirst)
	{
		pElement->m_posFind = pFirst;
		pResult = pFirst->GetEntry();
	}
	return pResult;
}

//=============================================================================
CXmlElement * CXmlParser::GetNextSibling(CXmlElement *pElement) 
//=============================================================================
{
	CXmlElement *pResult = NULL;
	if (pElement && pElement->m_posFind)
	{
		pElement->m_posFind = pElement->m_posFind->GetNext();
		if (pElement->m_posFind)
			pResult = pElement->m_posFind->GetEntry();
	}
	return pResult;
}

//=============================================================================
CXmlElement * CXmlParser::FindElement(CXmlElement *pElement, LPCTSTR lpszName) 
//=============================================================================
{
	pElement->m_posFind = NULL;
	
	pElement->m_strFind = lpszName;
	
	CXmlListElement *pos = pElement->m_ChildElements.GetFirst();
	while (pos != NULL)
	{
		CXmlElement *pResult = pos->GetEntry();
		if (pResult->m_strName.CompareNoCase(lpszName) == 0)
		{
			pElement->m_posFind = pos;
			return pResult;
		}
		pos = pos->GetNext();
	}
	return NULL;
}

//=============================================================================
CXmlElement * CXmlParser::FindNextElement(CXmlElement *pElement) 
//=============================================================================
{
	CXmlElement *pResult = NULL;
	CXmlListElement *pos = pElement->m_posFind;
	while (pos != NULL)
	{
		pos = pos->GetNext();
		if (pos)
		{
			CXmlElement *pEntry = pos->GetEntry();
			if (pEntry && pEntry->m_strName.CompareNoCase(pElement->m_strFind) == 0)
			{
				pResult = pEntry;
				break;
			}
		}
	}
	return pResult;
}

//=============================================================================
CXmlElement * CXmlParser::AddElement(CXmlElement *pElement, 
									   LPCTSTR lpszName, 
									   LPCTSTR lpszAttributes /*= NULL*/,
									   LPCTSTR lpszData /*= NULL*/)
//=============================================================================
{
	TRACE(_T("in * CXmlParser::AddElement <%s>\n"), lpszName);

	CXmlElement *pNewElement = new CXmlElement;

	pNewElement->m_strName = lpszName;
	pNewElement->m_strName.TrimLeft();
	pNewElement->m_strName.TrimRight();

	if (lpszData)
	{
		pNewElement->m_strData = lpszData;
		pNewElement->m_strData.Replace(_T("&"), _T("&amp;"));
		pNewElement->m_strData.Replace(_T("<"), _T("&lt;"));
		pNewElement->m_strData.Replace(_T(">"), _T("&gt;"));
	}
	if (lpszAttributes)
		pNewElement->m_strAttributes = lpszAttributes;
	
	pElement->m_ChildElements.AddTail(pNewElement);

	return pNewElement;
}

//=============================================================================
CString CXmlParser::Generate(BOOL bUnicode)
//=============================================================================
{
	TRACE(_T("in CXmlParser::Generate\n"));

	//_T("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"))
	//_T("<?xml version=\"1.0\" encoding=\"UTF-16\"?>\r\n"))

	CString strResult = _T("");
	strResult.Format(_T("<?xml version=\"1.0\" encoding=\"UTF-%d\"?>\r\n"),
		bUnicode ? 16 : 8);

	CString strTag = _T("");

	m_nLevel = -1;
	CreateTag(&m_RootElement, strTag);

	strResult += strTag;
	return strResult;
}

//=============================================================================
BOOL CXmlParser::CreateTag(CXmlElement *pElement, CString& strResult)
//=============================================================================
{
	TRACE(_T("in CXmlParser::CreateTag\n"));

	// make sure we start empty
	strResult = _T("");

	_ASSERTE(pElement);

	if (!pElement || pElement->m_strName.IsEmpty())
		return FALSE;

	TRACE(_T("<%s>\n"), pElement->m_strName);

	int i = 0;

	m_nLevel++;

	// add spaces before start-tag
	for (i = 0; i < m_nLevel; i++)
		strResult += _T("\t");

	// add start-tag
	strResult += _T("<");
	strResult += pElement->m_strName;

	if (!pElement->m_strAttributes.IsEmpty())
	{
		strResult += _T(" ");
		strResult += pElement->m_strAttributes;
	}
	
	// process child elements
	CXmlListElement *pos = pElement->m_ChildElements.GetFirst();

	if ((pos && !pos->IsRoot()) || !pElement->m_strData.IsEmpty())
	{
		strResult += _T(">");

		if (!pElement->m_strData.IsEmpty())
			strResult += pElement->m_strData;
		else
			strResult += _T("\r\n");

		while (pos && !pos->IsRoot())
		{
			CXmlElement *pChildElement = pos->GetEntry();

			if (pChildElement)
			{
				CString strTag = _T("");
				CreateTag(pChildElement, strTag);
				strResult += strTag;
			}
			pos = pos->GetNext();
		}
		
		if (pElement->m_strData.IsEmpty())
		{
			// add spaces before end tag
			for (i = 0; i < m_nLevel; i++)
				strResult += _T("\t");
		}

		// add end-tag
		strResult += _T("</");
		strResult += pElement->m_strName;
		strResult += _T(">\r\n");
	}
	else	// no data, no children
	{
		strResult += _T("/>\r\n");
	}

	m_nLevel--;

	return TRUE;
}

//=============================================================================
BOOL CXmlParser::Load(LPCTSTR lpszFileName, ENCODING eEncoding /*= Default*/)
//=============================================================================
{
	TRACE(_T("in CXmlParser::Load\n"));
	BOOL rc = FALSE;

	DWORD dwDesiredAccess = GENERIC_READ;
	DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;

#ifdef _DEBUG
	TCHAR szCurDir[MAX_PATH*2];
	szCurDir[0] = _T('\0');
	::GetCurrentDirectory(sizeof(szCurDir)/sizeof(TCHAR)-2, szCurDir);
	TRACE(_T("----- cur dir = <%s>\n"), szCurDir);
#endif

	HANDLE hFile = ::CreateFile(lpszFileName, dwDesiredAccess, dwShareMode, 
		NULL, OPEN_EXISTING, dwFlagsAndAttributes, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		TRACE(_T("ERROR: CreateFile failed - %s\n"), lpszFileName);
	}
	else
	{
		DWORD dwFileSize = ::GetFileSize(hFile, NULL);
		if ((dwFileSize == 0) || (dwFileSize == INVALID_FILE_SIZE))
		{
			TRACE(_T("ERROR: GetFileSize failed - %s\n"), lpszFileName);
		}
		else
		{
			BYTE *buf = new BYTE [dwFileSize + 100];
			memset(buf, 0, dwFileSize + 100);

			DWORD dwBytesRead = 0;
			if (!::ReadFile(hFile, buf, dwFileSize, &dwBytesRead, NULL))
			{
				TRACE(_T("ERROR: ReadFile failed - %s\n"), lpszFileName);
			}
			else
			{
				BOOL bFileIsUnicode = FALSE;

				// first 2 bytes might be BOM
				WCHAR cFirstChar = 0;
				memcpy(&cFirstChar, buf, sizeof(WCHAR));

				if (eEncoding == Ansi)
				{
				}
				else if (eEncoding == Unicode)
				{
					bFileIsUnicode = TRUE;
				}
				else
				{
					// default - look for BOM
					
					// if first two bytes are Unicode byte-order marker, let's say it's Unicode
					if (cFirstChar == (wchar_t)0xFEFF)
						bFileIsUnicode = TRUE;
				}

				TRACE(_T("bFileIsUnicode=%d\n"), bFileIsUnicode);

				int index = 0;
				if (cFirstChar == (WCHAR)0xFEFF)
					index = 2;	// skip BOM

				CString strXML = _T("");
				BYTE *outbuf = &buf[index];

#ifdef _UNICODE
				if (bFileIsUnicode)
				{
					strXML = (LPCTSTR) outbuf;
				}
				else
				{
					TRACE(_T("converting to unicode\n"));
					int wlen = MultiByteToWideChar(CP_ACP, 0, 
								(LPCSTR)outbuf, -1, NULL, 0);
					TRACE(_T("wlen=%d\n"), wlen);
					WCHAR *pszText = new WCHAR [wlen+16];
					memset(pszText, 0, (wlen+16)*sizeof(WCHAR));
					MultiByteToWideChar(CP_ACP, 0, (LPCSTR)outbuf, 
						-1, pszText, wlen+2);
					strXML = pszText;
					delete [] pszText;
				}
#else
				if (bFileIsUnicode)
				{
					TRACE(_T("converting to ansi\n"));
					LPCWSTR wp = (LPCWSTR) outbuf;
					int alen = WideCharToMultiByte(CP_ACP, 0, wp, -1,
									NULL, 0, NULL, NULL);
					TRACE(_T("alen=%d\n"), alen);
					char *pszText = new char [alen+16];
					memset(pszText, 0, (alen+16)*sizeof(char));
					WideCharToMultiByte(CP_ACP, 0, wp, -1, 
						pszText, alen+1, NULL, NULL);
					strXML = pszText;
					delete [] pszText;
				}
				else
				{
					strXML = (LPCTSTR) outbuf;
				}
#endif
				CloseHandle(hFile);

				if (!strXML.IsEmpty())
				{
					rc = Parse(strXML);
					TRACE(_T("Parse returned %d\n"), rc);
				}
			}

			if (buf)
				delete [] buf;
		}
	}

	return rc;
}

//=============================================================================
BOOL CXmlParser::Save(LPCTSTR lpszFileName, ENCODING eEncoding /*= Default*/)
//=============================================================================
{
	TRACE(_T("in CXmlParser::Save\n"));

	BOOL bFileIsUnicode = FALSE;

	if (eEncoding == Ansi)
	{
	}
	else if (eEncoding == Unicode)
	{
		bFileIsUnicode = TRUE;
	}
	else
	{
#ifdef _UNICODE
		bFileIsUnicode = TRUE;
#endif
	}

	TRACE(_T("bFileIsUnicode=%d\n"), bFileIsUnicode);

	CString strXML = Generate(bFileIsUnicode);
	TRACE(_T("strXML=<%s>\n"), strXML);

	int len = strXML.GetLength();			// number of TCHARs
	int maxlen = len * sizeof(WCHAR);		// max number of bytes
	BYTE *buf = new BYTE [maxlen + 100];
	memset(buf, 0, maxlen + 100);

	int index = 0;

	if (bFileIsUnicode)
	{
		buf[0] = 0xFF;
		buf[1] = 0xFE;
		index = 2;
	}

	BYTE *outbuf = &buf[index];

	int nBytes = len;

#ifdef _UNICODE
	if (bFileIsUnicode)
	{
		_tcscpy((LPTSTR)outbuf, strXML);
		nBytes = maxlen + 2;	// include BOM
	}
	else
	{
		TRACE(_T("converting to ansi\n"));
		WideCharToMultiByte(CP_ACP, 0, strXML, -1, 
			(LPSTR)outbuf, len+16, NULL, NULL);
	}
#else
	if (bFileIsUnicode)
	{
		TRACE(_T("converting to unicode\n"));
		MultiByteToWideChar(CP_ACP, 0, strXML, 
			-1, (LPWSTR)outbuf, maxlen+16);
		nBytes = maxlen + 2;	// include BOM
	}
	else
	{
		_tcscpy((LPTSTR)outbuf, strXML);
	}
#endif

	DWORD dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
	DWORD dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;

#ifdef _DEBUG
	TCHAR szCurDir[MAX_PATH*2];
	szCurDir[0] = _T('\0');
	::GetCurrentDirectory(sizeof(szCurDir)/sizeof(TCHAR)-2, szCurDir);
	TRACE(_T("----- cur dir = <%s>\n"), szCurDir);
#endif

	HANDLE hFile = ::CreateFile(lpszFileName, dwDesiredAccess, dwShareMode, 
		NULL, CREATE_ALWAYS, dwFlagsAndAttributes, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		TRACE(_T("ERROR: CreateFile failed - %s\n"), lpszFileName);
	}
	else
	{
		DWORD dwBytesWritten = 0;
		::WriteFile(hFile, buf, nBytes, &dwBytesWritten, NULL);
		CloseHandle(hFile);
	}

	if (buf)
		delete [] buf;

	return TRUE;
}
