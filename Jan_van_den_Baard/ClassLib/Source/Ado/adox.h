#ifdef _IMPORT_ADO_
#ifndef _ADOX_H_
#define _ADOX_H_
//
//  MODULE:   AdoX.h
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
//
// 02-Nov-2003 (JBa) - Initial ClassLib version.
//

#include "adobase.h"
using namespace ADOX;

// Forward declarations.
class ClsADOXIndex;
class ClsADOXCatalog;

// ClsADOXUser class definition.
class ClsADOXUser : public ClsADOBase
{
	_NO_COPY( ClsADOXUser );
public:
	ClsADOXUser( ClsADOXCatalog* pCat );
	virtual ~ClsADOXUser();

	void GetName( ClsString& strName );
	BOOL Create( LPCTSTR lpstrUserName );
	BOOL Open( LPCTSTR lpstrUserName );
	BOOL ChangePassword( LPCTSTR lpstrOldPassword, LPCTSTR lpstrNewPassword );

	inline _UserPtr GetUser() { return m_pUser; }
	inline _CatalogPtr GetCatalog() { return m_pCatalog; }

protected:
	// Data.
	_UserPtr    m_pUser;
	_CatalogPtr m_pCatalog;
};

// ClsADOXView class definition.
class ClsADOXView : public ClsADOBase
{
	_NO_COPY( ClsADOXView );
public:
	ClsADOXView( ClsADOXCatalog* pCat );
	virtual ~ClsADOXView();
	
	void GetCommand( ClsString& strCommand );
	void GetName( ClsString& strName );
	BOOL Open( LPCTSTR lpstrViewName );
	BOOL Create( ClsString strName, ClsString strCommand );

	inline ViewPtr GetView() { return m_pView; }
	inline _CatalogPtr GetCatalog() { return m_pCatalog; }

protected:
	// Data.
	ViewPtr	    m_pView;
	_CatalogPtr m_pCatalog;
};

// ClsADOXProcedure class definition.
class ClsADOXProcedure : public ClsADOBase
{
	_NO_COPY( ClsADOXProcedure );
public:
	ClsADOXProcedure( ClsADOXCatalog* pCat );
	virtual ~ClsADOXProcedure();
	
	void GetName( ClsString& strName );
	BOOL Open( LPCTSTR lpstrProcedureName );
	BOOL Create( ClsString strName, ClsString strCommand );
	void GetCommand( ClsString &strCommand );

	inline ProcedurePtr GetProcedure() { return m_pProcedure; }
	inline _CatalogPtr GetCatalog() { return m_pCatalog; }

protected:
	// Data.
	ProcedurePtr m_pProcedure;
	_CatalogPtr  m_pCatalog;
};

// ClsADOXTable class definition.
class ClsADOXTable : public ClsADOBase
{
	_NO_COPY( ClsADOXTable );
public:
	enum DataType
	{
		typeSmallInt	     = ADOX::adSmallInt,
		typeInteger	     = ADOX::adInteger,
		typeUnsignedTinyInt  = ADOX::adUnsignedTinyInt,
		typeUnsignedSmallInt = ADOX::adUnsignedSmallInt,
		typeUnsignedInt	     = ADOX::adUnsignedInt,
		typeUnsignedBigInt   = ADOX::adUnsignedBigInt,
		typeSingle	     = ADOX::adSingle,
		typeDouble	     = ADOX::adDouble,
		typeCurrency	     = ADOX::adCurrency,
		typeDecimal	     = ADOX::adDecimal,
		typeNumeric	     = ADOX::adNumeric,
		typeBoolean	     = ADOX::adBoolean,
		typeDate	     = ADOX::adDate,
		typeDBDate	     = ADOX::adDBDate,
		typeDBTime	     = ADOX::adDBTime,
		typeDBTimeStamp	     = ADOX::adDBTimeStamp,
		typeBSTR	     = ADOX::adBSTR,
		typeVarChar	     = ADOX::adVarChar,
		typeLongVarChar	     = ADOX::adLongVarChar,
		typeWChar	     = ADOX::adWChar,
		typeVarWChar	     = ADOX::adVarWChar,
		typeLongVarWChar     = ADOX::adLongVarWChar,
		typeBinary	     = ADOX::adBinary,
		typeVarBinary	     = ADOX::adVarBinary,
		typeLongVarBinary    = ADOX::adLongVarBinary,
		typeChapter	     = ADOX::adChapter,
		typeFileTime	     = ADOX::adFileTime,
		typePropVariant      = ADOX::adPropVariant,
		typeVarNumeric	     = ADOX::adVarNumeric
	};
	
	ClsADOXTable( ClsADOXCatalog* pCat, LPCTSTR lpstrTableName = _T( "" ));
	ClsADOXTable( ClsADOXCatalog* pCat, int nTableIndex );
	virtual ~ClsADOXTable();

	BOOL Create( LPCTSTR lpstrTableName );
	BOOL Open( LPCTSTR lpstrTableName );
	BOOL Open( long nTableIndex );
	BOOL AddField( LPCTSTR lpstrFieldName, enum DataType Type, int nLength = 0 );
	BOOL AddIndex( ClsADOXIndex pIndex );
	BOOL DeleteField( LPCTSTR lpstrFieldName );
	void GetName( ClsString& strTableName );

