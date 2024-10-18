#include <atomic>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

template <typename T>
class ring_buffer
{
public:
    ring_buffer(size_t capacity) : tail(0),
                                   storage(capacity + 1),
                                   head(0)
    {
        capacityForPush = storage.size();
        capacityForPop = storage.size();
    }

    bool push(T value)
    {
        size_t curr_tail = tail.load(std::memory_order_relaxed); // tail
        size_t next_tail = (curr_tail + 1) % capacityForPush;

        if (next_tail == head.load(std::memory_order_acquire)) // head
            return false;

        storage[curr_tail] = std::move(value);            // storage
        tail.store(next_tail, std::memory_order_release); // tail

        return true;
    }

    bool pop(T &value)
    {
        size_t curr_head = head.load(std::memory_order_relaxed); // head

        if (curr_head == tail.load(std::memory_order_acquire)) // tail
            return false;

        value = std::move(storage[curr_head]); // storage

        head.store((curr_head + 1) % capacityForPop, std::memory_order_release); // head

        return true;
    }

private:
    size_t capacityForPop{0};
    std::atomic<size_t> tail;

    std::vector<T> storage;

    size_t capacityForPush{0};
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
    int ms = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

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