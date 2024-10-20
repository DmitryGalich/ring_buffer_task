# Ring buffer optimisation

### Solution

Using std::memory_order_* modifiers while loading and storing values in atomic variables

### With threads checking

```
mkdir build && cd build
g++ -std=c++17 -O2 -pthread -fsanitize=thread ../main.cpp && ./a.out
```

Average time:

```
time: 3469ms sum: 49999995000000
time: 3426ms sum: 49999995000000
time: 3492ms sum: 49999995000000
3462
```

### Without threads checking

```
mkdir build && cd build
g++ -std=c++17 -O2 -pthread ../main.cpp && ./a.out
```

Average time:

```
time: 441ms sum: 49999995000000
time: 425ms sum: 49999995000000
time: 376ms sum: 49999995000000
414
```
