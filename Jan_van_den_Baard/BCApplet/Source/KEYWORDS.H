//
//	keywords.h
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	Keyword editor dialog.
//

// Forward declarations.
class KWEdit;
class SyntaxPage;

// Keyword dialog class.
class KeywordEdit : public ClsDialog
{
	friend class KWEdit;	 // Needs to access some protected fields.
	friend class SyntaxPage; // Needs to access some protected functions.
	_NO_COPY( KeywordEdit );

public:
	// Construction/destruction.
	KeywordEdit();
	virtual ~KeywordEdit();
	
	// Implementation.
	void Edit( ClsWindow& Parent, LPPARSER pParser );
	
protected:
	// Overidables.
	virtual LRESULT OnInitDialog( LPPROPSHEETPAGE pPsp );
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Helpers.
	void SetupControls();
	static LPTSTR StringArrayCopy( LPARRAY pArray, LPTSTR pszString );
	static void FreeKeyList( LPKEYLIST pKeyList );
	void FreeKeyHashes();
	static BOOL AddKeyword2Node( LPKEYWORDS pKeywords, LPTSTR pszKeyword );
	static BOOL Hash2List( LPKEYLIST pKeyList, LPPARSER pParser );
	BOOL ReadKeywords( LPCTSTR pszFilename );
	BOOL UseKeywords( LPKEYLIST pKeyList );

	// Static helpers.
	static int _cdecl CompareKeywords( LPTSTR *ppStr1, LPTSTR *ppStr2 );
	static void RemoveTrailingBlanks( LPTSTR pszString );
	
	// Data
	EditToolbar	m_Tools, m_Keys;
	ColorList	m_List;
	LPPARSER	m_pParser;
	ClsListBox	m_KeyList;
	ClsCheckBox	m_Case;
	KEYLIST		m_KeyLst;
	BOOL		m_bCase;
};
