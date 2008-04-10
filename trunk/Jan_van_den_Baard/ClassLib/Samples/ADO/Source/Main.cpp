// This should be defined before you include any of the
// ClassLib headers if you need the ADO/ADOX support classes.

#include "stdafx.h"

#define _IMPORT_ADO_

#include "../../../Source/all.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE 
static char THIS_FILE[] = __FILE__;
#endif  

// Thread messages posted to the main dialog by the
// thread which reads the contents of the recordset.
#define THREAD_STOPPED		WM_USER + 3000
#define THREAD_MESSAGE		WM_USER + 3001	// lParam = ( ClsString * )pMessage
#define THREAD_ADDCOLUMN	WM_USER + 3002	// wParam = Index, lParam = ( ClsString * )pColumnName
#define THREAD_ADDLINE		WM_USER + 3003	// wParam = Line, lParam = ( ClsString * )pColumnData
#define THREAD_ADDCOLUMNDATA	WM_USER + 3004	// wParam = Index, lParam = ( ClsString * )pColumnData
#define THREAD_CLEARLISTVIEW	WM_USER + 3005

// A simple ClsBrowser derived class which opens
// a connection via the Datalink selection dialog.
class ConnBrowser : public ClsBrowser
{
	_NO_COPY( ConnBrowser );
public:
	ConnBrowser() : m_pDatabase( NULL ) {;}
	virtual ~ConnBrowser() {;}

	// Database on which the connection is opened.
	void SetDatabase( ClsADODatabase *pDatabase ) { m_pDatabase = pDatabase; }

protected:
	// We do not want the edit control to let anybody
	// actually edit so we swallow a bunch of messages.
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		switch ( uMsg )
		{
			case	WM_GETDLGCODE:
				// We want 'm all.
				return DLGC_WANTALLKEYS;

				// And then we swallow them all.
			case	WM_CHAR:
			case	WM_DEADCHAR:
			case	WM_SYSCHAR:
			case	WM_SYSDEADCHAR:
			case	WM_KEYUP:
			case	WM_SYSKEYUP:
			case	WM_KEYDOWN:
			case	WM_SYSKEYDOWN:

				// Also intercept these messages.
			case	WM_PASTE:
			case	WM_CUT:
				return 0;
		}
		// The rest goes to the base-class.
		return ClsBrowser::WindowProc( uMsg, wParam, lParam );
	}

	// Here's where we end up if the user clicks
	// on the browser button.
	virtual void OnBrowserClicked()
	{
		// Do we have a database?
		if ( m_pDatabase )
		{
			try
			{
				// Close the database (if necessary).
				if ( m_pDatabase->IsOpen()) 
					m_pDatabase->Close();

				// Calling the OpenPrompt() method on ClsADODatabase
				// opens the Datalink dialog with the current connection
				// string preselected.
				if ( m_pDatabase->OpenPrompt())
					// Show the connection string in the
					// browser control.
					SetWindowText( m_pDatabase->GetConnectionString());
				else
				{
					// Dialog probably canceled.
					SetWindowText( _T( "" ));
					m_pDatabase->SetConnectionString( _T( "" ));
				}
			}
			catch ( _com_error& e )
			{
				// Show the error message.
				MessageBox( m_pDatabase->GetLastErrorDescription() + ClsString( _T( "\r\n" )) + m_pDatabase->GetLastErrorString());
				SetWindowText( _T( "" ));
				m_pDatabase->SetConnectionString( _T( "" ));
				e;
			}
		}
	}

	// Must be overridden because it is a pure
	// virtual in the base class.
	virtual void OnFileDropped( HDROP drop ) {;}

private:
	// The database.
	ClsADODatabase	*m_pDatabase;
};

class ADODlg : public ClsSizeDialog
{
	_NO_COPY( ADODlg );
public:
	// No-op constructor and destructor.
	ADODlg() : m_bIsRunning( FALSE ) {;}
	virtual ~ADODlg() {;}

protected:
	// Show error.
	void ShowError( ClsString msg, _com_error& e )
	{
		// Show the error.
		MessageBox( msg, e.ErrorMessage(), MB_OK | MB_ICONERROR );

		// Clear the GUI.
		m_Label3.SetWindowText( e.Description());
	}

