#include <atomic>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

template <typename T>
class ring_buffer
{
public:
    ring_buffer(size_t capacity) : storage(capacity + 1),
                                   tail(0),
                                   head(0)
    {
    }

    bool push(T value)
    {
        // Setting order memory_order_relaxed cause tail variable only in push() can be modified.
        // No need to use here memory_order_acquire
        size_t curr_tail = tail.load(std::memory_order_relaxed);

        // Setting order memory_order_acquire cause head variable in pop() can be modified.
        // So we MUST use memory_order_acquire and NOT memory_order_relaxed
        size_t curr_head = head.load(std::memory_order_acquire);

        if (get_next(curr_tail) == curr_head)
        {
            return false;
        }

        storage[curr_tail] = std::move(value);

        // Setting order memory_order_release cause we modify tail variable
        tail.store(get_next(curr_tail), std::memory_order_release);

        return true;
    }

    bool pop(T &value)
    {
        // Setting order memory_order_relaxed cause head variable only in push() can be modified.
        // No need to use here memory_order_acquire
        size_t curr_head = head.load(std::memory_order_relaxed);

        // Setting order memory_order_acquire cause tail variable in pop() can be modified.
        // So we MUST use memory_order_acquire and NOT memory_order_relaxed
        size_t curr_tail = tail.load(std::memory_order_acquire);

        if (curr_head == curr_tail)
        {
            return false;
        }

        value = std::move(storage[curr_head]);

        // Setting order memory_order_release cause we modify head variable
        head.store(get_next(curr_head), std::memory_order_release);

        return true;
    }

private:
    size_t get_next(size_t slot) const
    {
        return (slot + 1) % storage.size();
    }

private:
    std::vector<T> storage;
    std::atomic<size_t> tail;
    std::atomic<size_t> head;
};

int test()
{
    int count = 10000000;

    ring_buffer<int> buffer(1024);

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