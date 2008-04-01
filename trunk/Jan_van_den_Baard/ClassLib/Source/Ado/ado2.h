#ifdef _IMPORT_ADO_
#ifndef _ADO2_H_
#define _ADO2_H_
//
//  MODULE: Ado2.h
//
//	AUTHOR: Carlos Antollini 
//
//  mailto: cantollini@hotmail.com
//
//	Date: 07/02/2003
//
//	Version 2.10
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
//	3) Added some extra error checking throughout the code.
//	4) All classes except for ClsJetEngine are inherited from "ClsADOBase" 
//         now. This is a simple base class which encapsulates some shared 
//	   functionality.
//
// 02-Nov-2003 (JBa) - Initial ClassLib version.
// 13-Jul-2004 (JBa) - SetConnectionString() now also set's the string to
//		       m_pConnection (ClsADODatabase).

#include "adobase.h"
using namespace ADODB;

// Forward declarations.
class ClsADOCommand;

// ADO Field information sturcture.
struct StrADOFieldInfo
{
	TCHAR	m_strName[ 30 ]; 
	short	m_nType;
	long	m_lSize; 
	long	m_lDefinedSize;
	long	m_lAttributes;
	short	m_nOrdinalPosition;
	BOOL	m_bRequired;   
	BOOL	m_bAllowZeroLength; 
	long	m_lCollatingOrder;  
};

// Converts values to ClsString objects. Some of these
// conversions are directly supported by the ClsString
// class. I should look into dropping these...
ClsString IntToStr( int nVal );
ClsString LongToStr( long lVal );
ClsString ULongToStr( unsigned long ulVal );
ClsString DblToStr( double dblVal, int ndigits = 20 );
ClsString DblToStr( float fltVal );

// ClsJetEngine class definition.
class ClsJetEngine
{
	_NO_COPY( ClsJetEngine );
public:
	ClsJetEngine() {;}
	virtual ~ClsJetEngine() {;}
	static BOOL CCompactDatabase( ClsString strDatabaseSource, ClsString strDatabaseDestination );
};

// ClsADODatabase class definition.
class ClsADODatabase : public ClsADOBase
{
	_NO_COPY( ClsADODatabase );
public:
	enum ClsADOConnectModeEnum
	{	
		connectModeUnknown	  = adModeUnknown,
		connectModeRead		  = adModeRead,
		connectModeWrite	  = adModeWrite,
		connectModeReadWrite	  = adModeReadWrite,
		connectModeShareDenyRead  = adModeShareDenyRead,
		connectModeShareDenyWrite = adModeShareDenyWrite,
		connectModeShareExclusive = adModeShareExclusive,
		connectModeShareDenyNone  = adModeShareDenyNone
	};

	ClsADODatabase();
	virtual ~ClsADODatabase();
	
	// Implementation.
	BOOL OpenPrompt();
	BOOL Open( LPCTSTR lpstrConnection = _T( "" ), LPCTSTR lpstrUserID = _T( "" ), LPCTSTR lpstrPassword = _T( "" ));
	BOOL Execute( LPCTSTR lpstrExec );
	DWORD GetRecordCount( _RecordsetPtr m_pRs );
	BOOL IsOpen();
	void Close();

	// Inlined implementation.
	inline _ConnectionPtr GetActiveConnection() const { return m_pConnection; }
	inline int GetRecordsAffected() const { return m_nRecordsAffected; }
	inline long BeginTransaction() { _ASSERT_VALID( m_pConnection ); return m_pConnection->BeginTrans(); }
	inline long CommitTransaction()	{ _ASSERT_VALID( m_pConnection ); return m_pConnection->CommitTrans(); }
	inline long RollbackTransaction() { _ASSERT_VALID( m_pConnection ); return m_pConnection->RollbackTrans(); }
	inline void SetConnectionMode( ClsADOConnectModeEnum nMode ) { _ASSERT_VALID( m_pConnection ); m_pConnection->PutMode(( enum ConnectModeEnum )nMode ); }
	inline void SetConnectionString( LPCTSTR lpstrConnection ) { _ASSERT_VALID( m_pConnection ); m_pConnection->ConnectionString = _bstr_t( lpstrConnection ); m_strConnection = lpstrConnection; }
	inline ClsString GetConnectionString() const { return m_strConnection; }
	inline void SetConnectionTimeout( long nConnectionTimeout = 30 ) { m_nConnectionTimeout = nConnectionTimeout; }

protected:
	// Data.
	_ConnectionPtr	m_pConnection;
	ClsString	m_strConnection;
	int		m_nRecordsAffected;
	long		m_nConnectionTimeout;
};

