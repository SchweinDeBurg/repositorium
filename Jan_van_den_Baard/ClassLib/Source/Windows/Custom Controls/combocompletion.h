#ifndef _COMBOCOMPLETION_H_
#define _COMBOCOMPLETION_H_
//
// Copyright (c) Chris Maunder 1997.
// Please feel free to use and distribute.
//
// Code implementation for this class library
// by Jan van den Baard.
//

#include "../controls/combobox.h"
#include "../../strings/string.h"

// This helper class has the actual completion code
// in it. It contains two methods which needs to be 
// called from their corresponding methods of a ClsComboBox
// derived class.
class ClsCompletion
{
public:
	ClsCompletion() {;}
	virtual ~ClsCompletion() {;}

	// Check for the delete and backspace keys.
	void PreTranslateMessage( ClsComboBox *pCombo, LPMSG pMsg )
	{
		_ASSERT_VALID( pCombo ); // Must be valid.
		_ASSERT_VALID( pCombo->GetSafeHWND()); // Must be valid.

		// If the style says there is no edit
		// control we do not use auto-completion.
		DWORD dwStyle = pCombo->GetStyle();
		if ( ! ( dwStyle & ( CBS_SIMPLE | CBS_DROPDOWN )))
			// Disable auto-completion.
			m_bAutoComplete = FALSE;
		else
		{
			// Need to check for backspace/delete. These will modify the text in
			// the edit box, causing the auto complete to just add back the text
			// the user has just tried to delete.
			if ( pMsg->message == WM_KEYDOWN || pMsg->message == 0x200e )
			{
				// Enable auto-completion by default.
				m_bAutoComplete = TRUE;

				// Check for the delete or backspace key.
				int nVirtKey = ( int ) pMsg->wParam;
				if ( nVirtKey == VK_DELETE || nVirtKey == VK_BACK )
					// Disable auto-completion.
					m_bAutoComplete = FALSE;
			}
		}
	}

	// improved by Christophe Jaillet
	void OnEditUpdate( ClsComboBox *pCombo )
	{
		_ASSERT_VALID( pCombo ); // Must be valid.
		_ASSERT_VALID( pCombo->GetSafeHWND()); // Must be valid.

		// if we are not to auto update the text, get outta here
		if ( ! m_bAutoComplete )
			return;

		// Get the text in the edit box
		ClsString str( pCombo->GetSafeHWND());
		int nLength = str.GetStringLength();

		// Currently selected range
		DWORD dwCurSel = pCombo->GetEditSel( NULL, NULL );
		WORD dStart = LOWORD( dwCurSel );
		WORD dEnd   = HIWORD( dwCurSel );

		// Search for first match
		int posLastRead = pCombo->FindString( -1, str );
		if (posLastRead == CB_ERR)
		{
			return;
		}

		// Here we have something to process !
		ClsString autoCompletedString;
		pCombo->GetLBText( posLastRead, autoCompletedString );

		while ( posLastRead != CB_ERR )
		{
			// Look for the next one
			int newPos = pCombo->FindString( posLastRead, str );

			// BE CAREFUL, WE DON'T WANT TO LOOP !!!
			if ( newPos == CB_ERR || newPos <= posLastRead )
			{
				break;
			}
			// Process the next string
			else
			{
				// Update the current position in the list
				posLastRead = newPos;

				// Get the next matching element from the list
				ClsString lastRead;
				pCombo->GetLBText( posLastRead, lastRead );

				// Compute the min of the length of the string we want to compare in order to
				// avoid overflow in the following loop
				int lengthMin = min( autoCompletedString.GetStringLength(), lastRead.GetStringLength());

				// Loop to find what is common to both strings
				int i = 0;
				while (( i < lengthMin ) && ( _totlower( autoCompletedString[ i ] ) == _totlower( lastRead[ i ] )))
				{
					i++;
				}

				// Keep only what is common to both strings
				autoCompletedString = autoCompletedString.Left( i );
			}
		}

		// Update the display
		pCombo->SetWindowText( autoCompletedString );

		// Set the text selection as the additional text that we have added
		if ( dEnd < nLength && dwCurSel != CB_ERR )
			pCombo->SetEditSel( dStart, dEnd );
		else
			pCombo->SetEditSel( nLength, -1 );
	}

protected:
	BOOL	m_bAutoComplete;
};

// ClsComboBox derived class which implements automatic
// text completion for combo-boxes with an edit control.
//
// Original code by Chris Maunder as found on the
// CodeProject web site (www.codeproject.com).
class ClsComboCompletion : public ClsComboBox
{
	_NO_COPY( ClsComboCompletion );
public:
	// Constructor.
	ClsComboCompletion() {;}

	// Destructor.
	virtual ~ClsComboCompletion() {;}

protected:

	// Check if completion needs to be on or off.
	virtual BOOL PreTranslateMessage( LPMSG pMsg )
	{
		// Call the helper class method.
		m_Completion.PreTranslateMessage( this, pMsg );

		// Call the base class.
		return ClsComboBox::PreTranslateMessage(pMsg);
	}

	// Do the actual completion.
	virtual LRESULT OnEditUpdate( BOOL& bNotifyParent )
	{
		// Call the helperclass method.
		bNotifyParent = FALSE;
		m_Completion.OnEditUpdate( this );
		return 0;
	}

	// Data.
	ClsCompletion	m_Completion;
};

#endif // _COMBOCOMPLETION_H_