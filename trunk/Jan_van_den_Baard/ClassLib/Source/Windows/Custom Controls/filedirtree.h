#ifndef _FILEDIRTREE_H_
#define _FILEDIRTREE_H_
//
// filedirtree.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../common controls/treeview.h"
#include "../../coords/rect.h"

// A directory and or file browser treeview
// control.
class ClsFileDirTree : public ClsTreeView
{
	_NO_COPY( ClsFileDirTree );
public:
	// Constructor.
	ClsFileDirTree();

	// Destructor.
	virtual ~ClsFileDirTree();

	// Selection types as returned by
	// TypeOfSelection().
	enum 
	{
		STYPE_FILE = 0,
		STYPE_DIRECTORY,
		STYPE_UNKNOWN,
		STYPE_NONE
	};

	// Attributes.
	inline BOOL& DoubleBuffer()
	{ return ( BOOL& )m_bDoubleBuffer; }
	inline DWORD& DisplayMask()
	{ return ( DWORD& )m_dwDontShow; }
	inline ClsString& FileFilter()
	{ return ( ClsString& )m_strFilter; }
	inline ClsString& LoadingText()
	{ return ( ClsString& )m_strLoading; }
	inline COLORREF& LoadingTextColor()
	{ return ( COLORREF& )m_crLoadingColor; }
	
	// Implementation.
	BOOL SetupTree( LPCTSTR pszRoot = NULL );
	BOOL GetSelectedPath( ClsString& strPath ) const;
	BOOL SelectPath( const ClsString& strPath );
	BOOL SelectPath( LPCTSTR pszPath );
	void ShowFiles( BOOL bShow = TRUE );
	int TypeOfSelection() const;

protected:
	// Overidables.
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual LRESULT OnItemExpanding( LPNMTREEVIEW pTreeView, BOOL& bNotifyParent );
	virtual LRESULT OnItemExpanded( LPNMTREEVIEW pTreeView, BOOL& bNotifyParent );
	virtual LRESULT OnEraseBkgnd( ClsDC *pDC );
	virtual LRESULT OnPaint( ClsDC *pDC );
	virtual void OnRenderLoadingItem( ClsDC *pDC, ClsRect crBounds );
	virtual void OnDriveEmpty( ClsString strPath );

	// Helpers.
	BOOL DisplayDrives();
	HTREEITEM AddTreeItem( HTREEITEM hParent, LPCTSTR pszPath, BOOL Parse = TRUE, LPCTSTR pszDriveName = NULL );
	ClsString GetPath( HTREEITEM hItem ) const;
	BOOL AddDirectory( HTREEITEM hParent );
	BOOL HasEntries( HTREEITEM hItem );
	void ConvertDriveName( ClsString& strDrive );

	// Data.
	ClsString	m_strFilter;
	ClsString	m_strLoading;
	ClsString	m_strInitial;
	COLORREF	m_crLoadingColor;
	DWORD		m_dwDontShow;
	BOOL		m_bShowFiles;
	BOOL		m_bDoubleBuffer;
	BOOL		m_bSilent;
};

#endif // _FILEDIRTREE_H_
