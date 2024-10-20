# Ring buffer optimization

### Solution

```cpp
    private:
    std::atomic<size_t> tail;
    // Moving storage between variables helping to aviod false sharing cache
    // of CPU between push() and pop() functions from separate threads when working 
    // with tail and head variables
    std::vector<T> storage;
    std::atomic<size_t> head;
```

### With threads checking

```
mkdir build && cd build
g++ -std=c++17 -O2 -pthread -fsanitize=thread ../main.cpp && ./a.out
```

Average time:

```
time: 5515ms sum: 49999995000000
time: 5844ms sum: 49999995000000
time: 5577ms sum: 49999995000000
5645
```

### Without threads checking

```
mkdir build && cd build
g++ -std=c++17 -O2 -pthread ../main.cpp && ./a.out
```

Average time:

```
time: 466ms sum: 49999995000000
time: 414ms sum: 49999995000000
time: 426ms sum: 49999995000000
435
```
