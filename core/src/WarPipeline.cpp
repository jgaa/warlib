

#include <string>
#include <boost/asio.hpp>

#if BOOST_VERSION < 107000
#  include <boost/context/all.hpp>
#endif

#include <boost/concept_check.hpp>
#include <warlib/WarPipeline.h>
#include <warlib/WarLog.h>
#include <warlib/debug_helper.h>

#ifndef WIN32
#   include <pthread.h>
#endif

using namespace std;
using namespace war;
using namespace std::string_literals;

std::ostream& operator << (std::ostream& o, const war::task_t& task)
{
    return o << log::Esc(task.second);
}

std::ostream& operator << (std::ostream& o, const war::Pipeline& pipeline)
{
    return o << "{ pipeline: " << pipeline.GetName()
        << " #" << pipeline.GetId() << '}';
}

war::Pipeline::Pipeline(const string &name,
                        int id,
                        const std::size_t capacity,
                        int pinTo)
: io_context_ { new io_context_t }, name_ (name)
, closed_ {false}, capacity_ { capacity }, count_ {0}
, closing_ {false}, id_ {id}
{
    WAR_LOG_FUNCTION;
    LOG_TRACE3_F_FN(log::LA_THREADS) << log::Esc(name_);

    my_sync_t sync;
    thread_.reset(new thread(&war::Pipeline::Run, this, ref(sync), pinTo));

    // Wait for the thread to run so that we can re-throw any early exceptions
    sync.get_future().get();
}

war::Pipeline::~Pipeline()
{
    WAR_LOG_FUNCTION;
    LOG_TRACE3_F_FN(log::LA_THREADS) << log::Esc(name_);
    if (thread_ && thread_->joinable()) {
        LOG_TRACE3_F_FN(log::LA_THREADS) << log::Esc(name_) << "Joining";
        thread_->join();
        LOG_TRACE3_F_FN(log::LA_THREADS) << log::Esc(name_) << "Joined";
    }
}

void war::Pipeline::Run(my_sync_t & sync, const int pinTo)
{
    WAR_LOG_FUNCTION;

    unique_lock<mutex> waiter_lock(waiter_);

#ifndef WIN32
    if (pinTo != -1) {
        cpu_set_t cs{};
        CPU_SET(pinTo, &cs);
        if (pthread_setaffinity_np(pthread_self(), 1, &cs) == 0) {
            LOG_TRACE1_FN << "This thread is pinned to CPU# " << pinTo;
        } else {
            const log::Errno err;
            LOG_WARN_FN << "Failed to pin thread to CPU# " << pinTo
                << ": " << err;
        }
    }
#endif

    try {
        debug::SetThreadName(name_);
        work_guard_ = make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>(boost::asio::make_work_guard(*io_context_));
    }
    catch (...) {
        LOG_ERROR_FN << "Caught exception! Aborting this operation!";
        sync.set_exception(current_exception());
        return;
    }
    // Makes sure that we got here before the original thread can leave the constructor.
    sync.set_value();

    LOG_DEBUG_F_FN(log::LA_THREADS) << "Starting Pipeline thread loop "
        << log::Esc(name_);
    try {
        io_context_->run();
    }
    WAR_CATCH_ALL_E;

    LOG_DEBUG_F_FN(log::LA_THREADS) << "Ending Pipeline thread loop "
        << log::Esc(name_)
        << ". The total number of tasks I ran was " << tasks_run_ << ".";
}

void war::Pipeline::Post(task_t &&task)
{
    WAR_LOG_FUNCTION;

    if (closing_) {

        LOG_WARN_FN << "The pipeline " << log::Esc(name_)
            << " is closing. Task dismissed: " << task;
        return;
    }

    LOG_TRACE3_F_FN(log::LA_THREADS) << "Posting [move] task on Pipeline "
        << task;

    AddingTask();
    boost::asio::post(*io_context_, bind(&war::Pipeline::ExecTask_, this, task, true, true));
}

void war::Pipeline::Post(const task_t &task)
{
    WAR_LOG_FUNCTION;
    Post(task_t(task));
}

