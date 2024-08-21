#pragma once

#include <functional>
#include <condition_variable>
#include <thread>
#include <cmath>
#include <future>

#include "ConcurrentQueue.h"

class WorkerUnit;

class ThreadPool {
private:
    class WorkerUnit {
    private:
        unsigned id_;
        ThreadPool &pool_;
    public:

        WorkerUnit(ThreadPool &pool, unsigned id) : pool_(pool), id_(id) {}

        void operator()()
        {
            std::optional<std::function<void()>> func;
            while (!pool_.shutdown_)
            {
                {
                    std::unique_lock<std::mutex> lock(pool_.cond_mutex_);
                    pool_.cond_lock_.wait(lock, [&] { return pool_.shutdown_ || !pool_.c_queue_.empty(); });

                    func = pool_.c_queue_.try_pop();
                }
                if (func)
                {
                    (*func)();
                }
            }
        }
    };

    std::atomic_bool shutdown_;
    std::vector<std::jthread> threads_;
    ConcurrentQueue<std::function<void()>> c_queue_;
    std::mutex cond_mutex_;
    std::condition_variable cond_lock_;
public:
    explicit ThreadPool(unsigned nr_threads = std::max((int)std::thread::hardware_concurrency(), 4))
        : threads_(std::vector<std::jthread>(nr_threads)), shutdown_(false){

        for (unsigned id = 0; id < nr_threads; ++id)
        {
            threads_[id] = std::jthread(WorkerUnit(*this, id));
        }
    }

    ThreadPool(const ThreadPool &) = delete;

    ThreadPool(ThreadPool &&) = delete;

    ThreadPool & operator=(const ThreadPool &) = delete;

    ThreadPool & operator=(ThreadPool &&) = delete;

    void shutdown()
    {
        shutdown_ = true;
        cond_lock_.notify_all();

        // why do I still need this...?
        for (auto & th : threads_)
            th.join();
    }

    template<typename F, typename... Args>
    auto submit(F&& f, Args&&... args) -> std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>
    {
        using ReturnType = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>;

        auto task_ptr = std::make_shared<std::packaged_task<ReturnType()>>(
                std::forward<F>(f), std::forward<Args>(args)...
        );

        c_queue_.push([task_ptr]() { (*task_ptr)(); });
        cond_lock_.notify_one();

        return task_ptr->get_future();
    }
};