	// WM_DESTROY message handler.
	virtual LRESULT OnDestroy()
	{
		MSG msg;
		// Make sure we free any pending messages.
		while ( PeekMessage( &msg, GetSafeHWND(), THREAD_MESSAGE, THREAD_ADDCOLUMNDATA, PM_REMOVE ))
			delete ( ClsString * )msg.lParam;

		// Call the base class.
		return ClsSizeDialog::OnDestroy();
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

	// WM_COMMAND handler.
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl )
	{
		switch ( nCtrlID )
		{
			case	IDC_OK:
				// Is the thread running?
				if ( m_bIsRunning )
					// Stop the thread.
					m_Thread.Stop();

				// Bye, bye...
				EndDialog( 0 );
				break;

			// Connection browser.
			case	IDC_CONN:
			{
				// Did the contents change?
				if ( nNotifyCode == EN_CHANGE )
				{
					try
					{
						// Is the database opened?
						if ( m_Database.IsOpen())
						{
							// Read the column information and the
							// contents of the first table in the
							// ComboBox.
							ReadTables();
							//ReadTableContents();
							RunThread();
						}
						else
						{
							// Nothing to show...
							m_Tables.EnableWindow( FALSE );
							m_Tables.ResetContent();
							ClearListView();
						}
					}
					catch( _com_error& e )
					{
						// Display the error information.
						ShowError( m_Database.GetLastErrorString(), e );
						m_Database.Close();
					}
				}
			}

			case	IDC_TABLES:
			{
				// Combo-box selection. Read the new table.
				if ( nNotifyCode == CBN_SELCHANGE )
				{
					// Store the selected table and run
					// the thread.
					m_Tables.GetWindowText( m_TableName );
					RunThread();
				}
			}

			default:
				break;
		}
		// Pass to the base class.
		return ClsDialog::OnCommand( nNotifyCode, nCtrlID, hWndCtrl );
	}

