/* ==========================================================================
    File :          RRECRuler.h

    Class :         CRRECRuler

    Author :        Johan Rosengren, Abstrakt Mekanik AB
                    Iain Clarke

    Date :          2004-04-19

    Purpose :       This class encapsulates a ruler that can be used with
                    "CRulerRichEditCtrl". The class is derived from "CWnd", and
                    draws a ruler.

    Description :   A plain "CWnd"-derived class. The mouse messages
                    "WM_LBUTTONDOWN", "WM_MOUSEMOVE" and "WM_LBUTTONUP" are handled,
                    and a registered message is sent to the control parent.

    Usage :         Only tested with "CRulerRichEditCtrl". Add a member to the
                    parent class, and create it with "Create". Handle the
                    registered message "urm_RULERACTION" in the parent class.
                    The parent class is also expected to handle
                    "urm_GETSCROLLPOS", which should return the current
                    horisontal scrollbar position.

   ========================================================================*/

#ifndef _RRECRULER_H_
#define _RRECRULER_H_

class CRRECRuler : public CWnd
{
public:
// Construction/creation/destruction
	CRRECRuler();
	virtual ~CRRECRuler();
	virtual BOOL Create(const CRect& rect, CWnd* parent, UINT id);

// Implementation
	void SetMode(int mode);
	int GetMode() const;
	void SetMargin(int margin);
	void SetTabStops(const CDWordArray& arr);
	void SetBackgroundColor(COLORREF color)
	{
		m_crBack = color;
		Invalidate();
	}

protected:
// Message handlers
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnEnable(BOOL bEnable);
	DECLARE_MESSAGE_MAP()

private:
// Internal data
	int m_margin;       // Left margin of ruler in pixels
	int m_mode;         // MODE_INCH/MODE_METRIC, what units
	                    // to use for the ruler measure.
	int m_physicalInch; // The number of device pixels for a
	                    // physical inch of the window CDC

	CDWordArray m_tabs; // Tabulator settings in device pixels
	COLORREF m_crBack;
};

#endif // _RRECRULER_H_
