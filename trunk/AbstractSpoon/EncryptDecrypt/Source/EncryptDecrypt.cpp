// EncryptDecrypt.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"
#include "Encryption.h"

BOOL APIENTRY DllMain( HANDLE /*hModule*/, 
                       DWORD  /*ul_reason_for_call*/, 
                       LPVOID /*lpReserved*/
					 )
{
    return TRUE;
}

IEncryption* CreateEncryptionInterface() 
{
	return new CEncryption;
}