	// WM_INITDIALOG handler.
	virtual LRESULT OnInitDialog( PROPSHEETPAGE *psp )
	{
		// Attach controls to the classes.
		m_Label1.Attach( GetDlgItemHandle( IDC_LABEL1 ));
		m_Label2.Attach( GetDlgItemHandle( IDC_LABEL2 ));
		m_Label3.Attach( GetDlgItemHandle( IDC_LABEL3 ));
		m_ConnBrowser.Attach( GetDlgItemHandle( IDC_CONN ));
		m_Tables.Attach( GetDlgItemHandle( IDC_TABLES ));
		m_Contents.Attach( GetDlgItemHandle( IDC_CONTENT ));
		m_OK.Attach( GetDlgItemHandle( IDC_OK ));

		// Add an ellipsis (...) when the text get's to long.
		m_Label3.ModifyStyle( 0, SS_ENDELLIPSIS );

		// Change some listview styles and add a dummy column.
		m_Contents.SetExtendedListViewStyle( LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
		
		// The Database object must throw COM errors.
		m_Database.ThrowComErrors();
		m_ConnBrowser.SetDatabase( &m_Database );

		// This style will prevent flickering when the dialog
		// is resized.
		ModifyStyle( 0, WS_CLIPCHILDREN );
		
		// We must create the layout engine controls here. This way the "ClsDialog"
		// class will distribute it's font to the layout engine controls aswell.
		m_H1.Create(     *this, Offsets( 0, 0, 0, 0 ), LAYOUT_Horizontal, TRUE, TAG_END );
		m_H2.Create(     *this, Offsets( 0, 0, 0, 0 ), LAYOUT_Horizontal, TRUE, TAG_END );
		m_H3.Create(     *this, Offsets( 0, 0, 0, 0 ), LAYOUT_Horizontal, TRUE, LAYOUT_FrameGroup, TRUE, LAYOUT_Title, _T( "Recordset" ), TAG_END );
		m_H4.Create(     *this, Offsets( 0, 0, 0, 0 ), LAYOUT_Horizontal, TRUE, TAG_END );
		m_V1.Create(     *this, Offsets( 0, 0, 0, 0 ), LAYOUT_Horizontal, FALSE, TAG_END );
		m_V2.Create(     *this, Offsets( 0, 0, 0, 0 ), LAYOUT_Horizontal, FALSE, TAG_END );
		m_V3.Create(     *this, Offsets( 0, 0, 0, 0 ), LAYOUT_Horizontal, FALSE, TAG_END );
		m_Master.Create( *this, Offsets( 6, 6, 6, 6 ), LAYOUT_Spacing, 6, LAYOUT_Horizontal, FALSE, LAYOUT_Master, TRUE, TAG_END );

		// Base class.
		return ClsDialog::OnInitDialog( psp );
	}

	// This overidable will be called when ClsDialog has finished
	// distributing the font to it's children.
	virtual void OnFontDistributed()
	{
		// Call the base class.
		ClsSizeDialog::OnFontDistributed();

		// Add the controls to the child groups.
		m_V1.AddSpacingMember();
		m_V1.AddMember( &m_Label1, NULL, ATTR_FixMinHeight, TRUE, TAG_END );
		m_V1.AddSpacingMember();

		m_H1.AddMember( &m_V1, NULL, ATTR_FixMinWidth, TRUE, TAG_END );
		m_H1.AddMember( &m_ConnBrowser, NULL, TAG_END );

		m_V2.AddSpacingMember();
		m_V2.AddMember( &m_Label2, NULL, ATTR_FixMinWidth, TRUE, TAG_END );
		m_V2.AddSpacingMember();

		m_V3.AddSpacingMember();
		m_V3.AddMember( &m_Label3, NULL, TAG_END );
		m_V3.AddSpacingMember();
		
		m_H2.AddMember( &m_V2, NULL, ATTR_FixMinWidth, TRUE, TAG_END );
		m_H2.AddMember( &m_Tables, NULL, ATTR_FixMinWidth, TRUE, TAG_END );
		m_H2.AddMember( &m_V3, NULL, TAG_END );
		
		m_H3.AddMember( &m_Contents, NULL, TAG_END );
		
		m_H4.AddSpacingMember();
		m_H4.AddMember( &m_OK, NULL, ATTR_FixMinSize, TRUE, TAG_END );
		m_H4.AddSpacingMember();

		// Add the child groups to the master group.
		m_Master.AddMember( &m_H1, NULL, ATTR_FixMinHeight, TRUE, TAG_END );
		m_Master.AddMember( &m_H2, NULL, ATTR_FixMinHeight, TRUE, TAG_END );
		m_Master.AddMember( &m_H3, NULL, TAG_END );
		m_Master.AddMember( &m_H4, NULL, ATTR_FixMinHeight, TRUE, TAG_END );

		// Was there an error in any of the above call's?
		if ( m_Master.Error())
		{
			EndDialog( 0 );
			return;
		}

		// Compute the minimum size of the master group.
		if ( m_Master.OnGetMinSize( m_MinSize ) == FALSE )
		{
			EndDialog( 0 );
			return;
		}

		// Add frame and caption sizes so that we know the minimum
		// size of the dialog.
		m_MinSize.CY() += ( ::GetSystemMetrics( SM_CYFRAME ) * 2 ) + ::GetSystemMetrics( SM_CYCAPTION );
		m_MinSize.CX() += ::GetSystemMetrics( SM_CXFRAME ) * 2;
		
		// Relayout the master layout engine control.
		m_Master.Relayout();
	}

	virtual LRESULT OnClose()
	{
		// If the thread is running 
		// we stop it here.
		if ( m_bIsRunning )
			m_Thread.Stop();

		// Call the base class.
		return ClsSizeDialog::OnClose();
	}

	virtual BOOL OnOK() { return FALSE; }
	virtual BOOL OnCancel() 
	{
		// If the thread is running 
		// we stop it here.
		if ( m_bIsRunning )
			m_Thread.Stop();
		return TRUE;
	}

	// Window procedure override.
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		switch ( uMsg )
		{
			case WM_GETMINMAXINFO:
			{
				// Fill in the minimum size of the dialog.
				LPMINMAXINFO pmmi = ( LPMINMAXINFO )lParam;
				pmmi->ptMinTrackSize.x = m_MinSize.CX();
				pmmi->ptMinTrackSize.y = m_MinSize.CY();
				return 0;
			}

			case THREAD_STOPPED:
				// We are not running anymore.
				m_bIsRunning = FALSE;
				m_Thread.Destroy();

				// Enable the controls.
				m_ConnBrowser.EnableWindow( TRUE );
				m_Tables.EnableWindow( TRUE );
				break;

			case THREAD_MESSAGE:
			{
				// Setup the message in the label control.
				ClsString *pMsg = ( ClsString * )lParam;
				m_Label3.SetWindowText( *pMsg );
				delete pMsg;
			}
			break;

			case THREAD_ADDCOLUMN:
			{
				// Add a column to the listview.
				ClsString *pStr = ( ClsString * )lParam;

				// Is the thread stull running?
				if ( m_Thread.IsRunning())
				{
					// We do not race the thread.
					m_Thread.Pause();
					m_Contents.InsertColumn(( int )wParam, *pStr );
					m_Thread.Run();
				}
				// Free the memory.
				delete pStr;
			}
			break;

			case THREAD_ADDLINE:
			{
				// Add a line including the first column to the listview.
				ClsString *pStr = ( ClsString * )lParam;

				// Is the thread still running?
				if ( m_Thread.IsRunning())
				{
					// We do not race the thread.
					m_Thread.Pause();

					// Create the item to add to the listview.
					LVITEM it;
					it.mask     = LVIF_TEXT;
					it.iItem    = ( int )wParam;
					it.iSubItem = 0;
					it.pszText  = *pStr;
					m_Contents.InsertItem( it );
					
					// Mark the current line.
					m_nItem = ( int )wParam;
					m_Thread.Run();
				}
				// Free the memory.
				delete pStr;
			}
			break;

			case THREAD_ADDCOLUMNDATA:
			{
				// Add data to a listview column.
				ClsString *pStr = ( ClsString * )lParam;

				// Is the thread running?
				if ( m_Thread.IsRunning())
				{
					// No racing.
					m_Thread.Pause();

					// Set the column text.
					m_Contents.SetItemText( m_nItem, ( int )wParam, *pStr );
					m_Thread.Run();
				}
				// Free the memory.
				delete pStr;
			}
			break;

			case THREAD_CLEARLISTVIEW:
				// Remove all data and column from the listview.
				ClearListView();
				break;
		}

		// Call the base class.
		return ClsSizeDialog::WindowProc( uMsg, wParam, lParam );
	}

