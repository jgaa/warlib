
#include <ctime>
#include <cstring>
#include <iomanip>
#include <thread>

#include <warlib/WarLog.h>
#include <warlib/impl.h>

#include <boost/filesystem.hpp>

std::ostream& operator << (std::ostream& out, const war::log::LogLevel& level)
{
    return out << war::log::LogEventHandler::GetLevelName(level);
}

std::ostream& operator << (std::ostream& out, const war::log::Errno& err)
{
    return err.Explain(out);
}

std::ostream& operator << (std::ostream& out, const war::log::Esc& esc)
{
    return out << '\"' << esc.GetVal() << '\"';
}


namespace war { namespace log {


    //////////////////////////////////// LogEngine /////////////////////////////////////

    LogEngine *LogEngine::instance_;
    LogLevel LogEngine::current_level_;
    filter_t LogEngine::current_filter_;

    LogEngine::LogEngine()
    {
        WAR_ASSERT(0 == instance_);
        instance_ = this;
    }

    LogEngine::~LogEngine()
    {
        WAR_ASSERT(instance_ == this);
        instance_ = 0;
        current_level_ = LL_FATAL;
        current_filter_ = 0;
    }

    void LogEngine::DoSubmit ( Log& log ) noexcept
    {
        try {
            const LogEventHandler::SubmitInfo si = {
                log.GetLevel(),
                log.GetFilter(),
                std::move(log.Get().str()),
                std::chrono::system_clock::now() };

            WAR_LOCK;
            for(LogEventHandler::ptr_t &h: handlers_) {
                if (si.level_ <= h->GetLevel()) {
                    h->Submit(si);
                }
            }
        } catch(...) {
            // Fatal. We can not continue.
            std::cerr << "Failed to log event" <<std::endl;
            std::terminate();
        }
    }

    void LogEngine::AddHandler (LogEventHandler::ptr_t handler)
    {
        {
            WAR_LOCK;
            handlers_.push_back (handler);
        }

        LOG_DEBUG << "Added log-handler \"" << handler->GetName() << "\".";

        UpdateLevelAndFilter();
    }

    void LogEngine::UpdateLevelAndFilter()
    {
        LogLevel level = LL_FATAL;
        filter_t filter = 0;

        {
            WAR_LOCK;
            for (const auto & h: handlers_) {
                if ( h->GetLevel() > level )
                    level = h->GetLevel();

                filter |= h->GetFilter();
            }

            current_level_ = level;
            current_filter_ = filter;
        }

        std::ostringstream filters;
        LogEventHandler::WriteFilter(filters, current_filter_, 0);
        LOG_DEBUG << "Log-level is now " << current_level_ << " and filter is "
            << filters.str() << '(' << current_filter_ << ')';
    }

    LogLevel LogEngine::GetLevelFromName(const std::string& name) noexcept
    {
        for(int ll = LL_FATAL; ll <= LL_TRACE4; ++ll) {
            if (name == LogEventHandler::GetLevelName((LogLevel)ll))
                return static_cast<LogLevel>(ll);
        }

        std::ostringstream msg;
        msg << "No such log-level: " << "\""  << name << "\"";
        WAR_THROW(msg.str());
    }


    //////////////////////////////////// LogEventHandler /////////////////////////////////////



    void LogEventHandler::WriteLevel(std::ostream& out,
                                     const SubmitInfo &si) const noexcept
    {
        out << si.level_;
    }

    void LogEventHandler::WriteFilter(std::ostream& out,
                                      const SubmitInfo &si) const noexcept
    {
        WriteFilter(out, si.filter_);
    }

    void LogEventHandler::WriteFilter(std::ostream& out, const filter_t filter,
                                      const filter_t hide) noexcept
    {
        bool virgin = true;
#define ADD(name) if ((filter & (LA_ ## name & ~hide)) == LA_ ## name) \
        { \
            if (!virgin) { \
                out << "|"; \
            } \
            out << #name; \
            virgin=false; \
        }
        ADD(GENERAL)
        ADD(SECURITY)
        ADD(TRANSFER)
        ADD(AUTH)
        ADD(IO)
        ADD(NETWORK)
        ADD(THREADS)
        ADD(IPC)
        ADD(STATS)
        ADD(FUNCTION_CALL)
#undef ADD
        if (!virgin) {
            out << ' ';
        }
    }

