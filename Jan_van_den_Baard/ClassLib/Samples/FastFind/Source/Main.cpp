//
// FastFind
//
// Mainly shows the usage of the classes:
//
//	ClsBoyerMoore		- The search class.
//	ClsFindFile		- Directory scanning.
//	ClsWorkerThread		- Threading.
//	ClsLayoutEngine		- Resizable GUI.
//

#include "stdafx.h"

#include "../../../Source/all.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Thread messages posted to the main dialog by the
// search thread.
#define THREAD_STOPPED		WM_USER + 3000
#define THREAD_PROCESSING	WM_USER + 3001	// lParam = ( ClsString * )pFileName
#define THREAD_FOUND		WM_USER + 3002  // wParam = nLineNr, lParam = ( ClsString * )pLineStr

// A simple list view which re-sizes the columns when
// the control itself is resized.
class List : public ClsListView
{
	_NO_COPY( List );
public:
	// No-op constructor and destructor.
	List() {;}
	virtual ~List() {;}

protected:
	// WM_SIZE handler.
	virtual LRESULT OnSize( UINT nSizeType, int nWidth, int nHeight )
	{
		SetColumnWidth( 0, nWidth / 2 );
		SetColumnWidth( 1, LVSCW_AUTOSIZE_USEHEADER );
		SetColumnWidth( 2, LVSCW_AUTOSIZE_USEHEADER );
		return ClsListView::OnSize( nSizeType, nWidth, nHeight );
	}
};

// ClsDialog derived class which encapsulates the whole
// FastFind functionality.
class FastFindDlg : public ClsSizeDialog
{
	_NO_COPY( FastFindDlg );
public:
	// Constructor.
	FastFindDlg() {;}

	// Destructor.
	virtual ~FastFindDlg() 
	{ 
		// If the icon exists, destroy it.
		if ( m_hIcon ) 
			::DestroyIcon( m_hIcon );
	}

protected:
	// Convert the tabs in the string to spaces.
	// Uses a tab-size of 8.
	static ClsString *ConvertForPrint( LPCTSTR pszSource )
	{
		// Compute the "real" length.
		int nNewLen = 0;
		for ( int i = 0; i < ( int )_tcslen( pszSource ); i++ )
		{
			// Is it a tab?
			if ( pszSource[ i ] == _T( '\t' ))
				// Add the number of spaces to
				// the next tab stop.
				nNewLen += ( 8 - ( nNewLen % 8 ));
			else
				// One more.
				nNewLen++;
		}

		// Allocate buffer which can hold the
		// "expanded" string.
		ClsString *str = new ClsString();
		str->AllocateBuffer(( nNewLen + 1 ) * sizeof( TCHAR ));
		str->SetStringLength( nNewLen );

		// Expand tabs.
		LPTSTR pszDest = *str;
		int nPos = 0;
		for ( int i = 0; i < ( int )_tcslen( pszSource ); i++ )
		{
			// A tab?
			if ( pszSource[ i ] == _T( '\t' ))
			{
				// Compute the number of spaces to the next
				// tab-stop.
				int	nSpace = ( 8 - ( nPos % 8 ));

				// Add the spaces.
				while ( nSpace )
				{
					pszDest[ nPos ] = _T( ' ' );
					nSpace--;
					nPos++;
				}
			}
			else
				// Copy the character.
				pszDest[ nPos++ ] = pszSource[ i ];
		}
		return str;
	}

	// Add a backslash to the string.
	static void AddBackslash( ClsString& str )
	{
		// Add a backslash to the string if not yet present.
		if ( str[ str.GetStringLength() - 1 ] != _T( '\\' ))
			str += _T( '\\' );
	}

