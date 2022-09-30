#ifndef QUEUE_HPP
#define QUEUE_HPP

#include <queue>

template<typename T>
struct Queue {
	Queue(unsigned int size) : capacity{size}, storage{} {}
	void Push(T element) {
		/* We cannot insert an element if we already excedded the maximum capacity */
		if (capacity <= storage.size()) {
			return;
		}
		storage.push(element);
	}
	T Pop() {
		T element = storage.front();
		storage.pop();
		return element;
	}
	int Count() const {
		return storage.size();
	}
	int Size() const {
		return capacity;
	}

private:
	const unsigned int capacity;
	std::queue<T> storage;
};
#endif // QUEUE_HPP
