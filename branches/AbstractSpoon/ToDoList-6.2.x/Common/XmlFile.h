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
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-switches
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// XmlFile.h: interface for the CXmlFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XMLFILE_H__54045703_1A98_41AB_B1A1_0B165C350AA0__INCLUDED_)
#define AFX_XMLFILE_H__54045703_1A98_41AB_B1A1_0B165C350AA0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include <../src/mfc/fixalloc.h>

// global fns for translating text to/from xml representations eg '&lt;' becomes '<' and vice versa
CString& XML2TXT(CString& xml);
CString& TXT2XML(CString& txt);
CString& TXT2HTML(CString& txt);

class CXmlDocumentWrapper;
class CXmlNodeWrapper;

enum XI_TYPE
{
	XIT_ELEMENT,
	XIT_ATTRIB,
	XIT_CDATA,
};

class CXmlItem
{
public:
	CXmlItem(CXmlItem* pParent = NULL, LPCTSTR szName = NULL, LPCTSTR szValue = NULL, XI_TYPE nType = XIT_ELEMENT);
	CXmlItem(const CXmlItem& xi, CXmlItem* pParent = NULL); // copies siblings too
	virtual ~CXmlItem();

	void Copy(const CXmlItem& xi, BOOL bCopySiblings);
	void Reset();

	const CXmlItem* GetItem(LPCTSTR szItemName, LPCTSTR szSubItemName = NULL) const;
	CXmlItem* GetItem(LPCTSTR szItemName, LPCTSTR szSubItemName = NULL);
	BOOL HasItem(LPCTSTR szItemName, LPCTSTR szSubItemName = NULL) const;

	const CXmlItem* FindItem(LPCTSTR szItemName, LPCTSTR szItemValue, BOOL bSearchChildren = TRUE) const;
	const CXmlItem* FindItem(LPCTSTR szItemName, int nItemValue, BOOL bSearchChildren = TRUE) const;
	const CXmlItem* FindItem(LPCTSTR szItemName, double dItemValue, BOOL bSearchChildren = TRUE) const;
	CXmlItem* FindItem(LPCTSTR szItemName, LPCTSTR szItemValue, BOOL bSearchChildren = TRUE);
	CXmlItem* FindItem(LPCTSTR szItemName, int nItemValue, BOOL bSearchChildren = TRUE);
	CXmlItem* FindItem(LPCTSTR szItemName, double dItemValue, BOOL bSearchChildren = TRUE);

	BOOL NameIs(LPCTSTR szName) const;
	CString GetName() const;
	CString GetValue() const;
	int GetNameLen() const;
	int GetValueLen() const;

	CString GetItemValue(LPCTSTR szItemName, LPCTSTR szSubItemName = NULL) const;
	int GetItemCount() const;

	// returns the number of subitems having this name ie inc. siblings
	int GetItemCount(LPCTSTR szItemName) const;

	int GetValueI() const;
	int GetItemValueI(LPCTSTR szItemName, LPCTSTR szSubItemName = NULL) const;

	double GetValueF() const;
	double GetItemValueF(LPCTSTR szItemName, LPCTSTR szSubItemName = NULL) const;

	// returns the new item
	CXmlItem* AddItem(LPCTSTR szName, LPCTSTR sValue = NULL, XI_TYPE nType = XIT_ATTRIB);
	CXmlItem* AddItem(LPCTSTR szName, int nValue, XI_TYPE nType = XIT_ATTRIB);
	CXmlItem* AddItem(LPCTSTR szName, double dValue, XI_TYPE nType = XIT_ATTRIB);
	CXmlItem* AddItem(const CXmlItem& xi, BOOL bCopySiblings); // item and all attributes are copied
	CXmlItem* AddItem(CXmlItem* pXI);

	// returns the new or existing item
	CXmlItem* SetItemValue(LPCTSTR szName, LPCTSTR sValue, XI_TYPE nType = XIT_ATTRIB);
	CXmlItem* SetItemValue(LPCTSTR szName, int nValue, XI_TYPE nType = XIT_ATTRIB);
	CXmlItem* SetItemValue(LPCTSTR szName, double dValue, XI_TYPE nType = XIT_ATTRIB);

	BOOL SetName(LPCTSTR sName); // can't have any siblings
	void SetValue(LPCTSTR sValue);
	void SetValue(int nValue);
	void SetValue(double dValue);

