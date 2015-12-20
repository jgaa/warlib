#pragma once

#include <war_error_handling.h>
#include <boost/lexical_cast.hpp>

namespace war {

/*! Add a new, unique entry to a map
 *
 * \exception war::ExceptionAlreadyExist if the value already exist in the map.
 */
template <typename ContT, typename KeyT, typename ValT>
void WarMapAddUnique(ContT& container, const KeyT& key, ValT&& val) {

    if (container.find(key) != container.end()) {
        WAR_THROW_T(war::ExceptionAlreadyExist, boost::lexical_cast<std::string>(key));
    }

    container[key] = val;
}

} // war
