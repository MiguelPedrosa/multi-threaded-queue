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
 - [ ] Includes tests in cppunit/googletest.
 - [ ] Uses CMake for building.


# Requirements:

  - Building Documentation: the documentation generated during testing used version `1.9.3` of [`Doxygen`](https://doxygen.nl/index.html).
  - Compiler: The compiler used during development was `g++`, version `12.2.0`.


# Building:

  - `g++ -g -std=c++17 -Wall main.cpp -o test`


# Unit tests:

  - `./test`


# Design Choices:

Because I want to reuse this code in other projects, the design of the container was affected by a few personal decisions:

 - I wanted to avoid a container that used mutexes correctlt implement multi-threading. As such, the design of the project was based in the [UniQ Multithreading Libray](https://github.com/bittnkr/uniq), that implements a **lock-free circular buffered queue**, with a few modifications of my own.
 - I wanted to avoid memory allocations, as such the container uses a compile-time constant for dictating its storage size. My uses don't require a resizeable container. This aproach should reduce memory consuption as the container can be just a local variable.
 - The container's capacity has to be a power of 2. This constraint steams from needing the internal indexes to remain below the capacity and this method simplifies the implementation.


# Performance Issues:

 - **Using `std::optional` for each element of storage**: The issue was to destinguish if the storage was empty of full when the insert and extract indexes hold the same value. The implemented solution was to use an optional to validate if each element does in fact exist. The issue is that it creates an additional overhead that scales with the size of the container.
    - An alternative could be to use a boolean that registers if the container is full, when the insertion index catchs up to the removal index. This was not done to simplify the code and avoid more checks.