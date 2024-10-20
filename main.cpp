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

        // Has its own capacity to avoid false cache sharing capacity variable
        // between push() and pop()
        size_t capacity_{0};

    private:
        // Shift for cache filling
        char padding_[64 - sizeof(std::atomic<size_t>)]; // size of cache of common CPUs
    };

public:
    RingBuffer(size_t capacity) : storage_(capacity + 1)
    {
        head_.capacity_ = storage_.size();
        tail_.capacity_ = storage_.size();
    }

    bool push(T value)
    {
        // HEAD in cache ---------------

        // Setting order memory_order_acquire cause head variable in pop() can be modified.
        // So we MUST use memory_order_acquire and NOT memory_order_relaxed
        size_t curr_head = head_.value_.load(std::memory_order_acquire);
        // -----------------------------

        // TAIL in cache ---------------

        // Setting order memory_order_relaxed cause tail variable only in push() can be modified.
        // No need to use here memory_order_acquire
        size_t curr_tail = tail_.value_.load(std::memory_order_relaxed);
        size_t next_tail = (curr_tail + 1) % tail_.capacity_;

        if (next_tail == curr_head)
            return false;
        // -----------------------------

        // STORAGE in cache ------------
        storage_[curr_tail] = std::move(value);
        // -----------------------------

        // TAIL in cache ---------------

        // Setting order memory_order_release cause we are modifing tail variable
        tail_.value_.store(next_tail, std::memory_order_release);
        // -----------------------------

        return true;
    }

    bool pop(T &value)
    {
        // TAIL in cache ---------------

        // Setting order memory_order_acquire cause head variable in pop() can be modified.
        // So we MUST use memory_order_acquire and NOT memory_order_relaxed
        size_t curr_tail = tail_.value_.load(std::memory_order_acquire);
        // -----------------------------

        // HEAD in cache ---------------

        // Setting order memory_order_relaxed cause head variable only in push() can be modified.
        // No need to use here memory_order_acquire
        size_t curr_head = head_.value_.load(std::memory_order_relaxed);

        if (curr_head == curr_tail)
            return false;

        // Setting order memory_order_release cause we are modifing head variable
        head_.value_.store((curr_head + 1) % head_.capacity_, std::memory_order_release);
        // -----------------------------

        // STORAGE in cache ------------
        value = std::move(storage_[curr_head]);
        // -----------------------------

        return true;
    }

private:
    PaddedAtomic tail_;
    PaddedAtomic head_;
    // There is no need to set storage variable between tail and head variables
    // to avoid false cache sharing cause tail and head has own address shifts
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
