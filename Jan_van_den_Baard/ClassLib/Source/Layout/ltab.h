#ifndef _LTAB_H_
#define _LTAB_H_
//
// ltab.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../windows/common controls/tab.h"
#include "../collectors/linkedlist.h"

// Used by the AddMember() method.
#ifndef WND_HEAD
	#define WND_HEAD (( ClsWindow * )-1 )
#endif

// A tab control derived class which handles layout
// engine controls.
class ClsLTab : public ClsTab
{
	_NO_COPY( ClsLTab );
public:
	// Constructor.
	ClsLTab();

	// Destructor.
	virtual ~ClsLTab();

	// These do nothing! The InsertItem(), DeleteItem() and
	// DeleteAllItems() functions are not valid in this
	// class.
	inline BOOL DeleteItem( int iItem ) { return FALSE; }
	inline BOOL DeleteAllItems() { return FALSE; }
	inline int InsertItem( int iItem, const TCITEM& item ) { return -1; }

	// Implementation.
	BOOL Create( ClsWindow *parent, UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS );
	BOOL SetTabInfo( ClsWindow *pWindow, LPCTSTR pszTitle, int iImage );
	BOOL AddMember( ClsWindow *pWindow, ClsWindow *pPred, LPCTSTR pszTitle, int iImage );
	BOOL RemMember( ClsWindow *pWindow );
	BOOL SetCurSel( int iItem );
	inline BOOL& Error() { return ( BOOL& )m_bError; }

protected:
	// Member or pages are stored in a linked list
	// of these objects.
	class ClsTMember : public ClsNode
	{
		_NO_COPY( ClsTMember );
	public:
		ClsTMember() : m_pMember( NULL ) {;}
		virtual ~ClsTMember() {;}
		ClsWindow *m_pMember;
	};
	ClsLinkedList<ClsTMember>	m_Members;
	BOOL				m_bError;
	BOOL				m_bMadeVisible;

	// Helpers.
	void MakeCurrentVisible();
	void SetCurrentBounds();

	// Overidables.
	virtual BOOL OnGetMinSize( ClsSize& szMinSize );
	virtual LRESULT OnSize( UINT nSizeType, int nWidth, int nHeight );
	virtual LRESULT OnSelChange( LPNMHDR pNMHDR, BOOL& bNotifyParent );
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
};

#endif // _LTAB_H_