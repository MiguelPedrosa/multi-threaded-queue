#include "Queue.hpp"
#include <atomic>
#include <iostream>
#include <limits>
#include <thread>
#include <vector>


int main() {
    Queue<int, 8> queue;
    constexpr int totalAmount = 100;
	constexpr int maxTries = 20;
    std::atomic<long> total = 0;

    auto consumer = [&]() {
        int tries = 0;

        while (true) {
            if (tries > maxTries) {
                break;
            }
            if (auto value = queue.Pop()) {
                total -= *value;
                tries = 0;
            } else {
                tries++;
            }
        }
    };
    auto producer = [&]() {
        long tries = 0;

        for (int i = 1; i < totalAmount; /**/) {
            if (tries > maxTries) {
                break;
            }
            if (queue.Push(i)) {
                tries = 0;
                total += i++;
            } else {
                tries++;
            }
        }
    };

    std::vector<std::thread> threads;
    for (int i = 0; i < 3; i++) {
        threads.push_back(std::thread(consumer));
        threads.push_back(std::thread(producer));
    }
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Final outcome is " << total << '\n';
}
