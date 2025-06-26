#include "PriorityQueue.h"

#include <iostream>

template <typename T>
PriorityQueue<T>::PriorityQueue(T null_element) {
    size_ = 0;
    heap_.push_back(null_element);
}

template <typename T>
bool PriorityQueue<T>::Empty() const {
    return size_ == 0;
}

template <typename T>
void PriorityQueue<T>::SiftUp(std::size_t vertex) {
    while (vertex != 1) {
        std::size_t parent = vertex / 2;
        if (heap_[vertex] < heap_[parent]) {
            std::swap(heap_[parent], heap_[vertex]);
            vertex = parent;
        } else {
            break;
        }
    }
}

template <typename T>
void PriorityQueue<T>::SiftDown(std::size_t vertex) {
    std::size_t lson = vertex * 2;
    std::size_t rson = vertex * 2 + 1;

    std::size_t to = (rson > size_ || heap_[lson] < heap_[rson] ? lson : rson);
    if (to > size_ || heap_[vertex] <= heap_[to]) {
        return;
    }

    std::swap(heap_[to], heap_[vertex]);
    SiftDown(to);
}

template <typename T>
void PriorityQueue<T>::Push(T value) {
    ++size_;
    heap_.push_back(value);
    SiftUp(size_);
}

template <typename T>
void PriorityQueue<T>::Pop() {
    std::swap(heap_[1], heap_[size_]);
    --size_;
    heap_.pop_back();
    SiftDown(1);
}

template <typename T>
T PriorityQueue<T>::Top() const {
    return heap_[1];
}

template <typename T>
std::size_t PriorityQueue<T>::Size() const {
    return size_;
}