	BOOL RemoveItem(CXmlItem* pXIChild); // must be a direct child (does not destroy item)
	BOOL DeleteItem(CXmlItem* pXIChild); // must be a direct child
	BOOL DeleteItem(LPCTSTR szItemName); // must be a direct child
	void DeleteAllItems();

	const CXmlItem* GetParent() const;
	const CXmlItem* GetSibling() const;
	CXmlItem* GetParent();
	CXmlItem* GetSibling();

	BOOL IsAttribute(int nMaxAttribLen = 8192) const;
	BOOL IsCDATA() const;
	BOOL SetType(XI_TYPE nType);
	XI_TYPE GetType() const
	{
		return m_nType;
	}

	POSITION GetFirstItemPos() const;
	const CXmlItem* GetNextItem(POSITION& pos) const;
	CXmlItem* GetNextItem(POSITION& pos);

	// matching helpers
	BOOL NameMatches(LPCTSTR szName) const;
	BOOL NameMatches(const CXmlItem* pXITest) const;
	BOOL ValueMatches(LPCTSTR szValue, BOOL bIgnoreCase = TRUE) const;
	BOOL ValueMatches(const CXmlItem* pXITest, BOOL bIgnoreCase = TRUE) const;
	BOOL ItemValueMatches(const CXmlItem* pXITest, LPCTSTR szItemName, BOOL bIgnoreCase = TRUE) const;

	// sorting
	void SortItems(LPCTSTR szItemName, LPCTSTR szKeyName, BOOL bAscending = TRUE); // sort by string value
	void SortItemsI(LPCTSTR szItemName, LPCTSTR szKeyName, BOOL bAscending = TRUE); // sort by int value
	void SortItemsF(LPCTSTR szItemName, LPCTSTR szKeyName, BOOL bAscending = TRUE); // sort by float value

	static void ValidateString(CString& sText, char cReplace = ' ');

protected:
	CString m_sName;
	CString m_sValue;
	XI_TYPE m_nType;

	CXmlItem* m_pParent;
	CXmlItem* m_pSibling;

	CTypedPtrList<CPtrList, CXmlItem*> m_lstItems; // children

protected:
	BOOL AddSibling(CXmlItem* pXI); // must share the same name and parent
	const CXmlItem* GetItemEx(LPCTSTR szItemName, LPCTSTR szSubItemName = NULL) const;
	const CXmlItem* FindItemEx(LPCTSTR szItemName, LPCTSTR szItemValue, BOOL bSearchChildren = TRUE) const;

	enum XI_SORTKEY { XISK_STRING, XISK_INT, XISK_FLOAT };
	void SortItems(LPCTSTR szItemName, LPCTSTR szKeyName, XI_SORTKEY nKey, BOOL bAscending);

	static int CompareItems(const CXmlItem* pXIItem1, const CXmlItem* pXIItem2, LPCTSTR szKeyName, XI_SORTKEY nKey);

	static CString ToString(int nValue);
	static CString ToString(double dValue);

// C4995: name was marked as #pragma deprecated
#pragma warning(disable: 4995)
	DECLARE_FIXED_ALLOC(CXmlItem);
#pragma warning(default: 4995)
};

class IXmlParse
{
public:
	// return TRUE to continue parsing
	virtual BOOL Continue(LPCTSTR szItem, LPCTSTR szValue) const = 0;
};

enum XF_OPEN
{
	XF_READ,
	XF_WRITE,
	XF_READWRITE,
};

enum // load errors. if >0 then == GetLastFileError
{
	XFL_NONE        = 0,
	XFL_CANCELLED   = -1,
	XFL_MISSINGROOT = -2,
	XFL_BADMSXML    = -3,
	XFL_LAST        = -99
};

struct XMLHEADER
{
};

class CXmlFile : protected CFile
{
public:
	CXmlFile(LPCTSTR szRootItemName = NULL);
	virtual ~CXmlFile();

	BOOL Load(LPCTSTR szFilePath, LPCTSTR szRootItemName = NULL, IXmlParse* pCallback = NULL);
	BOOL Save(LPCTSTR szFilePath);

	void Copy(const CXmlFile& file);
	void Reset();
	int GetItemCount() const;

	// extended interface
	BOOL Open(LPCTSTR szFilePath, XF_OPEN nOpenFlags);
	virtual BOOL SaveEx();
	virtual BOOL LoadEx(LPCTSTR szRootItemName = NULL, IXmlParse* pCallback = NULL);
	void Close();
	int GetLastFileError();

