# Ring buffer optimisation

### Solution - NOT CORRECT. JUST FOR FUN

Using std::memory_order_relaxed modifiers while loading and storing values in atomic variables

### With threads checking

```
mkdir build && cd build
g++ -std=c++17 -O2 -pthread -fsanitize=thread ../main.cpp && ./a.out
```

Error:

```
WARNING: ThreadSanitizer: data race (pid=89399)
...
ThreadSanitizer: reported 1 warnings
zsh: abort      ./a.out
```

Average time:

```
time: 934ms sum: 49999994998975
time: 519ms sum: 49999994998975
time: 522ms sum: 49999994817550
658
```

### Without threads checking

```
mkdir build && cd build
g++ -std=c++17 -O2 -pthread ../main.cpp && ./a.out
```

Average time:

```
time: 104ms sum: 49999995000000
time: 63ms sum: 49999995000000
time: 66ms sum: 49999995000000
77
```