	// Scan directory. This code get's called on 
	// the spawned thread. It uses recursion to traverse
	// sub-directories when necessary.
	BOOL ScanDirectory( LPCTSTR pszDir )
	{
		ClsString	strPath, str;
		ClsFindFile	find;
		ClsStdioFile	file;

		// Are we still running?
		if ( m_Thread.Wait() != ClsWorkerThread::wtRunning )
			return FALSE;

		// Do we need to scan the sub-directories?
		if ( m_bRecur )
		{
			// Assign path.
			strPath = pszDir;

			// Add a backslash if not yet present.
			AddBackslash( strPath );

			// Add filter.
			strPath += _T( "*.*" );

			// Start the scan.
			if ( find.FindFile( strPath ))
			{
				// Continue until we are done...
				do
				{
					// Still running?
					if ( m_Thread.Wait() != ClsWorkerThread::wtRunning )
					{
						// Close handle and exit.
						find.Close();
						return FALSE;
					}

					// Is this a directory?
					if ( ! find.IsDots() && find.IsDirectory())
					{
						// Get the full path of the directory.
						find.GetFilePath( strPath );

						// Scan this directory. Recursion hard at work...
						if ( ScanDirectory( strPath ) == FALSE )
						{
							// Probably a shutdown event during
							// recursion. Close handle and return FALSE.
							find.Close();
							return FALSE;
						}
					}
				} while ( find.FindNextFile());
				// Close the handle.
				find.Close();
			}
		}

		// Get the index of the first delimited part of the string.
		int nIndex = m_StrType.GetDelimitedPart( _T( ';' ), 0, str );

		// Valid?
		if ( nIndex > 0 )
		{
			// Scan using all filters.
			do
			{
				// Assign path.
				strPath = pszDir;

				// Add a backslash if necessary.
				AddBackslash( strPath );

				// Add the filter string.
				strPath += str;

				// Scan the directory.
				if ( find.FindFile( strPath ))
				{
					// Continue until we are done.
					do
					{
						// Still running?
						if ( m_Thread.Wait() != ClsWorkerThread::wtRunning )
						{
							// Close handle and exit.
							find.Close();
							return FALSE;
						}

						// We only show the correct files.
						if ( ! find.IsDots() && ! find.IsDirectory())
						{
							// Get the file name.
							ClsString strName;
							if ( find.GetFilePath( strName ))
							{
								// Show the file we are processing. Simply do a PostMessage()
								// to the dialog which handles the GUI stuff. It will also delete
								// the ClsString we allocate here.
								ClsString *pStr = new ClsString( strName );
								PostMessage( THREAD_PROCESSING, 0, ( LPARAM )pStr );

								// Gracefully handle IO errors.
								try
								{
									// Read the file line-by-line...
									TCHAR szBuf[ 4096 ];
									file.Open( strName, _T( "r" ));
									m_nFiles++;
									int nLine = 0;
									
									while ( file.GetS( szBuf, 4096 ))
									{
										// Still running?
										if ( m_Thread.Wait() != ClsWorkerThread::wtRunning )
										{
											// Close the file and the
											// find-file handles and exit.
											file.Close();
											find.Close();
											return FALSE;
										}

										// Get the base pointer of the buffer.
										TCHAR *ptr = szBuf;
										int    nIdx;

										// Increase line number.
										nLine++;

										// Find all occurrences on this line.
										BOOL bAnyFound = FALSE;
										while (( _tcslen( ptr )) && (( nIdx = m_BoyerMoore.FindForward(( char * )ptr, ( int )_tcslen( ptr ))) >= 0 ))
										{
											// Are we at the first found entry?
											if ( bAnyFound == FALSE )
											{
												// Strip white spaces from the end of the input buffer.
												while ( _istspace( szBuf[ _tcslen( szBuf ) - 1 ] ))
													szBuf[ _tcslen( szBuf ) - 1 ] = 0;

												// Increase lines-found counter.
												m_nLines++;
											}

											// Found a least one.
											bAnyFound = TRUE;

											// Convert the line for print and post it to the
											// main thread for the GUI stuff.
											ClsString *pStr = ConvertForPrint( szBuf );
											PostMessage( THREAD_FOUND, nLine, ( LPARAM )pStr );
											
											// Increase search pointer so we can search the rest of the line.
											ptr += nIdx + 1;
										}
									}
									// Close file.
									file.Close();
								}
								catch( ClsException& )
								{
									// Continue on IO errors...
									continue;
								}
							}
						}
					} while ( find.FindNextFile());
					// Close the handle.
					find.Close();
				}
			// Next filter...
			} while (( nIndex = m_StrType.GetDelimitedPart( _T( ';' ), nIndex, str )) > 0 );
		}
		return TRUE;
	}

