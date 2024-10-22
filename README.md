# Ring buffer optimization

### Result

* Average time - 125 ms (Before optimization - 550 ms)
* Boost - 88%
* No data races

### Solution

1. Set memory_order_* for load() and store() operations with atomic variables.
    * Branch - [memory_order](https://github.com/DmitryGalich/ring_buffer_task/tree/memory_order)
    * Average time - 414 ms (Before optimization - 550 ms)
    * Boost - 25%

2. Using compare_exchange_*() functions.
    * Branch - [compare_exchange_operations](https://github.com/DmitryGalich/ring_buffer_task/tree/compare_exchange_operations)
    * Boost - 18%

3. Capacity
    * Aviod false cache sharing capacity by creating two capacities variables - one for head, another for tail. Remove get_next() function and move logic of calculation next index to avoid coping of argument and result values.
    * Branch - [two_capacities](https://github.com/DmitryGalich/ring_buffer_task/tree/two_capacities)
    * Average time - 517 ms (Before optimization - 550 ms)
    * Boost - 6%

4. Add cache padding for head and tail variables.
    * Branch - [PaddedAtomic](https://github.com/DmitryGalich/ring_buffer_task/tree/PaddedAtomic)
    * Average time - 440 ms (Before optimization - 550 ms)
    * Boost - 20%

* Reorder lines inside push() and pop() functions to group all operations with each of variables as it possible by business logic. It reducing caache switching operations.

### Unsuccesefull Solutions

1. Setting get_next() as inline function
    * Branch - [inline](https://github.com/DmitryGalich/ring_buffer_task/tree/inline)
    * Boost - 0%

2. Set memory_order_relaxed for all load() and store() operations with atomic variables.
    * Branch - [all_relaxed](https://github.com/DmitryGalich/ring_buffer_task/tree/all_relaxed)
    * Boost - 86%
    * **DATA RACE ERROR**

### With threads checking

```
mkdir build && cd build
g++ -std=c++17 -O2 -pthread -fsanitize=thread ../main.cpp && ./a.out
```

Average time:

```
time: 3597ms sum: 49999995000000
time: 3607ms sum: 49999995000000
time: 3614ms sum: 49999995000000
3606
```

### Without threads checking

```
mkdir build && cd build
g++ -std=c++17 -O2 -pthread ../main.cpp && ./a.out
```

Average time:

```
time: 151ms sum: 49999995000000
time: 113ms sum: 49999995000000
time: 113ms sum: 49999995000000
125
```
