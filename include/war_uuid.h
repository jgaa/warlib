 #pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>


namespace war {

inline std::string get_uuid_as_string() {
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    return boost::uuids::to_string(uuid);
}

inline boost::uuids::uuid get_uuid_from_string(const std::string& str) {
    return boost::uuids::string_generator()(str);
}

} // war
