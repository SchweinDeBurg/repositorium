/* ==========================================================================
    File :          RulerRichEditCtrl.h

    Class :         CRulerRichEditCtrl

    Author :        Johan Rosengren, Abstrakt Mekanik AB
                    Iain Clarke

    Date :          2004-04-17

    Purpose :       "CRulerRichEditCtrl" is a "CWnd" derived class containing an
                    embedded RTF-control, a ruler-control with dragable tab-
                    positions and a formatting toolbar. The class can be used
                    to - for example - add a complete mini-editor to a modal
                    or modeless dialog box.

    Description :   The class mainly handles mouse messages. The mouse
                    messages are sent from the ruler control, and are
                    button down, where the a check is made for the cursor
                    located on one of the tab-markers, mouse move, where an
                    XORed line is drawn across the RTF-control and button up,
                    where a new tab position is set. The class also handles
                    the toolbar buttons, setting styles as
                    appropriate for the selected text.

    Usage :         Add a "CRulerRichEditCtrl"-member to the parent class.
                    Call Create to create the control. "GetRichEditCtrl" can
                    be used to access the embedded RTF-control. Remember to
                    call "AfxInitRichEdit(2)"!

                    The contents can be saved to disk by calling "Save", and
                    loaded from disk by calling "Load". The two functions
                    will automatically display a file dialog if the file
                    name parameter of the calls are left empty.

                    "GetRTF" and "SetRTF" can be used to get and set the
                    contents of the embedded RTF-control as RTF
                    respectively.

                    The ruler measures can be displayed as inches or
                    centimeters, by calling "SetMode". "GetMode" will get the
                    current mode.

   ========================================================================*/

#if !defined(AFX_RULERRICHEDITCTRL_H__4CD13283_82E4_484A_83B4_DBAD5B64F17C__INCLUDED_)
#define AFX_RULERRICHEDITCTRL_H__4CD13283_82E4_484A_83B4_DBAD5B64F17C__INCLUDED_

#include "RulerRichEdit.h"
#include "RRECRuler.h"
#include "RRECToolbar.h"
#include "IDs.h"

/////////////////////////////////////////////////////////////////////////////
// Helper structs

#ifdef _UNICODE
struct CharFormat : public CHARFORMAT2W
#else
struct CharFormat : public CHARFORMAT2
#endif
{
	CharFormat()
	{
		memset(this, 0, sizeof(CharFormat));
		cbSize = sizeof(CharFormat);
	};
};

struct ParaFormat2 : public PARAFORMAT2
{
	ParaFormat2(DWORD mask)
	{
		memset(this, 0, sizeof(ParaFormat2));
		cbSize = sizeof(ParaFormat2);
		dwMask = mask;
	}
};

struct ParaFormat : public PARAFORMAT
{
	ParaFormat(DWORD mask)
	{
		memset(this, 0, sizeof(ParaFormat));
		cbSize = sizeof(ParaFormat);
		dwMask = mask;
	}
};

/////////////////////////////////////////////////////////////////////////////
// CRulerRichEditCtrl window

class CRulerRichEditCtrl : public CWnd
{
public:
// Construction/creation/destruction
	CRulerRichEditCtrl();
	virtual ~CRulerRichEditCtrl();
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, BOOL autohscroll = FALSE);

// Attributes
	void    SetMode(int mode);
	int     GetMode() const;

	void ShowToolbar(BOOL show = TRUE);
	void ShowRuler(BOOL show = TRUE);

	BOOL IsToolbarVisible() const;
	BOOL IsRulerVisible() const;

	void SetWordWrap(BOOL bWrap = TRUE);
	BOOL HasWordWrap() const
	{
		return m_bWordWrap;
	}

	CRulerRichEdit& GetRichEditCtrl();

// Implementation
	CString GetRTF();
	int GetRTFLength();
	void    SetRTF(const CString& rtf);
	BOOL    Save(CString& filename);
	BOOL    Load(CString& filename);

	void SetReadOnly(BOOL readOnly);
	BOOL GetReadOnly() const;

