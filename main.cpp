#include <atomic>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

template <typename T>
class RingBuffer
{
    struct alignas(64) PaddedAtomic
    {
        std::atomic<size_t> value_{0};

        size_t capacity_{0};

    private:
        char padding_[64 - sizeof(std::atomic<size_t>)];
    };

public:
    RingBuffer(size_t capacity) : storage_(capacity + 1)
    {
        head_.capacity_ = storage_.size();
        tail_.capacity_ = storage_.size();
    }

    bool push(T value)
    {
        size_t curr_tail = tail_.value_.load(std::memory_order_relaxed);
        size_t curr_head = head_.value_.load(std::memory_order_acquire);
        size_t next_tail = (curr_tail + 1) % tail_.capacity_;

        if (next_tail == curr_head)
            return false;

        storage_[curr_tail] = std::move(value);

        size_t expected_tail = curr_tail;
        while (!tail_.value_.compare_exchange_weak(expected_tail, next_tail, std::memory_order_release, std::memory_order_relaxed))
        {
            expected_tail = curr_tail;
        }

        return true;
    }

    bool pop(T &value)
    {
        size_t curr_head = head_.value_.load(std::memory_order_relaxed);
        size_t curr_tail = tail_.value_.load(std::memory_order_acquire);

        if (curr_head == curr_tail)
            return false;

        value = std::move(storage_[curr_head]);

        size_t next_head = (curr_head + 1) % head_.capacity_;

        size_t expected_head = curr_head;
        while (!head_.value_.compare_exchange_strong(expected_head, next_head, std::memory_order_release, std::memory_order_relaxed))
        {
            expected_head = curr_head;
        }

        return true;
    }

private:
    PaddedAtomic tail_;
    PaddedAtomic head_;
    std::vector<T> storage_;
};

int test()
{
    int count = 10000000;

    RingBuffer<int> buffer(1024);

    auto start = std::chrono::steady_clock::now();

    std::thread producer([&]()
                         {
        for (int i = 0; i < count; ++i)
        {
            while (!buffer.push(i))
            {
                std::this_thread::yield();
            }
        } });

    uint64_t sum = 0;

    std::thread consumer([&]()
                         {
        for (int i = 0; i < count; ++i)
        {
            int value;

            while (!buffer.pop(value))
            {
                std::this_thread::yield();
            }

            sum += value;
        } });

    producer.join();
    consumer.join();

    auto finish = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

    std::cout << "time: " << ms << "ms ";
    std::cout << "sum: " << sum << std::endl;
    return ms;
}

int main()
{
    int count = 3;
    int sum = 0;
    for (int i = 0; i < count; ++i)
    {
        sum += test();
    }

    std::cout << sum / count << std::endl;

    return 0;
};
