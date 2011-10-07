// XmlParser.h: interface for the CXmlParser class.

#ifndef XMLPARSER_H
#define XMLPARSER_H

#include "CLinkedList.h"

class CXmlElement;
typedef CLinkedList<CXmlElement> CXmlList;
typedef CLinkedListElement<CXmlElement> CXmlListElement;

//=============================================================================
class CXmlElement
//=============================================================================
{
public:
	CXmlElement() 
	{
		TRACE(_T("in CXmlElement\n"));
		m_posFind = NULL;
		m_pParentElement = NULL;
	};

	virtual ~CXmlElement() 
	{
		TRACE(_T("in ~CXmlElement <%s>\n"), m_strName);
		CXmlListElement *pos = m_ChildElements.GetFirst();
		while (pos != NULL && !pos->IsRoot())
		{
			CXmlListElement *posnext = pos->GetNext();
			pos->DeleteRemove();
			pos = posnext;
		}
	};

	CString GetValue(LPCTSTR attribute);

	CString			m_strName;
	CString			m_strAttributes;
	CString			m_strData;
	CString			m_strFind;

	CXmlList			m_ChildElements;
	CXmlListElement *	m_posFind;
	CXmlElement *		m_pParentElement;
};

//=============================================================================
class CXmlParser
//=============================================================================
{
public:
	CXmlParser();
	virtual ~CXmlParser();

	BOOL Parse(LPCTSTR lpszString);

	// must be the same as CXmlProfile
	enum ENCODING { Default = 0, Ansi, Unicode };

	BOOL Load(LPCTSTR lpszFileName, ENCODING eEncoding = Default);
	BOOL Save(LPCTSTR lpszFileName, ENCODING eEncoding = Default);
	CXmlElement *GetFirstChild(CXmlElement *pElement);
	CXmlElement *GetRootElement() { return &m_RootElement; }
	CXmlElement * CXmlParser::GetNextSibling(CXmlElement *pElement);
	
	CXmlElement *FindElement(CXmlElement *pElement, LPCTSTR lpszName);
	CXmlElement *FindNextElement(CXmlElement *pElement);

	CXmlElement *AddElement(CXmlElement *pElement, 
							LPCTSTR lpszName, 
							LPCTSTR lpszAttributes = NULL,
							LPCTSTR lpszData = NULL);

	void DeleteContents();

protected:
	int ValidateTag(CString& strTag, CString& strResult);
	BOOL CreateTag(CXmlElement *pElement, CString& strTag);
	CString Generate(BOOL bUnicode);

	int				m_nLevel;
	CString		m_strXml;
	CXmlElement		m_RootElement;
	CXmlElement *	m_pCurElement;
};

#endif //XMLPARSER_H
