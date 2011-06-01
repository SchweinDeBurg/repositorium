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
// - added AbstractSpoon Software copyright notice and license information
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

// XmlFile.cpp: implementation of the CXmlFile class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "XmlFile.h"
#include "../../CodeProject/Source/Misc.h"
#include "../../CodeProject/Source/FileMisc.h"

#include "XmlNodeWrapper.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

#include "XmlCharMap.h"
#include "HtmlCharMap.h"

#define TOBSTRING(szValue) CXmlNodeWrapper::ConvertStringToBSTR(szValue)

CString& XML2TXT(CString& xml)
{
	return CHtmlCharMap::ConvertFromRep(xml);   // we use the html map for backwards compatibility
}
CString& TXT2XML(CString& txt)
{
	return CXmlCharMap::ConvertToRep(txt);
}
CString& TXT2HTML(CString& txt)
{
	return CHtmlCharMap::ConvertToRep(txt);
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// C4995: name was marked as #pragma deprecated
#pragma warning(disable: 4995)
IMPLEMENT_FIXED_ALLOC(CXmlItem, 1024);
#pragma warning(default: 4995)

CXmlItem::CXmlItem(CXmlItem* pParent, LPCTSTR szName, LPCTSTR szValue, XI_TYPE nType) :
	m_pParent(pParent), m_pSibling(NULL), m_sName(szName), m_sValue(szValue), m_nType(nType)
{
	ValidateString(m_sValue);
}

CXmlItem::CXmlItem(const CXmlItem& xi, CXmlItem* pParent) :
	m_pParent(pParent), m_pSibling(NULL), m_sName(xi.m_sName), m_sValue(xi.m_sValue), m_nType(xi.m_nType)
{
	Copy(xi, TRUE);
}

CXmlItem::~CXmlItem()
{
	Reset();
}

void CXmlItem::Copy(const CXmlItem& xi, BOOL bCopySiblings)
{
	Reset();

	// copy own name and value
	m_sName = xi.GetName();
	m_sValue = xi.GetValue();
	m_nType = xi.GetType();

	// copy siblings
	if (bCopySiblings)
	{
		const CXmlItem* pXISibling = xi.GetSibling();

		if (pXISibling)
		{
			m_pSibling = new CXmlItem(*pXISibling, m_pParent);
		}
	}

	// copy children
	POSITION pos = xi.GetFirstItemPos();

	while (pos)
	{
		const CXmlItem* pXIChild = xi.GetNextItem(pos);
		ASSERT(pXIChild);

		AddItem(*pXIChild, TRUE);
	}
}

void CXmlItem::Reset()
{
	// delete children
	POSITION pos = m_lstItems.GetHeadPosition();

	while (pos)
	{
		delete m_lstItems.GetNext(pos);
	}

	m_lstItems.RemoveAll();

	// and siblings
	// note: because sibling ~tor calls its own Reset() the chain
	// of siblings will be correctly cleaned up
	delete m_pSibling;
	m_pSibling = NULL;
	m_nType = XIT_ELEMENT;
}

const CXmlItem* CXmlItem::GetItem(LPCTSTR szItemName, LPCTSTR szSubItem) const
{
	return GetItemEx(szItemName, szSubItem);
}

CXmlItem* CXmlItem::GetItem(LPCTSTR szItemName, LPCTSTR szSubItem)
{
	// cast away constness
	return (CXmlItem*)GetItemEx(szItemName, szSubItem);
}

BOOL CXmlItem::HasItem(LPCTSTR szItemName, LPCTSTR szSubItemName) const
{
	return (GetItem(szItemName, szSubItemName) != NULL);
}

// special internal version
const CXmlItem* CXmlItem::GetItemEx(LPCTSTR szItemName, LPCTSTR szSubItem) const
{
	POSITION pos = m_lstItems.GetHeadPosition();

	while (pos)
	{
		// match on item name first
		const CXmlItem* pXI = m_lstItems.GetNext(pos);

		if (pXI->NameMatches(szItemName))
		{
			// then subitem
			if (szSubItem && *szSubItem)
			{
				return pXI->GetItemEx(szSubItem);
			}

			// else
			return pXI;
		}
	}

	// not found
	return NULL;
}

const CXmlItem* CXmlItem::FindItem(LPCTSTR szItemName, LPCTSTR szItemValue, BOOL bSearchChildren) const
{
	return FindItemEx(szItemName, szItemValue, bSearchChildren);
}

CXmlItem* CXmlItem::FindItem(LPCTSTR szItemName, LPCTSTR szItemValue, BOOL bSearchChildren)
{
	// cast away constness
	return (CXmlItem*)FindItemEx(szItemName, szItemValue, bSearchChildren);
}

const CXmlItem* CXmlItem::FindItem(LPCTSTR szItemName, int nItemValue, BOOL bSearchChildren) const
{
	return FindItemEx(szItemName, ToString(nItemValue), bSearchChildren);
}

const CXmlItem* CXmlItem::FindItem(LPCTSTR szItemName, double dItemValue, BOOL bSearchChildren) const
{
	return FindItemEx(szItemName, ToString(dItemValue), bSearchChildren);
}

CXmlItem* CXmlItem::FindItem(LPCTSTR szItemName, int nItemValue, BOOL bSearchChildren)
{
	// cast away constness
	return (CXmlItem*)FindItemEx(szItemName, ToString(nItemValue), bSearchChildren);
}

CXmlItem* CXmlItem::FindItem(LPCTSTR szItemName, double dItemValue, BOOL bSearchChildren)
{
	// cast away constness
	return (CXmlItem*)FindItemEx(szItemName, ToString(dItemValue), bSearchChildren);
}

// special internal version
const CXmlItem* CXmlItem::FindItemEx(LPCTSTR szItemName, LPCTSTR szItemValue, BOOL bSearchChildren) const
{
	// check our name and value
	if (m_sName.CompareNoCase(szItemName) == 0 && m_sValue.Compare(szItemValue) == 0)
	{
		return this;
	}

	const CXmlItem* pFound = NULL;

	// search all our children
	if (bSearchChildren)
	{
		POSITION pos = GetFirstItemPos();

		while (pos && !pFound)
		{
			const CXmlItem* pXIChild = GetNextItem(pos);
			ASSERT(pXIChild);

			pFound = pXIChild->FindItemEx(szItemName, szItemValue, TRUE); // child will search its own siblings
		}
	}

	// then our siblings
	if (!pFound)
	{
		// only get first sibling as each sibling does its own
		const CXmlItem* pXISibling = GetSibling();

		if (pXISibling)
		{
			pFound = pXISibling->FindItemEx(szItemName, szItemValue, TRUE);
		}
	}

	return pFound;
}

CString CXmlItem::GetItemValue(LPCTSTR szItemName, LPCTSTR szSubItem) const
{
	const CXmlItem* pXI = GetItem(szItemName, szSubItem);

	if (pXI)
	{
		return pXI->GetValue();
	}

	// else
	return _T("");
}

int CXmlItem::GetItemCount(LPCTSTR szItemName) const
{
	int nCount = 0;
	const CXmlItem* pXI = GetItem(szItemName);

	while (pXI)
	{
		nCount++;
		pXI = pXI->GetSibling();
	}

	return nCount;
}

CXmlItem* CXmlItem::AddItem(LPCTSTR szName, LPCTSTR szValue, XI_TYPE nType)
{
	ASSERT(szName && *szName);

	if (!(szName && *szName))
	{
		return NULL;
	}

	CXmlItem* pXI = new CXmlItem(this, szName, szValue, nType);

	return AddItem(pXI);
}

CXmlItem* CXmlItem::AddItem(const CXmlItem& xi, BOOL bCopySiblings)
{
	CXmlItem* pXI = new CXmlItem(this);
	pXI->Copy(xi, bCopySiblings);

	return AddItem(pXI);
}

CXmlItem* CXmlItem::AddItem(CXmlItem* pXI)
{
	CXmlItem* pXIParent = pXI->GetParent();

	if (pXIParent && pXIParent != this)
	{
		pXIParent->RemoveItem(pXI);
	}

	pXI->m_pParent = this;

	// if an item of the same name already exists then add this
	// item as a sibling to the existing item else its a new item
	// so add and map name to this object
	CXmlItem* pXPExist = GetItem(pXI->GetName());

	if (pXPExist)
	{
		pXPExist->AddSibling(pXI);
	}
	else
	{
		m_lstItems.AddTail(pXI);
	}

	return pXI;
}

CXmlItem* CXmlItem::AddItem(LPCTSTR szName, int nValue, XI_TYPE nType)
{
	ASSERT(nType != XIT_CDATA);
	return AddItem(szName, ToString(nValue), nType);
}

CXmlItem* CXmlItem::AddItem(LPCTSTR szName, double dValue, XI_TYPE nType)
{
	ASSERT(nType != XIT_CDATA);
	return AddItem(szName, ToString(dValue), nType);
}

CXmlItem* CXmlItem::SetItemValue(LPCTSTR szName, LPCTSTR sValue, XI_TYPE nType)
{
	CXmlItem* pXI = GetItem(szName);

	if (!pXI)
	{
		return AddItem(szName, sValue, nType);
	}

	// else already exists
	pXI->SetValue(sValue);
	pXI->SetType(nType);

	return pXI;
}

CXmlItem* CXmlItem::SetItemValue(LPCTSTR szName, int nValue, XI_TYPE nType)
{
	ASSERT(nType != XIT_CDATA);
	return SetItemValue(szName, ToString(nValue), nType);
}

CXmlItem* CXmlItem::SetItemValue(LPCTSTR szName, double dValue, XI_TYPE nType)
{
	ASSERT(nType != XIT_CDATA);
	return SetItemValue(szName, ToString(dValue), nType);
}

BOOL CXmlItem::SetName(LPCTSTR szName)
{
	// can't have any siblings
	if (!szName || !(*szName) || GetSibling())
	{
		return FALSE;
	}

	m_sName = szName;
	return TRUE;
}

void CXmlItem::SetValue(LPCTSTR szValue)
{
	m_sValue = szValue;
	ValidateString(m_sValue);
}

void CXmlItem::SetValue(int nValue)
{
	m_sValue = ToString(nValue);
}

void CXmlItem::SetValue(double dValue)
{
	m_sValue = ToString(dValue);
}

BOOL CXmlItem::RemoveItem(CXmlItem* pXI)
{
	if (!pXI)
	{
		return FALSE;
	}

	// lookup by name first
	LPCTSTR szName = pXI->GetName();
	CXmlItem* pXIMatch = GetItem(szName);

	if (!pXIMatch)
	{
		return FALSE;
	}

	// now search the sibling chain looking for exact match
	CXmlItem* pXIPrevSibling = NULL;

	while (pXIMatch != pXI)
	{
		pXIPrevSibling = pXIMatch;
		pXIMatch = pXIMatch->GetSibling();
	}

	if (!pXIMatch) // no match
	{
		return FALSE;
	}

	// else
	ASSERT(pXIMatch == pXI);

	CXmlItem* pNextSibling = pXI->GetSibling();

	if (!pXIPrevSibling) // head of the chain
	{
		POSITION pos = m_lstItems.Find(pXI);

		if (!pNextSibling)
		{
			m_lstItems.RemoveAt(pos);
		}
		else
		{
			m_lstItems.SetAt(pos, pNextSibling);
		}
	}
	else // somewhere else in the chain
	{
		pXIPrevSibling->m_pSibling = pNextSibling; // can be NULL
	}

	// clear item's sibling
	pXI->m_pSibling = NULL;

	// and parent
	pXI->m_pParent = NULL;

	return TRUE;
}

BOOL CXmlItem::DeleteItem(CXmlItem* pXI)
{
	if (RemoveItem(pXI))
	{
		delete pXI;
		return TRUE;
	}

	return FALSE;
}

BOOL CXmlItem::DeleteItem(LPCTSTR szItemName)
{
	CXmlItem* pXI = GetItem(szItemName);

	if (pXI)
	{
		pXI->Reset(); // delete children and siblings
		DeleteItem(pXI); // delete pXI
	}

	return TRUE;
}

BOOL CXmlItem::AddSibling(CXmlItem* pXI)
{
	ASSERT(pXI);

	if (!pXI)
	{
		return FALSE;
	}

	// must share the same name and parent
	ASSERT(m_sName.CompareNoCase(pXI->GetName()) == 0 && m_pParent == pXI->GetParent());

	if (!(m_sName.CompareNoCase(pXI->GetName()) == 0 && m_pParent == pXI->GetParent()))
	{
		return FALSE;
	}

	if (!m_pSibling)
	{
		m_pSibling = pXI;
	}
	else
	{
		m_pSibling->AddSibling(pXI);   // recursive
	}

	return TRUE;
}

POSITION CXmlItem::GetFirstItemPos() const
{
	return m_lstItems.GetHeadPosition();
}

const CXmlItem* CXmlItem::GetNextItem(POSITION& pos) const
{
	if (!pos)
	{
		return NULL;
	}

	// else
	return m_lstItems.GetNext(pos);
}

CXmlItem* CXmlItem::GetNextItem(POSITION& pos)
{
	if (!pos)
	{
		return NULL;
	}

	// else
	return m_lstItems.GetNext(pos);
}

BOOL CXmlItem::NameMatches(const CXmlItem* pXITest) const
{
	if (!pXITest)
	{
		return FALSE;
	}

	return NameMatches(pXITest->GetName());
}

BOOL CXmlItem::NameMatches(LPCTSTR szName) const
{
	return (m_sName.CompareNoCase(szName) == 0);
}

BOOL CXmlItem::ValueMatches(const CXmlItem* pXITest, BOOL bIgnoreCase) const
{
	if (!pXITest)
	{
		return FALSE;
	}

	// else
	return ValueMatches(pXITest->GetValue(), bIgnoreCase);
}

BOOL CXmlItem::ValueMatches(LPCTSTR szValue, BOOL bIgnoreCase) const
{
	if (bIgnoreCase)
	{
		return (m_sValue.CompareNoCase(szValue) == 0);
	}

	// else
	return (m_sValue == szValue);
}

BOOL CXmlItem::ItemValueMatches(const CXmlItem* pXITest, LPCTSTR szItemName, BOOL bIgnoreCase) const
{
	if (!szItemName || !*szItemName)
	{
		return FALSE;
	}

	const CXmlItem* pXIItem = GetItem(szItemName);
	const CXmlItem* pXITestItem = pXITest->GetItem(szItemName);

	if (pXIItem && pXITestItem)
	{
		return pXIItem->ValueMatches(pXITestItem, bIgnoreCase);
	}

	// else
	return FALSE;
}

void CXmlItem::SortItems(LPCTSTR szItemName, LPCTSTR szKeyName, BOOL bAscending)
{
	SortItems(szItemName, szKeyName, XISK_STRING, bAscending);
}

void CXmlItem::SortItemsI(LPCTSTR szItemName, LPCTSTR szKeyName, BOOL bAscending)
{
	SortItems(szItemName, szKeyName, XISK_INT, bAscending);
}

void CXmlItem::SortItemsF(LPCTSTR szItemName, LPCTSTR szKeyName, BOOL bAscending)
{
	SortItems(szItemName, szKeyName, XISK_FLOAT, bAscending);
}

void CXmlItem::SortItems(LPCTSTR szItemName, LPCTSTR szKeyName, XI_SORTKEY nKey, BOOL bAscending)
{
	if (!szItemName || !szKeyName)
	{
		return;
	}

	// 1. sort immediate children first
	CXmlItem* pXIItem = GetItem(szItemName);

	if (!pXIItem)
	{
		return;
	}

	// make sure item has key value
	if (!pXIItem->GetItem(szKeyName))
	{
		return;
	}

	// make sure at least one sibling exists
	BOOL bContinue = (pXIItem->GetSibling() != NULL);

	while (bContinue)
	{
		CXmlItem* pXIPrev = NULL;
		CXmlItem* pXISibling = NULL;

		// get this again because we have to anyway
		// for subsequent loops
		pXIItem = GetItem(szItemName);
		POSITION pos = m_lstItems.Find(pXIItem);

		// reset continue flag so that if there are no
		// switches then the sorting is done
		bContinue = FALSE;
		pXISibling = pXIItem->GetSibling();

		while (pXISibling)
		{
			int nCompare = CompareItems(pXIItem, pXISibling, szKeyName, nKey);

			if (!bAscending)
			{
				nCompare = -nCompare;
			}

			if (nCompare > 0)
			{
				// switch items
				if (pXIPrev)
				{
					pXIPrev->m_pSibling = pXISibling;
				}

				else // we're at the head of the chain
				{
					m_lstItems.SetAt(pos, pXISibling);
					//	m_mapItems[szItemName] = pXISibling;
				}

				pXIItem->m_pSibling = pXISibling->m_pSibling;
				pXISibling->m_pSibling = pXIItem;
				pXIPrev = pXISibling;

				bContinue = TRUE; // loop once more
			}
			else
			{
				pXIPrev = pXIItem;
				pXIItem = pXISibling;
			}

			pXISibling = pXIItem->GetSibling(); // next
		}
	}

	// 2. sort children's children
	pXIItem = GetItem(szItemName);

	while (pXIItem)
	{
		pXIItem->SortItems(szItemName, szKeyName, nKey, bAscending);
		pXIItem = pXIItem->GetSibling();
	}
}

int CXmlItem::CompareItems(const CXmlItem* pXIItem1, const CXmlItem* pXIItem2,
	LPCTSTR szKeyName, XI_SORTKEY nKey)
{
	LPCTSTR szValue1 = pXIItem1->GetItemValue(szKeyName);
	LPCTSTR szValue2 = pXIItem2->GetItemValue(szKeyName);

	double dDiff = 0;

	switch (nKey)
	{
	case XISK_STRING:
		dDiff = (double)CString(szValue1).CompareNoCase(szValue2);
		break;

	case XISK_INT:
		dDiff = (double)(_ttoi(szValue1) - _ttoi(szValue2));
		break;

	case XISK_FLOAT:
		dDiff = _tstof(szValue1) - _tstof(szValue2);
		break;
	}

	return (dDiff < 0) ? -1 : ((dDiff > 0) ? 1 : 0);
}

void CXmlItem::ValidateString(CString& sText, char cReplace)
{
	// remove nasties that XML does not like
	int nLen = sText.GetLength();

	for (int nChar = 0; nChar < nLen; nChar++)
	{
		UINT c = sText[nChar];

		// some specific chars we don't like
		switch (c)
		{
		case 0x2026: // ellipsis
			sText.SetAt(nChar, cReplace);
			break;
		}

		if ((c < 0x20 && (c == 0x09 || c == 0x0A || c == 0x0D)) ||
			(c >= 0x20 && c < 0x82) || c > 0x9F) // 0x82-0x9F are special windows chars
		{
			continue;   // valid
		}

		// else
		sText.SetAt(nChar, cReplace);
	}
}

BOOL CXmlItem::NameIs(LPCTSTR szName) const
{
	return (m_sName == szName);
}

CString CXmlItem::GetName() const
{
	return m_sName;
}

CString CXmlItem::GetValue() const
{
	return m_sValue;
}

int CXmlItem::GetNameLen() const
{
	return m_sName.GetLength();
}

int CXmlItem::GetValueLen() const
{
	return m_sValue.GetLength();
}

int CXmlItem::GetItemCount() const
{
	return m_lstItems.GetCount();
}

int CXmlItem::GetValueI() const
{
	return _ttoi(m_sValue);
}

int CXmlItem::GetItemValueI(LPCTSTR szItemName, LPCTSTR szSubItemName) const
{
	return _ttoi(GetItemValue(szItemName, szSubItemName));
}

double CXmlItem::GetValueF() const
{
	return Misc::Atof(m_sValue);
}

double CXmlItem::GetItemValueF(LPCTSTR szItemName, LPCTSTR szSubItemName) const
{
	return Misc::Atof(GetItemValue(szItemName, szSubItemName));
}

void CXmlItem::DeleteAllItems()
{
	Reset();
}

const CXmlItem* CXmlItem::GetParent() const
{
	return m_pParent;
}

const CXmlItem* CXmlItem::GetSibling() const
{
	return m_pSibling;
}

CXmlItem* CXmlItem::GetParent()
{
	return m_pParent;
}

CXmlItem* CXmlItem::GetSibling()
{
	return m_pSibling;
}

BOOL CXmlItem::SetType(XI_TYPE nType)
{
	// some sanity checks
	ASSERT(nType == XIT_ELEMENT || !m_lstItems.GetCount());

	m_nType = nType;

	return TRUE;
}

BOOL CXmlItem::IsCDATA() const
{
	return (m_nType == XIT_CDATA);
}

BOOL CXmlItem::IsAttribute(int nMaxAttribLen) const
{
	return (m_nType == XIT_ATTRIB && GetValueLen() <= nMaxAttribLen && !m_lstItems.GetCount());
}

CString CXmlItem::ToString(int nValue)
{
	CString sValue;
	sValue.Format(_T("%d"), nValue);
	return sValue;
}

CString CXmlItem::ToString(double dValue)
{
	CString sValue;
	sValue.Format(_T("%.8f"), dValue);
	return sValue;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXmlFile::CXmlFile(LPCTSTR szRootItemName) : m_xiRoot(NULL, szRootItemName)
{
}

CXmlFile::~CXmlFile()
{
}

BOOL CXmlFile::Load(LPCTSTR szFilePath, LPCTSTR szRootItemName, IXmlParse* pCallback)
{
	if (!szFilePath || !*szFilePath)
	{
		return FALSE;
	}

	if (GetFileHandle() != (HANDLE)CFile::hFileNull)
	{
		Close();
	}

	if (!szRootItemName && *(m_xiRoot.GetName()))
	{
		szRootItemName = m_xiRoot.GetName();
	}

	m_pCallback = pCallback;

	if (Open(szFilePath, XF_READ))
	{
		return LoadEx(szRootItemName, pCallback);
	}

	// else
	return FALSE;
}

BOOL CXmlFile::Save(LPCTSTR szFilePath)
{
	if (!szFilePath || !*szFilePath)
	{
		return FALSE;
	}

	if (GetFileHandle() != (HANDLE)CFile::hFileNull)
	{
		Close();
	}

	if (Open(szFilePath, XF_WRITE))
	{
		BOOL bRes = SaveEx();
		Close();

		return bRes;
	}

	// error handling
	m_nFileError = GetLastError();

	return FALSE;
}

BOOL CXmlFile::Open(LPCTSTR szFilePath, XF_OPEN nOpenFlag)
{
	if (!szFilePath || !*szFilePath)
	{
		return FALSE;
	}

	if (GetFileHandle() != (HANDLE)CFile::hFileNull)
	{
		Close();
	}

	BOOL bRes = FALSE;

	switch (nOpenFlag)
	{
	case XF_READ:
		bRes = CFile::Open(szFilePath, CFile::shareDenyNone | CFile::modeRead);
		break;

	case XF_WRITE:
		bRes = CFile::Open(szFilePath, CFile::shareExclusive | CFile::modeWrite | CFile::modeCreate);
		break;

	case XF_READWRITE:
		bRes = CFile::Open(szFilePath, CFile::shareExclusive | CFile::modeReadWrite |
				CFile::modeCreate | CFile::modeNoTruncate);
		break;
	}

	if (!bRes)
	{
		m_nFileError = GetLastError();
	}

	return bRes;
}

BOOL CXmlFile::SaveEx()
{
	if (GetFileHandle() == (HANDLE)CFile::hFileNull)
	{
		return FALSE;
	}

	BOOL bRes = FALSE;
	CString sXml;

	if (Export(sXml))
	{
		try
		{
			// move to start
			Seek(0, CFile::begin);

			// write the xml
			ATL::CT2A aXmlText(sXml);
			Write((const void*)(LPSTR)aXmlText, strlen(aXmlText));

			// update the file end
			VERIFY(::SetEndOfFile(GetFileHandle()));

			// verify file length matches length of xml
			if (::GetFileSize(GetFileHandle(), NULL) == (DWORD)strlen(aXmlText))
			{
				bRes = TRUE;
			}
		}
		catch (...)
		{
			m_nFileError = GetLastError();
		}
	}

	return bRes;
}

BOOL CXmlFile::LoadEx(LPCTSTR szRootItemName, IXmlParse* pCallback)
{
	m_nFileError = XFL_NONE; // reset

	if (GetFileHandle() == (HANDLE)CFile::hFileNull)
	{
		m_nFileError = ERROR_INVALID_HANDLE;
		return FALSE;
	}

	// concatenate entire file into one long string
	char* pFileContents = NULL;

	try
	{
		Seek(0, CFile::begin); // move to start

		// preallocate string to avoid excessive memory allocations
		DWORD dwLength = static_cast<DWORD>(GetLength());
		pFileContents = new char[dwLength + 1];
		ZeroMemory(pFileContents, dwLength + 1);

		BOOL bContinue = TRUE;
		char* pFilePtr = pFileContents;

		while (bContinue)
		{
			DWORD nRead = Read(pFilePtr, dwLength);
			pFilePtr += nRead;
			bContinue = (nRead != dwLength);
		}

		pFileContents[dwLength] = 0;
	}
	catch (...)
	{
		m_nFileError = GetLastError();

		// cleanup
		delete [] pFileContents;
		m_pCallback = NULL;

		return FALSE;
	}

	if (!szRootItemName && m_xiRoot.GetNameLen())
	{
		szRootItemName = m_xiRoot.GetName();
	}

	m_pCallback = pCallback;

	LPCSTR szFile = pFileContents;
	BOOL bRes = FALSE;

	try
	{
		CXmlDocumentWrapper doc;

		if (doc.IsValid())
		{
			if (!doc.LoadXML(ATL::CA2T(szFile)))
			{
				// try removing any bad chars
				CString sFile(szFile);
				FixInputString(sFile, szRootItemName);

				// then try again
				if (!doc.LoadXML(sFile))
				{
					m_nFileError = XFL_BADMSXML;
				}
			}

			// now read it into CXmlItem structures
			if (m_nFileError == XFL_NONE)
			{
				if (!ParseRootItem(szRootItemName, &doc))
				{
					m_nFileError = XFL_MISSINGROOT;
				}
				else
				{
					bRes = TRUE;
				}
			}
		}
	}
	catch (...)
	{
		m_nFileError = XFL_BADMSXML;
	}

	// cleanup
	delete [] pFileContents;
	m_pCallback = NULL;

	return bRes;
}

void CXmlFile::FixInputString(CString& sXml, LPCTSTR szRootItem)
{
	CXmlItem::ValidateString(sXml);

	// check for any other obvious problems
	if (szRootItem)
	{
		CString sRoot(szRootItem);
		sRoot = '<' + sRoot;

		int nRoot = sXml.Find(sRoot);
		int nHeader = sXml.Find(_T("<?xml"));

		if (nHeader > nRoot)
		{
			// what should we do?
		}
	}
}

BOOL CXmlFile::ParseRootItem(LPCTSTR szRootItemName, CXmlDocumentWrapper* pDoc)
{
	ASSERT(pDoc);

	m_xiRoot.Reset();

	CString sRootItem(szRootItemName), sItem;
	sRootItem.TrimLeft();
	sRootItem.TrimRight();

	// save off the header string
	// Valik - Change IXMLDOMNode* to IXMLDOMNodePtr to prevent an ambiguous symbol error (C2872) in VC 7.1
	MSXML2::IXMLDOMNodePtr pNode = pDoc->AsNode();
	CXmlNodeWrapper node(pNode);
	pNode->Release();

	if (0 != node.Name().CompareNoCase(sRootItem))
	{
		return FALSE;
	}

	m_sHeader = pDoc->GetHeader();

	// parse rest of file
	ParseItem(m_xiRoot, &node);

	return TRUE;
}

BOOL CXmlFile::ParseItem(CXmlItem& xi, CXmlNodeWrapper* pNode)
{
	int nNumAttrib = pNode->NumAttributes();

	for (int nAttrib = 0; nAttrib < nNumAttrib; nAttrib++)
	{
		CString sName(pNode->GetAttribName(nAttrib));
		CString sVal(pNode->GetAttribVal(nAttrib));

		xi.AddItem(sName, sVal);

		if (!ContinueParsing(sName, sVal))
		{
			return TRUE;
		}
	}

	int nNumNodes = pNode->NumNodes();

	for (int nNode = 0; nNode < nNumNodes; nNode++)
	{
		// Valik - Change IXMLDOMNode* to IXMLDOMNodePtr to prevent an ambiguous symbol error (C2872) in VC 7.1
		MSXML2::IXMLDOMNodePtr pChildNode = pNode->GetNode(nNode);
		CXmlNodeWrapper nodeChild(pChildNode);
		pChildNode->Release();

		CString sName(nodeChild.Name());
		CString sVal(nodeChild.GetText());

		// Valik - Fully qualify NODE_CDATA_SECTION to prevent an ambiguous symbol error (C2872) in VC 7.1
		int nNodeType = nodeChild.NodeTypeVal();
		XI_TYPE nType = XIT_ELEMENT;

		if (nNodeType == MSXML2::NODE_CDATA_SECTION)
		{
			nType = XIT_CDATA;
		}

		else if (nNodeType == MSXML2::NODE_ATTRIBUTE)
		{
			nType = XIT_ATTRIB;
		}

		// if sName is empty then sVal relates to pNode
		if (!sName.IsEmpty())
		{
			CXmlItem* pXI = xi.AddItem(sName, sVal, nType);

			if (!ContinueParsing(sName, sVal))
			{
				return TRUE;
			}

			ParseItem(*pXI, &nodeChild);
		}
		// need to take care here not to overwrite a node's value by carriage returns
		// which can result if we load the XML preserving whitespace
		else
		{
			BOOL bHasValue = (xi.GetValueLen() != 0);
			BOOL bValueIsCR = (sVal == _T("\n"));

			if (nodeChild.IsPreservingWhiteSpace() && bHasValue && bValueIsCR)
			{
				// ignore
				ASSERT(1); // for debugging
			}
			else
			{
				xi.SetValue(sVal);
				xi.SetType(nType);
			}
		}
	}

	return TRUE;
}

void CXmlFile::Copy(const CXmlFile& file)
{
	m_xiRoot.Reset();
	m_xiRoot.Copy(file.m_xiRoot, TRUE);
}

BOOL CXmlFile::Export(CString& sOutput) const
{
	BOOL bRes = FALSE;
	sOutput.Empty();

	try
	{
		CXmlDocumentWrapper doc(GetHeader(), m_xiRoot.GetName());

		if (BuildDOM(&doc))
		{
			sOutput = doc.GetXML(TRUE);

			if (sOutput.IsEmpty()) // sanity check
			{
				m_nFileError = XFL_BADMSXML;
			}
			else
			{
				// carriage return after each attribute
				sOutput.Replace(_T("><"), _T(">\r\n<"));

				bRes = TRUE;
			}
		}
	}
	catch (...)
	{
		m_nFileError = XFL_BADMSXML;
	}

	return bRes;
}

BOOL CXmlFile::BuildDOM(CXmlDocumentWrapper* pDoc) const
{
	if (!pDoc || !pDoc->IsValid())
	{
		return FALSE;
	}

	// Valik - Change IXMLDOMNode* to IXMLDOMNodePtr to prevent an ambiguous symbol error (C2872) in VC 7.1
	MSXML2::IXMLDOMNodePtr pNode = pDoc->AsNode();
	CXmlNodeWrapper node(pNode);
	pNode->Release();

	BOOL bRes = Export(&m_xiRoot, &node);

	return bRes;
}

BOOL CXmlFile::Transform(LPCTSTR szTransformPath, CString& sOutput, const CString& sEncoding) const
{
	CXmlDocumentWrapper doc(m_sHeader, m_xiRoot.GetName());

	if (BuildDOM(&doc))
	{
		sOutput = doc.Transform(szTransformPath);

		// encoding
		// note: the Transform function always returns UTF-16
		if (!sEncoding.IsEmpty())
		{
			sOutput.Replace(_T("UTF-16"), sEncoding);
		}
	}
	else
	{
		sOutput.Empty();
	}

	return TRUE;
}

BOOL CXmlFile::TransformToFile(LPCTSTR szTransformPath, LPCTSTR szOutputPath, const CString& sEncoding) const
{
	CString sOutput;

	if (Transform(szTransformPath, sOutput, sEncoding))
	{
		CFile output;

		if (output.Open(szOutputPath, CFile::modeCreate | CFile::modeWrite))
		{
			output.Write((void*)(LPCTSTR)sOutput, sOutput.GetLength());
			output.Close();

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CXmlFile::Export(const CXmlItem* pItem, CXmlNodeWrapper* pNode) const
{
	// own value
	if (pItem->GetValueLen())
	{
		pNode->SetText(pItem->GetValue());
	}

	// attributes and items
	POSITION pos = pItem->GetFirstItemPos();
	int nNode = 0;

	while (pos)
	{
		const CXmlItem* pXIChild = pItem->GetNextItem(pos);
		ASSERT(pXIChild);

		CString sItem = pXIChild->GetName();

		if (pXIChild->IsAttribute())
		{
			ASSERT(!pXIChild->GetSibling());
			pNode->SetValue(sItem, pXIChild->GetValue());
		}
		else if (pXIChild->IsCDATA())
		{
			// create a named node to wrap the CDATA
			MSXML2::IXMLDOMNodePtr pChildNode = pNode->InsertNode(nNode++, (LPCTSTR)sItem);
			MSXML2::IXMLDOMCDATASectionPtr pCData =
				pNode->ParentDocument()->createCDATASection(TOBSTRING(ATL::CT2A(pXIChild->GetValue())));
			pChildNode->appendChild(pCData);
		}
		else // node
		{
			while (pXIChild)
			{
				// Valik - Change IXMLDOMNode* to IXMLDOMNodePtr to prevent an ambiguous symbol error (C2872) in VC 7.1
				MSXML2::IXMLDOMNodePtr pChildNode = pNode->InsertNode(nNode++, (LPCTSTR)sItem);
				CXmlNodeWrapper nodeChild(pChildNode);
				ASSERT(nodeChild.IsValid());
				pChildNode->Release();

				Export(pXIChild, &nodeChild);

				// siblings
				pXIChild = pXIChild->GetSibling();
			}
		}
	}

	return TRUE;
}

void CXmlFile::Trace() const
{
#ifdef _DEBUG
	CString sXml;
	Export(sXml);

	// because the string might be long, output it in chunks of 255 chars
	int nPos = 0;

	while (nPos < sXml.GetLength())
	{
		OutputDebugString(sXml.Mid(nPos, 255));
		nPos += 255;
	}
#endif
}

void CXmlFile::Reset()
{
	m_xiRoot.Reset();
}

int CXmlFile::GetItemCount() const
{
	return m_xiRoot.GetItemCount();
}

void CXmlFile::Close()
{
	CFile::Close();
}

int CXmlFile::GetLastFileError()
{
	return m_nFileError;
}

const CXmlItem* CXmlFile::Root() const
{
	return &m_xiRoot;
}

CXmlItem* CXmlFile::Root()
{
	return &m_xiRoot;
}

const CXmlItem* CXmlFile::GetItem(LPCTSTR szItemName) const
{
	return m_xiRoot.GetItem(szItemName);
}

CXmlItem* CXmlFile::GetItem(LPCTSTR szItemName)
{
	return m_xiRoot.GetItem(szItemName);
}

const CXmlItem* CXmlFile::FindItem(LPCTSTR szItemName, LPCTSTR szItemValue, BOOL bSearchChildren) const
{
	return m_xiRoot.FindItem(szItemName, szItemValue, bSearchChildren);
}

CXmlItem* CXmlFile::FindItem(LPCTSTR szItemName, LPCTSTR szItemValue, BOOL bSearchChildren)
{
	return m_xiRoot.FindItem(szItemName, szItemValue, bSearchChildren);
}

const CXmlItem* CXmlFile::FindItem(LPCTSTR szItemName, int nItemValue, BOOL bSearchChildren) const
{
	return m_xiRoot.FindItem(szItemName, nItemValue, bSearchChildren);
}

CXmlItem* CXmlFile::FindItem(LPCTSTR szItemName, int nItemValue, BOOL bSearchChildren)
{
	return m_xiRoot.FindItem(szItemName, nItemValue, bSearchChildren);
}

const CXmlItem* CXmlFile::FindItem(LPCTSTR szItemName, double dItemValue, BOOL bSearchChildren) const
{
	return m_xiRoot.FindItem(szItemName, dItemValue, bSearchChildren);
}

CXmlItem* CXmlFile::FindItem(LPCTSTR szItemName, double dItemValue, BOOL bSearchChildren)
{
	return m_xiRoot.FindItem(szItemName, dItemValue, bSearchChildren);
}

CXmlItem* CXmlFile::AddItem(LPCTSTR szName, LPCTSTR szValue, XI_TYPE nType)
{
	return m_xiRoot.AddItem(szName, szValue, nType);
}

CXmlItem* CXmlFile::AddItem(LPCTSTR szName, int nValue, XI_TYPE nType)
{
	return m_xiRoot.AddItem(szName, nValue, nType);
}

CXmlItem* CXmlFile::AddItem(LPCTSTR szName, double dValue, XI_TYPE nType)
{
	return m_xiRoot.AddItem(szName, dValue, nType);
}

BOOL CXmlFile::DeleteItem(CXmlItem* pXI)
{
	return m_xiRoot.DeleteItem(pXI);
}

BOOL CXmlFile::DeleteItem(LPCTSTR szItemName)
{
	return m_xiRoot.DeleteItem(szItemName);
}

CString CXmlFile::GetItemValue(LPCTSTR szItemName, LPCTSTR szSubItemName) const
{
	return m_xiRoot.GetItemValue(szItemName, szSubItemName);
}

int CXmlFile::GetItemValueI(LPCTSTR szItemName, LPCTSTR szSubItemName) const
{
	return m_xiRoot.GetItemValueI(szItemName, szSubItemName);
}

double CXmlFile::GetItemValueF(LPCTSTR szItemName, LPCTSTR szSubItemName) const
{
	return m_xiRoot.GetItemValueF(szItemName, szSubItemName);
}

CXmlItem* CXmlFile::SetItemValue(LPCTSTR szName, LPCTSTR sValue, XI_TYPE nType)
{
	return m_xiRoot.SetItemValue(szName, sValue, nType);
}

CXmlItem* CXmlFile::SetItemValue(LPCTSTR szName, int nValue, XI_TYPE nType)
{
	return m_xiRoot.SetItemValue(szName, nValue, nType);
}

CXmlItem* CXmlFile::SetItemValue(LPCTSTR szName, double dValue, XI_TYPE nType)
{
	return m_xiRoot.SetItemValue(szName, dValue, nType);
}

CString CXmlFile::GetFilePath() const
{
	return CFile::GetFilePath();
}

CString CXmlFile::GetFileName() const
{
	return FileMisc::GetFileNameFromPath(GetFilePath());
}

const HANDLE CXmlFile::GetFileHandle() const
{
	return (HANDLE)CFile::m_hFile;
}

CString CXmlFile::GetHeader() const
{
	return m_sHeader.IsEmpty() ? CXmlDocumentWrapper().GetHeader() : m_sHeader;
}

void CXmlFile::SetHeader(LPCTSTR szHeader)
{
	m_sHeader = szHeader;
	m_sHeader.MakeLower();
}

void CXmlFile::SortItems(LPCTSTR szItemName, LPCTSTR szKeyName, BOOL bAscending)
{
	m_xiRoot.SortItems(szItemName, szKeyName, bAscending);
}

void CXmlFile::SortItemsI(LPCTSTR szItemName, LPCTSTR szKeyName, BOOL bAscending)
{
	m_xiRoot.SortItemsI(szItemName, szKeyName, bAscending);
}

void CXmlFile::SortItemsF(LPCTSTR szItemName, LPCTSTR szKeyName, BOOL bAscending)
{
	m_xiRoot.SortItemsF(szItemName, szKeyName, bAscending);
}

BOOL CXmlFile::ContinueParsing(LPCTSTR szItem, LPCTSTR szValue)
{
	return (!m_pCallback || m_pCallback->Continue(szItem, szValue));
}
