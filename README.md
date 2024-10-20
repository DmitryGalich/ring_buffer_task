# Ring buffer optimization

### Solution

```cpp
    // Using structs with padding to aviod false sharing cache of CPU
    PaddedAtomic tail;
    PaddedAtomic head;
```

### With threads checking

```
mkdir build && cd build
g++ -std=c++17 -O2 -pthread -fsanitize=thread ../main.cpp && ./a.out
```

Average time:

```
time: 5579ms sum: 49999995000000
time: 5514ms sum: 49999995000000
time: 5555ms sum: 49999995000000
5549
```

### Without threads checking

```
mkdir build && cd build
g++ -std=c++17 -O2 -pthread ../main.cpp && ./a.out
```

Average time:

```
time: 459ms sum: 49999995000000
time: 436ms sum: 49999995000000
time: 427ms sum: 49999995000000
440
```
