#pragma once
#ifndef WAR_THREADPOOL_H
#define WAR_THREADPOOL_H

#include <atomic>

#include <warlib/WarPipeline.h>

namespace war {

    class Threadpool
    {
    public:
        using pinning_t = std::vector<int>;
        /*! Construct a threadpool

            \param numThreads Number of threads to start. This
                is a fixed number that cannot be changed later.
                If this value is 0, a reasonable value will be
                selected, based on the available hardware-
                concurrency.
                The current alogrithm is: max(2, cores -1)

            \param maxPerThreadQueueCapacity Maximum number of tasks that can
                be queued for each thread in the thread-pool.

            \param pinning Definition of CPU pinning for threads. The array is
                read from start to end, and if the array (a[n]) is large enough
                and contain another value than -1, the thread-pool will try to
                pin the thread (n) to a CPU-ID equal to the value. On Linux,
                CPU's are named from 0 - n, incremented by each core (or, if
                hyper-threading is enabled, incremented by one for each HT
                thread for each core). Pinning can in theory give a higher
                performance, as the cache-lines for the thread never has to be
                migrated to another CPU. The real performance-gain (or drop)
                will depend on the actual system and it's load.
        */
        Threadpool(unsigned numThreads = 0,
                   unsigned maxPerThreadQueueCapacity = 1024,
                   pinning_t *pinning = nullptr);
        ~Threadpool();

        void Post(const task_t &task);
        void Post(task_t &&task);

        void PostWithTimer(const task_t &task, const std::uint32_t milliSeconds);
        void PostWithTimer(task_t &&task, const std::uint32_t milliSeconds);

        /*! Get a pipeline

            This method currently uses a round-robin approach for balancing.
        */
        Pipeline &GetAnyPipeline();
        void Close();
        void WaitUntilClosed();
        std::size_t GetNumThreads() const noexcept { return capacity_; }
        Pipeline& GetPipeline(std::size_t id) { return *pool_.at(id); }

    private:
        void JoinAll();

        using pool_t = std::vector<std::unique_ptr<Pipeline>>;

        pool_t pool_;
        std::atomic_uint round_robin_next_thread_;
        const unsigned capacity_;
        std::mutex close_mutex_;
        std::atomic_bool closed_;
        std::mutex finish_mutex_;
        std::atomic_bool finished_;
    };




} //namespace

#endif // WAR_THREADPOOL_H