	void ClearListView()
	{
		// No visual updates.
		m_Contents.SetRedraw( FALSE );

		// Clear the contents and the columns.
		m_Contents.DeleteAllItems();
		while ( m_Contents.DeleteColumn( 0 ));

		// Visual updates on and force a
		// refresh now.
		m_Contents.SetRedraw( TRUE );
		m_Contents.UpdateWindow();
	}

	void ReadTables()
	{
		ClsString name;
		ClsADOXCatalog cat;

		// We want errors thrown.
		cat.ThrowComErrors();

		// Clear the ComboBox and the
		// Listview control.
		m_Tables.ResetContent();
		ClearListView();

		// Is the database opened?
		if ( m_Database.IsOpen())
		{
			try
			{
				// Open the catalog of the database.
				cat.Open( m_Database.GetConnectionString());

				// A table count?
				if ( cat.GetTableCount())
				{
					// Iterate the tables.
					for ( int i = 0; i < cat.GetTableCount(); i++ )
					{
						// Get the name and add it to the ComboBox.
						cat.GetTableName( i, name );

						// Skip the tables starting with the characters
						// MSys. They should not be accessed.
						if ( name.Find( _T( "MSys" )) == -1 ) 
							m_Tables.AddString( name );
					}

					// Select the first table in the list.
					m_Tables.SetCurSel( 0 );
					m_Tables.EnableWindow();
					m_Tables.GetWindowText( m_TableName );
				}
				else
					// No tables read.
					m_Tables.EnableWindow( FALSE );
			}
			catch( _com_error& e )
			{
				// Error, bliep...
				ShowError( cat.GetLastErrorString(), e );
			}
		}
	}

	void RunThread()
	{
		// Only if we are not already running.
		if ( ! m_bIsRunning )
		{
			// Disable controls.
			m_ConnBrowser.EnableWindow( FALSE );
			m_Tables.EnableWindow( FALSE );

			// Start the thread.
			m_Thread.Start( ReadTableThread, ( LPVOID )this );
			m_bIsRunning = TRUE;
		}
	}

	static UINT WINAPI ReadTableThread( LPVOID pParam )
	{
		// The parameter is a pointer to the ADODlg object
		// which has initiated this thread.
		ADODlg *pDlg = ( ADODlg * )pParam;

		// Load the recordset...
		pDlg->ReadTableContents();

		// Make sure the main thread knows we have stopped.
		pDlg->PostMessage( THREAD_STOPPED );
		return 0;
	}

