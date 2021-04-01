#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <list>
#include <ctime>
#include <mutex>
#include <cerrno>
#include <locale>
#include <memory>
#include <vector>
#include <chrono>
#include <boost/system/error_code.hpp>
#include <boost/utility/string_ref.hpp>
#include <warlib/basics.h>


#define __WAR_LOG_WITH_LEVEL_AND_FILTER(level, filter) war::log::LogEngine::IsRelevant(level, filter) && war::log::Log(level, filter).Get()

#define LOG_FATAL __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_FATAL, war::log::LA_GENERAL)
#define LOG_FATAL_F(filter) __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_FATAL, filter)
#define LOG_ERROR __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_ERROR, war::log::LA_GENERAL)
#define LOG_ERROR_F(filter) __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_ERROR, filter)
#define LOG_WARN __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_WARNING, war::log::LA_GENERAL)
#define LOG_WARN_F(filter) __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_WARNING, filter)
#define LOG_INFO __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_INFO, war::log::LA_GENERAL)
#define LOG_INFO_F(filter) __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_INFO, filter)
#define LOG_NOTICE __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_NOTICE, war::log::LA_GENERAL)
#define LOG_NOTICE_F(filter) __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_NOTICE, filter)
#define LOG_DEBUG __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_DEBUG, war::log::LA_GENERAL)
#define LOG_DEBUG_F(filter) __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_DEBUG, filter)
#define LOG_TRACE1 __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_TRACE1, war::log::LA_GENERAL)
#define LOG_TRACE1_F(filter) __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_TRACE1, filter)
#define LOG_TRACE2 __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_TRACE2, war::log::LA_GENERAL)
#define LOG_TRACE2_F(filter) __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_TRACE2, filter)
#define LOG_TRACE3 __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_TRACE3, war::log::LA_GENERAL)
#define LOG_TRACE3_F(filter) __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_TRACE3, filter)
#define LOG_TRACE4 __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_TRACE4, war::log::LA_GENERAL)
#define LOG_TRACE4_F(filter) __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_TRACE4, filter)

#define LOG_FATAL_FN __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_FATAL, war::log::LA_GENERAL) << "{" << WAR_FUNCTION_NAME << "}: "
#define LOG_FATAL_F_FN(filter) __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_FATAL, filter) << "{" << WAR_FUNCTION_NAME << "}: "
#define LOG_ERROR_FN __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_ERROR, war::log::LA_GENERAL) << "{" << WAR_FUNCTION_NAME << "}: "
#define LOG_ERROR_F_FN(filter) __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_ERROR, filter) << "{" << WAR_FUNCTION_NAME << "}: "
#define LOG_WARN_FN __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_WARNING, war::log::LA_GENERAL) << "{" << WAR_FUNCTION_NAME << "}: "
#define LOG_WARN_F_FN(filter) __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_WARNING, filter) << "{" << WAR_FUNCTION_NAME << "}: "
#define LOG_INFO_FN __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_INFO, war::log::LA_GENERAL) << "{" << WAR_FUNCTION_NAME << "}: "
#define LOG_INFO_F_FN(filter) __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_INFO, filter) << "{" << WAR_FUNCTION_NAME << "}: "
#define LOG_NOTICE_FN __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_NOTICE, war::log::LA_GENERAL) << "{" << WAR_FUNCTION_NAME << "}: "
#define LOG_NOTICE_F_FN(filter) __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_NOTICE, filter) << "{" << WAR_FUNCTION_NAME << "}: "
#define LOG_DEBUG_FN __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_DEBUG, war::log::LA_GENERAL) << "{" << WAR_FUNCTION_NAME << "}: "
#define LOG_DEBUG_F_FN(filter) __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_DEBUG, filter) << "{" << WAR_FUNCTION_NAME << "}: "
#define LOG_TRACE1_FN __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_TRACE1, war::log::LA_GENERAL) << "{" << WAR_FUNCTION_NAME << "}: "
#define LOG_TRACE1_F_FN(filter) __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_TRACE1, filter) << "{" << WAR_FUNCTION_NAME << "}: "
#define LOG_TRACE2_FN __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_TRACE2, war::log::LA_GENERAL) << "{" << WAR_FUNCTION_NAME << "}: "
#define LOG_TRACE2_F_FN(filter) __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_TRACE2, filter) << "{" << WAR_FUNCTION_NAME << "}: "
#define LOG_TRACE3_FN __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_TRACE3, war::log::LA_GENERAL) << "{" << WAR_FUNCTION_NAME << "}: "
#define LOG_TRACE3_F_FN(filter) __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_TRACE3, filter) << "{" << WAR_FUNCTION_NAME << "}: "
#define LOG_TRACE4_FN __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_TRACE4, war::log::LA_GENERAL) << "{" << WAR_FUNCTION_NAME << "}: "
#define LOG_TRACE4_F_FN(filter) __WAR_LOG_WITH_LEVEL_AND_FILTER(war::log::LL_TRACE4, filter) << "{" << WAR_FUNCTION_NAME << "}: "


