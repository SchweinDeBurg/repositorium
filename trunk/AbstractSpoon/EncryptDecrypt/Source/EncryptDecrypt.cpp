// Copyright by Richard Butler.

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - added AbstractSpoon Software copyright notice and licenese information
// - adjusted #include's paths
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-switches
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
//*****************************************************************************

// EncryptDecrypt.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"
#include "Encryption.h"

BOOL APIENTRY DllMain(HANDLE /*hModule*/, DWORD  /*ul_reason_for_call*/, LPVOID /*lpReserved*/)
{
	return TRUE;
}

IEncryption* CreateEncryptionInterface()
{
	return new CEncryption;
}
