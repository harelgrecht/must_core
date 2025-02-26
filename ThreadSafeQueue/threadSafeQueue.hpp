#pragma once

#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class ThreadSafeQueue {
    public:
        ThreadSafeQueue(size_t queueCapacity);
        ~ThreadSafeQueue() = default;
        
        ThreadSafeQueue(const ThreadSafeQueue&) = delete;
        ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;


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
ThreadSafeQueue<T>::ThreadSafeQueue(size_t Capacity) : queueCapacity(Capacity) { }

template <typename T>
bool ThreadSafeQueue<T>::isEmpty() {
    return queue.empty();
}

template <typename T>
bool ThreadSafeQueue<T>::isFull() {
    return queue.size() >= queueCapacity;
}

template <typename T>
size_t ThreadSafeQueue<T>::queueSize() {
    return queue.size();
}

template <typename T>
void ThreadSafeQueue<T>::enqueue(T value) {
    std::unique_lock<std::mutex> lock(mutex);
    conditionVariable.wait(lock,[this]() { return !isFull(); });
    queue.push(std::move(value));
    conditionVariable.notify_one();
}

template <typename T>
T ThreadSafeQueue<T>::dequeue() {
    std::unique_lock<std::mutex> lock(mutex);
    conditionVariable.wait(lock,[this]() { return !isEmpty(); });
    T item = queue.pop();
    conditionVariable.notify_one();
    return item;
}