namespace war {

/*! Logging library
 *
 * Currently, logging is done from the thread that performs the logging.
 * The library still performs well.
 *
 * Logging via a dedicated thread is planned for the future.
 *
 * \note Most of the frequently used methods are marked as noexcept.
 *      This means that the application will crash if an exception
 *      is thrown to or in one of these methods.
 *      This makes sense, because in most cases, you don't want the
 *      application to continue to run without the accountability that
 *      logging provides. If this turns out to be a problem, please
 *      file a bug-report to the project on Source Forge with an explanation
 *      of why you are getting exceptions, and a suggestion on how to
 *      handle it.
 */


namespace log {

struct Esc
{
    Esc(const boost::string_ref& v) : val_(v) {};
    Esc(const std::string& v) : val_(v) {};
    Esc(const char *v) : val_(v, v ? strlen(v) : 0) {};

    const boost::string_ref& GetVal() const {
        return val_;
    }

private:
    const boost::string_ref val_;
};

class Log;

/*! Log Level */
enum LogLevel {
    /// Fatal error. The application terminates
    LL_FATAL,
    /// Error
    LL_ERROR,
    /// Warning
    LL_WARNING,
    /// Information. Typically program version, internal modules being enabled etc.
    LL_INFO,
    /// Notice. Typically some action that is started or completed.
    LL_NOTICE,
    /// Debug messages. These provides verbose information regarding the processing
    LL_DEBUG,
    /// Trace message
    LL_TRACE1,
    /// Trace message
    LL_TRACE2,
    /// Trace message
    LL_TRACE3,
    /// Trace message
    LL_TRACE4
};

/*! Log regarding bitflag.

  Note that one log-message may regard several matters.
*/
enum LogAbout {
    /// General message
    LA_GENERAL  = 0x00000001,
    /// Security
    LA_SECURITY = 0x00000002,
    /// File transfer
    LA_TRANSFER = 0x00000004,
    /// Authentication
    LA_AUTH = 0x00000008,
    /// IO operations (file system)
    LA_IO = 0x00000010,
    /// Network operations (relevant for debug and trace-levels)
    LA_NETWORK = 0x00000020,
    /// Threads (relevant for debug and trace-levels)
    LA_THREADS = 0x00000040,
    /// Inter process commnication (relevant for debug and trace-levels)
    LA_IPC = 0x00000080,
    /// Statistics updates
    LA_STATS = 0x00000100,
    /// Function called and left
    LA_FUNCTION_CALL = 0x00000200
};

typedef unsigned int filter_t;

/// Convenience
enum LogAboutDefaults {
    /// Te default log filter
    LA_DEFAULT_ENABLE = LA_GENERAL | LA_SECURITY | LA_TRANSFER | LA_AUTH
                        | LA_IO | LA_NETWORK | LA_THREADS | LA_IPC | LA_STATS
                        | LA_FUNCTION_CALL
};

/*! The log event handler interface

    \note Any constructor or method may throw war::ExceptionBase
        derived exceptions
*/

class LogEventHandler
{
public:
    using ptr_t = std::shared_ptr<LogEventHandler>;
    struct Exception : public war::ExceptionBase {};

    LogEventHandler(const std::string& name, const LogLevel level,
                    const filter_t filter)
        : name_ (name), level_ (level), filter_ (filter) {}

    virtual ~LogEventHandler() {}

    LogEventHandler(const LogEventHandler&) = delete;
    LogEventHandler& operator = (const LogEventHandler&) = delete;

    struct SubmitInfo {
        const LogLevel level_;
        const filter_t filter_;
        const std::string buf_;
        const std::chrono::system_clock::time_point time_;
    };

    virtual void Submit (const SubmitInfo& info ) noexcept  = 0;

