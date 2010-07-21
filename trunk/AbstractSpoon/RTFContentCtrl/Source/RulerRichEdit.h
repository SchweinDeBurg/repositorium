/* ==========================================================================
File :			RuleRichEdit.h

  Class :			CRulerRichEdit
  
	Author :		Johan Rosengren, Abstrakt Mekanik AB
	Iain Clarke
	
	  Date :			2004-04-17
	  
		Purpose :		"CRulerRichEdit" is derived from "CWnd". 
		
		  Description :	The class, in addition to the normal "CWnd", 
		  handles horizontal scrollbar messages - forcing an 
		  update of the parent (to synchronize the ruler). The 
		  change notification is called for the same reason. 
		  "WM_GETDLGCODE" is handled, we want all keys in a 
		  dialog box instantiation.
		  
			Usage :			This class is only useful as a child of the 
			"CRulerRichEditCtrl".
			
========================================================================*/

#if !defined(AFX_RULERRICHEDIT_H__E10A8ED3_2E1D_402E_A599_003214085F1A__INCLUDED_)
#define AFX_RULERRICHEDIT_H__E10A8ED3_2E1D_402E_A599_003214085F1A__INCLUDED_

// RulerRichEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRulerRichEdit window

#include "../../Common/UrlRichEditCtrl.h"

class CRulerRichEdit : public CUrlRichEditCtrl
{
public:
// Construction/destruction
	CRulerRichEdit();
	virtual ~CRulerRichEdit();

	void Paste(BOOL bSimple = FALSE);
	BOOL IsIMEComposing() const { return m_bIMEComposing; }
	BOOL SetParaFormat(PARAFORMAT2 &pf);
	BOOL SetParaFormat(PARAFORMAT &pf);
	BOOL GetParaFormat(PARAFORMAT2 &pf);
	BOOL GetParaFormat(PARAFORMAT &pf);

protected:
	BOOL m_bPasteSimple;
	BOOL m_bIMEComposing;

protected:
// Message handlers
	//{{AFX_MSG(CRulerRichEdit)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnDropFiles(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnIMEStartComposition(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnIMEEndComposition(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()

	virtual CLIPFORMAT GetAcceptableClipFormat(LPDATAOBJECT lpDataOb, CLIPFORMAT format);
	virtual int OnToolHitTest(CPoint pt, TOOLINFO* pTI) const;
	virtual HRESULT GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect);

};

#endif // !defined(AFX_RULERRICHEDIT_H__E10A8ED3_2E1D_402E_A599_003214085F1A__INCLUDED_)
