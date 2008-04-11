// 
// A brain-dead MDI/Splitter sample...
// 
// Simple shows a file tree in the left pane and the MDI client
// in the right pane. Selecting a file in the tree will open it as
// a readonly MDI child.
//

#include "stdafx.h"

#include "../../../Source/all.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ID's
enum
{
	IDC_FILEDIRTREE	= 500,
	IDT_NEW, IDT_CLOSE, IDT_SAVE, IDT_SAVEALL, IDT_CUT, IDT_COPY,
	IDT_PASTE, IDT_DELETE, IDT_UNDO, IDT_REDO, IDT_PRINT,
	IDT_EXIT, IDT_FULLSCREEN
};

// Simple static toolbar.
class Toolbar : public ClsToolbar
{
	_NO_COPY( Toolbar );
public:
	// Construction/destruction.
	Toolbar() {;}
	virtual ~Toolbar() {;}

	// Create toolbar.
	BOOL Create( ClsWindow *pParent )
	{
		// Load the bitmap.
		ClsBitmap bm;
		if ( bm.LoadBitmap( MAKEINTRESOURCE( IDB_BITMAP )))
		{
			// Put it into a imagelist.
			if ( m_Images.Create( 16, 16, ILC_MASK, 13, 0 ))
			{
				// Add the bitmap.
				if ( m_Images.AddMasked( bm, RGB( 255, 0, 255 )) >= 0 )
				{
					// Create the toolbar.
					ClsRect rcEmpty( 0, 0, 0, 0 );
					if ( ClsToolbar::Create( pParent, rcEmpty, 0, WS_CHILD | WS_VISIBLE | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT ))
					{
						// Set the toolbar.
						SetImageList( m_Images );
						SetExtendedStyle( TBSTYLE_EX_DRAWDDARROWS );
						ButtonStructSize( sizeof( TBBUTTON ));
						SetButtonSize( 16, 15 );

						// Add the buttons.
						AddButtons( sizeof( m_tbTools ) / sizeof( TBBUTTON ), &m_tbTools[ 0 ] );
						AutoSize();
						return TRUE;
					}
				}
			}
		}
		return FALSE;
	}

protected:
	virtual LRESULT OnEraseBkgnd( ClsDC *pDC )
	{
		// Render toolbar background.
		ClsRect rc; GetClientRect( rc );
		pDC->FillRect( rc, ( HBRUSH )( COLOR_BTNFACE + 1 ));
		return 1;
	}

	// Handle TBN_GETINFOTIP notification.
	LRESULT OnGetInfoTip( LPNMTBGETINFOTIP pInfoTip, BOOL& bNotifyParent )
	{
		// Setup the info struct.
		TBBUTTONINFO	tbbi;
		tbbi.cbSize = sizeof( tbbi );
		tbbi.dwMask = TBIF_LPARAM;

		// Get button info.
		GetButtonInfo( pInfoTip->iItem, tbbi );

		// Any text?
		if ( tbbi.lParam )
		{
			// Setup the tooltip text. We have set these in the lParam
			// field of the TBBUTTON structure below.
			pInfoTip->pszText = ( LPTSTR )tbbi.lParam;
			bNotifyParent = FALSE;
			return TRUE;
		}

		// Pass to the base class.
		return ClsToolbar::OnGetInfoTip( pInfoTip, bNotifyParent );
	}

	// Data.
	ClsImageList	m_Images;
	static TBBUTTON	m_tbTools[ 19 ];
};

