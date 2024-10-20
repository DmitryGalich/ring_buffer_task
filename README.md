# Ring buffer optimisation

### With threads checking

```
mkdir build && cd build
g++ -std=c++17 -O2 -pthread -fsanitize=thread ../main.cpp && ./a.out
```

Average time:

```
time: 5633ms sum: 49999995000000
time: 5640ms sum: 49999995000000
time: 5731ms sum: 49999995000000
5668
```

### Without threads checking

```
mkdir build && cd build
g++ -std=c++17 -O2 -pthread ../main.cpp && ./a.out
```

Average time:

```
time: 573ms sum: 49999995000000
time: 534ms sum: 49999995000000
time: 555ms sum: 49999995000000
554
```
