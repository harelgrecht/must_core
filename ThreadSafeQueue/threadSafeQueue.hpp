#pragma once

#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class threadSafeQueue {
    public:
        threadSafeQueue(size_t queueCapacity);
        ~threadSafeQueue() = default;
        
        threadSafeQueue(const threadSafeQueue&) = delete;
        threadSafeQueue& operator=(const threadSafeQueue&) = delete;


        void enqueue(T value);
        T dequeue();
        bool isEmpty();
        bool isFull();
        size_t queueSize();

    private:
        std::queue<T> queue;
        std::mutex mutex;
        std::condition_variable conditionVariable;
        size_t queueCapacity;
};

template <typename T>
threadSafeQueue<T>::threadSafeQueue(size_t Capacity) : queueCapacity(Capacity) { }

template <typename T>
bool threadSafeQueue<T>::isEmpty() {
    return queue.empty();
}

template <typename T>
bool threadSafeQueue<T>::isFull() {
    return queue.size() >= queueCapacity;
}

template <typename T>
size_t threadSafeQueue<T>::queueSize() {
    return queue.size();
}

template <typename T>
void threadSafeQueue<T>::enqueue(T value) {
    std::unique_lock<std::mutex> lock(mutex);
    conditionVariable.wait(lock,[this]() { return !isFull(); });
    queue.push(std::move(value));
    conditionVariable.notify_one();
}

template <typename T>
T threadSafeQueue<T>::dequeue() {
    std::unique_lock<std::mutex> lock(mutex);
    conditionVariable.wait(lock,[this]() { return !isEmpty(); });
    T item = queue.pop();
    conditionVariable.notify_one();
    return item;
}