// Buttons for the toolbar. They are static for this sample but you can add, insert, remove
// change them dynamically aswell. I abuse the lParam field of the structure to store the
// tool-tip texts of the buttons to keep things a bit simple.
TBBUTTON Toolbar::m_tbTools[ 19 ] = 
{
	{ 0,			IDT_NEW,	TBSTATE_ENABLED,  TBSTYLE_BUTTON, { 0, 0 }, ( DWORD_PTR )_T( "New" ),		0 },
	{ 0,			0,		TBSTATE_ENABLED,  TBSTYLE_SEP,    { 0, 0 }, 0,					0 },
	{ 1,			IDT_CLOSE,	TBSTATE_ENABLED,  TBSTYLE_BUTTON, { 0, 0 }, ( DWORD_PTR )_T( "Close" ),		0 },
	{ 2,			IDT_SAVE,	TBSTATE_ENABLED,  TBSTYLE_BUTTON, { 0, 0 }, ( DWORD_PTR )_T( "Save" ),		0 },
	{ 3,			IDT_SAVEALL,	TBSTATE_ENABLED,  TBSTYLE_BUTTON, { 0, 0 }, ( DWORD_PTR )_T( "Save All" ),	0 },
	{ 0,			0,		TBSTATE_ENABLED,  TBSTYLE_SEP,    { 0, 0 }, 0,					0 },
	{ 5,			IDT_CUT,	TBSTATE_ENABLED,  TBSTYLE_BUTTON, { 0, 0 }, ( DWORD_PTR )_T( "Cut" ),		0 },
	{ 6,			IDT_COPY,	TBSTATE_ENABLED,  TBSTYLE_BUTTON, { 0, 0 }, ( DWORD_PTR )_T( "Copy" ),		0 },
	{ 7,			IDT_PASTE,	TBSTATE_ENABLED,  TBSTYLE_BUTTON, { 0, 0 }, ( DWORD_PTR )_T( "Paste" ),		0 },
	{ 8,			IDT_DELETE,	TBSTATE_ENABLED,  TBSTYLE_BUTTON, { 0, 0 }, ( DWORD_PTR )_T( "Delete" ),	0 },
	{ 0,			0,		TBSTATE_ENABLED,  TBSTYLE_SEP,    { 0, 0 }, 0,					0 },
	{ 10,			IDT_UNDO,	TBSTATE_ENABLED,  TBSTYLE_BUTTON, { 0, 0 }, ( DWORD_PTR )_T( "Undo" ),		0 },
	{ 11,			IDT_REDO,	TBSTATE_ENABLED,  TBSTYLE_BUTTON, { 0, 0 }, ( DWORD_PTR )_T( "Redo" ),		0 },
	{ 0,			0,		TBSTATE_ENABLED,  TBSTYLE_SEP,    { 0, 0 }, 0,					0 },
	{ 4,			IDT_PRINT,	TBSTATE_ENABLED,  TBSTYLE_BUTTON, { 0, 0 }, ( DWORD_PTR )_T( "Print" ),		0 },
	{ 0,			0,		TBSTATE_ENABLED,  TBSTYLE_SEP,    { 0, 0 }, 0,					0 },
	{ 12,			IDT_EXIT,	TBSTATE_ENABLED,  TBSTYLE_BUTTON, { 0, 0 }, ( DWORD_PTR )_T( "Exit" ),		0 },
	{ 0,			0,		TBSTATE_ENABLED,  TBSTYLE_SEP,    { 0, 0 }, 0,					0 },
	{ 9,			IDT_FULLSCREEN,	TBSTATE_ENABLED,  TBSTYLE_CHECK,  { 0, 0 }, ( DWORD_PTR )_T( "Full Screen" ),   0 },
};

// Simple ClsFileDirTree derived class
// that handles empty drives.
class FileDirTree : public ClsFileDirTree
{
	_NO_COPY( FileDirTree );
public:
	// Construction/destruction.
	FileDirTree() {;}
	virtual ~FileDirTree() {;}

protected:
	// Handle empty drives.
	virtual void OnDriveEmpty( ClsString strPath )
	{
		// Show that the drive is empty or it does not
		// contain any of the files specified by the filter.
		SHFILEINFO sfi;
		SHGetFileInfo( strPath, 0, &sfi, sizeof( sfi ), SHGFI_DISPLAYNAME );
		{
			ClsString str;
			str.Format( _T( "\"%s\"\r\nis empty or it does not contain any\r\n\"%s\"\r\nfiles." ), sfi.szDisplayName, ( LPCTSTR )FileFilter());
			MessageBox( str, _T( "MDI Sample" ), MB_ICONWARNING | MB_OK );
		}
	}
};
	
// Simple MDI child.
class MDIChildWnd : public ClsMDIChildWindow
{
	_NO_COPY( MDIChildWnd );
public:
	// Construction/destruction.
	MDIChildWnd() {;}
	virtual ~MDIChildWnd() {;}

	// Create a child.
	BOOL Create( ClsString& str, ClsMDIMainWindow *pFrame )
	{
		// We let the base create it for us. This will automatically call the
		// OnMDINCCreate() overidable in which we do our thing...
		ClsRect rc( CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT );

		// Create it.
		BOOL bMaximized; if ( pFrame->MDIGetActive( &bMaximized ) == NULL ) bMaximized = TRUE;
		return ClsMDIChildWindow::Create( str, WS_VISIBLE | ( bMaximized ? WS_MAXIMIZE : 0 ), rc, pFrame );
	}

protected:
	// Handle the WM_CLOSE messages.
	virtual LRESULT OnClose()
	{
		// Destroy the window.
		MDIDestroy();
		return 0;
	}

