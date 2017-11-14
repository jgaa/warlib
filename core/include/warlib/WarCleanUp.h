#pragma once

#include <thread>
#include <warlib/WarLog.h>

namespace war {


/*! RIAA cleanup in the destructor.
 *
 * I observed that some coroutines are unwinded using the main-
 * thread after the worker-thread is terminated. This caused a
 * segfault during shutdown under load as data was partially
 * deleted (the socket_ was freed' and in some cases also the
 * thread specific connection manager.
 *
 * We therefore must check that we are executing under the
 * correct thread (the therad that created the object).
 *
 * (I don't know if this is a bug or "feature" of asio).
 *
 * For convenience, use the factory below to construct an
 * instance of the correct lambda type.
 */
template <typename T>
class WarCleanUp
{
public:
    using fn_t = T;

    WarCleanUp(WarCleanUp&&) = default;

    WarCleanUp(fn_t&& fn, const char *name)
    : fn_{std::move(fn)}, name_{name}
    {
    }

    ~WarCleanUp() {
        if (thread_id_ != std::this_thread::get_id()) {
            LOG_DEBUG << name_ << ": Executing under the wrong thread! Will do nothing.";
            return;
        }

        try {
            LOG_TRACE1 << name_ << ": Calling the cleanup handler";
            fn_();
        } WAR_CATCH_ERROR;
    }

private:
    const std::thread::id thread_id_;
    fn_t fn_;
    const char *name_;
};

template <typename T>
WarCleanUp<T> WarCleanUpFactory(T&& fn, const char *name) {
    return WarCleanUp<T>(std::move(fn), name);
}

} // namespace


