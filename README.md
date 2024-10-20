# Оптимизиация кольцевого буфера

Сборка и запуск:

```
mkdir build && cd build
g++ -std=c++17 -O2 -pthread ../main.cpp && ./a.out
```

Сборка и запус с проверкой коректности работы потоков:

```
mkdir build && cd build
g++ -std=c++17 -O2 -pthread -fsanitize=thread ../main.cpp && ./a.out
```
