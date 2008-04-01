#ifndef _CRC16_H_
#define _CRC16_H_
//
// crc16.h
//
// (C) Copyright 1999-2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"

// Class to perform 16 bit CRC checksumming.
class ClsCrc16
{
public:
	// Constructor/Destructor.
	ClsCrc16();
	ClsCrc16( WORD wPolynomial );
	virtual ~ClsCrc16();

	// Implementation
	void CrcInitialize( WORD wPolynomial = 0x1021 );
	WORD CrcAdd( LPBYTE pData, DWORD dwLength );
	WORD Crc( LPBYTE pData, DWORD dwLength );
	inline void CrcReset() { m_sCRC = 0xFFFF; }
	inline WORD CrcGet() const { return ( WORD )( m_sCRC ^ 0xFFFF ); }
	inline WORD CrcTableIndex( int nIndex ) const { _ASSERT( nIndex >= 0 && nIndex <= 255 ); _ASSERT_VALID( m_pCrcTable ); return m_pCrcTable[ nIndex ]; };

	// Operator overloads.
	inline BOOL operator==( WORD wCrc ) const { return ( BOOL )(( m_sCRC ^ 0xFFFF ) == wCrc ); }

protected:
	// Data
	static WORD	s_wCrc16[ 256 ];
	WORD	       *m_pCrcAllocated;
	WORD	       *m_pCrcTable;
	WORD		m_sCRC;
};
#endif