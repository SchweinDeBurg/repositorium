//
//	optiontree.h
//
//	(C) Copyright 2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	Option treeview.
//

// OptionTree class.
class OptionTree : public ClsTreeView
{
	_NO_COPY( OptionTree );
public:
	// Construction/destruction.
	OptionTree();
	virtual ~OptionTree();

	// Implementation.
	BOOL Initialize();
	BOOL SetOptions( LPPARSER lpp );

protected:
	// Overidables.
	virtual LRESULT OnClick( LPNMHDR pNMHDR, BOOL& bNotifyParent );
	virtual LRESULT OnSelChanged( LPNMTREEVIEW pTreeView, BOOL& bNotifyParent );
	virtual LRESULT OnItemExpanding( LPNMTREEVIEW pTreeView, BOOL& bNotifyParent );

	// Helpers.
	BOOL AddOption( LPCTSTR pszName );
	BOOL SetOption( LPCTSTR pszName, LPBOOL pStorage );
	BOOL AddSubOption( LPCTSTR pszParent, LPCTSTR pszName );
	BOOL SetSubOption( LPCTSTR pszParent, LPCTSTR pszName, LPBOOL pStorage );
	HTREEITEM FindOption( LPCTSTR pszName, HTREEITEM hItem = NULL );
	BOOL SetCheckState( HTREEITEM hItem, BOOL bChecked );
	BOOL GetCheckState( HTREEITEM hItem );

	// Data.
	TCHAR		m_szText[ 4096 ];	// Item text buffer.
	LPPARSER	m_pParser;		// Current parser.
};
