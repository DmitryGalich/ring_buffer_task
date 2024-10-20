# Ring buffer optimization

### Solution

* Set memory_order_* for load() and store() operations with atomic variables.
* Aviod false cache sharing capacity by creating two capacities variables - one for head, another for tail.
* Add cache padding for head and tail variables.
* Reorder lines inside push() and pop() functions to group all operations with each of variables as it possible by business logic. It reducing caache switching operations.
* Remove get_next() function and move logic of calculation next index to avoid coping of argument and result values.

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
