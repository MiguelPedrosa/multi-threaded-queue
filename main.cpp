#include "Queue.hpp"
#include <iostream>

int main() {
    Queue<int, 4> queue;

    queue.Push(1);
    queue.Push(2);
    queue.Push(3);
    std::cout << "Popped element: " << queue.Pop() << '\n';
    std::cout << "Popped element: " << queue.Pop() << '\n';
    queue.Push(4);
    std::cout << "Popped element: " << queue.Pop() << '\n';
    std::cout << "Popped element: " << queue.Pop() << '\n';
}