	// Thread which does the actual find-in-files.
	static UINT WINAPI FindThreadProc( LPVOID pParam )
	{
		// The parameter is a pointer to the FastFindDlg object
		// which has initiated this search thread.
		FastFindDlg *pDlg = ( FastFindDlg * )pParam;

		// Scan the directory...
		pDlg->m_Critical.Lock();
		pDlg->ScanDirectory( pDlg->m_StrFolder );
		pDlg->m_Critical.Unlock();

		// Done.
		MessageBeep(( UINT )-1 );

		// Make sure the main thread knows it too.
		pDlg->PostMessage( THREAD_STOPPED );
		return 0;
	}

	// WM_DESTROY message handler...
	virtual LRESULT OnDestroy()
	{
		MSG msg;
		// Make sure that any messages still pending are
		// processed.
		while ( PeekMessage( &msg, GetSafeHWND(), THREAD_PROCESSING, THREAD_FOUND, PM_REMOVE ))
			// Delete the object.
			delete ( ClsString * )msg.lParam;

		// The base class.
		return ClsSizeDialog::OnDestroy();
	}

	// WM_INITDIALOG message handler...
	virtual LRESULT OnInitDialog( PROPSHEETPAGE *p )
	{
		// Loadup and set icon.
		m_hIcon = ( HICON )::LoadImage( ClsGetApp()->GetResourceHandle(), MAKEINTRESOURCE( IDI_FIND ), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR );
		SetIcon( m_hIcon, FALSE );

		// Create layout engine controls. We do this before "ClsDialog" get's
		// a chance to distribute it's font to it's children so that when it does
		// the layout engine controls will get the font set too.
		m_Master.Create( *this, Offsets( 6, 6, 6, ::GetSystemMetrics( SM_CYHSCROLL )), LAYOUT_Master, TRUE, LAYOUT_Horizontal, FALSE, TAG_END );
		m_Left.Create( *this, Offsets( 0, 0, 0, 0 ), LAYOUT_Horizontal, FALSE, LAYOUT_EqualMinWidth, TRUE, TAG_END );
		m_Right.Create( *this, Offsets( 0, 0, 0, 0 ), LAYOUT_Horizontal, FALSE, TAG_END );
		m_StatBut.Create( *this, Offsets( 0, 0, 0, 0 ), LAYOUT_Horizontal, TRUE, TAG_END );
		m_LeftRight.Create( *this, Offsets( 0, 0, 0, 0 ), LAYOUT_Spacing, 6, LAYOUT_Horizontal, TRUE, TAG_END );

		// Setup info bar control.
		m_Proc.Attach( GetDlgItemHandle( IDC_PROC ));
		m_Proc.CompactAsPath() = TRUE;

		// Setup list.
		m_List.Attach( GetDlgItemHandle( IDC_LIST ));
		m_List.SetExtendedListViewStyle( LVS_EX_FULLROWSELECT );
		m_List.InsertColumn( 0, _T( "File" ));
		m_List.InsertColumn( 1, _T( "Line Nr." ));
		m_List.InsertColumn( 2, _T( "Line" ));

		// Setup other controls.
		m_Find.Attach( GetDlgItemHandle( IDC_FIND ));
		m_Type.Attach( GetDlgItemHandle( IDC_TYPES ));
		m_Case.Attach( GetDlgItemHandle( IDC_CASE ));
		m_Recur.Attach( GetDlgItemHandle( IDC_RECUR ));
		m_Folder.Attach( GetDlgItemHandle( IDC_FOLDER ));
		m_Folder.ShowFiles() = FALSE;
		m_Folder.LoadingTextColor() = RGB( 255, 0, 0 );
		m_Occ.Attach( GetDlgItemHandle( IDC_OCC ));
		m_Go.Attach( GetDlgItemHandle( IDC_GO ));
		m_About.Attach( GetDlgItemHandle( IDC_ABOUT ));

		// Call the base class. The base will also distribute the
		// dialog font.
		return ClsSizeDialog::OnInitDialog( p );
	}

