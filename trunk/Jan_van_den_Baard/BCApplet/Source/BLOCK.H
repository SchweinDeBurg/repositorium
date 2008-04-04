//
//	block.h
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	Textblock configuration dialog.
//

// Block dialog class.
class BlockEdit : public ClsDialog
{
	_NO_COPY( BlockEdit );

public:
	// Construction/destruction.
	BlockEdit();
	virtual ~BlockEdit();
	
	// Implementation.
	BOOL EditBlock( ClsWindow& Parent, LPBLOCK pBlock, LPPARSER pParser );
	
protected:
	// Overidables.
	virtual LRESULT OnInitDialog( LPPROPSHEETPAGE pPsp );
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );

	// Helpers.
	BOOL CloneBlock( LPBLOCK pBlockA, LPBLOCK pBlockB );
	void SetupControls();

	// Data.
	ClsEdit		m_Name, m_Start, m_End;
	ClsCheckBox	m_SOL;
	ClsRadioButton	m_EOL, m_White, m_Chars;
	ClsColorPicker	m_Color, m_BkColor;
	LPPARSER	m_pParser;
	BLOCK		m_Block;
};