	// WM_SIZE handler.
	virtual LRESULT OnSize( UINT nSizeType, int nWidth, int nHeight )
	{
		// Size the edit control so it fits
		// into the client area.
		ClsRect rc;
		GetClientRect( rc );
		m_Edit.MoveWindow( rc );

		// Base class.
		return ClsMDIChildWindow::OnSize( nSizeType, nWidth, nHeight );
	}

	// WM_MDICREATE handler.
	virtual LRESULT OnMDINCCreate( LPCREATESTRUCT pCS )
	{
		// Make sure the child has a client edge. NOTE: Simply adding this
		// bit to the CREATESTRUCT.dwExStyle will not work... 
		//
		// We do it here like this also because the PreCreateWindow() overide
		// will not be called when creating MDI child windows.
		ModifyExStyle( 0, WS_EX_CLIENTEDGE );

		// Create edit control.
		ClsRect rc( 0, 0, 0, 0 );
		if ( m_Edit.Create( this, rc, 0, NULL, WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_LEFT | ES_READONLY | ES_MULTILINE ) == FALSE )
			return FALSE;

		// Setup the system fixed font for the edit control.
		ClsFont font;
		font.Attach( ::GetStockObject( SYSTEM_FIXED_FONT ), FALSE );
		m_Edit.SetFont( &font );

		// Load the file into memory.
		ClsFile file;
		ClsString name( pCS->lpszName );	// This contains the file name...
		TCHAR *pData = NULL;
		try
		{
			// Open the file, allocate a buffer and load it.
			file.Open( name, ClsFile::fileRead );
			DWORD dwSize = file.GetFileSize();
			pData = new TCHAR[ dwSize + 1 ];
			memset( pData, 0, ( dwSize + 1 ) * sizeof( TCHAR ));	// Make sure the buffer is 0-terminated.
			file.Read( pData, dwSize );

			// Set it into the edit control, release the
			// buffer and close the file.
			m_Edit.SetWindowText( pData );
			delete[] pData;
			file.Close();
		}
		catch( ClsFileException& )
		{
			// IO error on the file reading...
			MessageBox( _T( "File IO error" ), _T( "MDI Sample" ), MB_ICONERROR | MB_OK );
			if ( pData ) delete[] pData;
			return FALSE;
		}
		catch( ClsMemoryException& )
		{
			// Unable to allocate memory...
			file.Close();
			MessageBox( _T( "Out of memory" ), _T( "MDI Sample" ), MB_ICONERROR | MB_OK );
			return FALSE;
		}
		return TRUE;
	}

	// Data.
	ClsEdit	m_Edit;
};

// Simple MDI frame.
class MDIFrame : public ClsMDIMainWindow
{
	_NO_COPY( MDIFrame );
public:
	// Construction/destruction.
	MDIFrame() {;}
	virtual ~MDIFrame() {;}

protected:
	// A child window has been closed.
	void OnMDIChildRemoved( ClsMDIChildWindow *pWnd )
	{
		// Look it up in our array.
		for ( int i = 0; i < m_Children.GetSize(); i++ )
		{
			// Is this the one?
			if ( m_Children[ i ] == reinterpret_cast<MDIChildWnd *>( pWnd ))
			{
				// Free it and remove the entry
				// from the array.
				delete pWnd;
				m_Children.RemoveAt( i, 1 );
				break;
			}
		}
	}

	// WM_NOTIFY handler.
	virtual LRESULT OnNotify( LPNMHDR pNMHDR )
	{
		// Is this the correct notification from the tree?
		if ( pNMHDR->idFrom == IDC_FILEDIRTREE && pNMHDR->code == TVN_SELCHANGED )
		{
			// Get the current selected path.
			ClsString s;
			m_Tree.GetSelectedPath( s );

			// Was the selection a file?
			if ( m_Tree.TypeOfSelection() == ClsFileDirTree::STYPE_FILE )
			{
				// Create a new child.
				MDIChildWnd *pNew = new MDIChildWnd;

				// Get the path of the current tree selection.
				ClsString str;
				if ( m_Tree.GetSelectedPath( str ))
				{
					// Create the child window.
					if ( pNew->Create( str, this ))
					{
						// Add it to our array.
						m_Children.Add( pNew );
						return 0;
					}
				}
				// Failed...
				delete pNew;
			}
			return 0;
		}
		// Base class.
		return ClsMDIMainWindow::OnNotify( pNMHDR );
	}

	// We do not want the system to erase the background of
	// the frame window.
	virtual LRESULT OnEraseBkgnd( ClsDC *pDC )
	{ 
		return 1; 
	}

