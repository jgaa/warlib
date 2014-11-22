#pragma once

#ifdef WIN32
#	include <windows.h>
const DWORD MS_VC_EXCEPTION=0x406D1388;
#endif

namespace war {
namespace debug {

// http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
#ifdef _MSC_VER
#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
    DWORD dwType; // Must be 0x1000.
    LPCSTR szName; // Pointer to name (in user addr space).
    DWORD dwThreadID; // Thread ID (-1=caller thread).
    DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

inline void SetThreadName(const std::string& name)
{
    if (::IsDebuggerPresent()) {
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = name.c_str();
        info.dwThreadID = -1;
        info.dwFlags = 0;

        __try
        {
            RaiseException(MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR),
                            (ULONG_PTR*)&info);
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
        }
    }
}

#else
inline void SetThreadName(const std::string& threadName) {}
#endif

}
} // namespaces

