#pragma once
#ifndef WAR_PIPELINE_H
#define WAR_PIPELINE_H

#include <war_basics.h>
#include <thread>
#include <functional>
#include <memory>
#include <future>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <cstdint>
#include <iostream>
#include <atomic>

#include <war_asio.h>

namespace war {


/*! Single-thread task sequencer, timer and asio io_service instance

    This class is used as a sequential task sequencer, timer
    and IO object for boost::asio. By assigning a connection
    to an instance of this class, we can basically pretend that
    the objects that belong to the connection are single-threaded,
    as long as we only access its properties trough the task-sequencer.
    This simplifies multithreading quite a bit, and reduce the need for
    expensive mutexes all around.
*/
class Pipeline {
public:
    /*! Thrown from post/dispatch functions if the capacity of the queue is used up */
    struct ExceptionCapacityExceeded : public ExceptionBase {};

    /*! Construct a Pipeline
     *
     * \param name Name of the pipeline (primarily for logging).
     * \param id Numerical, unique ID for the pipeline
     * \param capacity Max number of queued tasks
     * \param pinTo CPU-ID to pin the therad to. If -1, the OS is free
     *      to schedule the thread on any CPU.
     */
    Pipeline(const std::string &name = "Pipeline",
             int id = -1,
             const std::size_t capacity = 1024,
             int pinTo = -1);
    ~Pipeline();

    Pipeline& operator = (const Pipeline&) = delete;
    Pipeline& operator = (const Pipeline&&) = delete;

    /*! Run or post a task to the end of the task sequencer queue.

        The task will be executed when the task sequenceer thread
        is ready. That means that it can be executed bbefore the
        method returns, if the calling thread is not the same as
        the task sequenser. However, if called from the same thread,
        the task will always be queued for later execution. This
        makes it easy to prevent unwanted recursing, by posponing the
        task until we have completed the task at hand.
    */
    void Post(const task_t &task);
    void Post(task_t &&task);

    /*! Run if possible, or post a task to the end of the task sequencer queue.

        This works as Post, except that it will run the task immediately if
        it's called from the task sequencers own thread, and then return when the task
        has completed execution.

        In other words; in the same thread it works like a function-call, from other threads,
        it use the task-sequencer queue.
    */
    void Dispatch(const task_t &task);
    void Dispatch(task_t &&task);

    /*! Post a task on the sequencer, and delay the execution

        Works like Post, exept that it will wait for at least
        milliSeconds milli-seconds (1/1000 second) befort the
        task is executed. The method returns immediately.
    */
    void PostWithTimer(const task_t &task, const std::uint32_t milliSeconds);
    void PostWithTimer(task_t &&task, const std::uint32_t milliSeconds);

    /*! Returns the asio io-service */
    io_service_t &GetIoService() noexcept { return *io_service_.get(); }

    /*! Closes the task-sequenser.

        All the tasks in the queue will be dismissed, including timers.

        Close() may pass the actual work to another thread and return immediately.
    */
    void Close();

    /*! Check if the queue is closed */
    bool IsClosed() const noexcept { return closed_; }

    /*! Check if we are shutting down the pipeline */
    bool IsClosing() const noexcept { return closing_; }

    /*! Returnd true if called by the worker-thread for the pipeline. */
    bool IsPipelineThread() const noexcept { return thread_->get_id() == std::this_thread::get_id(); }

    /*! Return only after the pipeline has shut down and the worker-thread is done
        May sleep indefinately if noone Close's the pipeline.
    */
    void WaitUntilClosed() const;

    /*! Returns the capacity of the internal task queue. */
    size_t GetCapacity() const noexcept { return capacity_; }

    /*! Returns the number of tasks currently queued for immediate processing.
        Timer-tasks are not counted.
    */
    size_t GetCount() const noexcept { return count_; }

    int GetId() const noexcept { return id_; }

    const std::string& GetName() const noexcept { return name_; }

private:
    using my_sync_t = std::promise<void>;
    void Run(my_sync_t& sync, int pinTo);
    void ExecTask_(const task_t& task, bool counting);
    void OnTimer_(
        const timer_t& timer,
        const task_t& task,
        const boost::system::error_code& ec);
    void AddingTask();

    std::unique_ptr<boost::asio::io_service> io_service_;
    std::unique_ptr<boost::asio::io_service::work> work_;
    std::unique_ptr<std::thread> thread_;
    const std::string name_;
    std::atomic<bool> closed_;
    const std::size_t capacity_;
    std::atomic<size_t> count_;
    mutable std::mutex waiter_;
    std::mutex close_mutex_;
    std::atomic<bool> closing_;
    std::int64_t tasks_run_ = 0;
    int id_; // Thread number in threadpool, starting at 0. -1 if not in threadpool;
};

} // namespace

std::ostream& operator << (std::ostream& o, const war::task_t& task);
std::ostream& operator << (std::ostream& o, const war::Pipeline& pipeline);

#endif //WAR_PIPELINE_H
