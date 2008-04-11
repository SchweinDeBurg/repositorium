//
// DynamicDialog sample.
//
// Shows the usage of dynamic dialog creation.
//

#include "stdafx.h"

#include "../../../Source/all.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Entry point.
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nShowCmd )
{
	int		rc = 0;

	// Setup app.
	if ( ClsGetApp()->Setup( hInstance, lpCmdLine, nShowCmd ))
	{
		// Check OS version.
		if ( ClsGetApp()->GetPlatformID() != VER_PLATFORM_WIN32_WINDOWS &&
		     ClsGetApp()->GetPlatformID() != VER_PLATFORM_WIN32_NT )
		{
			MessageBox( NULL, _T( "Windows 95/98/ME/NT/2000 required!" ), _T( "\"ClsFileDirTree\" Sample" ), MB_OK );
			return NULL;
		}
		
		// Create and open the dialog.
		ClsMessageBox mb;

		mb.Body() = ISEQ_CENTER ISEQ_BOLD _T( "ClsMessageBox\n\n" ) ISEQ_NORMAL
			    ISEQ_LEFT _T( "This class allows you to create dialogs simular to the way the\n" )
				      _T( "system MessageBox() API does. The major difference between the\n" )
				      _T( "system solution and the solution of this class is that this class\n" )
				      _T( "uses the \"" ) 
				      ISEQ_BOLD ISEQ_TEXTRGB( _T( "200,0,0" )) 
				      _T( "ClsInfoControl" ) 
				      ISEQ_NORMAL ISEQ_TEXTSYSCOLOR( IG_COLOR_BTNTEXT ) 
				      _T( "\" control to display it's body text.\n\n" )
				_T( "By using command sequences in the text this control can show\ntext styles like:\n\n" )
				ISEQ_CENTER ISEQ_BOLD _T( "Bold" ) ISEQ_NORMAL _T( "," ) 
				ISEQ_ITALIC _T( "Italic" ) ISEQ_NORMAL _T( "," )
				ISEQ_UNDERLINE _T( "Underline" ) ISEQ_NORMAL _T( ", and " )
				ISEQ_BOLD ISEQ_ITALIC ISEQ_UNDERLINE _T( "Combinations" ) ISEQ_NORMAL
				_T( "\n\n" )
				_T( "Also text in " )
				ISEQ_BACKRGB( _T( "255,255,245" ))
				ISEQ_TEXTRGB( _T( "255,0,0C" ))
				ISEQ_TEXTRGB( _T( "0,155,0o" ))
				ISEQ_TEXTRGB( _T( "0,0,255l" ))
				ISEQ_TEXTRGB( _T( "0,155,0o" ))
				ISEQ_TEXTRGB( _T( "255,0,0r" ))
				ISEQ_TEXTRGB( _T( "0,155,0s" ))
				ISEQ_TEXTSYSCOLOR( IG_COLOR_BTNTEXT )
				ISEQ_BACKSYSCOLOR( IG_COLOR_BTNFACE )
				_T( " is possible...\n\n" )
				_T( "Besides that text can also be\n\n" )
				ISEQ_LEFT _T( "left aligned\n" )
				ISEQ_RIGHT _T( "right aligned\n" )
				ISEQ_CENTER _T( "or centered" );
		mb.Buttons() = _T( "&OK" );
		mb.Title() = _T( "A litle test..." );
		mb.Flags() = ClsMessageBox::MBF_ICONQUESTION;

		FONTDATA fd;
		fd.sPointSize = 9;
		fd.sWeight = FW_NORMAL;
		fd.bItalic = FALSE;
		_tcscpy( fd.szFacename, _T( "Courier New"));

		mb.MsgBox( NULL );
	}
	return rc;
}