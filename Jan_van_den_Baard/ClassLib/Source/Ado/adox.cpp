//
//  MODULE:   AdoX.cpp
//
//	AUTHOR: Carlos Antollini 
//
//  mailto: cantollini@hotmail.com
//
//	Date: 06/14/2002
//
//	Version 1.02
// 
// ******************************************************************************
//
// Implementation for this class-library by Jan van den Baard. For the 
// implementation I have made a few changes in respect to the original
// version:
//
//	1) Made the classes UNICODE aware. They should compile into a UNICODE
//	   aware app OK now.
//	2) Moved the ctor and dtor code out of the header (.h) file and into the
//	   code (.cpp) file.
//	3) Made all destructors virtual.
//	4) Made the _UserPtr, _CatalogPtr etc. data pointers protected and added
//	   Getxxx() methods to get to them.
//	5) Added some extra error checking throughout the code.
//	6) All classes are inherited from "ClsADOBase" now. This is a simple
//	   base class which encapsulates some shared functionality.
//	7) Removed ::CoInitialize() and ::CoUninitialize() calls. Client code
//	   should take care of that.
//	8) Code now compiles under Warning level 4 without warnings.
//	9) Fixed some problems from the original classes like memory leaks, no
//	   error handling in the class constructors etc.
//
// 02-Nov-2003 (JBa) - Initial ClassLib version.
//

#include "../stdafx.h"

#include "adox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE 
static char THIS_FILE[] = __FILE__;
#endif  

///////////////////////////////////////////////////////
//
//  ClsADOXProcedure Class
//
ClsADOXUser::ClsADOXUser( ClsADOXCatalog* pCat )
{
	m_strClassName	= _T( "ClsADOXUser" );
	m_pCatalog	= pCat->GetCatalog();
	m_pUser		= NULL;
	ClsADOBase::CheckHResult( m_pUser.CreateInstance( __uuidof( User )));
}

ClsADOXUser::~ClsADOXUser()
{
	if ( m_pUser ) m_pUser.Release();
}

BOOL ClsADOXUser::Open( LPCTSTR lpstrUserName )
{
	_ASSERT_VALID( m_pCatalog );

	m_pUser = m_pCatalog->Users->GetItem( lpstrUserName );
	return m_pUser != NULL;
}

void ClsADOXUser::GetName( ClsString& strName )
{
	_ASSERT_VALID( m_pUser );
	_variant_t vName;

	vName   = m_pUser->GetName();
	strName = vName.bstrVal;
}

BOOL ClsADOXUser::ChangePassword( LPCTSTR lpstrOldPassword, LPCTSTR lpstrNewPassword )
{
	_ASSERT_VALID( m_pUser );
	
	m_pUser->ChangePassword( _bstr_t( lpstrOldPassword ), _bstr_t( lpstrNewPassword ));
	return TRUE;
}