	inline _TablePtr GetTable() { return m_pTable; }
	inline _CatalogPtr GetCatalog() { return m_pCatalog; }

protected:
	// Data.
	_TablePtr   m_pTable;
	_CatalogPtr m_pCatalog;
};

// ClsADOXIndex class definition.
class ClsADOXIndex : public ClsADOBase
{
	_NO_COPY( ClsADOXIndex );
public:
	enum DataType
	{
		typeSmallInt		= ADOX::adSmallInt,
		typeInteger		= ADOX::adInteger,
		typeUnsignedTinyInt	= ADOX::adUnsignedTinyInt,
		typeUnsignedSmallInt	= ADOX::adUnsignedSmallInt,
		typeUnsignedInt		= ADOX::adUnsignedInt,
		typeUnsignedBigInt	= ADOX::adUnsignedBigInt,
		typeSingle		= ADOX::adSingle,
		typeDouble		= ADOX::adDouble,
		typeCurrency		= ADOX::adCurrency,
		typeDecimal		= ADOX::adDecimal,
		typeNumeric		= ADOX::adNumeric,
		typeBoolean		= ADOX::adBoolean,
		typeDate		= ADOX::adDate,
		typeDBDate		= ADOX::adDBDate,
		typeDBTime		= ADOX::adDBTime,
		typeDBTimeStamp		= ADOX::adDBTimeStamp,
		typeBSTR		= ADOX::adBSTR,
		typeVarChar		= ADOX::adVarChar,
		typeLongVarChar		= ADOX::adLongVarChar,
		typeWChar		= ADOX::adWChar,
		typeVarWChar		= ADOX::adVarWChar,
		typeLongVarWChar	= ADOX::adLongVarWChar,
		typeBinary		= ADOX::adBinary,
		typeVarBinary		= ADOX::adVarBinary,
		typeLongVarBinary	= ADOX::adLongVarBinary,
		typeChapter		= ADOX::adChapter,
		typeFileTime		= ADOX::adFileTime,
		typePropVariant		= ADOX::adPropVariant,
		typeVarNumeric		= ADOX::adVarNumeric
	};
	
	ClsADOXIndex();
	virtual ~ClsADOXIndex();

	BOOL Create( LPCTSTR lpstrIndexName );
	BOOL AddField( LPCTSTR lpstrIndexName, enum DataType Type, int nLength = 0 );
	void SetPrimarKey( BOOL bPrimary = TRUE );

	inline GetIndexPtr() { return m_pIndex; }

protected:
	// Data.
	_IndexPtr	m_pIndex;
};

// ClsADOXCatalog class definition.
class ClsADOXCatalog : public ClsADOBase
{
	_NO_COPY( ClsADOXCatalog );
public:
	ClsADOXCatalog();
	virtual ~ClsADOXCatalog();

	BOOL Open( LPCTSTR lpstrConnection );
	BOOL CreateDatabase( LPCTSTR lpstrCreate );
	BOOL AddTable( ClsADOXTable pTable );
	BOOL AddUser( ClsADOXUser pUser, LPCTSTR lpstrPassword );

	long GetViewCount();
	void GetTableName( long nTableIndex, ClsString &strTableName );
	void GetProcedureName( long nProcedureIndex, ClsString &strProcedureName );
	void GetViewName( long nViewIndex, ClsString &strViewName );
	void GetUserName( long nUserIndex, ClsString &strUserName );
	void GetGroupName( long nGroupIndex, ClsString &strGroupName );
	BOOL DeleteTable( LPCTSTR lpstrTableName );
	BOOL DeleteTable( long nTableIndex );
	BOOL DeleteProcedure( long nProcedureIndex );
	BOOL DeleteProcedure( LPCTSTR lpstrProcedureName );
	BOOL DeleteView( LPCTSTR lpstrViewName );
	BOOL DeleteView( long nViewIndex );
	BOOL DeleteGroup( LPCTSTR lpstrGroupName );
	BOOL DeleteGroup( long nGroupIndex );
	BOOL DeleteUser( LPCTSTR lpstrUserName );
	BOOL DeleteUser( long nViewIndex );

	inline long GetProcedureCount() { _ASSERT_VALID( m_pCatalog ); return m_pCatalog->Procedures->GetCount(); }
	inline long GetTableCount() { _ASSERT_VALID( m_pCatalog ); return m_pCatalog->Tables->GetCount(); }
	inline long GetUserCount() { _ASSERT_VALID( m_pCatalog ); return m_pCatalog->Users->GetCount(); }
	inline long GetGroupCount() { _ASSERT_VALID( m_pCatalog ); return m_pCatalog->Groups->GetCount(); }
	inline _CatalogPtr GetCatalog() { return m_pCatalog; }

protected:
	// Data.
	_CatalogPtr m_pCatalog;	
};

#endif // _ADOX_H_
#endif // _IMPORT_ADO_