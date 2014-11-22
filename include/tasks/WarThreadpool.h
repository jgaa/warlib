#pragma once
#ifndef WAR_THREADPOOL_H
#define WAR_THREADPOOL_H

#include <atomic>

#include <tasks/WarPipeline.h>

namespace war {

    class Threadpool
    {
    public:
        /*! Construct a threadpool

            \param numThreads Number of threads to start. This
                is a fixed number that cannot be changed later.
                If this value is 0, a reasonable value will be
                selected, based on the available hardware-
                concurrency.
                The current alogrithm is: max(2, cores -1)
        */
        Threadpool(unsigned numThreads = 0, unsigned maxPerThreadQueueCapacity = 1024);
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
        std::atomic_size_t round_robin_next_thread_;
        const unsigned capacity_;
        std::mutex close_mutex_;
        std::atomic_bool closed_;
        std::mutex finish_mutex_;
        std::atomic_bool finished_;
    };




} //namespace

#endif // WAR_THREADPOOL_H
