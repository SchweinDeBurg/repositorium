// AfxGadgets library.
// Copyright (c) 2004-2005 by Elijah Zarezky,
// All rights reserved.

// SortingListCtrl.h - interface of the CSortingListCtrl class

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
