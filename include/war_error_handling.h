#pragma once

#include <string>
#include <exception>
#include <boost/exception/all.hpp>
#include <cassert>
#include <functional>

/*! Assert in debug-builds, throws in release build

    TODO: Change so that we jump into the debugger when we are debugging,
    else, throw.
*/
#ifdef _DEBUG
#    define WAR_ASSERT_OR_THROW(expr, expl) assert(expr)
#    define WAR_ASSERT(expr) assert(expr)
#else
#    define WAR_ASSERT_OR_THROW(expr, expl) {if (!expr) WAR_THROW(expl) ; }
#    define WAR_ASSERT(expr)
#endif

#if _MSC_VER
#   define WAR_IS_DEBUGGING() ::IsDebuggerPresent()
#else
#   define WAR_IS_DEBUGGING() false
#endif

#ifdef _DEBUG
/*! If the expression returns false: In debugger, assert(), outside debugger, throw */
#   define WAR_ASSERT_OR_THROW_T(expr, exception_type, expl) {\
    if (!expr) { \
        if (WAR_IS_DEBUGGING()) { assert(expr); } \
        WAR_THROW_T(exception_type, expl); \
    }}
#else
#    define WAR_ASSERT_OR_THROW_T(expr, exception_type, expl)  {if (!expr) WAR_THROW_T(exception_type, expl) ; }
#endif


namespace war {

typedef boost::error_info<struct tag_expl, std::string> exception_expl;

/*! Base class for exceptions thrown by objects in the war namespace.

These exceptions is based on boost::exception handling
*/
struct ExceptionBase: virtual std::exception, virtual boost::exception { };

/*! Object already exist */
struct ExceptionAlreadyExist : public ExceptionBase {};

/*! Object not found */
struct ExceptionNotFound : public ExceptionBase {};

/*! Iterator or value is out of range */
struct ExceptionOutOfRange : public ExceptionBase {};

/*! The weak pointer has expired */
struct ExceptionWeakPointerHasExpired : public ExceptionBase {};

/*! Failed top open file */
struct ExceptionFileOpen : public ExceptionBase {};

/*! Failed top open file */
struct ExceptionAccessDenied : public ExceptionBase {};

/*! Syntax / parse error in path */
struct ExceptionBadPath : public ExceptionBase {};

/*! Securitu measure*/
struct ExceptionSecurityCheck : public ExceptionBase {};

/*! Some member object is not present */
struct ExceptionMissingInternalObject : public ExceptionBase {};

/*! Some member object is not present */
struct ExceptionNotImplemented : public ExceptionBase {};

struct ExceptionBadCast : public ExceptionBase {};

struct ExceptionParseError : public ExceptionBase {};

struct ExceptionFailedToConnect : public ExceptionBase {};

struct ExceptionIoError : public ExceptionBase {};

} // namespace


#define WAR_FUNCTION_NAME __FUNCTION__

#define WAR_EXCEPTION_TYPE(exception_type, expl) { exception_type _war_exception_for_throw; _war_exception_for_throw << war::exception_expl(expl)

#define WAR_EXCEPTION(expl) WAR_EXCEPTION_TYPE(Exception, expl)

#define WAR_EXCEPTION_THROW BOOST_THROW_EXCEPTION(_war_exception_for_throw); }

#define WAR_THROW(expl) WAR_EXCEPTION(expl); WAR_EXCEPTION_THROW

#define WAR_THROW_T(exception_type, expl) WAR_EXCEPTION_TYPE(exception_type, expl); WAR_EXCEPTION_THROW

#define WAR_THROW_WEAK_PTR_EXPIRED WAR_THROW_T(ExceptionWeakPointerHasExpired, "The weak pointer has expired");

#define WAR_THROW_NOT_IMPLEMENTED WAR_THROW_T(ExceptionNotImplemented, "Not Implemented")

#define WAR_CATCH_NORMAL \
catch(const war::ExceptionBase& ex) { \
    LOG_WARN_FN << "Caught exception [" << typeid(ex).name() << "]: " << ex; \
} catch(const boost::exception& ex) { \
    LOG_WARN_FN << "Caught boost exception [" << typeid(ex).name() << "]: " << ex; \
} catch(const std::exception& ex) { \
    LOG_WARN_FN << "Caught standad exception [" << typeid(ex).name() << "]: " << ex; \
}

#define WAR_CATCH_ERROR \
catch(const war::ExceptionBase& ex) { \
    LOG_ERROR_FN << "Caught exception [" << typeid(ex).name() << "]: " << ex; \
} catch(const boost::exception& ex) { \
    LOG_ERROR_FN << "Caught boost exception [" << typeid(ex).name() << "]: " << ex; \
} catch(const std::exception& ex) { \
    LOG_ERROR_FN << "Caught standad exception [" << typeid(ex).name() << "]: " << ex; \
}


#define WAR_CATCH_ALL_E \
catch(const war::ExceptionBase& ex) { \
    LOG_ERROR_FN << "Caught exception [" << typeid(ex).name() << "]: " << ex; \
} catch(const boost::exception& ex) { \
    LOG_ERROR_FN << "Caught boost exception [" << typeid(ex).name() << "]: " << ex; \
} catch(const std::exception& ex) { \
    LOG_ERROR_FN << "Caught standad exception [" << typeid(ex).name() << "]: " << ex; \
} catch(...) { \
    LOG_ERROR_FN << "Caught UNKNOWN exception! [" << typeid(std::current_exception()).name() << "]"; \
}

#define WAR_CATCH_ALL_EF(func) \
catch(const war::ExceptionBase& ex) { \
    LOG_ERROR_FN << "Caught exception [" << typeid(ex).name() << "]: " << ex; \
    func;\
} catch(const boost::exception& ex) { \
    LOG_ERROR_FN << "Caught boost exception [" << typeid(ex).name() << "]: " << ex; \
    func;\
} catch(const std::exception& ex) { \
    LOG_ERROR_FN << "Caught standad exception [" << typeid(ex).name() << "]:  " << ex; \
    func;\
} catch(...) { \
    LOG_ERROR_FN << "Caught UNKNOWN exception! [" << typeid(std::current_exception()).name() << "]"; \
    func;\
}

#ifdef DEBUG
#   define WAR_POINTER_ASSERT(ptr, classname) \
        WAR_ASSERT(ptr && (dynamic_cast<const classname *>(ptr) != nullptr))
#else
#   define WAR_POINTER_ASSERT(ptr, classname)
#endif

