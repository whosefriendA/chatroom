#include <thread>
#include <condition_variable>
#include <functional>
#include <queue>
#include <mutex>
#include <atomic>
using namespace std;
struct Task
{
    using funcv = std::function<void()>;

    Task() = default;
    Task(funcv f) : function(std::move(f)) {}

    void execute()
    {
        if (function)
        {
            function();
        }
    }
    funcv function;
};

class ThreadPool
{
public:
    ThreadPool(size_t numThreads) : stop(false)
    {
        for (size_t i = 0; i < numThreads; ++i)
        {
            workers.emplace_back(&ThreadPool::worker, this);
        }
    }

    ~ThreadPool()
    {
        stop = true;
        condition.notify_all();
        for (thread &worker : workers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    }

    void addTask(const Task &task)
    {
        {
            unique_lock<mutex> lock(tasksMutex);
            taskqueue.push(task);
        }
        condition.notify_one();
    }

    void worker()
    {
        while (true)
        {
            Task task;
            {
                unique_lock<mutex> lock(tasksMutex);
                condition.wait(lock, [this]
                { return stop || !taskqueue.empty(); });
                if (stop && taskqueue.empty())
                {
                    return;
                }
                task = taskqueue.front();
                taskqueue.pop();
            }
            task.execute();
        }
    }

private:
    vector<thread> workers;
    queue<Task> taskqueue;
    mutex tasksMutex;
    condition_variable condition;
    atomic<bool> stop;
};