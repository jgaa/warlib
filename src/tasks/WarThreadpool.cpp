#include "war_tasks.h" // PCH file
#include "war_impl.h"
#include <algorithm>
#include <thread>
#include "tasks/WarThreadpool.h"
#include "log/WarLog.h"

using namespace std;
using namespace war;

war::Threadpool::Threadpool(const unsigned numThreads,
                            unsigned maxPerThreadQueueCapacity,
                            pinning_t *pinning)
: capacity_(numThreads > 0 ? numThreads
                           : max<unsigned>(2, thread::hardware_concurrency() - 1))
{
    closed_ = false;
    finished_ = false;
    round_robin_next_thread_ = 0;

    LOG_NOTICE << "Starting threadpool with " << capacity_ << " threads.";

    pool_.reserve(capacity_);
    for (unsigned i = 0; i < capacity_; ++i) {
        auto name = string("Pool-worker_") + to_string(i);
        pool_.emplace_back(std::unique_ptr<Pipeline>
                           (new Pipeline {name, static_cast<int>(i),
                               maxPerThreadQueueCapacity,
                               ((pinning && pinning->size() > i) ? pinning->at(i) : -1)
                        }));
    }
}

war::Threadpool::~Threadpool()
{
    WAR_LOG_FUNCTION;
    Close();
    JoinAll();
}

void war::Threadpool::Post(const task_t &task)
{
    WAR_LOG_FUNCTION;
    GetAnyPipeline().Post(task);
}

void war::Threadpool::Post(task_t &&task)
{
    WAR_LOG_FUNCTION;
    GetAnyPipeline().Post(task);
}

void war::Threadpool::PostWithTimer(const task_t &task, const std::uint32_t milliSeconds)
{
    WAR_LOG_FUNCTION;
    GetAnyPipeline().PostWithTimer(task, milliSeconds);
}

void war::Threadpool::PostWithTimer(task_t &&task, const std::uint32_t milliSeconds)
{
    WAR_LOG_FUNCTION;
    GetAnyPipeline().PostWithTimer(task, milliSeconds);
}

Pipeline& war::Threadpool::GetAnyPipeline()
{
    WAR_LOG_FUNCTION;
    return *pool_[++round_robin_next_thread_ % capacity_].get();
}

void war::Threadpool::Close()
{
    WAR_LOG_FUNCTION;
    if (closed_)
        return;

    for(auto & pipeline: pool_) {
        pipeline->Close();
    }

    closed_ = true;
}

void war::Threadpool::WaitUntilClosed()
{
    WAR_LOG_FUNCTION;
    JoinAll();
}

void war::Threadpool::JoinAll()
{
    WAR_LOG_FUNCTION;
    lock_guard<mutex> lock(close_mutex_);
    if (finished_)
        return;

    if (!closed_) {
        LOG_WARN_FN << "Joining all before calling Close(). "
                    << "This may lead to an infinite wait if noone else Close()";
    }

    for (auto & pipeline : pool_) {
        pipeline->WaitUntilClosed();
    }

    finished_ = true;
}