// ClsADORecordset class definition.
class ClsADORecordset : public ClsADOBase
{
	_NO_COPY( ClsADORecordset );
public:
	enum ClsADOOpenEnum
	{
		openUnknown    = 0,
		openQuery      = 1,
		openTable      = 2,
		openStoredProc = 3
	};

	enum ClsADOEditEnum
	{
		dbEditNone = 0,
		dbEditNew  = 1,
		dbEdit     = 2
	};
	
	enum ClsADOPositionEnum
	{
	
		positionUnknown = -1,
		positionBOF     = -2,
		positionEOF     = -3
	};
	
	enum ClsADOSearchEnum
	{	
		searchForward  = 1,
		searchBackward = -1
	};

	enum ClsADODataType
	{
		typeEmpty		= ADODB::adEmpty,
		typeTinyInt		= ADODB::adTinyInt,
		typeSmallInt		= ADODB::adSmallInt,
		typeInteger		= ADODB::adInteger,
		typeBigInt		= ADODB::adBigInt,
		typeUnsignedTinyInt	= ADODB::adUnsignedTinyInt,
		typeUnsignedSmallInt	= ADODB::adUnsignedSmallInt,
		typeUnsignedInt		= ADODB::adUnsignedInt,
		typeUnsignedBigInt	= ADODB::adUnsignedBigInt,
		typeSingle		= ADODB::adSingle,
		typeDouble		= ADODB::adDouble,
		typeCurrency		= ADODB::adCurrency,
		typeDecimal		= ADODB::adDecimal,
		typeNumeric		= ADODB::adNumeric,
		typeBoolean		= ADODB::adBoolean,
		typeError		= ADODB::adError,
		typeUserDefined		= ADODB::adUserDefined,
		typeVariant		= ADODB::adVariant,
		typeIDispatch		= ADODB::adIDispatch,
		typeIUnknown		= ADODB::adIUnknown,
		typeGUID		= ADODB::adGUID,
		typeDate		= ADODB::adDate,
		typeDBDate		= ADODB::adDBDate,
		typeDBTime		= ADODB::adDBTime,
		typeDBTimeStamp		= ADODB::adDBTimeStamp,
		typeBSTR		= ADODB::adBSTR,
		typeChar		= ADODB::adChar,
		typeVarChar		= ADODB::adVarChar,
		typeLongVarChar		= ADODB::adLongVarChar,
		typeWChar		= ADODB::adWChar,
		typeVarWChar		= ADODB::adVarWChar,
		typeLongVarWChar	= ADODB::adLongVarWChar,
		typeBinary		= ADODB::adBinary,
		typeVarBinary		= ADODB::adVarBinary,
		typeLongVarBinary	= ADODB::adLongVarBinary,
		typeChapter		= ADODB::adChapter,
		typeFileTime		= ADODB::adFileTime,
		typePropVariant		= ADODB::adPropVariant,
		typeVarNumeric		= ADODB::adVarNumeric,
		typeArray		= ADODB::adVariant
	};
	
