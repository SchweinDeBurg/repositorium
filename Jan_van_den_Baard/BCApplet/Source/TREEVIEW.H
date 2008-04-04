//
//	treeview.h
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	Treeview.
//

// Page selection tree.
class SelectionTree : public ClsTreeView
{
	_NO_COPY( SelectionTree );
public:
	// Construction/destruction.
	SelectionTree();
	virtual ~SelectionTree();

	// Implementation.
	HTREEITEM AddStringItem( LPCTSTR pszName, Page *pPage /*= NULL*/, HTREEITEM hParent /*= TVI_ROOT*/, BOOL bFirst /* = FALSE */ );
	LPPARSER GetCurrParser();
	inline void SetBar( ClsInfoBar *pBar ) { m_pBar = pBar; };

protected:
	// Overidables...
	virtual LRESULT OnSelChanged( LPNMTREEVIEW pNMHDR, BOOL& bNotifyParent );
	virtual LRESULT OnItemExpanding(  LPNMTREEVIEW pNMHDR, BOOL& bNotifyParent );

	// Data.
	ClsInfoBar	*m_pBar;
	ClsImageList	 m_Images;
	Page		*m_pCurrPage;
};
