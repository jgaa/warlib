#pragma once

#ifdef _MSC_VER
#   include <warlib/win/msvc_pragmas.h>
#endif

#include <functional>

namespace war {

using func_t = std::function<void ()>;
using task_t = std::pair<func_t, const char *>;
typedef std::function<void ()> war_func_t;

} // namespace

#ifdef __GNUC__
#	ifndef LIKELY
#		define LIKELY(expr) __builtin_expect(!!(expr), 1)
#	endif

#	ifndef UNLIKELY
#		define UNLIKELY(expr) __builtin_expect(!!(expr), 0)
#	endif
#else
#	ifndef LIKELY
#		define LIKELY(expr) expr
#	endif

#	ifndef UNLIKELY
#		define UNLIKELY(expr) expr
#	endif
#endif

#include <warlib/error_handling.h>