	enum ClsADOSchemaType 
	{
		schemaSpecific			= adSchemaProviderSpecific,	
		schemaAsserts			= adSchemaAsserts,
		schemaCatalog			= adSchemaCatalogs,
		schemaCharacterSet		= adSchemaCharacterSets,
		schemaCollections		= adSchemaCollations,
		schemaColumns			= adSchemaColumns,
		schemaConstraints		= adSchemaCheckConstraints,
		schemaConstraintColumnUsage	= adSchemaConstraintColumnUsage,
		schemaConstraintTableUsage	= adSchemaConstraintTableUsage,
		shemaKeyColumnUsage		= adSchemaKeyColumnUsage,
		schemaTableConstraints		= adSchemaTableConstraints,
		schemaColumnsDomainUsage	= adSchemaColumnsDomainUsage,
		schemaIndexes			= adSchemaIndexes,
		schemaColumnPrivileges		= adSchemaColumnPrivileges,
		schemaTablePrivileges		= adSchemaTablePrivileges,
		schemaUsagePrivileges		= adSchemaUsagePrivileges,
		schemaProcedures		= adSchemaProcedures,
		schemaTables			= adSchemaTables,
		schemaProviderTypes		= adSchemaProviderTypes,
		schemaViews			= adSchemaViews,
		schemaProcedureParameters	= adSchemaProcedureParameters,
		schemaForeignKeys		= adSchemaForeignKeys,
		schemaPrimaryKeys		= adSchemaPrimaryKeys,
		schemaProcedureColumns		= adSchemaProcedureColumns,
		schemaDBInfoKeywords		= adSchemaDBInfoKeywords,
		schemaDBInfoLiterals		= adSchemaDBInfoLiterals,
		schemaCubes			= adSchemaCubes,
		schemaDimensions		= adSchemaDimensions,
		schemaHierarchies		= adSchemaHierarchies, 
		schemaLevels			= adSchemaLevels,
		schemaMeasures			= adSchemaMeasures,
		schemaProperties		= adSchemaProperties,
		schemaMembers			= adSchemaMembers,
	}; 

	ClsADORecordset();
	ClsADORecordset( ClsADODatabase* pAdoDatabase );
	virtual ~ClsADORecordset();

	BOOL SetFieldValue( int nIndex, int nValue );
	BOOL SetFieldValue( LPCTSTR lpFieldName, int nValue );
	BOOL SetFieldValue( int nIndex, long lValue );
	BOOL SetFieldValue( LPCTSTR lpFieldName, long lValue );
	BOOL SetFieldValue( int nIndex, unsigned long lValue );
	BOOL SetFieldValue( LPCTSTR lpFieldName, unsigned long lValue );
	BOOL SetFieldValue( int nIndex, double dblValue );
	BOOL SetFieldValue( LPCTSTR lpFieldName, double dblValue );
	BOOL SetFieldValue( int nIndex, const ClsString& strValue );
	BOOL SetFieldValue( LPCTSTR lpFieldName, const ClsString& strValue );
	BOOL SetFieldValue( int nIndex, const ClsDateTime& time );
	BOOL SetFieldValue( LPCTSTR lpFieldName, const ClsDateTime& time );
	BOOL SetFieldValue( int nIndex, bool bValue );
	BOOL SetFieldValue( LPCTSTR lpFieldName, bool bValue );
	BOOL SetFieldValue( int nIndex, const ClsCurrency& cyValue );
	BOOL SetFieldValue( LPCTSTR lpFieldName, const ClsCurrency& cyValue );
	BOOL SetFieldValue( int nIndex, const _variant_t& vtValue );
	BOOL SetFieldValue( LPCTSTR lpFieldName, const _variant_t& vtValue );
	BOOL SetFieldEmpty( int nIndex );
	BOOL SetFieldEmpty( LPCTSTR lpFieldName );

	BOOL Clone( ClsADORecordset& pRs );	
	void CancelUpdate();
	BOOL Update();
	void Edit();
	BOOL AddNew();
	BOOL AddNew( CADORecordBinding &pAdoRecordBinding );

	BOOL Find( LPCTSTR lpFind, int nSearchDirection = ClsADORecordset::searchForward );
	BOOL FindFirst( LPCTSTR lpFind );
	BOOL FindNext();

