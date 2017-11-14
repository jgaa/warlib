#pragma once

#include <boost/filesystem.hpp>

namespace war {

/*! In Windows, there is no sanity checks at all, at any level.
 *
 * If you open files with certain "magic" names, Windows will
 * gladly hang, crash or open up for malicious attacks.
 * We therefore have to check that a file-name received over the
 * Internet is safe to open before we can open it.
 */

#ifdef WIN32
bool validate_filename_as_safe(const boost::filesystem::path& path);
#else
inline bool validate_filename_as_safe(const boost::filesystem::path& path) {
    return true;
}
#endif

} // namespace
