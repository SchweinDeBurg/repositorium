#ifndef _ANIMATION_H_
#define _ANIMATION_H_
//
// animation.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "commoncontrol.h"

// A wrapper for the animation common control.
class ClsAnimation : public ClsCommonControl
{
	_NO_COPY_PARAM( ClsAnimation, ClsCommonControl )
public:
	// Constructor.
	ClsAnimation() : ClsCommonControl( ICC_ANIMATE_CLASS ) 
	{;}

	// Destructor.
	virtual ~ClsAnimation()
	{;}

	// Create the control.
	inline BOOL Create( ClsWindow *parent, UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | ACS_CENTER )
	{
		_ASSERT( m_hWnd == NULL ); // May not be valid.
		
		// Create the control.
		HWND hWnd = Animate_Create( parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID, dwStyle, ClsGetInstanceHandle());
		if ( hWnd )
			// Attach the control.
			return Attach( hWnd, TRUE );
		return FALSE;
	}

	// Close the animation
	inline void Close()
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Close the animation.
		Animate_Close( m_hWnd );
	}

	// Open an animation.
	inline BOOL Open( LPSTR pszName, HINSTANCE hInstance = NULL )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Open the animation.
		return Animate_OpenEx( m_hWnd, hInstance ? hInstance : ClsGetResourceHandle(), pszName );
	}

	// Play the animation.
	inline BOOL Play( UINT nFrom = 0, UINT nTo = -1, UINT cRepeat = -1 )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Play the animation.
		return Animate_Play( m_hWnd, nFrom, nTo, cRepeat );
	}

	// Seek in the animation.
	inline BOOL Seek( UINT nFrame )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Seek to the frame.
		return Animate_Seek( m_hWnd, nFrame );
	}

	// Stop the animation.
	inline BOOL Stop()
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Stop the animation.
		return Animate_Stop( m_hWnd );
	}

protected:
	// Overidables:
	virtual LRESULT OnStart( BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnStop(  BOOL& bNotifyParent ) { return 0; }

	// Notification handler.
	virtual LRESULT OnReflectedCommand( UINT nNotifyCode, BOOL& bNotifyParent )
	{
		// What's the code?
		switch ( nNotifyCode )
		{
			case	ACN_START: return OnStart( bNotifyParent );
			case	ACN_STOP:  return OnStop( bNotifyParent );
		}
		// Call the base class.
		return ClsCommonControl::OnReflectedCommand( nNotifyCode, bNotifyParent );
	}
};

#endif // _ANIMATION_H_