	BOOL RecordBinding( CADORecordBinding &pAdoRecordBinding );
	DWORD GetRecordCount();
	BOOL IsOpen();
	void Close();
	BOOL Open( _ConnectionPtr mpdb, LPCTSTR lpstrExec = _T( "" ), int nOption = ClsADORecordset::openUnknown );
	BOOL Open( LPCTSTR lpstrExec = _T( "" ), int nOption = ClsADORecordset::openUnknown );
	BOOL OpenSchema( int nSchema, LPCTSTR SchemaID = _T( "" ));
	
	BOOL GetFieldValue( LPCTSTR lpFieldName, int& nValue );
	BOOL GetFieldValue( int nIndex, int& nValue );
	BOOL GetFieldValue( LPCTSTR lpFieldName, long& lValue );
	BOOL GetFieldValue( int nIndex, long& lValue );
	BOOL GetFieldValue( LPCTSTR lpFieldName, unsigned long& ulValue );
	BOOL GetFieldValue( int nIndex, unsigned long& ulValue );
	BOOL GetFieldValue( LPCTSTR lpFieldName, double& dbValue );
	BOOL GetFieldValue( int nIndex, double& dbValue );
	BOOL GetFieldValue( LPCTSTR lpFieldName, ClsString& strValue, ClsString strDateFormat = _T( "" ));
	BOOL GetFieldValue( int nIndex, ClsString& strValue, ClsString strDateFormat = _T( "" ));
	BOOL GetFieldValue( LPCTSTR lpFieldName, ClsDateTime& time );
	BOOL GetFieldValue( int nIndex, ClsDateTime& time );
	BOOL GetFieldValue( int nIndex, bool& bValue );
	BOOL GetFieldValue( LPCTSTR lpFieldName, bool& bValue );
	BOOL GetFieldValue( int nIndex, ClsCurrency& cyValue );
	BOOL GetFieldValue( LPCTSTR lpFieldName, ClsCurrency& cyValue );
	BOOL GetFieldValue( int nIndex, _variant_t& vtValue );
	BOOL GetFieldValue( LPCTSTR lpFieldName, _variant_t& vtValue );
	
	BOOL IsFieldNull( LPCTSTR lpFieldName );
	BOOL IsFieldNull( int nIndex );
	BOOL IsFieldEmpty( LPCTSTR lpFieldName );
	BOOL IsFieldEmpty( int nIndex );	

	BOOL GetFieldInfo( LPCTSTR lpFieldName, StrADOFieldInfo* fldInfo );
	BOOL GetFieldInfo( int nIndex, StrADOFieldInfo* fldInfo );
	BOOL AppendChunk( LPCTSTR lpFieldName, LPVOID lpData, UINT nBytes );
	BOOL AppendChunk( int nIndex, LPVOID lpData, UINT nBytes );

	BOOL GetChunk( LPCTSTR lpFieldName, ClsString& strValue );
	BOOL GetChunk( int nIndex, ClsString& strValue );
	
	BOOL GetChunk( LPCTSTR lpFieldName, LPVOID pData );
	BOOL GetChunk( int nIndex, LPVOID pData );

	ClsString GetString( LPCTSTR lpCols, LPCTSTR lpRows, LPCTSTR lpNull, long numRows = 0 );

