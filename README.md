# Ring buffer optimization

### Result

* Average time - 229 ms (Before optimization - 550 ms)
* Boost - 59%
* No data races

### Solution

1. Set memory_order_* for load() and store() operations with atomic variables.
    * Branch - [memory_order](https://github.com/DmitryGalich/ring_buffer_task/tree/memory_order)
    * Average time - 414 ms (Before optimization - 550 ms)
    * Boost - 25%

2. Capacity
    * Aviod false cache sharing capacity by creating two capacities variables - one for head, another for tail. Remove get_next() function and move logic of calculation next index to avoid coping of argument and result values.
    * Branch - [two_capacities](https://github.com/DmitryGalich/ring_buffer_task/tree/two_capacities)
    * Average time - 517 ms (Before optimization - 550 ms)
    * Boost - 6%

3. Add cache padding for head and tail variables.
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
time: 3469ms sum: 49999995000000
time: 3500ms sum: 49999995000000
time: 3470ms sum: 49999995000000
3479
```

### Without threads checking

```
mkdir build && cd build
g++ -std=c++17 -O2 -pthread ../main.cpp && ./a.out
```

Average time:

```
time: 255ms sum: 49999995000000
time: 217ms sum: 49999995000000
time: 217ms sum: 49999995000000
229
```
