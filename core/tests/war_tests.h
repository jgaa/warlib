#pragma once

#include <set>
#include <vector>
#include <set>

#include "lest/lest.hpp"

#include <warlib/WarLog.h>

namespace warlib {
namespace {

#define STARTCASE(name) { CASE(#name) { \
    LOG_DEBUG << "================================"; \
    LOG_INFO << "Test case: " << #name; \
    LOG_DEBUG << "================================";

#define ENDCASE \
    LOG_DEBUG << "============== ENDCASE ============="; \
}},

template<typename T1, typename T2>
bool compare(const T1& left, const T2& right) {
    const auto state = (left == right);
    if (!state) {
        std::cerr << ">>>> '" << left << "' is not equal to '" << right << "'" << std::endl;
    }
    return state;
}
} // anonymous namespace

} // warlib
