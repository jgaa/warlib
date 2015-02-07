#pragma once

#include <ctime>
/* Practical stuff for implementation use */

#include "war_basics.h"
#include "war_debug_helper.h"

#define WAR_LOCK std::lock_guard<std::mutex> __war_lock(lock_)
#define WAR_UNLOCK __war_lock.unlock();

#ifdef WIN32
    inline int strcasecmp(const char *s1, const char *s2) {
        return stricmp(s1, s2);
    }
#endif

#ifdef WIN32
    inline bool war_localtime(const std::time_t& when, std::tm& tm) {
        return localtime_s(&tm, &when) != 0;
    }
#else
    inline bool war_localtime(const std::time_t& when, std::tm& tm) {
        return localtime_r(&when, &tm) != nullptr;
    }
#endif

#ifdef WIN32
    inline bool war_gmtime(const std::time_t& when, std::tm& tm) {
        return gmtime_s(&tm, &when) != 0;
    }
#else
    inline bool war_gmtime(const std::time_t& when, std::tm& tm) {
        return gmtime_r(&when, &tm) != nullptr;
    }
#endif

// Microsoft loves to mess up the public name space with lame macros
#ifdef WIN32
#   ifdef min
#       undef min
#   endif
#   ifdef max
#       undef max
#   endif
#endif