protected:
// Formatting
	virtual void DoFont();
	virtual void DoColor();
	virtual void DoBold();
	virtual void DoItalic();
	virtual void DoUnderline();
	virtual void DoLeftAlign();
	virtual void DoCenterAlign();
	virtual void DoRightAlign();
	virtual void DoIndent();
	virtual void DoOutdent();
	virtual void DoBullet();
	virtual void DoNumberList();
	virtual void DoStrikethrough();
	virtual void DoSuperscript();
	virtual void DoSubscript();

	void SetCurrentFontName(const CString& font);
	void SetCurrentFontSize(int points);
	void SetCurrentFontColor(COLORREF color, BOOL bForeground);

	//void InsertHorizontalRule();

// Overrides
	//{{AFX_VIRTUAL(CRulerRichEditCtrl)
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

protected:
// Message handlers
	//{{AFX_MSG(CRulerRichEditCtrl)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnButtonFont();
	afx_msg void OnButtonColor();
	afx_msg void OnButtonBold();
	afx_msg void OnButtonStrikethrough();
	afx_msg void OnButtonItalic();
	afx_msg void OnButtonUnderline();
	afx_msg void OnButtonLeftAlign();
	afx_msg void OnButtonCenterAlign();
	afx_msg void OnButtonRightAlign();
	afx_msg void OnButtonIndent();
	afx_msg void OnButtonOutdent();
	afx_msg void OnButtonBullet();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnSetText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetText(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetTextLength(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG
	afx_msg void OnButtonNumberList();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnSetFont(WPARAM wp, LPARAM lp);
	afx_msg void OnEnHScroll();
	afx_msg void OnEnSelChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnUpdateToolbar(WPARAM wParam, LPARAM lParam);
	afx_msg void OnButtonTextColor();
	afx_msg void OnButtonBackColor();
	afx_msg void OnButtonWordwrap();

	LRESULT OnTrackRuler(WPARAM mode, LPARAM pt);
	LRESULT OnGetScrollPos(WPARAM, LPARAM);
	LRESULT OnSetCurrentFontName(WPARAM font, LPARAM size);
	LRESULT OnSetCurrentFontSize(WPARAM font, LPARAM size);
	LRESULT OnSetCurrentFontColor(WPARAM font, LPARAM size);

	DECLARE_MESSAGE_MAP()

protected:
	// Internal data
	int             m_rulerPosition;    // The x-position of the ruler line when dragging a tab
	CPen            m_pen;              // The pen to use for drawing the XORed ruler line

	CDWordArray     m_tabs;             // An array containing the tab-positions in device pixels
	int             m_margin;           // The margin to use for the ruler and buttons

	int             m_physicalInch;     // The number of pixels for an inch on screen
	int             m_movingtab;        // The tab-position being moved, or -1 if none
	int             m_offset;           // Internal offset of the tab-marker being moved.
	CharFormat      m_cfDefault;

	BOOL            m_showToolbar;
	BOOL            m_showRuler;
	BOOL            m_readOnly;
	BOOL            m_bWordWrap;

	// Sub-controls
	CRulerRichEdit  m_rtf;
	CRRECToolBar    m_toolbar;
	CRRECRuler      m_ruler;

	// Private helpers
	void    SetTabStops(LPLONG tabs, int size);
	void    UpdateTabStops();

	BOOL    CreateToolbar();
	BOOL    CreateRuler();
	BOOL    CreateRTFControl(BOOL autohscroll);
	void    CreateMargins();

	void    UpdateToolbarButtons();
	void    UpdateEditRect();

	void    SetEffect(int mask, int effect);
	void    SetAlignment(int alignment);

	void    LayoutControls(int width, int height);

	BOOL FixupTabStops(ParaFormat& para);
	void BuildTabStops(ParaFormat& para);
};

#endif // !defined(AFX_RULERRICHEDITCTRL_H__4CD13283_82E4_484A_83B4_DBAD5B64F17C__INCLUDED_)