	inline ClsString GetQuery() const { return m_strQuery; }
	inline void SetQuery( LPCSTR strQuery ) { m_strQuery = strQuery; }
	inline long GetFieldCount() const { _ASSERT_VALID( m_pRecordset ); return m_pRecordset->Fields->GetCount(); }
	inline BOOL IsEof() { _ASSERT_VALID( m_pRecordset ); return m_pRecordset->adoEOF == VARIANT_TRUE; }
	inline BOOL IsEOF() { _ASSERT_VALID( m_pRecordset ); return m_pRecordset->adoEOF == VARIANT_TRUE; }
	inline BOOL IsBof() { _ASSERT_VALID( m_pRecordset ); return m_pRecordset->BOF == VARIANT_TRUE; }
	inline BOOL IsBOF() { _ASSERT_VALID( m_pRecordset ); return m_pRecordset->BOF == VARIANT_TRUE; }
	inline void MoveFirst() { _ASSERT_VALID( m_pRecordset ); m_pRecordset->MoveFirst(); }
	inline void MoveNext() { _ASSERT_VALID( m_pRecordset ); m_pRecordset->MoveNext(); }
	inline void MovePrevious() { _ASSERT_VALID( m_pRecordset ); m_pRecordset->MovePrevious(); }
	inline void MoveLast() { _ASSERT_VALID( m_pRecordset ); m_pRecordset->MoveLast(); }
	inline long GetAbsolutePage() { _ASSERT_VALID( m_pRecordset ); return m_pRecordset->GetAbsolutePage(); }
	inline void SetAbsolutePage( int nPage ) { _ASSERT_VALID( m_pRecordset ); m_pRecordset->PutAbsolutePage(( enum PositionEnum )nPage); }
	inline long GetPageCount() { _ASSERT_VALID( m_pRecordset ); return m_pRecordset->GetPageCount(); }
	inline long GetPageSize() { _ASSERT_VALID( m_pRecordset ); return m_pRecordset->GetPageSize(); }
	inline void SetPageSize( int nSize ) { _ASSERT_VALID( m_pRecordset ); m_pRecordset->PutPageSize(nSize); }
	inline long GetAbsolutePosition() { _ASSERT_VALID( m_pRecordset ); return m_pRecordset->GetAbsolutePosition(); }
	inline void SetAbsolutePosition( int nPosition ) { _ASSERT_VALID( m_pRecordset ); m_pRecordset->PutAbsolutePosition(( enum PositionEnum )nPosition ); }
	inline void GetBookmark() { m_varBookmark = m_pRecordset->Bookmark; }
	inline BOOL IsConnectionOpen() { return m_pConnection != NULL && m_pConnection->GetState() != adStateClosed; }
	inline _RecordsetPtr GetRecordset() { return m_pRecordset; }
	inline _ConnectionPtr GetActiveConnection() { return m_pConnection; }

	BOOL SetBookmark();
	BOOL Delete();

	BOOL SetFilter( LPCTSTR strFilter );
	BOOL SetSort( LPCTSTR lpstrCriteria );
	BOOL SaveAsXML( LPCTSTR lpstrXMLFile );
	BOOL OpenXML( LPCTSTR lpstrXMLFile );
	BOOL Execute( ClsADOCommand* pCommand );
	BOOL Requery();

protected:
	// Helpers.
	BOOL PutFieldValue( LPCTSTR lpFieldName, _variant_t vtFld );
	BOOL PutFieldValue( _variant_t vtIndex, _variant_t vtFld );
	BOOL GetFieldInfo( FieldPtr pField, StrADOFieldInfo* fldInfo );
	BOOL GetChunk( FieldPtr pField, ClsString& strValue );
	BOOL GetChunk( FieldPtr pField, LPVOID lpData );
	BOOL AppendChunk( FieldPtr pField, LPVOID lpData, UINT nBytes );		

	// Data.
	_RecordsetPtr		m_pRecordset;
	_CommandPtr		m_pCmd;
	_ConnectionPtr		m_pConnection;
	int			m_nSearchDirection;
	ClsString		m_strFind;
	_variant_t		m_varBookFind;
	_variant_t		m_varBookmark;
	int			m_nEditStatus;
	IADORecordBinding      *m_pRecBinding;
	ClsString		m_strQuery;
};

// ClsADOParameter class definition.
class ClsADOParameter : public ClsADOBase
{
	_NO_COPY( ClsADOParameter );
public:
	enum ClsADOParameterDirection
	{
		paramUnknown	 = adParamUnknown,
		paramInput	 = adParamInput,
		paramOutput	 = adParamOutput,
		paramInputOutput = adParamInputOutput,
		paramReturnValue = adParamReturnValue 
	};

	ClsADOParameter( int nType, long lSize = 0, int nDirection = paramInput, const ClsString& strName = _T( "" ));
	virtual ~ClsADOParameter();