    void LogEventHandler::WriteTimestamp(std::ostream& out,
                                         const SubmitInfo &si) const noexcept
    {
        std::tm my_tm{};
        time_t when = std::chrono::system_clock::to_time_t(si.time_);
        auto when_rounded = std::chrono::system_clock::from_time_t( when );
        if (when_rounded > si.time_) {
            --when;
            when_rounded -= std::chrono::seconds(1);
        }
        int milliseconds = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>
            (si.time_ - when_rounded).count();

        if (war_localtime(when, my_tm)) {
        // No support for put_time in g++ 4.8

            out << std::setfill('0') << std::setw(4) << (my_tm.tm_year + 1900)
                << '-' << std::setw(2) << (my_tm.tm_mon + 1)
                << '-' << std::setw(2) << my_tm.tm_mday
                << ' ' << std::setw(2) << my_tm.tm_hour
                << ':' << std::setw(2) << my_tm.tm_min
                << '.' << std::setw(3) << std::setfill('0') << milliseconds;

//          out << std::put_time(&my_tm, "%Y-%m-%d %H:%M:%S.")
//              << std::setw(3) << std::setfill('0') << milliseconds;
        }
        else {
            out << "0000-00-00 00:00:00.000";
        }
    }

    void LogEventHandler::WriteMessage(std::ostream& out,
                                       const SubmitInfo &si) const noexcept
    {
        std::string::const_iterator ch = si.buf_.begin();
        const std::string::const_iterator end = si.buf_.end();
        for(; ch != end; ++ch) {
            const char c = *ch;
            if ('\r' == c)
                continue;
            if ('\n' == c) {
                if (++ch == end)
                    break; // end of buffer. Do nothing.
                --ch;
                out << std::endl << "  ";
            } else if (std::iscntrl(c, locale_) && !std::isspace(c, locale_)) {
                // Output '%xx' URL style encoding for control characters
                const std::ios::fmtflags saved = out.flags();
                out << '%' << std::setw(2) << std::setfill('0') << std::hex << ((unsigned int)c);
                out.flags(saved);
            } else {
                out << c;
            }
        }
    }

    const char *LogEventHandler::GetLevelName(const LogLevel level) noexcept
    {
        static const char *names[] = { "FATAL", "ERROR", "WARN", "INFO",
            "NOTICE", "DEBUG", "TRACE1", "TRACE2", "TRACE3", "TRACE4" };

        WAR_ASSERT(level >= war::log::LL_FATAL);
        WAR_ASSERT(level <= war::log::LL_TRACE4);

        const char *name = names[level];

        return name;
    }

    void LogEventHandler::WriteDefaulInfo(std::ostream& out,
                                          const SubmitInfo &si) const noexcept
    {
        WriteTimestamp(out, si);
        out << ' ' << std::this_thread::get_id() << ' ';
        WriteLevel(out, si);
        out << ": ";
        WriteFilter(out, si);
        WriteMessage(out, si);
        out << std::endl;
    }

    //////////////////////////////////// LogToFile /////////////////////////////////////

    LogToFile::LogToFile(const path_t& path,
        const bool truncateFileOnOpen,
        const std::string& name, const LogLevel level,
        const filter_t filter)
        : LogEventHandler(name, level, filter), path_(path)
    {
        std::ios_base::openmode mode = std::ios_base::out | std::ios_base::app;
        if (truncateFileOnOpen && boost::filesystem::is_regular_file(path))
            mode = std::ios_base::out | std::ios_base::trunc;

        out_.open(path.c_str(), mode);
        if (!out_.is_open()) {
            WAR_EXCEPTION("Failed to open log-file for append")
                << boost::errinfo_file_name(path_.c_str())
                << boost::errinfo_errno(errno);
            WAR_EXCEPTION_THROW;
        }

        out_.sync_with_stdio(false);
    }

    LogToFile::LogEventHandler::ptr_t LogToFile::Create(const path_t& path,
        const bool truncateFileOnOpen,
        const std::string& name,
        const LogLevel level,
        const filter_t filter)
    {
        return LogEventHandler::ptr_t(new LogToFile(path, truncateFileOnOpen,
                                                    name, level, filter));
    }


    //////////////////////////////////// Errno /////////////////////////////////////

    std::ostream& Errno::Explain(std::ostream& out) const
    {
        const char *errstr = "*** Missing error explanation ***";
#ifdef __GNUC__
        char buf[512] = {};
        strerror_r(GetErrno(), buf, sizeof(buf));
        errstr = buf;
#elif defined(_MSC_VER)
        char buf[512] = {};
        if (strerror_s(buf, sizeof(buf), GetErrno()) == 0)
            errstr = buf;
#elif defined(WIN32)
        errstr = strerror(GetErrno());
#else
#   error "Don't know how to fetch error string"
#endif

        return out << "{errno " << err_ << ": " << errstr << "}";
    }

    std::string Errno::Explain() const
    {
        std::ostringstream out;
        Explain(out);
        return out.str();
    }

}} // namespacesl

