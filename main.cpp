#include <atomic>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

struct alignas(64) PaddedAtomic
{
    std::atomic<size_t> value{0};
    char padding[64 - sizeof(std::atomic<size_t>)]; // Кэш-линия на большинстве архитектур 64 байта
};

template <typename T>
class ring_buffer
{
public:
    ring_buffer(size_t capacity) : storage(capacity + 1), tail{}, head{}
    {
        capacityForPush = storage.size();
        capacityForPop = storage.size();
    }

    bool push(T value)
    {
        size_t curr_tail = tail.value.load(std::memory_order_relaxed);
        size_t next_tail = (curr_tail + 1) % capacityForPush;
        size_t curr_head = head.value.load(std::memory_order_acquire);

        if (next_tail == curr_head)
            return false;

        storage[curr_tail] = std::move(value);
        tail.value.store(next_tail, std::memory_order_release);

        return true;
    }

    bool pop(T &value)
    {
        size_t curr_head = head.value.load(std::memory_order_relaxed);
        size_t curr_tail = tail.value.load(std::memory_order_acquire);

        if (curr_head == curr_tail)
            return false;

        value = std::move(storage[curr_head]);

        head.value.store((curr_head + 1) % capacityForPop, std::memory_order_release);

        return true;
    }

private:
    size_t capacityForPop{0};
    PaddedAtomic tail;

    std::vector<T> storage;

    size_t capacityForPush{0};
    PaddedAtomic head;
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