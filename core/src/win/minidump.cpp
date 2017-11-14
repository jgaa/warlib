
//#include "war.h"
#pragma warning( disable : 4091 /* ms header files */)

#include <Windows.h>
#include <DbgHelp.h>
#include <sstream>
#include <tchar.h>
#include <process.h>
#include <cstdio>
#include <ctime>
#include <limits.h>
#include "win/minidump.h"

namespace war {

	static TCHAR dump_path[MAX_PATH] = {};

	static void CreateMiniDump( EXCEPTION_POINTERS* pep ) 
	{
		// Open the file 
		typedef BOOL (*PDUMPFN)( 
			HANDLE hProcess, 
			DWORD ProcessId, 
			HANDLE hFile, 
			MINIDUMP_TYPE DumpType, 
			PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, 
			PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, 
			PMINIDUMP_CALLBACK_INFORMATION CallbackParam
			);


		HANDLE hFile = CreateFile(dump_path, GENERIC_READ | GENERIC_WRITE, 
			0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ); 

		const HMODULE h = ::LoadLibrary(_T("DbgHelp.dll"));
		if (h) {
			PDUMPFN pFn = (PDUMPFN)GetProcAddress(h, "MiniDumpWriteDump");

			if( ( hFile != NULL ) && ( hFile != INVALID_HANDLE_VALUE ) ) 
			{
				// Create the minidump 

				MINIDUMP_EXCEPTION_INFORMATION mdei; 

				mdei.ThreadId           = GetCurrentThreadId(); 
				mdei.ExceptionPointers  = pep; 
				mdei.ClientPointers     = TRUE; 

				MINIDUMP_TYPE mdt       = MiniDumpNormal; 

				BOOL rv = (*pFn)( GetCurrentProcess(), GetCurrentProcessId(), 
					hFile, mdt, (pep != 0) ? &mdei : 0, 0, 0 ); 

				CloseHandle( hFile ); 
			}

			FreeLibrary(h);
		}
	}

	static LONG WINAPI UnhandledExceptionFilter(struct _EXCEPTION_POINTERS *ExceptionInfo)
	{
		if (dump_path[0]) {
			CreateMiniDump(ExceptionInfo);
		}
		return EXCEPTION_EXECUTE_HANDLER;
	}

	void EnableMinidump(const std::string& baseName) {
		
		if (GetTempPath(sizeof(dump_path) / sizeof(TCHAR), dump_path)) {
			std::basic_ostringstream<TCHAR> path;
			path << dump_path << _T("\\") << baseName << std::time(NULL) <<_T("_") << _getpid() << _T(".dmp");
            strcpy_s(dump_path, path.str().c_str());

			SetUnhandledExceptionFilter(UnhandledExceptionFilter);
		}
	}

} // namespace

