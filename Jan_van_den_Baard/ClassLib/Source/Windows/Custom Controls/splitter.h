#ifndef _SPLITTER_H_
#define _SPLITTER_H_
//
// splitter.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../window.h"
#include "../../gdi/dc.h"

// Splitter size (constant).
#define SPLITTER_SIZE 4

// A control which acts as a splitter between two
// other windows.
class ClsSplitter : public ClsWindow
{
	_NO_COPY( ClsSplitter );
public:
	// Constructor.
	ClsSplitter();

	// Destructor.
	virtual ~ClsSplitter();

	// Implementation.
	BOOL Create( ClsWindow *parent, UINT nID, BOOL bHorizontal = TRUE );
	void SetPanes( HWND hPane1, HWND hPane2 );
	void GetPanes( LPHANDLE pPane1, LPHANDLE pPane2 ) const;
	void SetPaneMinSize( int nMinPane1, int nMinPane2 );
	void SetSplitterPosition( int nSplitPos );
	void RedrawPanes();
	void SetSplitRect( LPRECT lpRect );
	inline int GetSplitterPosition() const
	{ return m_nPosition; }
	inline BOOL IsSplitterMoving() const
	{ return m_bIsMoving; };
	inline BOOL& Horizontal()
	{ return ( BOOL& )m_bIsHorizontal; }
	inline void GetSplitRect( LPRECT lpRect ) const
	{ _ASSERT_VALID( lpRect ); *lpRect = m_SplitRect; };

protected:
	// Overidables.
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual BOOL OnGetMinSize( ClsSize& szMinSize );

	// Helpers.
	void MoveSplitter();
	void DrawSplitterBar();

	// Data.
	BOOL		m_bIsHorizontal;
	BOOL		m_bIsMoving;
	BOOL		m_bIsVisible;
	ClsDC	        m_pDC;
	ClsRect		m_SplitRect;
	int		m_nPosition, m_nStartPosition;
	int		m_nMinPane1, m_nMinPane2;
	HWND		m_hPane1, m_hPane2, m_hFocus;
	HCURSOR		m_hHorz, m_hVert;
	ClsBrush	m_HatchBrush;
	int		m_nSaveDC;
};

#endif // _SPLITTER_H_