    const std::string& GetName() const noexcept {
        return name_;
    }

    const LogLevel GetLevel() const noexcept {
        return level_;
    }

    const filter_t GetFilter() const noexcept {
        return filter_;
    }

    static const char *GetLevelName(const LogLevel level) noexcept;

    static void WriteFilter(std::ostream& out, const filter_t filter,
                            const filter_t hide = LA_GENERAL) noexcept;

    void SetLevel(const LogLevel level) {
        level_ = level;
    }

protected:
    /*! This is the default implementation for formatting output to a log-device. */
    void WriteDefaulInfo(std::ostream& out, const SubmitInfo &si) const noexcept;
    void WriteLevel(std::ostream& out, const SubmitInfo &si) const noexcept;
    void WriteFilter(std::ostream& out, const SubmitInfo &si) const noexcept;
    void WriteTimestamp(std::ostream& out, const SubmitInfo &si) const noexcept;

    /*! Writes the message in the log-event

    This method will ignore '\r', escape non-whitespace control characters
    as %hh (where hh is a two-digit hex number), and handle newline-characters
    by adding a new line, prefixed with two spaces.
    */
    void WriteMessage(std::ostream& out, const SubmitInfo &si) const noexcept;

private:
    std::string name_;
    LogLevel level_;
    filter_t filter_;
    const std::locale locale_;
};

/*! Log event-handler that prints to the console */
class LogToStream : public LogEventHandler
{
public:
    LogToStream(std::ostream& stream = std::clog,
                const std::string& name = "console",
                const LogLevel level = LL_NOTICE,
                const filter_t filter = LA_DEFAULT_ENABLE)
        : LogEventHandler(name, level, filter), out_(stream) {}

    virtual void Submit ( const SubmitInfo& info ) noexcept {
        WriteDefaulInfo(out_, info);
    }

private:
    std::ostream& out_;
};

/*! Log event handler that prints to a file on the file-system */
class LogToFile : public LogEventHandler
{
public:
    typedef std::string path_t;

    /*! Constructs a file log handler.

        \exception Exception if the file cannot be opened for append.
    */
    LogToFile(const path_t& path,
              const bool truncateFileOnOpen = false,
              const std::string& name = "file",
              const LogLevel level = LL_NOTICE,
              const filter_t filter = LA_DEFAULT_ENABLE);

    virtual void Submit (const SubmitInfo& info) noexcept {
        WriteDefaulInfo(out_, info);
    }

    /*! Helper */
    static LogEventHandler::ptr_t Create(const path_t& path,
                                         const bool truncateFileOnOpen = false,
                                         const std::string& name = "file",
                                         const LogLevel level = LL_NOTICE,
                                         const filter_t filter = LA_DEFAULT_ENABLE);

private:
    const path_t path_;
    std::ofstream out_;
};

/*! The log-manager.

  There must be one and only one instace of this object in an application
  that use this log library
*/

class LogEngine
{
public:
    struct Exception : public war::ExceptionBase {};

    LogEngine();
    ~LogEngine();

    LogEngine& operator = (const LogEngine&) = delete;

    /*! Returns true if the log-level and filter will be logged to at least one event-handler */
    static bool IsRelevant (const LogLevel level, const filter_t filter) noexcept {
        return (level <= current_level_) && ((filter & current_filter_) != 0);
    }

    /*! Submit an event to the log event handlers
    This method is therad safe, and log messages are passed to the
    event-handlers in the order they are received.
    */
    static void  Submit ( Log& log ) noexcept {
        WAR_ASSERT (instance_);
        instance_->DoSubmit ( log );
    }

    /*! Add a new log event-handler.
    The log-level and filters are adjusted to
    fit the most detailed log-level for the combined
    event-handlers that are added. Each event-handler
    will only priont log-messages relevant according to it's own
    level and filter.
    */
    void AddHandler ( LogEventHandler::ptr_t handler );

    /*! Get the log-level from it's name.

        \exception Exception on invalid name.
    */
    static LogLevel GetLevelFromName(const std::string& name) noexcept;

    static LogEngine& GetInstance() {
        WAR_ASSERT (instance_);
        return *instance_;
    }

private:
    void DoSubmit ( Log& log ) noexcept;
    void UpdateLevelAndFilter();

    typedef std::vector<LogEventHandler::ptr_t> handlers_t;
    handlers_t handlers_;
    mutable std::mutex lock_;

