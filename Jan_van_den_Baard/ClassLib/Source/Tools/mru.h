#ifndef _MRU_H_
#define _MRU_H_
//
// mru.h
//
// (C) Copyright 2000-2001 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"
#include "../menus/xpmenu.h"
#include "../strings/string.h"
#include "../windows/common controls/imagelist.h"

// A class which handles an MRU list including the
// creation of a MRU menu and saving and restoring
// to the registry.
class ClsMRU
{
	// No copy constructor.
	_NO_COPY( ClsMRU );
public:
	// Construction/destruction.
	ClsMRU();
	virtual ~ClsMRU();

	// Implementation.
	BOOL SetupMRU( HKEY hKey, LPCTSTR pszSubKey, UINT nBaseID = 0xDEAD, int nMax = 10 );
	BOOL AddMRUEntry( LPCTSTR pszPath );
	BOOL RemoveMRUEntry( LPCTSTR pszPath );
	BOOL GetMRUEntryPath( UINT nID, ClsString& str );
	BOOL SaveMRUList();
	BOOL SelectMRUEntry( LPCTSTR pszPath );
	
	inline int NumEntries() const
	{ return m_nCounter; };
	inline ClsString& EmptyName()
	{ return ( ClsString& )m_strEmptyItemName; }
	inline BOOL IsMRUSelection( UINT nID ) const
	{ return ( BOOL )( nID == m_nBaseID || ( nID > m_nBaseID && nID < m_nBaseID + m_nCounter )); }
	inline void XPStyle( BOOL bXPStyle )
	{ m_Menu.OldStyle() = ! bXPStyle; }
	inline operator HMENU()
	{ return ( HMENU )m_Menu; };
	inline ClsXPMenu *GetMenuObject()
	{ return &m_Menu; }
	inline void UseIcons( BOOL bUseIcons )
	{ m_bUseIcons = bUseIcons; SetupIcons(); }
	
protected:
	// Helpers.
	BOOL AddEmptyItem();
	BOOL RenumberMRUList();
	BOOL NewMRUEntry( LPCTSTR pszPath );
	void ClearMRUList();
	BOOL GetMRUEntry( int nNum, ClsString& str );
	int GetIconIndex( LPCTSTR pszPath );
	BOOL SetupIcons();


	// Data
	ClsXPMenu	m_Menu;
	ClsImageList	m_Images;
	HKEY		m_hParent;
	ClsString	m_strSubKey;
	ClsString	m_strEmptyItemName;
	int		m_nCounter;
	int		m_nMaxEntries;
	UINT		m_nBaseID;
	BOOL		m_bUseIcons;
};

#endif // _MRU_H_