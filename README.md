# Description

A multi-threaded queue for storing a finite number of elements. Uses a lock-free circular buffered queue.


# Implemented functionalities:

 - [x] Storage; basic pop and push operations.
 - [x] Reading thread blocks if the queue is empty.
 - [x] Writing thread blocks if the queue is full.
 - [x] Reading and writing threads block for a set amount of time. Functions return a boolean indicating the success of the operation.
 - [x] The project does not use `std::queue`, but instead a circular buffer.
 - [x] The project does not use `std::mutex`, but instead atomic operations.
 - [x] Documented using Doxygen.
 - [x] Includes tests in googletest.
 - [x] Uses CMake for building.


# Requirements:

The following table list the tools and respective versions used during development of this project. Other versions *probably* will work as well, but they haven't been tested.

  | Purpose         | Tool name    | Version  |
  | --------------- | ------------ | -------  |
  | Build Generator | `Cmake`      | `3.24.2` |
  | Compiler        | `g++`        | `12.2.0` |
  | Documentation   | `Doxygen`    | `1.9.3`  |
  | Test Suite      | `googletest` | `1.12.1` |

  Note: `googletest` should be automatically downloaded by cmake and not require an external download.


# Building:

From the directory that contains the `CMakeLists.txt`, run the following commands:
  * `$ cmake -S . -B build`
  * `$ cd build && make`


# Unit tests:

From within the `build` directory run the command:
  * `$ ./test_pairs`

### Note:

Sometimes the tests will fail not because of the logic of the functions, but because the push and pop operations have a timeout behaviour. As such, it is dificult to correctly assert the expected behaviour.

As a workaround, we calculate the worst possible outcome and compare it with the test's execution. We then see if it is within an acceptable margin of error. This margin can be changed by editing the `acceptableMargin` variable in file `/tests/single.cpp`, which, for now, is set to a 1% margin of error.

Another possibility would be to increase the amount of tries before giving out, but that quickly increases the test times.


# Design Choices:

Because I want to reuse this code in other projects, the design of the container was affected by a few personal decisions:

 - I wanted to avoid a container that used mutexes correctlt implement multi-threading. As such, the design of the project was based in the [UniQ Multithreading Libray](https://github.com/bittnkr/uniq), that implements a **lock-free circular buffered queue**, with a few modifications of my own.
 - I wanted to avoid memory allocations, as such the container uses a compile-time constant for dictating its storage size. My uses don't require a resizeable container. This aproach should reduce memory consuption as the container can be just a local variable.
 - The container's capacity has to be a power of 2. This constraint steams from needing the internal indexes to remain below the capacity and this method simplifies the implementation.


# Possible improvements:

 - **Using `std::optional` for each element of storage**: The issue was to destinguish if the storage was empty of full when the insert and extract indexes hold the same value. The implemented solution was to use an optional to validate if each element does in fact exist. The issue is that it creates an additional overhead that scales with the size of the container.
    - An alternative could be to use a boolean that registers if the container is full, when the insertion index catchs up to the removal index. This was not done to simplify the code and avoid more checks.
