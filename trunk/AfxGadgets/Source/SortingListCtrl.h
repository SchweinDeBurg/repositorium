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

// SortingListCtrl.h - interface of the CSortingListCtrl class

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

#if !defined(__SortingListCtrl_h)
#define __SortingListCtrl_h

class CSortingListCtrl: public CListCtrl
{
	DECLARE_DYNAMIC(CSortingListCtrl)
	DECLARE_MESSAGE_MAP()

// construction/destruction
protected:
	CSortingListCtrl(void);
	virtual ~CSortingListCtrl(void);

// operations
public:
	enum SORT_ORDER { SORT_ASCENDING = 1, SORT_DESCENDING = -1 };
	void SortItems(int iColumn, SORT_ORDER eOrder);

// overridables
protected:
	virtual int CompareItems(int iItemLhs, int iItemRhs) = 0;

// message map functions
protected:
	afx_msg void OnColumnClick(NMHDR* pHdr, LRESULT* pnResult);

// attributes
protected:
	int m_iSortColumn;
	int m_nSortOrder;		// SORT_ASCENDING or SORT_DESCENDING

// implementation helpers
private:
	static int CALLBACK CompareProc(LPARAM iItemLhs, LPARAM iItemRhs, LPARAM nData);

// diagnostic services
#if defined(_DEBUG)
public:
	virtual void AssertValid(void) const;
	virtual void Dump(CDumpContext& dumpCtx) const;
#endif
};

#endif	// __SortingListCtrl_h

// end of file
