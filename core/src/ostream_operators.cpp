// General ostream operators


#include <warlib/WarLog.h>


std::ostream& operator << (std::ostream& out, const std::exception& ex)
{
    return out << ex.what();
}

std::ostream& operator << (std::ostream& out, const war::ExceptionBase& ex)
{
    return out << boost::diagnostic_information(ex);
}

std::ostream& operator << (std::ostream& out, const boost::exception& ex)
{
    return out << boost::diagnostic_information(ex);
}

std::ostream& operator << (std::ostream& out, const boost::system::error_code& err)
{
    return out << "{ error-code=" << err.value()
        << ", category=" << war::log::Esc(err.category().name())
        << ", message=" << war::log::Esc(err.message())
        << " }";
}

std::ostream& operator << (std::ostream& out, const war::log::Timer& timer)
{
    return timer.Stream(out);
}