	// Called after ClsDialog has distributed the dialog
	// font to it's children.
	virtual void OnFontDistributed()
	{
		// Setup static controls. Attach them to the objects and
		// add them to the layout engine controls.
		for ( int i = 0; i < 5; i++ )
		{
			m_Statics[ i ].Attach( GetDlgItemHandle( IDC_STATIC_1 + i ));
			m_Left.AddSpacingMember();
			m_Left.AddMember( &m_Statics[ i ], NULL, ATTR_FixMinSize, TRUE, ATTR_RightAlign, TRUE, TAG_END );
			m_Left.AddSpacingMember();
		}

		// Add the controls to the layout engine controls.
		m_Right.AddMember( &m_Find, NULL, TAG_END );
		m_Right.AddMember( &m_Type, NULL, TAG_END );
		m_Right.AddMember( &m_Folder, NULL, TAG_END );
		m_Right.AddMember( &m_Proc, NULL, ATTR_UseControlSize, TRUE, TAG_END );
		m_StatBut.AddMember( &m_Occ, NULL, TAG_END );
		m_StatBut.AddMember( &m_About, NULL, ATTR_UseControlSize, TRUE, ATTR_FixMinWidth, TRUE, TAG_END );
		m_StatBut.AddMember( &m_Go, NULL, ATTR_UseControlSize, TRUE, ATTR_FixMinWidth, TRUE, TAG_END );
		m_Right.AddMember( &m_StatBut, NULL, TAG_END );

		// Get the minimum size of the "m_Right" layout-engine without
		// the checkboxes.
		ClsSize szRight; m_Right.OnGetMinSize( szRight );

		// Add the checkboxes.
		m_Right.AddMember( &m_Recur, NULL, ATTR_FixMinWidth, TRUE, TAG_END );
		m_Right.AddMember( &m_Case, NULL, ATTR_FixMinWidth, TRUE, TAG_END );

		// Combine left and right layout-engine controls.
		m_LeftRight.AddMember( &m_Left, NULL, ATTR_FixMinWidth, TRUE, ATTR_FixHeight, szRight.CY(), TAG_END );
		m_LeftRight.AddMember( &m_Right, NULL, TAG_END );

		// Setup the master layout engine.
		m_Master.AddMember( &m_LeftRight, NULL, ATTR_FixMinHeight, TRUE, TAG_END );
		m_Master.AddMember( &m_List, NULL, TAG_END );

		// Any errors?
		if ( m_Master.Error())
		{
			// Bye...
			EndDialog( TRUE );
			return;
		}

		// Compute the minimum size of the master group.
		if ( m_Master.OnGetMinSize( m_MinSize ) == FALSE )
		{
			// Bye...
			EndDialog( 0 );
			return;
		}

		// Add frame and caption sizes so that we know the minimum
		// size of the dialog.
		m_MinSize.CY() += ( ::GetSystemMetrics( SM_CYFRAME ) * 2 ) + ::GetSystemMetrics( SM_CYCAPTION );
		m_MinSize.CX() += ::GetSystemMetrics( SM_CXFRAME ) * 2;
		
		// Relayout the master layout engine control.
		m_Master.Relayout();

		// No thread running.
		m_bSearchInProgress = FALSE;
		
		// Call the base class.
		ClsSizeDialog::OnFontDistributed();
	}

	// WM_CLOSE handler.
	virtual LRESULT OnClose() 
	{
		// Is the search thread running?
		if ( m_bSearchInProgress ) 
		{ 
			// Stop the thread and delete the ClsWorkerThread object. 
			//
			// At this point there may still be a lot of
			// messages pending from the search thread. 
			// These messages are processed by the main 
			// thread before the window is actually destroyed.
			m_Thread.Stop();
		} 
		// Base class.
		return ClsDialog::OnClose(); 
	}

	// This is not allowed to end the dialog.
	virtual BOOL OnOK() { return FALSE; }

	// Escape key pressed.
	virtual BOOL OnCancel() 
	{
		// Search thread running?
		if ( m_bSearchInProgress )
		{
			// Stop the thread and do not
			// close the dialog.
			m_Thread.Stop();
			return FALSE;
		}
		// Close the dialog.
		return TRUE;
	}

