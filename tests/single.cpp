#include "Queue.hpp"
#include <atomic>
#include <cmath>
#include <gtest/gtest.h>
#include <thread>
#include <vector>

/**
 * @brief Basic test with equal amounts of consumer and producer threads
 * @details Creates an equal number of consumer and producer threads. Each
 * producer increments a variable and the consumer decrement an equal amount.
 * The end result should be 0, if all operations are successful.
 *
 * @tparam Capacity Decides queue capacity.
 * @param itemCount The amount of items to accumulate in total
 * @param maxTries Maximum number of attempts an operation can perform
 * @param threadPairs How many pairs of consumer and producer threads to
 * schedule
 * @return long
 */
template <std::size_t Capacity>
long basicTest(long itemCount, int maxTries, int threadPairs) {
    Queue<int, Capacity> queue;
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

        for (long i = 1; i < itemCount; /**/) {
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
    for (int i = 0; i < threadPairs; i++) {
        threads.push_back(std::thread(consumer));
        threads.push_back(std::thread(producer));
    }
    for (auto& t : threads) {
        t.join();
    }

    return total;
}

constexpr double acceptableMargin = 0.02;

/* Simple and base line test */
TEST(TestQueue, BasicTest) {
    constexpr long items = 1'000;
    constexpr long worstOutcome = items * (items + 1) / 2;

    long result = basicTest<16>(items, 10, 1);
    double margin = std::abs(double(result) / worstOutcome);

    EXPECT_LT(margin, acceptableMargin);
}

/* Tests a higher amount of items */
TEST(TestQueue, HigherItems) {
    constexpr long items = 1'000'000;
    constexpr long worstOutcome = items * (items + 1) / 2;

    long result = basicTest<16>(items, 20, 1);
    double margin = std::abs(double(result) / worstOutcome);

    EXPECT_LT(margin, acceptableMargin);
}

/* Tests a higher amount of consumer/producer pairs */
TEST(TestQueue, HigherPairs) {
    constexpr long items = 1'000'000;
    constexpr long worstOutcome = items * (items + 1) / 2;

    long result = basicTest<16>(items, 20, 3);
    double margin = std::abs(double(result) / worstOutcome);

    EXPECT_LT(margin, acceptableMargin);
}

/* Tests a lower amount of threads, meaning they block more ofter */
TEST(TestQueue, ThreadBlockage) {
    constexpr long items = 100;
    constexpr long worstOutcome = items * (items + 1) / 2;

    long result = basicTest<2>(items, 50, 1);
    double margin = std::abs(double(result) / worstOutcome);

    EXPECT_LT(margin, acceptableMargin);
}
