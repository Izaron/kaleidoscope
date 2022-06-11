# Kaleidoscope

A simple language made from this tutorial: https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html

## Build and run
```bash
docker build -t docker-kaleidoscope .
docker run --rm -v /home/sparkle/kaleidoscope/example:/example docker-kaleidoscope /example/fib.ka
```
It will make the `example/fib.o` file.

## Linking
Suppose you have this code in `fib.ka`:
```
# Compute the x'th fibonacci number.
def fib(x)
  if x < 3 then
    1
  else
    fib(x-1)+fib(x-2)
```

And this code in `main.cpp`:
```cpp
#include <iostream>

extern "C" double fib(double);

int main() {
    int n;
    while (std::cin >> n) {
        std::cout << fib(n) << std::endl;
    }
}
```

After compiling `fib.ka` to `fib.o`, you can link this file with C++ code:
```bash
clang++ main.cpp fib.o
```
