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
        // Save same value to two variables
        capacity_for_pop.store(storage.size());
        capacity_for_push.store(storage.size());
    }

    bool push(T value)
    {
        size_t curr_tail = tail.load();
        size_t next_tail = ((curr_tail + 1) % capacity_for_push.load(std::memory_order_relaxed));
        size_t curr_head = head.load();

        if (next_tail == curr_head)
        {
            return false;
        }

        storage[curr_tail] = std::move(value);
        tail.store(next_tail);

        return true;
    }

    bool pop(T &value)
    {
        size_t curr_head = head.load();
        size_t curr_tail = tail.load();

        if (curr_head == curr_tail)
        {
            return false;
        }

        value = std::move(storage[curr_head]);
        head.store(((curr_head + 1) % capacity_for_pop.load(std::memory_order_relaxed)));

        return true;
    }

private:
    std::vector<T> storage;
    std::atomic<size_t> capacity_for_pop;
    std::atomic<size_t> tail;
    std::atomic<size_t> capacity_for_push;
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

// g++ -std=c++17 -O2 -pthread main.cpp
// g++ -std=c++17 -O2 -pthread -fsanitize=thread main.cpp