	void ReadTableContents()
	{
		static int nItem = 0;

		// Is the database open?
		if ( m_Database.IsOpen())
		{
			// Create a recordset object and make it
			// throw errors.
			ClsADORecordset rs( &m_Database );
			rs.ThrowComErrors();

			try
			{
				// Could take some time.
				ClsWaitCursor wait;

				// Clear the listview.
				PostMessage( THREAD_CLEARLISTVIEW );

				// Start at item 0.
				nItem = 0;

				// Get the table name of the selected table and
				// enclose it in [] characters in case it contains
				// white spaces.
				ClsString table;
				table.Format( _T( "[%s]" ), ( LPCTSTR )m_TableName );

				// Table selection OK?
				if ( table.GetStringLength() > 2 )
				{
					// Tell 'm we are selecting the table.
					PostMessage( THREAD_MESSAGE, 0, ( LPARAM )new ClsString( _T( "Loading recordset..." )));

					// Actually open the recordset.
					rs.Open( table, ClsADORecordset::openTable );

					// Obtain the column names of the returned recordset.
					StrADOFieldInfo info;
					for ( int i = 0; i < rs.GetFieldCount(); i++ )
					{
						// Still running?
						if ( m_Thread.Wait() != ClsWorkerThread::wtRunning )
							break;

						// Get the information and add the names of the
						// column to the listview.
						rs.GetFieldInfo( i, &info );
						PostMessage( THREAD_ADDCOLUMN, i, ( LPARAM )new ClsString( info.m_strName ));
					}

					// Tell 'm we are loading the recordset.
					PostMessage( THREAD_MESSAGE, 0, ( LPARAM )new ClsString( _T( "Adding the records to the list..." )));

					// Iterate the whole recordset...
					for ( int i = 0; i < ( int )rs.GetRecordCount(); i++ )
					{
						// Still running?
						if ( m_Thread.Wait() != ClsWorkerThread::wtRunning )
							break;

						// Add this item and move to the next one.
						ClsString *val = new ClsString();
						rs.GetFieldValue( 0, *val, ClsString( _T( "%d/%m/%Y" )));
						PostMessage( THREAD_ADDLINE, nItem++, ( LPARAM )val );

						for ( int z = 1; z < rs.GetFieldCount(); z++ )
						{
							val = new ClsString();
							rs.GetFieldValue( z, *val, ClsString( _T( "%d/%m/%Y" )));
							PostMessage( THREAD_ADDCOLUMNDATA, z, ( LPARAM )val );
						}

						// Next record.
						rs.MoveNext();
					}

					// Format the result and show it.
					ClsString *in = new ClsString();
					in->Format( _T( "Read %ld records with each %ld columns (fields)." ), rs.GetRecordCount(), rs.GetFieldCount());
					PostMessage( THREAD_MESSAGE, 0, ( LPARAM )in );

					// Close the recordset.
					rs.Close();
				}
			}
			catch ( _com_error& e )
			{
				// Post the error.
				MessageBeep(( UINT )-1L );
				PostMessage( THREAD_MESSAGE, 0, ( LPARAM )new ClsString( rs.GetLastErrorDescription()));
				rs.Close();
				e;
			}
		}
	}

private:
	// Data.
	ClsWorkerThread	m_Thread;
	ClsString	m_TableName;
	ClsLayoutEngine	m_Master, m_H1, m_H2, m_H3, m_H4, m_V1, m_V2, m_V3;
	ClsStatic	m_Label1, m_Label2, m_Label3;
	ConnBrowser	m_ConnBrowser;
	ClsComboBox	m_Tables;
	ClsListView	m_Contents;
	ClsSize		m_MinSize;
	ClsButton	m_OK;
	ClsADODatabase	m_Database;
	int		m_nItem;
	BOOL		m_bIsRunning;
};

// Entry point.
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nShowCmd )
{
	int		rc = 0;

	// Initialize COM.
	if ( SUCCEEDED( ::CoInitialize( NULL )))
	{
		// Setup app.
		if ( ClsGetApp()->Setup( hInstance, lpCmdLine, nShowCmd ))
		{
			// Check OS version.
			if ( ClsGetApp()->GetPlatformID() != VER_PLATFORM_WIN32_WINDOWS &&
			ClsGetApp()->GetPlatformID() != VER_PLATFORM_WIN32_NT )
			{
				MessageBox( NULL, _T( "Windows 95/98/ME/NT/2000 required!" ), _T( "\"XPControls\" Sample" ), MB_OK );
				return NULL;
			}

			// Create and open the dialog.
			ADODlg dlg;
			dlg.DoModal( IDD_ADO );
		}
	}
	::CoUninitialize();
	return rc;
}
