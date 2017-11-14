
#define BOOST_TEST_MODULE WarlibTests
#include "war_tests.h"
#include <chrono>
#include <warlib/WarPipeline.h>
#include <warlib/basics.h>
#include <warlib/WarLog.h>


using namespace std;
using namespace war;
using namespace chrono_literals;


const lest::test specification[] = {

STARTCASE(Test_Coroutine)
{
    log::LogEngine log;
    log.AddHandler(log::LogToFile::Create("test_post_coroutine.log", true, "file",
                                          log::LL_TRACE4, log::LA_DEFAULT_ENABLE | log::LA_THREADS) );

    unique_ptr<Pipeline> pipeline { new Pipeline("UnitTest_CR") };

    boost::asio::spawn(pipeline->GetIoService(),
                       [&](boost::asio::yield_context yield) {

        LOG_NOTICE << "Posting PostWithTimer";
        pipeline->PostWithTimer({[&](){

            LOG_NOTICE << "Got there....";

        }, "PostWithTimer"}, 1000, yield);

        LOG_NOTICE << "Posting async wait.";
        pipeline->Post({[&](){

            LOG_NOTICE << "Going to sleep.";
            std::this_thread::sleep_for(1s);

            pipeline->Close();
        }, "Post"}, yield);

        LOG_NOTICE << "Posting async wait.";
    });
    pipeline->WaitUntilClosed();
} ENDCASE

STARTCASE(Test_Pipeline)
{

    log::LogEngine log;
    log.AddHandler(log::LogToFile::Create("test_pipeline.log", true, "file",
                                          log::LL_TRACE4, log::LA_DEFAULT_ENABLE | log::LA_THREADS) );

    const size_t capacity = 16;
    unique_ptr<Pipeline> pipeline { new Pipeline("UnitTest_MT", -1, capacity) };

    // Test that a callback is called at all
    {
        promise<bool> result;
        pipeline->Post({[&] {
            try {
                EXPECT(pipeline->IsPipelineThread());
                EXPECT(pipeline->GetCount() == 0);
                result.set_value(true);
            }
            catch (...) {
                result.set_exception(current_exception());
            }
        }, "Testing Post()"});

        // Post
        EXPECT(result.get_future().get());
    }

    // Test that a callback is called at all
    {
        promise<bool> result;
        pipeline->Dispatch({[&] {
            try {
                EXPECT(pipeline->IsPipelineThread());
                EXPECT(pipeline->GetCount() == 0);
                result.set_value(true);
            }
            catch (...) {
                result.set_exception(current_exception());
            }
        }, "Testing Dispatch()"});

        // Dispatch()
        EXPECT(result.get_future().get());
    }

    // Test that the timer works
    {
        promise<bool> result;
        const unsigned int sleep_period = 500; // milliseconds

        const auto start = chrono::high_resolution_clock::now();
        pipeline->PostWithTimer({[&result] {
            result.set_value(true);
        }, "Testing PostWithTimer()"}, sleep_period);

        const bool success = result.get_future().get();
        const auto end = chrono::high_resolution_clock::now();
        const auto ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();

        //PostWithTimer
        EXPECT(success);
        EXPECT(ms >= sleep_period);
    }

    // Test that tasks gets queued
    {
        EXPECT(pipeline->GetCount() == 0);

        {
            mutex barrier;
            unique_lock<mutex> lock(barrier);

            const task_t my_task {
                [] {
                    LOG_DEBUG << "I'm here!";
                }, "my_task"
            };

            promise<void> sync;
            pipeline->Post({[&] {
                sync.set_value();
                unique_lock<mutex> llock(barrier);
            }, "Lock the sequencer"});

            sync.get_future().get();
            EXPECT(pipeline->GetCount() == 0);

            // Queue max number of tasks == capacity
            for (size_t i = 0; i < pipeline->GetCapacity() - 2; ++i) {
                pipeline->Post(my_task);
            }
            promise<void> done_sync;
            pipeline->Post({[&] {
                LOG_DEBUG << "Last Task.";
                done_sync.set_value();
            }, "last task"});

            lock.unlock(); // Let the taks complete

            // Wait for the last task to complete
            done_sync.get_future().get();

        }
        EXPECT(pipeline->GetCount() == 0);
    }

    // Test capacity
    {
        EXPECT(pipeline->GetCount() == 0);

        {
            mutex barrier;
            unique_lock<mutex> lock(barrier);

            const task_t my_task {
                [] {
                    LOG_DEBUG << "I'm here!";
                }, "my_task"
            };

            promise<void> sync;
            pipeline->Post({[&] {
                sync.set_value();
                unique_lock<mutex> llock(barrier);
            }, "Lock the sequencer"});

            sync.get_future().get();
            EXPECT(pipeline->GetCount() == 0);

            // Queue max number of tasks == capacity
            for (size_t i = 0; i < pipeline->GetCapacity() -1; ++i) {
                pipeline->Post(my_task);
            }
            promise<void> done_sync;
            pipeline->Post({[&] {
                LOG_DEBUG << "I'm here! Last Task before capacity is exeeded.";
                done_sync.set_value();
            }, "last task"});

            EXPECT_THROWS_AS(pipeline->Post(my_task), Pipeline::ExceptionCapacityExceeded);

            lock.unlock(); // Let the taks complete

            // Wait for the last task to complete
            done_sync.get_future().get();

        }
        EXPECT(pipeline->GetCount() == 0);
    }

    // Test dispatch
    // We test this by recursing more levels than there is capacity to post tasks.
    // If we recurse directly, as we are supposed to do in the worker-thread, we
    // will not reach the capacity limit.
    {
        atomic<size_t> levels {pipeline->GetCapacity() + 2};
        const size_t max_levels = levels;
        mutex barrier;
        unique_lock<mutex> lock(barrier);
        promise<void> done_sync;

        const task_t recurse {
            [&] {
                const bool virgin = (max_levels == levels);
                if (--levels) {
                    pipeline->Dispatch(recurse);
                }
                unique_lock<mutex> llock(barrier);
                if (virgin)
                    done_sync.set_value();
            }, "recurse"
        };

        pipeline->Post(recurse);
        while (levels) {
            this_thread::sleep_for(chrono::milliseconds(30));
        }
        EXPECT(pipeline->GetCount() == 0);
        lock.unlock();

        done_sync.get_future().get(); // Wait for the tasks for finish
    }

    // IsPipelineThread() is broken
    EXPECT_NOT(pipeline->IsPipelineThread());
    // IsClosed() returned true before Close()
    EXPECT_NOT(pipeline->IsClosed());
    pipeline->Close();
    pipeline->WaitUntilClosed();
    EXPECT(pipeline->GetCount() == 0);
    // IsClosed() returned false after Close()
    EXPECT(pipeline->IsClosed());
    pipeline.reset();
} ENDCASE
}; //lest


int main( int argc, char * argv[] )
{
    return lest::run( specification, argc, argv );
}
