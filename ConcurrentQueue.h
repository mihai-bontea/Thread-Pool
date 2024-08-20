#pragma once

#include <atomic>
#include <mutex>
#include <queue>

template <typename T>
class ConcurrentQueue {
private:
    mutable std::mutex mutex_;
    std::queue<T> queue_;
public:
    ConcurrentQueue() = default;

    ConcurrentQueue(const ConcurrentQueue& other) = delete;

    ConcurrentQueue& operator=(const ConcurrentQueue& other) = delete;

    void push(T elem)
    {
        std::scoped_lock lock(mutex_);
        queue_.push(std::move(elem));
    }

    bool try_pop(T& elem)
    {
        std::scoped_lock lock(mutex_);

        if (queue_.empty())
        {
            return false;
        }
        elem = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    inline int size() const
    {
        std::scoped_lock lock(mutex_);
        return queue_.size();
    }

    inline bool empty() const
    {
        std::scoped_lock lock(mutex_);
        return queue_.empty();
    }
};

//template class ConcurrentQueue<int>;
