#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <iostream>
#include <optional>
#include <thread>

template <typename Type, std::size_t Capacity> struct Queue {

    static_assert(Capacity > 0, "Size of Queue must be larger than 0.");
    static_assert((Capacity & (Capacity - 1)) == 0,
                  "Size of Queue must be a power of 2.");

    static constexpr std::size_t Mask = Capacity - 1;

    bool Push(Type element) {
        std::size_t in;
        using namespace std::chrono_literals;
        auto start = std::chrono::high_resolution_clock::now();

        while (true) {
            in = inIndex;
            bool hasContent{storage[in]};

            while (in - outIndex == Capacity && !storage[in]) {
                auto current = std::chrono::high_resolution_clock::now();
                if (current - start >= 200ms) {
                    return false;
                }
                std::this_thread::yield();
            }

            std::size_t nextIndex = (in + 1) & Mask;
            bool exchangeSuccess = inIndex.compare_exchange_weak(in, nextIndex);

            if (!hasContent && exchangeSuccess) {
                break;
            }
        }

        storage[in] = element;
        return true;
    }

    std::optional<Type> Pop() {
        std::size_t out{};
        using namespace std::chrono_literals;
        auto start = std::chrono::high_resolution_clock::now();

        while (true) {
            out = outIndex;
            const bool hasContent{storage[out]};

            while (out == inIndex && !hasContent) {
                auto current = std::chrono::high_resolution_clock::now();
                if (current - start >= 200ms) {
                    return std::nullopt;
                }
                std::this_thread::yield();
            }

            std::size_t nextIndex = (out + 1) & Mask;
            const bool exchanged =
                outIndex.compare_exchange_weak(out, nextIndex);
            if (hasContent && exchanged) {
                break;
            }
        }

        Type element = *storage[out];
        storage[out] = std::nullopt;
        return element;
    }

    int Count() const {
        // if (inIndex >= outIndex) {
        //     return inIndex - outIndex;
        // } else {
        //     return (Capacity - outIndex) + inIndex;
        // }

        /* Simplification of the previous calculations to assert the amount of
         * elements in the container, without using a condition */
        return ((Capacity - outIndex) + inIndex) & Mask;
    }
    constexpr int Size() const { return Capacity; }

  private:
    std::array<std::optional<Type>, Capacity> storage{};
    std::atomic<std::size_t> inIndex{}, outIndex{};
};
#endif // QUEUE_HPP
