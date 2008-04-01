#ifndef _PANELBAR_H_
#define _PANELBAR_H_
//
// panelbar.h
//
// (C) Copyright 2004 Jan van den Baard.
//     All Rights Reserved.
//

#include "../window.h"
#include "flatbutton.h"
#include "../../collectors/simplearray.h"
#include "../../gdi/paintdc.h"

// A simple button derived from ClsFlatButton
// which shows the hand cursor when the mouse 
// is over it.
class ClsCaptionButton : public ClsFlatButton
{
	_NO_COPY( ClsCaptionButton );
public:
	ClsCaptionButton() {;}
	virtual ~ClsCaptionButton() {;}

protected:
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		if ( uMsg == WM_SETCURSOR )
		{
			// Mouse on the client?
			if ( LOWORD( lParam ) == HTCLIENT )
			{
				// Set the hand cursor.
				::SetCursor( ClsGetCursor( ClsApp::STC_HAND ));
				return 0;
			}
		}
		// Pass to the base-class.
		return ClsFlatButton::WindowProc( uMsg, wParam, lParam );
	}
};

// Base class for the panels. Derive from
// this class to get the panel functionality.
class ClsPanelBar;
class ClsPanel : public ClsWindow
{
	friend class ClsPanelBar;
	_NO_COPY( ClsPanel );
public:
	ClsPanel();
	virtual ~ClsPanel();

	// Implementation.
	BOOL Create( LPCTSTR pszTitle, ClsRect& rcBounds );
	BOOL SetPanelTitle( LPCTSTR pszTitle );
	ClsString GetPanelTitle() const { return m_strCaption; }
	ClsWindow *SetPanelClient( ClsWindow *pWindow );
	ClsWindow *GetPanelClient() const { return m_pClient; }

protected:
	// Overidables.
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual LRESULT OnEraseBkgnd( ClsDC *pDC );
	virtual LRESULT OnDestroy();
	virtual LRESULT OnSize( UINT nSizeType, int nWidth, int nHeight );
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );
	virtual LRESULT OnNotify( LPNMHDR pNMHDR );
	virtual BOOL OnGetMinSize( ClsSize& szMinSize );
	virtual void OnPanelActivated(); 
	virtual void OnPanelDeactivated();
	virtual void OnPanelAdded();
	virtual void OnPanelRemoved();
	virtual BOOL OnCreatePanelCaption();

	// Helpers.
	BOOL ShowClient( BOOL bShow = TRUE );

	// Data.
	HFONT		m_hFont;
	ClsString	m_strCaption;
	ClsButton      *m_pPanelCaption;
	ClsWindow      *m_pClient;
	int		m_nCaptionHeight;
	BOOL		m_bIsActive;
};

// The panelbar class which acts as a container
// for ClsPanel derived classes.
class ClsPanelBar : public ClsWindow
{
	friend class ClsPanel;
	_NO_COPY( ClsPanelBar );
public:
	ClsPanelBar();
	virtual ~ClsPanelBar();

	// Implementation.
	BOOL InsertPanel( ClsPanel *pPanel, ClsPanel *pPred = NULL );
	BOOL RemovePanel( ClsPanel *pPanel );
	BOOL ActivatePanel( ClsPanel *pPanel );
	ClsPanel *GetActivePanel() const;

protected:
	// Overidables.
	virtual LRESULT OnEraseBkgnd( ClsDC *pDC );
	virtual LRESULT OnSize( UINT nSizeType, int nWidth, int nHeight );
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );
	virtual LRESULT OnNotify( LPNMHDR pNMHDR );

	// Helpers.
	void LayoutPanels();

	// Data structure. Each panel added will
	// be stored like this.
	struct PanelData
	{
		ClsPanel	       *m_pPanel;	// The panel itself.
		ClsSize			m_szMinSize;	// It's minimum size.
	};

	// Data.
	ClsSimpleArray<PanelData>	m_PanelArray;
	int				m_nActivePanel;
};
#endif // _PANELBAR_H_