BOOL ClsADOXUser::Create( LPCTSTR lpstrUserName )
{
	_ASSERT_VALID( m_pUser );
	try
	{
		m_pUser->PutName( lpstrUserName );
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}


///////////////////////////////////////////////////////
//
//  ClsADOXView Class
//
ClsADOXView::ClsADOXView( ClsADOXCatalog* pCat )
{
	m_strClassName = _T( "ClsADOXView" );
	m_pCatalog     = pCat->GetCatalog();
	m_pView        = NULL;
	ClsADOBase::CheckHResult( m_pView.CreateInstance( __uuidof( View )));
}

ClsADOXView::~ClsADOXView()
{
	if ( m_pView ) m_pView.Release();
}

BOOL ClsADOXView::Open( LPCTSTR lpstrViewName )
{
	_ASSERT_VALID( m_pCatalog );

	m_pView = m_pCatalog->Views->GetItem( lpstrViewName );
	return m_pView != NULL;
}

void ClsADOXView::GetName( ClsString& strName )
{
	_ASSERT_VALID( m_pView );
	_variant_t vName;

	vName   = m_pView->GetName();
	strName = vName.bstrVal;
}

BOOL ClsADOXView::Create( ClsString strName, ClsString strCommand )
{
	ADODB::_CommandPtr pCommand = NULL;
	try
	{
		pCommand.CreateInstance( __uuidof( ADODB::Command ));
		pCommand->put_CommandText( _bstr_t( strCommand ));
		if ( SUCCEEDED( m_pCatalog->Views->Append( _bstr_t( strName ), pCommand )))
		{
			m_pCatalog->Views->Refresh();
			m_pView = m_pCatalog->Views->GetItem( _variant_t( strName ));
			pCommand.Release();
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}

void ClsADOXView::GetCommand( ClsString &strCommand )
{
	_ASSERT_VALID( m_pView );
	_variant_t vCommand;

	try
	{
		ADODB::_CommandPtr pCommand = NULL;
		pCommand.CreateInstance( __uuidof( ADODB::Command ));

		pCommand   = m_pView->GetCommand();
		vCommand   = pCommand->GetCommandText();
		strCommand = vCommand.bstrVal;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
	}
}

///////////////////////////////////////////////////////
//
//  ClsADOXProcedure Class
//
ClsADOXProcedure::ClsADOXProcedure( ClsADOXCatalog* pCat )
{
	m_strClassName = _T( "ClsADOXProcedure" );
	m_pCatalog     = pCat->GetCatalog();
	m_pProcedure   = NULL;
	ClsADOBase::CheckHResult( m_pProcedure.CreateInstance( __uuidof( Procedure )));
}

ClsADOXProcedure::~ClsADOXProcedure()
{
	if ( m_pProcedure ) m_pProcedure.Release();
}

BOOL ClsADOXProcedure::Open( LPCTSTR lpstrProcedureName )
{
	_ASSERT_VALID( m_pCatalog );

	m_pProcedure = m_pCatalog->Procedures->GetItem( lpstrProcedureName );
	return m_pProcedure != NULL;
}

void ClsADOXProcedure::GetName( ClsString& strName )
{
	_ASSERT_VALID( m_pProcedure );
	_variant_t vName;

	vName   = m_pProcedure->GetName();
	strName = vName.bstrVal;
}

BOOL ClsADOXProcedure::Create( ClsString strName, ClsString strCommand )
{
	ADODB::_CommandPtr pCommand = NULL;
	try
	{
		pCommand.CreateInstance( __uuidof( ADODB::Command ));
		pCommand->put_CommandText( _bstr_t( strCommand ));
		m_pCatalog->Procedures->Append( _bstr_t( strName ), pCommand );
		m_pCatalog->Procedures->Refresh();
		m_pProcedure = m_pCatalog->Procedures->GetItem( _variant_t( strName ));
		pCommand.Release();
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}

void ClsADOXProcedure::GetCommand( ClsString &strCommand )
{
	_ASSERT_VALID( m_pProcedure );
	_variant_t vCommand;

	try
	{
		ADODB::_CommandPtr pCommand = NULL;
		pCommand.CreateInstance( __uuidof( ADODB::Command ));
		pCommand   = m_pProcedure->GetCommand();
		vCommand   = pCommand->GetCommandText();
		strCommand = vCommand.bstrVal;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
	}
}

///////////////////////////////////////////////////////
//
// ClsADOXIndex Class
//
ClsADOXIndex::ClsADOXIndex()
{
	m_strClassName = _T( "ClsADOXIndex" );
	m_pIndex       = NULL;
	ClsADOBase::CheckHResult( m_pIndex.CreateInstance( __uuidof( Index )));
}

ClsADOXIndex::~ClsADOXIndex()
{
	if ( m_pIndex ) m_pIndex.Release();
}

BOOL ClsADOXIndex::Create( LPCTSTR lpstrIndexName )
{
	_ASSERT_VALID( m_pIndex );
	try
	{
		m_pIndex->PutName( lpstrIndexName );
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}

BOOL ClsADOXIndex::AddField( LPCTSTR lpstrIndexName, enum DataType Type, int nLength )
{
	_ASSERT_VALID( m_pIndex );
	try
	{
		m_pIndex->Columns->Append( lpstrIndexName, ( enum DataTypeEnum ) Type, nLength );
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}

void ClsADOXIndex::SetPrimarKey( BOOL bPrimary )
{
	_ASSERT_VALID( m_pIndex );
	m_pIndex->PutPrimaryKey( bPrimary ? -1 : 0 );
	m_pIndex->PutUnique( bPrimary ? -1 : 0 );
}

///////////////////////////////////////////////////////
//
// ClsADOXTAble Class
//
ClsADOXTable::ClsADOXTable( ClsADOXCatalog* pCat, LPCTSTR lpstrTableName )
{
	m_strClassName = _T( "ClsADOXTable" );
	m_pTable       = NULL;
	ClsADOBase::CheckHResult( m_pTable.CreateInstance( __uuidof( Table )));
	m_pCatalog     = NULL;
	m_pCatalog     = pCat->GetCatalog();
	if( lpstrTableName != NULL && _tcslen( lpstrTableName ) > 0 )
		Open( lpstrTableName );
}

ClsADOXTable::ClsADOXTable( ClsADOXCatalog* pCat, int nTableIndex )
{
	m_strClassName = _T( "ClsADOXTable" );
	m_pTable       = NULL;
	ClsADOBase::CheckHResult( m_pTable.CreateInstance( __uuidof( Table )));
	m_pCatalog     = NULL;
	m_pCatalog     = pCat->GetCatalog();
	if( nTableIndex > -1 )
		Open( nTableIndex );
}

ClsADOXTable::~ClsADOXTable()
{
	if ( m_pTable ) m_pTable.Release();
}

BOOL ClsADOXTable::Open( LPCTSTR lpstrTableName )
{
	_ASSERT_VALID( m_pCatalog );

	m_pTable = m_pCatalog->Tables->GetItem( lpstrTableName );
	return m_pTable != NULL;
}
	
BOOL ClsADOXTable::Open( long nTableIndex )
{
	_ASSERT_VALID( m_pCatalog );

	m_pTable = m_pCatalog->Tables->GetItem( nTableIndex );
	return m_pTable != NULL;
}
		
BOOL ClsADOXTable::Create( LPCTSTR lpstrTableName )
{
	_ASSERT_VALID( m_pTable );
	try
	{
		m_pTable->PutName( lpstrTableName );
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}

BOOL ClsADOXTable::AddField( LPCTSTR lpstrFieldName, enum DataType Type, int nLength )
{
	_ASSERT_VALID( m_pTable );
	try
	{
		m_pTable->Columns->Append( lpstrFieldName, ( enum DataTypeEnum ) Type, nLength );
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}

BOOL ClsADOXTable::AddIndex( ClsADOXIndex pIndex )
{
	_ASSERT_VALID( m_pTable );
	try
	{
		m_pTable->Indexes->Append( _variant_t(( IDispatch * )pIndex.GetIndexPtr()));
		m_pCatalog->Tables->Refresh();
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}

BOOL ClsADOXTable::DeleteField( LPCTSTR lpstrFieldName )
{
	_ASSERT_VALID( m_pTable );
	try
	{
		m_pTable->Columns->Delete( lpstrFieldName );
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}

void ClsADOXTable::GetName( ClsString& strTableName )
{
	_ASSERT_VALID( m_pTable );
	_variant_t vName;

	vName  = m_pTable->GetName();
	strTableName = vName.bstrVal;
}

////////////////////////////////////////////////////////
//
// ClsADOXCAtalog Class
//
ClsADOXCatalog::ClsADOXCatalog()
{
	m_pCatalog     = NULL;
	m_strClassName = _T( "ClsADOXCatalog" );
	ClsADOBase::CheckHResult( m_pCatalog.CreateInstance( __uuidof( Catalog )));
}

ClsADOXCatalog::~ClsADOXCatalog()
{
	if ( m_pCatalog ) m_pCatalog.Release();
}

BOOL ClsADOXCatalog::CreateDatabase( LPCTSTR lpstrCreate )
{
	_ASSERT_VALID( m_pCatalog );
	try
	{
		m_pCatalog->Create( _bstr_t( lpstrCreate ));	
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}

BOOL ClsADOXCatalog::Open( LPCTSTR lpstrConnection )
{
	_ASSERT_VALID( m_pCatalog );
	try
	{
		ADODB::_ConnectionPtr m_pConn = NULL;
		m_pConn.CreateInstance( __uuidof( ADODB::Connection ));
		m_pConn->Open( lpstrConnection, _T( "" ), _T( "" ), NULL );
		m_pCatalog->PutActiveConnection( variant_t(( IDispatch * )m_pConn ));
		//m_pCatalog->PutActiveConnection( _bstr_t( lpstrConnection ));
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}

BOOL ClsADOXCatalog::AddTable( ClsADOXTable pTable )
{
	_ASSERT_VALID( m_pCatalog );
	try
	{
		m_pCatalog->Tables->Append( _variant_t(( IDispatch * )pTable.GetTable()));
		m_pCatalog->Tables->Refresh();
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}

void ClsADOXCatalog::GetTableName( long nTableIndex, ClsString &strTableName )
{
	_ASSERT_VALID( m_pCatalog );
	_ASSERT( nTableIndex >= 0 && nTableIndex < m_pCatalog->Tables->GetCount());
	
	strTableName = ( LPCTSTR )m_pCatalog->Tables->GetItem( nTableIndex)->GetName();
}

BOOL ClsADOXCatalog::DeleteTable( LPCTSTR lpstrTableName )
{
	_ASSERT_VALID( m_pCatalog );

	try
	{
		m_pCatalog->Tables->Delete( lpstrTableName );
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}

BOOL ClsADOXCatalog::DeleteTable( long nTableIndex )
{
	_ASSERT_VALID( m_pCatalog );
	_ASSERT( nTableIndex >= 0 && nTableIndex < m_pCatalog->Tables->GetCount());

	try
	{
		m_pCatalog->Tables->Delete( nTableIndex );
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}

void ClsADOXCatalog::GetProcedureName( long nProcedure, ClsString &strProcedureName )
{
	_ASSERT_VALID( m_pCatalog );
	_ASSERT( nProcedure >= 0 && nProcedure < m_pCatalog->Procedures->GetCount());
	
	strProcedureName = ( LPCTSTR )m_pCatalog->Procedures->GetItem( nProcedure )->GetName();
}

BOOL ClsADOXCatalog::DeleteProcedure( long nProcedure )
{
	_ASSERT_VALID( m_pCatalog );
	_ASSERT( nProcedure >= 0 && nProcedure < m_pCatalog->Procedures->GetCount());
	
	try
	{
		m_pCatalog->Procedures->Delete( nProcedure );
		m_pCatalog->Procedures->Refresh();
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}

BOOL ClsADOXCatalog::DeleteProcedure( LPCTSTR lpstrProcedureName )
{
	_ASSERT_VALID( m_pCatalog );
	
	try
	{
		m_pCatalog->Procedures->Delete( lpstrProcedureName );
		m_pCatalog->Procedures->Refresh();
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}

long ClsADOXCatalog::GetViewCount()
{
	_ASSERT_VALID( m_pCatalog );
	try
	{
		return m_pCatalog->Views->GetCount();
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return -1;
	}
}

void ClsADOXCatalog::GetViewName( long nViewIndex, ClsString &strViewName )
{
	_ASSERT_VALID( m_pCatalog );
	_ASSERT( nViewIndex >= 0 && nViewIndex < m_pCatalog->Views->GetCount());
	
	strViewName = ( LPCTSTR )m_pCatalog->Views->GetItem( nViewIndex )->GetName();
}

BOOL ClsADOXCatalog::DeleteView( LPCTSTR lpstrViewName )
{
	_ASSERT_VALID( m_pCatalog );

	try
	{
		m_pCatalog->Views->Delete( lpstrViewName );
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}

BOOL ClsADOXCatalog::DeleteView( long nViewIndex )
{
	_ASSERT_VALID( m_pCatalog );
	_ASSERT( nViewIndex >= 0 && nViewIndex < m_pCatalog->Views->GetCount());

	try
	{
		m_pCatalog->Views->Delete( nViewIndex );
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}

void ClsADOXCatalog::GetGroupName( long nGroupIndex, ClsString &strGroupName )
{
	_ASSERT_VALID( m_pCatalog );
	_ASSERT( nGroupIndex >= 0 && nGroupIndex < m_pCatalog->Groups->GetCount());
	
	strGroupName = ( LPCTSTR )m_pCatalog->Groups->GetItem( nGroupIndex )->GetName();
}

BOOL ClsADOXCatalog::DeleteGroup( LPCTSTR lpstrGroupName )
{
	_ASSERT_VALID( m_pCatalog );

	try
	{
		m_pCatalog->Groups->Delete( lpstrGroupName );
		m_pCatalog->Groups->Refresh();
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}

BOOL ClsADOXCatalog::DeleteGroup( long nGroupIndex )
{
	_ASSERT_VALID( m_pCatalog );
	_ASSERT( nGroupIndex >= 0 && nGroupIndex < m_pCatalog->Groups->GetCount());

	try
	{
		m_pCatalog->Groups->Delete( nGroupIndex );
		m_pCatalog->Groups->Refresh();
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}

void ClsADOXCatalog::GetUserName( long nUserIndex, ClsString &strUserName )
{
	_ASSERT_VALID( m_pCatalog );
	_ASSERT( nUserIndex >= 0 && nUserIndex < m_pCatalog->Users->GetCount());
	
	strUserName = ( LPCTSTR )m_pCatalog->Users->GetItem( nUserIndex )->GetName();
}

BOOL ClsADOXCatalog::DeleteUser( LPCTSTR lpstrUserName )
{
	_ASSERT_VALID( m_pCatalog );

	try
	{
		m_pCatalog->Users->Delete( lpstrUserName );
		m_pCatalog->Users->Refresh();
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}

BOOL ClsADOXCatalog::DeleteUser( long nUserIndex )
{
	_ASSERT_VALID( m_pCatalog );
	_ASSERT( nUserIndex >= 0 && nUserIndex < m_pCatalog->Users->GetCount());

	try
	{
		m_pCatalog->Users->Delete( nUserIndex );
		m_pCatalog->Users->Refresh();
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}

BOOL ClsADOXCatalog::AddUser( ClsADOXUser pUser, LPCTSTR lpstrPassword )
{
	_ASSERT_VALID( m_pCatalog );
	try
	{
		m_pCatalog->Users->Append( _variant_t(( IDispatch * )pUser.GetUser()), _bstr_t( lpstrPassword ));
		m_pCatalog->Users->Refresh();
		return TRUE;
	}
	catch( _com_error &e )
	{
		dump_com_error( e );
		return FALSE;
	}
}