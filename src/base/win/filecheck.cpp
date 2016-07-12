

#include <windows.h>
#include "war_filecheck.h"


bool validate_filename_as_safe(const boost::filesystem::path& path)
{
    WIN32_FIND_DATA info;
    static const FILETIME ft_zero = {0,0};
    HANDLE h;

    // Devices usually have date set to zero
    h = FindFirstFile(name, &info);
    if (h != INVALID_HANDLE_VALUE)
    {
        FindClose(h);

        if ((memcmp(&info.ftCreationTime, &ft_zero, sizeof(ft_zero)) == 0)
            && (info.nFileSizeHigh == 0)
            && (info.nFileSizeLow == 0))
            return true;
    }

    // Don't look like a device
    return false;
}

