//
// CRC sample.
//
// Shows the usage of the ClsCrc32 classes.
//

#include "stdafx.h"

#include "../../../Source/all.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// A simple dialog class.
class CrcDlg : public ClsDialog
{
	_NO_COPY( CrcDlg );
public:
	// No-op constructor and destructor.
	CrcDlg() {;}
	virtual ~CrcDlg() {;}

protected:
	// WM_INITDIALOG handler.
	virtual LRESULT OnInitDialog( PROPSHEETPAGE *psp )
	{
		// Attach FileBrowser control.
		m_Browser.Attach( GetDlgItemHandle( IDC_FILE ));
		m_Browser.ShowFiles() = TRUE;
		m_Browser.LoadingTextColor() = RGB( 255, 0, 0 );
		return ClsDialog::OnInitDialog( psp );
	}
	
	// WM_COMMAND handler.
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
	{
		// Wassup?
		switch ( nCtrlID )
		{
			case	IDC_CRC:
			{
				// Use SEH to capture IO and memory errors.
				LPBYTE buf = NULL;
				try
				{
					ClsString str;
					ClsFile	  file;

					// Get the selected file name.
					str = m_Browser;

					// Valid?
					if ( str.GetStringLength())
					{
						ClsCrc32 crc;
						DWORD dwSize;

						// Allocate IO buffer.
						buf = new BYTE[ 32768 ];

						// Open the file.
						file.Open( str, ClsFile::fileRead );

						// Read the file in 32 KByte chunks.
						while (( dwSize = file.Read( buf, 32768 )) > 0 )
							// Add the CRC of this chunk to the
							// previous ones.
							crc.CrcAdd( buf, dwSize );

						// Close the file.
						file.Close();

						// Format CRC output string and set it to the
						// static control.
						str.Format( _T( "%lu ($%08.8lx)" ), crc.CrcGet(), crc.CrcGet());
						::SetWindowText( GetDlgItemHandle( IDC_SETCRC ), str );
					}
					else
					{
						// Duhh.
						MessageBox( _T( "You must specify a file." ), _T( "\"ClsCrc\" Sample" ));
						m_Browser.SetFocus();
					}
				}
				catch( ClsFileException& e) 
				{
					// Format error string and display an
					// error message box.
					ClsString err;
					err.FormatMessage( e.m_nCause );
					MessageBox( err, _T( "\"ClsCrc\" Sample" ), MB_ICONERROR | MB_OK  );
				}
				catch( ClsMemoryException& ) 
				{
					MessageBox( _T( "Out of memory" ), _T( "\"ClsCrc\" Sample" ), MB_ICONERROR | MB_OK );
				}
				// Free IO buffer if one was allocated.
				if ( buf ) delete buf;
				break;
			}
		}
		// Base class.
		return ClsDialog::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
	}

	// Data.
	ClsFileDirBrowser	m_Browser;
};

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
			MessageBox( NULL, _T( "Windows 95/98/ME/NT/2000/XP required!" ), _T( "\"ClsCrc\" Sample" ), MB_OK );
			return NULL;
		}

		// Create and open the dialog.
		CrcDlg	dlg;
		dlg.DoModal( IDD_CRC );
	}
	return rc;
}