	BOOL Export(CString& sOutput) const;
	BOOL Transform(LPCTSTR szTransformPath, CString& sOutput, const CString& sEncoding = _T("")) const;
	BOOL TransformToFile(LPCTSTR szTransformPath, LPCTSTR szOutputPath, const CString& sEncoding = _T("")) const;

	const CXmlItem* Root() const;
	CXmlItem* Root();

	const CXmlItem* GetItem(LPCTSTR szItemName) const;
	CXmlItem* GetItem(LPCTSTR szItemName);

	const CXmlItem* FindItem(LPCTSTR szItemName, LPCTSTR szItemValue, BOOL bSearchChildren = TRUE) const;
	const CXmlItem* FindItem(LPCTSTR szItemName, int nItemValue, BOOL bSearchChildren = TRUE) const;
	const CXmlItem* FindItem(LPCTSTR szItemName, double dItemValue, BOOL bSearchChildren = TRUE) const;

	CXmlItem* FindItem(LPCTSTR szItemName, LPCTSTR szItemValue, BOOL bSearchChildren = TRUE);
	CXmlItem* FindItem(LPCTSTR szItemName, int nItemValue, BOOL bSearchChildren = TRUE);
	CXmlItem* FindItem(LPCTSTR szItemName, double dItemValue, BOOL bSearchChildren = TRUE);

	CXmlItem* AddItem(LPCTSTR szName, LPCTSTR szValue = _T(""), XI_TYPE nType = XIT_ATTRIB);
	CXmlItem* AddItem(LPCTSTR szName, int nValue, XI_TYPE nType = XIT_ATTRIB);
	CXmlItem* AddItem(LPCTSTR szName, double dValue, XI_TYPE nType = XIT_ATTRIB);

	BOOL DeleteItem(CXmlItem* pXI);
	BOOL DeleteItem(LPCTSTR szItemName);

	CXmlItem* SetItemValue(LPCTSTR szName, LPCTSTR sValue, XI_TYPE nType = XIT_ATTRIB);
	CXmlItem* SetItemValue(LPCTSTR szName, int nValue, XI_TYPE nType = XIT_ATTRIB);
	CXmlItem* SetItemValue(LPCTSTR szName, double dValue, XI_TYPE nType = XIT_ATTRIB);

	CString GetItemValue(LPCTSTR szItemName, LPCTSTR szSubItemName = NULL) const;
	int GetItemValueI(LPCTSTR szItemName, LPCTSTR szSubItemName = NULL) const;
	double GetItemValueF(LPCTSTR szItemName, LPCTSTR szSubItemName = NULL) const;

	CString GetFilePath() const;
	CString GetFileName() const;
	const HANDLE GetFileHandle() const;

	CString GetHeader() const;
	void SetHeader(LPCTSTR szHeader);

	void Trace() const;

	// sorting
	void SortItems(LPCTSTR szItemName, LPCTSTR szKeyName, BOOL bAscending = TRUE);
	void SortItemsI(LPCTSTR szItemName, LPCTSTR szKeyName, BOOL bAscending = TRUE);
	void SortItemsF(LPCTSTR szItemName, LPCTSTR szKeyName, BOOL bAscending = TRUE);


protected:
	CXmlItem m_xiRoot;
	CString m_sHeader;
	IXmlParse* m_pCallback;
	mutable int m_nFileError;

protected:
	BOOL ParseItem(CXmlItem& xi, CXmlNodeWrapper* pNode); // returns false if the callback stopped it
	BOOL ParseRootItem(LPCTSTR szRootItemName, CXmlDocumentWrapper* pDoc);
	BOOL ContinueParsing(LPCTSTR szItem, LPCTSTR szValue);
	int GetItemType(const CString& sItem);

	static int StrCopy(CString& sDest, LPCTSTR szSrc, int nCount = -1);
	static int StrAppend(LPTSTR& szDest, LPCTSTR szSrc, int nCount = -1);

	BOOL BuildDOM(CXmlDocumentWrapper* pDoc) const;
	int Export(const CXmlItem* pItem, CXmlNodeWrapper* pNode) const;

	static void FixInputString(CString& sXml, LPCTSTR szRootItem);
};

#endif // !defined(AFX_XMLFILE_H__54045703_1A98_41AB_B1A1_0B165C350AA0__INCLUDED_)
