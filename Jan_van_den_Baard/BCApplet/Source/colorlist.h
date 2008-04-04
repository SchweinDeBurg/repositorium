//
//	colorlist.h
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	Color selection listbox.
//

// Forward declaration.
class KeyboardPage;

// ColorList class.
class ColorList : public ClsListBox
{
	friend class KeyboardPage;
	_NO_COPY( ColorList );
public:
	// Construction/destruction.
	ColorList();
	virtual ~ColorList();

	// Implementation.
	BOOL SetBlockList();
	BOOL SetCommonList();
	BOOL SetKeywordList( LPKEYLIST pKeys );
	BOOL SetIndentionList( LPSMARTLIST pSmart );
	inline void SetParser( LPPARSER pParser ) { m_pParser = pParser; }
	void MoveSelUp();
	void MoveSelDown();
	void RemoveSel();
	inline LPDWORD GetFlagArray() { return &m_dwFlags[ 0 ]; }
	inline COLORREF *GetDefArray() { return &m_crDefaults[ 0 ]; }
		
protected:
	// Ownerdraw stuff.
	virtual LRESULT OnReflectedMeasureItem( UINT nID, LPMEASUREITEMSTRUCT pMis, BOOL& bNotifyParent );
	virtual LRESULT OnReflectedDrawItem( UINT nID, LPDRAWITEMSTRUCT pDis, BOOL& bNotifyParent );

	// Helpers.
	static void DrawTextItem( LPDRAWITEMSTRUCT pDis, LPCTSTR pszName );
	void DrawColorEntry( LPDRAWITEMSTRUCT pDis, COLORREF crColor, LPCTSTR pszName );

	// Data.
	BOOL		m_bBlockList;
	LPKEYLIST       m_pKeys;
	LPSMARTLIST     m_pSmart;
	LPPARSER	m_pParser;
	static int	m_nColors[ CARR_SIZEOF ];
	static DWORD	m_dwFlags[ CARR_SIZEOF ];
	static COLORREF	m_crDefaults[ CARR_SIZEOF ];
};