    static LogEngine *instance_;
    static LogLevel current_level_;
    static filter_t current_filter_;
};

/*! Logging class

  This class is instatiated for one log-event. It will submit the log in it's destructor.
  Normally, we will use macross.
  */
class Log
{
public:
    Log (const LogLevel level, const filter_t filter) noexcept
        : level_ (level), filter_ (filter) {}

    Log& operator = (Log &&log) = delete;
    Log& operator = (const Log &log) = delete;

    ~Log() noexcept {
        LogEngine::Submit ( *this );
    }

    operator bool () const noexcept {
        return true;
    }

    std::ostringstream& Get() noexcept {
        return buf_;
    }

    LogLevel GetLevel() const noexcept {
        return level_;
    }

    filter_t GetFilter() const noexcept {
        return filter_;
    }

private:
    std::ostringstream buf_;
    LogLevel level_;
    filter_t filter_;
};


/*! Simple wrapper around errno */
struct Errno
{
    /*! Construct the object.

    Must be constructed after a system-error occurs, and before
    any new functions that may set or reset the error-code is called.
    */
    Errno(const int err = errno) : err_(err) {}

    /*! Get the error number */
    int GetErrno() const noexcept {
        return err_;
    }

    /*! Explain the error */
    std::string Explain() const;

    /*! Explain the error */
    std::ostream& Explain(std::ostream& out) const;

private:
    const int err_;
};

/*! Log the invocation of a funtiom

    Instantiate this class in the beginning of a function in order
    to log when the function is enetered and left.
    The log is by default at LOG_TRACE3 level.
*/
class LogFunctionCall
{
public:
    LogFunctionCall(const char *functionName,
                    const int line,
                    const char *file,
                    const LogLevel level = war::log::LL_TRACE3) noexcept
        : name_(functionName), file_(file), line_(line), level_(level)
    {
        try {
            __WAR_LOG_WITH_LEVEL_AND_FILTER(level_, LA_FUNCTION_CALL)
                << "Entering: " << name_ << ' ' << file_ << ' ' << line_;
        } catch(...) {}
    }

    ~LogFunctionCall() noexcept
    {
        try {
            __WAR_LOG_WITH_LEVEL_AND_FILTER(level_, LA_FUNCTION_CALL)
                << "Leaving: " << name_ << ' ' << file_ << ' ' << line_;
        } catch(...) {}
    }

private:
    const char *name_;
    const char *file_;
    const int line_;
    const LogLevel level_;
};

class Timer
{
public:
    Timer()
    {}

    std::ostream& Stream (std::ostream& o) const {

        const auto duration =
            (is_running_ ? std::chrono::steady_clock::now() : end_) - start_;

        const auto mi = std::chrono::duration_cast<std::chrono::minutes>(duration).count();
        if (mi > 5) {
            return o << mi << " minutes";
        }

        const auto sec = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
        if (sec > 5) {
            return o << sec << " seconds";
        }

        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        if (ms > 5) {
            return o << ms << " milliseconds";
        }

        return o
            <<  std::chrono::duration_cast<std::chrono::microseconds>(duration).count()
            << " microseconds";
    }

    void Stop() {
        end_ = std::chrono::steady_clock::now();
        is_running_ = false;
    }

private:
    std::chrono::steady_clock::time_point end_;
    bool is_running_ {true};
    const std::chrono::steady_clock::time_point start_
        = std::chrono::steady_clock::now();
};

#if defined(_DEBUG) || defined(DEBUG)
#  define WAR_LOG_FUNCTION war::log::LogFunctionCall _war_log_this_function(WAR_FUNCTION_NAME, __LINE__, __FILE__);
#else
#  define WAR_LOG_FUNCTION
#endif

}
} // namespace

std::ostream& operator << (std::ostream& out, const war::log::LogLevel& level);
std::ostream& operator << (std::ostream& out, const war::log::Errno& err);
std::ostream& operator << (std::ostream& out, const std::exception& ex);
std::ostream& operator << (std::ostream& out, const war::ExceptionBase& ex);
std::ostream& operator << (std::ostream& out, const boost::exception& ex);
std::ostream& operator << (std::ostream& out, const boost::system::error_code& err);
std::ostream& operator << (std::ostream& out, const war::log::Esc& esc);
std::ostream& operator << (std::ostream& out, const war::log::Timer& timer);