	BOOL SetValue( int nValue );
	BOOL SetValue( long lValue );
	BOOL SetValue( double dbValue );
	BOOL SetValue( const ClsString& strValue );
	BOOL SetValue( const ClsDateTime& time );
	BOOL SetValue( const _variant_t& vtValue );
	BOOL GetValue( int& nValue );
	BOOL GetValue( long& lValue );
	BOOL GetValue( double& dbValue );
	BOOL GetValue( ClsString& strValue, ClsString strDateFormat = _T( "" ));
	BOOL GetValue( ClsDateTime& time );
	BOOL GetValue( _variant_t& vtValue );
	
	inline void SetPrecision( int nPrecision ) { _ASSERT_VALID( m_pParameter ); m_pParameter->PutPrecision(( unsigned char )nPrecision ); }
	inline void SetScale( int nScale ) { _ASSERT_VALID( m_pParameter ); m_pParameter->PutNumericScale(( unsigned char )nScale ); }
	inline void SetName( ClsString strName ) { m_strName = strName; }
	inline ClsString GetName() { return m_strName; }
	inline int GetType() { return m_nType; }
	inline _ParameterPtr GetParameter() { return m_pParameter; }

protected:
	// Data.
	_ParameterPtr	m_pParameter;
	ClsString	m_strName;
	int		m_nType;
};

// ClsADOCommand class definition.
class ClsADOCommand : public ClsADOBase
{
	_NO_COPY( ClsADOCommand );
public:
	enum ClsADOCommandType
	{
		typeCmdText	   = adCmdText,
		typeCmdTable	   = adCmdTable,
		typeCmdTableDirect = adCmdTableDirect,
		typeCmdStoredProc  = adCmdStoredProc,
		typeCmdUnknown	   = adCmdUnknown,
		typeCmdFile	   = adCmdFile
	};
	
	ClsADOCommand( ClsADODatabase* pAdoDatabase, const ClsString& strCommandText = _T( "" ), int nCommandType = typeCmdStoredProc );
	virtual ~ClsADOCommand();		

	void SetText( const ClsString& strCommandText );
	void SetType( int nCommandType );

	BOOL AddParameter( ClsADOParameter* pAdoParameter );
	BOOL AddParameter( const ClsString& strName, int nType, int nDirection, long lSize, int nValue );
	BOOL AddParameter( const ClsString& strName, int nType, int nDirection, long lSize, long lValue );
	BOOL AddParameter( const ClsString& strName, int nType, int nDirection, long lSize, double dblValue, int nPrecision = 0, int nScale = 0 );
	BOOL AddParameter( const ClsString& strName, int nType, int nDirection, long lSize, const ClsString& strValue );
	BOOL AddParameter( const ClsString& strName, int nType, int nDirection, long lSize, const ClsDateTime& time );
	BOOL AddParameter( const ClsString& strName, int nType, int nDirection, long lSize, const _variant_t& vtValue, int nPrecision = 0, int nScale = 0 );
	BOOL Execute();

	inline void SetTimeout( long nTimeOut ) { _ASSERT_VALID( m_pCommand ); m_pCommand->PutCommandTimeout( nTimeOut ); }
	inline int GetType() { return m_nCommandType; }
	inline ClsString GetText() { return m_strCommandText; }
	inline int GetRecordsAffected() { return m_nRecordsAffected; }
	inline _CommandPtr GetCommand() { return m_pCommand; }

protected:
	// Data.
	_CommandPtr	m_pCommand;
	int		m_nCommandType;
	int		m_nRecordsAffected;
	ClsString	m_strCommandText;
};

// ClsADOException class definition.
class ClsADOException : public ClsException
{
	_NO_COPY( ClsADOException );
public:
	enum
	{
		noError,	// no error
		Unknown,	// unknown error
	};

	ClsADOException( int nCause = 0, const ClsString& strErrorString = _T( "" ));
	virtual ~ClsADOException();

	static int GetError( int nADOError );
};

#endif // _ADO2_H_
#endif // _IMPORT_ADO_