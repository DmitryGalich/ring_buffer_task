# Ring buffer optimization

### Solution

* Remove get_next() function and compute next index in each function to avoid coping of argument to get_next() of result from get_next()
* Create two separate capacity variables with same value for using in push() and pop() functions.

### With threads checking

```
mkdir build && cd build
g++ -std=c++17 -O2 -pthread -fsanitize=thread ../main.cpp && ./a.out
```

Average time:

```
time: 5041ms sum: 49999995000000
time: 5120ms sum: 49999995000000
time: 4951ms sum: 49999995000000
5037
```

### Without threads checking

```
mkdir build && cd build
g++ -std=c++17 -O2 -pthread ../main.cpp && ./a.out
```

Average time:

```
time: 545ms sum: 49999995000000
time: 507ms sum: 49999995000000
time: 500ms sum: 49999995000000
517
```
