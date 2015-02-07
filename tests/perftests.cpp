

#include <atomic>
#include <cstdint>
#include <chrono>

#include <tasks/WarThreadpool.h>
#include <log/WarLog.h>

#include <boost/program_options.hpp>

using namespace std;
using namespace war;


class Test
{
public:
    Test(const std::string& name) : name_(name) {}
    virtual ~Test() {}

    void RunTests()
    {
        LOG_NOTICE << "Starting test " << GetName();
        const auto start = std::chrono::steady_clock::now();
        try {
            DoRunTests();
        } WAR_CATCH_ALL_E;
        const auto finished = std::chrono::steady_clock::now();
        const auto extime = finished - start;
        LOG_NOTICE << "Ended test " << GetName()
            << " with execution-time "
            << std::chrono::duration <double, std::milli> (extime).count()
            << " ms.";
    }

    inline const std::string& GetName() { return name_; }

protected:
    virtual void DoRunTests() = 0;

private:

    const std::string name_;
};

class SimpleTest : public Test
{
public:
    SimpleTest(const std::string& name, uint64_t numTasks)
        : Test(name), pool_(0, static_cast<unsigned int>(numTasks)),
        total_tasks_processed_(0), total_tasks_queued_(0),
        done_{false},
        num_tasks_to_do_{numTasks}
    {
    }

protected:
    inline void QueueOne()
    {
        if (++total_tasks_queued_ > num_tasks_to_do_) {
            done_ = true;
        } else {
            pool_.Post(task_t{bind(&SimpleTest::OneTask, this), "simple"});
        }
    }

    void OneTask()
    {
        ++total_tasks_processed_;
        if (!done_) {
            QueueOne();
        }
    }

    void DoRunTests() override
    {
        LOG_NOTICE_FN << "I will now run " << num_tasks_to_do_ << " tasks";
        for(int i = 0; i < 1000; i++) {
            QueueOne();
        }

        while(total_tasks_processed_ < num_tasks_to_do_) {
            this_thread::sleep_for(chrono::milliseconds(50));
        }
        pool_.Close();
    }

private:
    Threadpool pool_;
    std::atomic<std::uint64_t> total_tasks_processed_;
    std::atomic<std::uint64_t> total_tasks_queued_;
    atomic<bool> done_;
    const uint64_t num_tasks_to_do_;

};


int main(int arc, char *argv[])
{
    log::LogEngine logger;
    logger.AddHandler(make_shared<log::LogToStream>());
    logger.AddHandler(make_shared<log::LogToFile>("perf_tests.log", true, "file", log::LL_TRACE1));

    SimpleTest ss("SimpleTest", 50000000L);
    ss.RunTests();

    return 0;
}
