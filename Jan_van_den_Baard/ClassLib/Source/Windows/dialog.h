#ifndef _DIALOG_H_
#define _DIALOG_H_
//
// dialog.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "window.h"

// Structure used to pass font data with
// dynamic dialog creation.
typedef struct tagFontData
{
	short	sPointSize;
	short	sWeight;
	BOOL	bItalic;
	TCHAR	szFacename[ LF_FACESIZE ];
} FONTDATA, FAR *LPFONTDATA;

// Structure used to dynamically create a dialog.
// I.E. Dialog creation without using a resource
//      based template.
typedef struct tagCreateDialog
{
	LPCTSTR		pszTitle;
	DWORD		dwStyle;
	DWORD		dwExStyle;
	DWORD		dwHelpID;
	int		nLeft;
	int		nTop;
	int		nWidth;
	int		nHeight;
	LPFONTDATA	pFontData;
} CREATEDIALOG, FAR *LPCREATEDIALOG;

// This class is a ClsWindow derived class that
// handles dialog boxes.
class ClsDialog : public ClsWindow
{
	_NO_COPY( ClsDialog );
public:
	// Constructor.
	ClsDialog();

	// Destructor.
	virtual ~ClsDialog();

	// Interface
	int DoModal( ClsWindow& cwParent, LPCTSTR pszDialogName );
	int DoModal( LPCTSTR pszDialogName );
	int DoModal( int nDialogID );
	int DoModal( ClsWindow& cwParent, int nDialogID );
	int DoModal( LPCREATEDIALOG pCreateDialog );
	int DoModal( ClsWindow& cwParent, LPCREATEDIALOG pCreateDialog );
	BOOL Create( ClsWindow& cwParent, LPCTSTR pszDialogName );
	BOOL Create( LPCTSTR pszDialogName );
	BOOL Create( ClsWindow& cwParent, LPCREATEDIALOG pCreateDialog );
	BOOL Create( LPCREATEDIALOG pCreateDialog );
	BOOL EndDialog( int nResult );

	// Overidables.
	virtual BOOL OnOK();
	virtual BOOL OnCancel();
	virtual LRESULT OnInitDialog( PROPSHEETPAGE *pPropSheetPage );
	virtual void OnFontDistributed();

protected:
	// Base class override. Calls OnInitDialog() when the WM_INITDIALOG
	// message is received.
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

	// EnumChildWindows() procedure to set the dialog font
	// to all controls.
	static BOOL CALLBACK DistributeDialogFont( HWND hWndChild, LPARAM lParam );

	// WM_COMMAND message handler which will call OnOK() when the
	// ID is IDOK and OnCancel() when the ID is IDCANCEL.
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );

	// Helper to construct a memory based dialog template.
	static HGLOBAL CreateDialogTemplate( LPCREATEDIALOG pCreateDialog );
	
	// Our own modal dialog box message pump. We do not use the
	// system modal dialog box procedures because we need to call
	// the ClsWindow->PreTranslateMessage() function for the
	// controls in our global window list.
	int ModalDialog( ClsWindow *pParent, LPCTSTR pszDialogName, LPCREATEDIALOG pCreateDialog = NULL );

	// Data.
	BOOL		m_bIsModal;
	BOOL		m_bKeepModal;
	BOOL		m_bContinueModal;
	int		m_nResult;
};

#endif // _DIALOG_H_