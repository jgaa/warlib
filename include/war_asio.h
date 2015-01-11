#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

namespace war {

using io_service_t = boost::asio::io_service;
using timer_t = std::shared_ptr<boost::asio::deadline_timer>;
using resolver_t = std::shared_ptr<boost::asio::ip::tcp::resolver>;

}

// class ostream;
// ostream& operator << (ostream& o, const boost::asio::ip::tcp::socket& v);

