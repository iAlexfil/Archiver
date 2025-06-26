#pragma once

#include <algorithm>
#include <vector>

template <typename T>
class PriorityQueue {
public:
    explicit PriorityQueue(T null_element);

    bool Empty() const;

    void SiftUp(std::size_t vertex);

    void SiftDown(std::size_t vertex);

    void Push(T value);

    void Pop();

    T Top() const;

    std::size_t Size() const;

private:
    std::vector<T> heap_;
    std::size_t size_;
};
