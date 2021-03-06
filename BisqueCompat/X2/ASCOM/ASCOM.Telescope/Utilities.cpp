//========================================================================
//
// TITLE:		UTILITIES.CPP
//
// FACILITY:	X2 Plugin for TheSky X and ASCOM drivers
//
// ABSTRACT:	
//
// USING:		
//
// ENVIRONMENT:	Microsoft Windows XP/Vista/7
//				Developed under Microsoft Visual C++ 9 (VS2008)
//
// AUTHOR:		Robert B. Denny
//
// Edit Log:
//
// When			Who		What
//----------	---		--------------------------------------------------
// 22-Apr-11	rbd		Initial edit, taken from TeleAPI/ASCOM plugin
// 12-Jul-11	rbd		0.9.3 - Log exceptions to TheSky X's comm log
// 12-Oct-11	rbd		0.9.5 - Improve error reporting in DrvFail.
// 04-Jan-12	rbd		1.0.1 - Release. No changes.
//========================================================================
#include "StdAfx.h"

// --------------
// ansi_to_bstr() - Convert ANSI to BSTR
// --------------
//
BSTR ansi_to_bstr(char *s)
{
	OLECHAR *os = ansi_to_uni(s);			// Convert to UNI
	BSTR bs = SysAllocString(os);			// Make BSTR

	if(bs == NULL)
	{
		drvFail("Out of memory in BSTR conversion.", NULL, true);
		return NULL;
	}

	free(os);						// Junk work buffer
	return(bs);								// Return BSTR
}


// -------------
// ansi_to_uni() - Convert ANSI to alloc()'ed unicode
// -------------
//
// Convert ANSI to Unicode, return malloc()'ed Unicode string
//
OLECHAR *ansi_to_uni(char *s)
{
	int ol = MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0);
	if(0 == ol)
	{
		drvFail("Unicode conversion failed.", NULL, true);
		return NULL;
	}

	OLECHAR *os = (OLECHAR *)malloc((ol + 4) * sizeof(WCHAR));
	if (NULL == os) 
	{
		drvFail("Out of memory in unicode conversion.", NULL, true);
		return NULL;
	}


	if (0 == MultiByteToWideChar(CP_ACP, 0, s, -1, os, ol))
	{
		//DWORD dwRes = GetLastError();	// Wiped out by ->Free()
		free(os);
		drvFail("Unicode conversion failed.", NULL, true);
		return NULL;
	}

	os[ol] = WCHAR(0);					// Assure terminated
	return(os);
}


// -------------
// uni_to_ansi() - Convert unicode to ANSI, return pointer to new[]'ed string
// -------------
//
char *uni_to_ansi(OLECHAR *os)
{
	char *cp;

	// Is this the right way??? (it works)
	int len = WideCharToMultiByte(CP_ACP,
								0,
								os, 
								-1, 
								NULL, 
								0, 
								NULL, 
								NULL); 
	cp = new char[len + 5];
	if(cp == NULL)
		drvFail("Out of memory.", NULL, true);
	if (0 == WideCharToMultiByte(CP_ACP, 
									0, 
									os, 
									-1, 
									cp, 
									len, 
									NULL, 
									NULL)) 
	{
		delete [] cp;
		drvFail("Failed to convert unicode to ansi.", NULL, true);
	}

	cp[len] = '\0';
	return(cp);
}


// -------
// drvFail
// -------
//
// Called to display a message box and then generate an exception
// that should be caught for recovery. This handles Error objects
// generated by the Automation object. If there is a live error 
// object and it has a non-empty Description string, then the 
// description string will be the error message. If not, then the
// string passed to us by the caller is the error.
//
void drvFail(char *msg, EXCEPINFO *ei, bool bCloseConn)
{
	bool handled = false;
	char buf[4096];

	if(!_bDoingInit)
	{
		if(ei != NULL)
		{
			__try
			{
				char *dsc = uni_to_ansi(ei->bstrDescription);
				char *src = uni_to_ansi(ei->bstrSource);
				_pLogger->out("== ERROR FROM DRIVER ==");
				_pLogger->out(msg);
				_pLogger->out(src);
				_pLogger->out(dsc);
				_pLogger->out("=======================");
				sprintf(buf, "%s\n%s", msg, dsc);
				MessageBox(NULL, buf, src, (MB_OK | MB_ICONSTOP | MB_SETFOREGROUND));
				delete [] dsc;
				delete[] src;
				handled = true;
			}
			__except(EXCEPTION_EXECUTE_HANDLER)
			{
				handled = false;
			}
		}
		if(!handled)
			MessageBox(NULL, msg, _szAlertTitle, (MB_OK & MB_ICONSTOP | MB_SETFOREGROUND));

		if(bCloseConn)												// If was a fatal error, 
			TermScope(true);										// shut ourselves down best efforts
	}
	ABORT;
}


// -------------------
// CreateRegistryKey()
// -------------------
//
// Creates a single key specified by pszSubKey. Again, taken from NTDLS' 
// answer to 
//
// http://stackoverflow.com/questions/508614/create-a-new-windows-registry-key-using-c
//
bool CreateRegistryKey(HKEY hKeyRoot, LPCTSTR pszSubKey)
{
    HKEY hKey;
    DWORD dwFunc;
    LONG  lRet;

    //------------------------------------------------------------------------------

    SECURITY_DESCRIPTOR SD;
    SECURITY_ATTRIBUTES SA;

    if(!InitializeSecurityDescriptor(&SD, SECURITY_DESCRIPTOR_REVISION))
        return false;

    if(!SetSecurityDescriptorDacl(&SD, true, 0, false))
        return false;

    SA.nLength             = sizeof(SA);
    SA.lpSecurityDescriptor = &SD;
    SA.bInheritHandle      = false;

    //------------------------------------------------------------------------------

    lRet = RegCreateKeyEx(
        hKeyRoot,
        pszSubKey,
        0,
        (LPTSTR)NULL,
        REG_OPTION_NON_VOLATILE,
        KEY_WRITE,
        &SA,
        &hKey,
        &dwFunc
    );

    if(lRet == ERROR_SUCCESS)
    {
        RegCloseKey(hKey);
        hKey = (HKEY)NULL;
        return true;
    }

    SetLastError((DWORD)lRet);
    return false;
}

// ----------------------------
// CreateRegistryKeyStructure()
// ----------------------------
//
// Create keys as needed for the entire path given. Taken from NTDLS' 
// answer to 
//
// http://stackoverflow.com/questions/508614/create-a-new-windows-registry-key-using-c
//
// I would have done it recursively, but I am lazy and just used this code.
//
// FAILURES WILL NOT RAISE ERRORS. Returns the number of subkeys created.
//
int CreateRegKeyStructure(HKEY hKey, const char *sPath)
{
    char sDir[MAX_PATH];

    int iNameSz = (int)strlen(sPath);
	int iCount = 0;
	int iPos = 0;

    for(iPos = 0; iPos < iNameSz; iPos++)
    {
        if(sPath[iPos] == '\\' || sPath[iPos] == '/')
        {
            sDir[iPos] = '\0';
			if(CreateRegistryKey(hKey, sDir))
			{
				iCount++;
			}
        }

        sDir[iPos] = sPath[iPos];
    }

     sDir[iPos] = '\0';
     if(CreateRegistryKey(hKey, sDir))
	 {
		 iCount++;
	 }

	 return iCount;
}