	// WM_SIZE message handler.
	virtual LRESULT OnSize( UINT nSizeType, int nWidth, int nHeight )
	{
		// Tell the master layout engine to layout it's
		// members.
		m_Master.Relayout();

		// Call the base class.
		return ClsSizeDialog::OnSize( nSizeType, nWidth, nHeight );
	}

	// Window procedure override.
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		// What's up?
		switch ( uMsg )
		{
			case	WM_GETMINMAXINFO:
			{
				// Fill in the minimum size of the dialog.
				LPMINMAXINFO pmmi = ( LPMINMAXINFO )lParam;
				pmmi->ptMinTrackSize.x = m_MinSize.CX();
				pmmi->ptMinTrackSize.y = m_MinSize.CY();
				return 0;
			}

			case	THREAD_STOPPED:
			{
				// Destroy thread.
				m_Thread.Destroy();

				// Enable GUI.
				m_Case.EnableWindow();
				m_Recur.EnableWindow();
				m_Find.EnableWindow();
				m_Type.EnableWindow();
				m_Folder.EnableWindow();
				m_About.EnableWindow();
				m_Go.SetWindowText( _T( "Start" ));
				m_Proc.SetWindowText( NULL );
				m_bSearchInProgress = FALSE;

				// Setup columns.
				m_List.SetColumnWidth( 0, LVSCW_AUTOSIZE_USEHEADER );
				m_List.SetColumnWidth( 1, LVSCW_AUTOSIZE_USEHEADER );
				m_List.SetColumnWidth( 2, LVSCW_AUTOSIZE_USEHEADER );

				// Final report.
				ClsString str;
				m_Critical.Lock();
				str.Format( _T( "%ld on %ld lines (Searched: %ld files)." ), m_nItem, m_nLines, m_nFiles );
				m_Critical.Unlock();
				m_Occ.SetWindowText( str );
				return 0;
			}
			
			case	THREAD_PROCESSING:
				// Only update GUI when the thread is still alive.
				if ( m_Thread.IsRunning())
					// Setup info-bar.
					m_Proc.SetWindowText( *(( ClsString * )lParam ));

				// We always must delete the string otherwise
				// we can have a giant memory leak.
				delete ( ClsString * )lParam;
				return 0;

			case	THREAD_FOUND:
			{
				// Only update GUI when the thread is still alive.
				ClsString *str = ( ClsString *)lParam, tmp;
				if ( m_Thread.IsRunning())
				{
					// Pause the thread so we do not pile up to many
					// message during this action.
					m_Thread.Pause();

					// Insert the information into the listview.
					LVITEM it;
					tmp = m_Proc;
					it.mask = LVIF_TEXT;
					it.iItem = m_nItem;
					it.iSubItem = 0;
					it.pszText = tmp;
					m_List.InsertItem( it );
					tmp = ( UINT )wParam;
					m_List.SetItemText( m_nItem, 1, tmp );
					m_List.SetItemText( m_nItem, 2, *str );

					// Update the occurrences control.
					tmp = m_nItem++;
					m_Occ.SetWindowText( tmp );

					// Go on.
					m_Thread.Run();
				}

				// Delete the string.
				delete str;
				break;
			}
		}
		// Base class.
		return ClsSizeDialog::WindowProc( uMsg, wParam, lParam );
	}

	// WM_COMMAND message handler.
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
	{
		switch ( nCtrlID )
		{
			case	IDC_FIND:
			{
				// Contents changed?
				if ( nNotifyCode == EN_CHANGE )
					// Enable start button.
					m_Go.EnableWindow(( BOOL )( m_Find.GetWindowTextLength() && m_Folder.GetWindowTextLength() && m_Type.GetWindowTextLength()));
				break;
			}

			case	IDC_TYPES:
			{
				// Contents changed?
				if ( nNotifyCode == EN_CHANGE )
					// Enable start button.
					m_Go.EnableWindow(( BOOL )( m_Find.GetWindowTextLength() && m_Folder.GetWindowTextLength() && m_Type.GetWindowTextLength()));
				break;
			}
			case	IDC_FOLDER:
			{
				// Contents changed?
				if ( nNotifyCode == EN_CHANGE )
					// Enable start button.
					m_Go.EnableWindow(( BOOL )( m_Find.GetWindowTextLength() && m_Folder.GetWindowTextLength() && m_Type.GetWindowTextLength()));
				break;
			}

			case	IDC_ABOUT:
			{
				// Show the about box.
				ClsMessageBox::MsgBox( *this, _T( "&OK" ),
						       ISEQ_CENTER _T( "The code for the ClsWorkerThread class is based on the\n" ) 
						       _T( "Using Worker Threads article by Joseph M. Newcomer as found on\n\n" )
						       ISEQ_BOLD ISEQ_UNDERLINE _T( "The Code Project\n" ) ISEQ_NOUNDERLINE _T( "www.codeproject.com" ),
						       _T( "\"FastFind\" Sample" ), 0 );
				return 0;
			}

			case	IDC_GO:
				// Search thread running?
				if ( m_bSearchInProgress )
					// Stop it.
					m_Thread.Stop();
				else
				{
					// Clear brainchild control.
					m_List.DeleteAllItems();
					m_Occ.SetWindowText( _T( "0" ));

					// Setup data for the search thread.
					m_Critical.Lock();
					m_nItem = m_nLines = m_nFiles = 0;
					m_bRecur = m_Recur.IsChecked();
					m_bCase = m_Case.IsChecked();
					m_StrFind = m_Find;
					m_StrType = m_Type;
					m_StrFolder = m_Folder;

					// Setup Boyer-Moore object.
					m_BoyerMoore.SetSearchString( m_StrFind.GetMBS() );
					m_BoyerMoore.SetCaseMode( m_bCase );
					m_Critical.Unlock();

					// Create thread.
					if ( m_Thread.Start( FindThreadProc, this ))
					{
						// Thread running.
						m_bSearchInProgress = TRUE;

						// Disable GUI.
						m_Case.EnableWindow( FALSE );
						m_Recur.EnableWindow( FALSE );
						m_Find.EnableWindow( FALSE );
						m_Type.EnableWindow( FALSE );
						m_Folder.EnableWindow( FALSE );
						m_About.EnableWindow( FALSE );

						// Change button.
						m_Go.SetWindowText( _T( "Stop" ));
						m_Go.SetFocus();
					}
				}
				break;
		}
		// Base class.
		return ClsSizeDialog::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
	}

	// Control objects.
	ClsLayoutEngine		m_Master, m_Left, m_Right, m_StatBut, m_LeftRight;
	ClsLStatic		m_Statics[ 7 ];
	List		m_List;
	ClsLPushButton		m_Go, m_About;
	ClsLCheckBox		m_Case, m_Recur;
	ClsLEdit		m_Find, m_Type;
	ClsFileDirBrowser	m_Folder;
	ClsInfoBar		m_Proc;
	ClsLStatic		m_Occ;

	// Misc data.
	ClsSize			m_MinSize;
	HICON			m_hIcon;
	int			m_nOccurences, m_nItem;

	// Thread related data.
	ClsWorkerThread	        m_Thread;
	BOOL			m_bRunning, m_bSearchInProgress, m_bRecur, m_bCase;
	ClsBoyerMoore		m_BoyerMoore;
	ClsString		m_StrFind, m_StrType, m_StrFolder;
	int			m_nLines, m_nFiles;
	ClsCriticalSection	m_Critical;
};

// Entry point.
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nShowCmd )
{
	// Setup app.
	if ( ClsGetApp()->Setup( hInstance, lpCmdLine, nShowCmd ))
	{
		// Check OS version.
		if ( ClsGetApp()->GetPlatformID() != VER_PLATFORM_WIN32_WINDOWS &&
		     ClsGetApp()->GetPlatformID() != VER_PLATFORM_WIN32_NT )
		{
			MessageBox( NULL, _T( "Windows 95/98/ME/NT/2000/XP required!" ), _T( "\"FastFind\" Sample" ), MB_OK );
			return NULL;
		}

		// Create and open the dialog.
		FastFindDlg	dlg;
		dlg.DoModal( IDD_FIF );
	}
	return 0;
}