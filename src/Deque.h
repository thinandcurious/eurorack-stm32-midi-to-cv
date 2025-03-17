#pragma once

#include "main.h"

template <typename T, size_t N>
class Deque {
public:
    void push(T value) {
        data[tail] = value;
        tail = (tail + 1) % N;
    }

    bool empty() {
        return head == tail;
    }
    
    T pop() {
        T value = data[head];
        head = (head + 1) % N;
        return value;
    }
private:
    T data[N];
    size_t head = 0;
    size_t tail = 0;
};