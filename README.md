# BoDealer
Bridge software by KotBasilio

Build upon DDS by Bo Haglund, Soren Hein, Bob Richardson   Rev X, 2014-11-16

## Building and testing

BoDealer requires CMake 3.20 or newer, a C++17 compiler, and Curses on Linux
and macOS.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
```

