#pragma once

#include <mutex>
#include <optional>
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

    std::optional<T> try_pop()
    {
        std::scoped_lock lock(mutex_);

        if (queue_.empty())
        {
            return std::nullopt;
        }

        T elem = std::move(queue_.front());
        queue_.pop();
        return elem;
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