	// WM_CLOSE handler.
	virtual LRESULT OnClose()
	{
		// Make sure the children are all destroyed.
		while ( m_Children.GetSize()) m_Children[ 0 ]->MDIDestroy();
		PostQuitMessage( 0 );
		return 0;
	}

	// WM_SIZE handler.
	virtual LRESULT OnSize( UINT nSizeType, int nWidth, int nHeight )
	{
		// Size the splitter which will in turn size and
		// move it's panes.
		ClsRect rc, tb;
		GetClientRect( rc ); 
		
		// Does it exist already?
		if ( m_Toolbar.GetSafeHWND())
		{
			// Pass the message on to the toolbar.
			m_Toolbar.SendMessage( WM_SIZE, nSizeType, MAKELPARAM( nWidth, nHeight ));

			// Adjust client rectangle.
			m_Toolbar.GetWindowRect( tb );
			rc.Top() += tb.Height();
		}

		// Setup splitter rectangle. This will automatically
		// resize and reposition the panes aswell.
		m_Splitter.SetSplitRect( rc );
		m_Splitter.RedrawPanes();
		return 0;
	}

	// WM_CREATE overidable.
	virtual LRESULT OnCreate( LPCREATESTRUCT pCS )
	{
		// First the base.
		if ( ClsMDIMainWindow::OnCreate( pCS ) == -1 )
			return -1;

		// Create file/dir treeview.
		ClsRect rc( 0, 0, 0, 0 );
		if ( m_Tree.Create( this, rc, IDC_FILEDIRTREE, WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS ))
		{
			// Setup the tree.
			m_Tree.ModifyExStyle( 0, WS_EX_CLIENTEDGE );
			m_Tree.FileFilter() = _T( "*.c;*.cpp;*.h;*.hpp;*.txt" );
			m_Tree.DoubleBuffer() = TRUE;
			m_Tree.LoadingTextColor() = RGB( 255, 0, 0 );
			if ( m_Tree.SetupTree())
			{
				// Create splitter.
				if ( m_Splitter.Create( this, 0 ))
				{
					// Setup the splitter and it's panes.
					m_Splitter.SetPanes( m_Tree, GetMDIClient()->GetSafeHWND());
					m_Splitter.SetPaneMinSize( 100, 100 );
					m_Splitter.SetSplitterPosition( 200 );
					
					// Load the menu.
					if ( m_Menu.Load( MAKEINTRESOURCE( IDR_MENU )))
					{
						// Set it up.
						m_Menu.ConvertMenus();
						MDISetMenu( &m_Menu, m_Menu.GetSubMenu( 0 ));
						DrawMenuBar();
						// Create toolbar.
						if ( m_Toolbar.Create( this ))
							// Ready to go...
							return 0;
					}
				}
			}
		}
		return -1;
	}

	// WM_COMMAND overidable.
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
	{
		// A toolbar button?
		if ( nCtrlID >= IDT_NEW && nCtrlID <= IDT_FULLSCREEN )
		{
			// Was it the exit button?
			if ( nCtrlID == IDT_EXIT )
				SendMessage( WM_CLOSE );
			else
			{
				// Do somthing stupid like telling you
				// what button you clicked (like you don't know yourself...)
				TBBUTTONINFO tbi;
				tbi.cbSize = sizeof( tbi );
				tbi.dwMask = TBIF_LPARAM;
				if ( m_Toolbar.GetButtonInfo( nCtrlID, tbi ))
				{
					ClsString str;
					str.Format( _T( "You clicked the \"%s\" button on the toolbar!" ), tbi.lParam );
					MessageBox( str, _T( "MDI Sample" ), MB_ICONINFORMATION | MB_OK );
				}
			}
		}
		// Closeup on selecting Exit from the menu.
		else if ( nCtrlID == IDM_EXIT )
			SendMessage( WM_CLOSE );

		// Base class.
		return ClsMDIMainWindow::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
	}

	// Data.
	ClsArray<MDIChildWnd *>	m_Children;
	ClsSplitter		m_Splitter;
	Toolbar			m_Toolbar;
	FileDirTree		m_Tree;
	ClsXPMenu		m_Menu;
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
			MessageBox( NULL, _T( "Windows 98/ME/NT/2000/XP required!" ), _T( "MDI Sample" ), MB_OK );
			return NULL;
		}

		// Create and open the frame window.
		MDIFrame main;
		if ( main.Create( 0, NULL, _T( "MDI Sample" ), WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL ))
		{
			// Messagepump...
			rc = ( int )main.HandleMessageTraffic();
		}
	}
	return rc;
}