void war::Pipeline::PostSynchronously(const task_t& task) {

    std::promise<void> promise;
    auto future = promise.get_future();

    Dispatch({[this, task, &promise]() mutable {
        try {
            ExecTask_(task, false, false);
            promise.set_value();
        } catch(...) {
            promise.set_exception(std::current_exception());
        }
    }, "Post Synchronously"});

    future.get();
}


void war::Pipeline::Dispatch(const task_t &task)
{
    WAR_LOG_FUNCTION;
    Dispatch(task_t(task));
}

void war::Pipeline::Dispatch(task_t &&task)
{
    WAR_LOG_FUNCTION;

    if (closing_) {

        LOG_WARN_FN << "The pipeline " << log::Esc(name_)
            << " is closing. Task dismissed: " << task;
        return;
    }

    LOG_TRACE3_F_FN(log::LA_THREADS) << "Dispatching [move] task on Pipeline " <<task;

    if (IsPipelineThread()) {
        ExecTask_(std::move(task), false);
    }
    else {
        Post(std::move(task));
    }
}

void war::Pipeline::PostWithTimer(const task_t &task,
                                  const uint32_t milliSeconds)
{
    WAR_LOG_FUNCTION;
    PostWithTimer(task_t(task), milliSeconds);
}

void war::Pipeline::PostWithTimer(task_t &&task,
                                  const uint32_t milliSeconds)
{
    WAR_LOG_FUNCTION;

    if (closing_) {
        LOG_WARN_FN << "The pipeline " << log::Esc(name_)
            << " is closing. Task dismissed: " << task;
        return;
    }

    LOG_TRACE3_F_FN(log::LA_THREADS) << "Posting task " << task
        << " on Pipeline " << log::Esc(name_)
        << " for execution in " << milliSeconds << " milliseconds.";

    timer_t timer(new boost::asio::deadline_timer(*io_context_));
    timer->expires_from_now(boost::posix_time::milliseconds(milliSeconds));
    timer->async_wait(bind(&war::Pipeline::OnTimer_, this, timer,
                           std::move(task), placeholders::_1));
}

void war::Pipeline::Close()
{
    WAR_LOG_FUNCTION;

    if (closing_)
        return;

    if (io_context_) {
        LOG_TRACE1_F_FN(log::LA_THREADS) << "Posting Close on Pipeline "
            << log::Esc(name_);
        boost::asio::post(*io_context_, [this] {
            if (!closed_) {
                LOG_DEBUG << "Shutting down Pipeline " << log::Esc(name_);
                closed_ = true;
                work_guard_.reset();
                io_context_->stop();
                LOG_DEBUG << "Finisheed shutting down Pipeline "
                    << log::Esc(name_);
            }
        });
        closing_ = true;
    }
}

void war::Pipeline::OnTimer_(const timer_t& /*timer*/, const task_t &task, const boost::system::error_code& ec)
{
    WAR_LOG_FUNCTION;
    if (!ec) {
        ExecTask_(task, false);
    }
    else {
        LOG_DEBUG_FN << "Task " << task << " failed with error: " << ec;
    }
}

void war::Pipeline::AddingTask()
{
    if (++count_ > capacity_) {
        --count_;
        WAR_THROW_T(ExceptionCapacityExceeded,
                    "Out of capicity in Pipeline \""s + name_ + "\""s);
    }
}

void war::Pipeline::ExecTask_(const task_t& task, bool counting, bool autoCatch)
{
    WAR_LOG_FUNCTION;

    if (counting) {
        --count_;
    }
    if (closing_) {
        LOG_DEBUG_FN << "Dismissing task " << task
        << ". Pipeline " << log::Esc(name_)
        << " is closed.";
        return;
    }
    LOG_TRACE3_F_FN(log::LA_THREADS) << "Executing task " << task;

    if (autoCatch) {
        try {
            task.first();
            ++tasks_run_;
        }
        WAR_CATCH_ALL_E;
    } else {
        task.first();
        ++tasks_run_;
    }
    LOG_TRACE3_F_FN(log::LA_THREADS) << "Finished executing task " << task;
}

void  war::Pipeline::WaitUntilClosed() const
{
    LOG_TRACE1_FN << "Waiting for waiter_" << " on Pipeline " << log::Esc(name_);
    lock_guard<std::mutex> lock { waiter_ };
    LOG_TRACE1_FN << "Done waiting for waiter_" << " on Pipeline " << log::Esc(name_);
}
