#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <queue>

template <typename Type, std::size_t Capacity> struct Queue {

    static_assert((Capacity & (Capacity - 1)) == 0,
                  "Queue must have a size that is a power of 2.");

    void Push(Type element) {
        /* We cannot insert an element if we already excedded the maximum
         * capacity */
        if (Capacity <= storage.size()) {
            return;
        }
        storage.push(element);
    }
    Type Pop() {
        Type element = storage.front();
        storage.pop();
        return element;
    }
    int Count() const { return storage.size(); }
    constexpr int Size() const { return Capacity; }

  private:
    std::queue<Type> storage;
};
#endif // QUEUE_HPP
