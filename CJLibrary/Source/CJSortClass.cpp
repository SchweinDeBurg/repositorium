// Copyright (c) Iuri Apollonio 1998
// Use & modify as you want & need, and leave those 3 lines.
// http://www.codeguru.com
// class extended by Max Poliashenko.
//
// CJSortClass.cpp: implementation of the CCJSortClass class.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 8/07/01 8:38 $
 * $Revision: 2 $
 * $Archive: /Projects/AddIns/ResOrg/CJLibrary/CJLibrary/CJSortClass.cpp $
 *
 * $History: CJSortClass.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Andy         Date: 8/07/01    Time: 8:38
 * Updated in $/Projects/AddIns/ResOrg/CJLibrary/CJLibrary
 * String sorts now handle strings with trailing numerics better
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 12:19p
 * Updated in $/CodeJock/CJLibrary
 * Fixed assertion bug with constructior.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJSortClass.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


static int FindTrailingNumber(const CString& s)
{
	int nPos = -1;

	int nLen = s.GetLength();

	for (int n = nLen - 1; n >= 0; n--)
	{
		if (!::_istdigit(s[n]) )
		{
			nPos = n + 1;
			break;
		}
	}
	if (nPos >= nLen)
	{
		nPos = -1;		// Not found
	}
	return nPos;
}


static int StrCmpEx(const CString& sFirst, const CString& sSecond)
{
	int nFirst	= ::FindTrailingNumber(sFirst);
	int nSecond	= ::FindTrailingNumber(sSecond);

	if ( (nFirst >= 0) || (nSecond >= 0) )
	{
		CString sFirstPrefix	= (nFirst >= 0) ? sFirst.Left(nFirst) : sFirst;
		CString sSecondPrefix	= (nSecond >= 0) ? sSecond.Left(nSecond) : sSecond;

		if (sFirstPrefix.CompareNoCase(sSecondPrefix) == 0)
		{
			int nFirstSuffix	= (nFirst >= 0) ? _ttoi(sFirst.Mid(nFirst)) : 0;
			int nSecondSuffix	= (nSecond >= 0) ? _ttoi(sSecond.Mid(nSecond)) : 0;

			if (nFirstSuffix > nSecondSuffix)
			{
				return 1;
			}
			else if (nFirstSuffix < nSecondSuffix)
			{
				return -1;
			}
		}
	}
	return sFirst.CompareNoCase(sSecond);
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCJSortClass::CCJSortClass(CListCtrl* pWnd, const int nCol)
{
	ASSERT(pWnd);
	m_pWnd = pWnd;
	
	int nCount = m_pWnd->GetItemCount();
	DWORD dwData;
	CString strItemText;
	
	// replace Item data with pointer to CSortItem structure
	for (int i = 0; i < nCount; i++)
	{
		dwData = m_pWnd->GetItemData(i); // save current data to restore it later
		strItemText = m_pWnd->GetItemText(i, nCol); 
		m_pWnd->SetItemData(i, (DWORD) new CSortItem(dwData, strItemText));
	}
}

CCJSortClass::~CCJSortClass()
{
	ASSERT(m_pWnd);
	int nCount = m_pWnd->GetItemCount();
	CSortItem * pItem;
	for (int i = 0; i < nCount; i++)
	{
		pItem = (CSortItem *) m_pWnd->GetItemData(i);
		ASSERT(pItem);
		m_pWnd->SetItemData(i, pItem->m_dwData);
		delete pItem;
	}
}

void CCJSortClass::Sort(bool bAsc, DATA_TYPE eType)
{
	long lParamSort = eType;
	
	// if lParamSort positive - ascending sort order, negative - descending
	if (!bAsc)
		lParamSort *= -1; 
	
	m_pWnd->SortItems(Compare, lParamSort);
}

int CALLBACK CCJSortClass::Compare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CSortItem* item1 = (CSortItem *) lParam1;
	CSortItem* item2 = (CSortItem *) lParam2;

	ASSERT(item1 && item2);
	
	// restore data type and sort order from lParamSort
	// if lParamSort positive - ascending sort order, negative - descending
	short	  sOrder = lParamSort < 0 ? (short) -1 : (short) 1; 
	
	DATA_TYPE eType  = (DATA_TYPE) (lParamSort * sOrder); // get rid of sign
	
	// declare typed buffers
	COleDateTime t1, t2;
	
	switch (eType)
	{
	case DT_INT:
		return ( _ttol( item1->m_strItemText ) - _ttol( item2->m_strItemText )) * sOrder;

	case DT_DEC:
#ifdef _UNICODE
		{
			char szText1[ 256 ];		
			char szText2[ 256 ];		
		
			WideCharToMultiByte( CP_ACP,
								 0,
								 item1->m_strItemText,
								 -1,
								 szText1,
								 255,
								 NULL,
								 NULL );
			
			WideCharToMultiByte( CP_ACP,
								 0,
								 item2->m_strItemText,
								 -1,
								 szText2,
								 255,
								 NULL,
								 NULL );
		
		
			return ( atof( szText1 ) < atof( szText2 ) ? -1 : 1) * sOrder;
		}
#else
		return ( atof ( item1->m_strItemText ) < atof( item2->m_strItemText ) ? -1 : 1) * sOrder;
#endif
		
	case DT_DATETIME:
		if( t1.ParseDateTime( item1->m_strItemText ) && t2.ParseDateTime( item2->m_strItemText ))
		{
			return (t1 < t2 ? -1 : 1 ) * sOrder;
		}
		return 0;

	case DT_STRING:
		return ::StrCmpEx(item1->m_strItemText, item2->m_strItemText) * sOrder;
		
	default:
		ASSERT("Error: attempt to sort a column without type.");
		return 0;
	}
}
