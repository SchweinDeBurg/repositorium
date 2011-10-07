// Copyright by Richard Butler.

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - added AbstractSpoon Software copyright notice and license information
// - adjusted #include's paths
// - reformatted using Artistic Style 2.02 with the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-cases
//      --max-instatement-indent=2
//      --style=allman
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
//*****************************************************************************

// EncryptDecrypt.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"
#include "Encryption.h"

BOOL APIENTRY DllMain(HANDLE /*hModule*/, DWORD /*dwReason*/, LPVOID /*lpReserved*/)
{
	return TRUE;
}

IEncryption* CreateEncryptionInterface()
{
	return new CEncryption;
}
