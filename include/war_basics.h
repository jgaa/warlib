#pragma once

#include <functional>

namespace war {

using func_t = std::function<void ()>;
using task_t = std::pair<func_t, const char *>;
typedef std::function<void ()> war_func_t;

} // namespace

#ifndef LIKELY
#   define LIKELY(expr) __builtin_expect(!!(expr), 1)
#endif

#ifndef UNLIKELY
#define UNLIKELY(expr) __builtin_expect(!!(expr), 0)
#endif

#include "war_error_handling.h"
