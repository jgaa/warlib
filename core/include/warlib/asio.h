#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

namespace war {

using io_context_t = boost::asio::io_context;
using timer_t = std::shared_ptr<boost::asio::deadline_timer>;
using resolver_t = std::shared_ptr<boost::asio::ip::tcp::resolver>;
}

// class ostream;
// ostream& operator << (ostream& o, const boost::asio::ip::tcp::socket& v);

