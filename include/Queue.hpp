#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <array>
#include <atomic>
#include <chrono>
#include <optional>
#include <thread>

/**
 * @brief Multi-threaded queue to store a finite number of elements
 * @details The project uses a circular buffer to implement a lockless queue
 * for multi-threaded storage of any type of elements. There are two atomic
 * indexes: one that contains the next position to insert an element and one
 * that indicates the next item to pop. The storage will contain elements from
 * the removal index to the insertion index and both values are always lower
 * than the Capacity. If both indexes point to the same value, then we don't
 * know if the storage is empty or full. To solve this, we wrap each element in
 * storage with std::optional to register if the element is present or not.
 *
 * @tparam Type The type of the elements we intend to store.
 * @tparam Capacity The size of the queue. Has to be a power of 2.
 */
template <typename Type, std::size_t Capacity> struct Queue {

    static_assert(Capacity > 0, "Size of Queue must be larger than 0.");
    static_assert((Capacity & (Capacity - 1)) == 0,
                  "Size of Queue must be a power of 2.");

    /**
     * @brief Used as substituition for the modulus operator to ensure that the
     * indexes always have values below Capacity.
     *
     */
    static constexpr std::size_t Mask = Capacity - 1;

    /**
     * @brief Tries to push the argument into the queue
     * @details The operation might block if the storage is full. In that case,
     * the function yields its thread's execution to allow other threads to run
     * and maybe free space in the storage. If no space is cleared within 200ms,
     * then the element can't be pushed and the functions leaves with a false
     * return.
     *
     * @param element Element to be inserted into storage
     * @return true If push operation was successful
     * @return false If the storage was full and it didn't release space within
     * the available timeframe
     */
    bool Push(Type element) {
        std::size_t in;
        using namespace std::chrono_literals; // Easier syntax when writing
                                              // constants in miliseconds
        auto startTime = std::chrono::high_resolution_clock::now();

        while (true) {
            in = inIndex;
            bool hasContent{storage[in]};

	    /* outIndex is an atomic, so it's value will update after another
	     * thread calls Pop(). */
            while (in - outIndex == Capacity && !storage[in]) {
                auto currentTime = std::chrono::high_resolution_clock::now();
                if (currentTime - startTime >= 200ms) {
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

    /**
     * @brief Tries to pop an element from the queue
     * @details The operation might block if the storage is empty. In that case,
     * the function yields its thread's execution to allow other threads to run
     * and maybe populate space in the storage. If item is inserted within
     * 200ms, then an element can't be poped and the functions leaves with an
     * empty container.
     *
     * @return std::optional<Type> Returns std::nullopt if no element was pushed
     * within the 200ms time window or the next element in the queue.
     */
    std::optional<Type> Pop() {
        std::size_t out{};
        using namespace std::chrono_literals; // Easier syntax when writing
                                              // constants in miliseconds
        auto startTime = std::chrono::high_resolution_clock::now();

        while (true) {
            out = outIndex;
            bool hasContent{storage[out]};

	    /* inIndex is an atomic, so it's value will update after another
	     * thread calls Push(). */
            while (out == inIndex && !hasContent) {
                auto currentTime = std::chrono::high_resolution_clock::now();
                if (currentTime - startTime >= 200ms) {
                    return std::nullopt;
                }
                std::this_thread::yield();
            }

            std::size_t nextIndex = (out + 1) & Mask;
            bool exchanged = outIndex.compare_exchange_weak(out, nextIndex);
            if (hasContent && exchanged) {
                break;
            }
        }

        Type element = *storage[out];
        storage[out] = std::nullopt;
        return element;
    }

    /**
     * @brief Calculates the elements present in storage.
     *
     * @return int Elements present in storage.
     */
    int Count() const {
        /* Edge case when storage is full that is not covered in the last return
         * calcutation */
        if (outIndex == inIndex && storage[outIndex]) {
            return Size();
        }
        // if (inIndex >= outIndex) {
        //     return inIndex - outIndex;
        // } else {
        //     return (Capacity - outIndex) + inIndex;
        // }
        /* Simplification of the commented calculations */
        return ((Capacity - outIndex) + inIndex) & Mask;
    }

    /**
     * @brief Returns the maximum capacity of the container.
     * @details Since this value is a template parameter, we can make the
     * function constexpr.
     *
     * @return constexpr int Maximum capacity of the container.
     */
    constexpr int Size() const { return Capacity; }

  private:
    /**
     * @brief Fixed size storage that holds the container's values. All values
     * start at std::nullopt since we default innitiaze the array.
     */
    std::array<std::optional<Type>, Capacity> storage{};
    /**
     * @brief The insertion and removal indexes of the container. These values
     * start at 0 since we default innitiaze them.
     */
    std::atomic<std::size_t> inIndex{}, outIndex{};
};
#endif // QUEUE_HPP
