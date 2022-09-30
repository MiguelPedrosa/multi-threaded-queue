#include "Queue.hpp"
#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

int main() {
    Queue<int, 8> queue;
    constexpr int totalAmount = 100;

    std::atomic<int> sum = 0, writtenAmount{}, readAmount{};
    constexpr int expectedSum = totalAmount * (totalAmount + 1) / 2;

    auto consumer = [&]() {
        while (true) {
            int i = readAmount;
            if (i > totalAmount) {
                break;
            }
            bool canOperate = readAmount.compare_exchange_weak(i, i + 1);
            if (canOperate) {
                sum += queue.Pop();
            }
        }
    };

    auto producer = [&]() {
        while (true) {
            int i = writtenAmount;
            if (i > totalAmount) {
                break;
            }
            bool canOperate = writtenAmount.compare_exchange_weak(i, i + 1);
            if (canOperate) {
                queue.Push(i);
            }
        }
    };

    std::vector<std::thread> threads;
    threads.push_back(std::thread(producer));
    threads.push_back(std::thread(producer));
    threads.push_back(std::thread(consumer));
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Expected outcome was " << expectedSum << '\n';
    std::cout << "Calculted outcome is " << sum << '\n';
}
