#pragma once

#include <iostream>
#include <vector>
#include "../ThreadSafeQueue/threadSafeQueue.hpp"

constexpr size_t HEADER_SIZE = 8;

template <typename T>
class TunnelTransmitterProcess {
    public:
        TunnelTransmitterProcess(threadSafeQueue<T>& toSendProcessQueue, threadSafeQueue<T>& sendQueue);

    private:
        T rawPacketBuffer_;
        T payloadBuffer_;
        
        threadSafeQueue<T> toSendProcessQueue;
        threadSafeQueue<T> sendQueue;

        void getPayload();
        void mainProcess();

};

template <typename T>
TunnelTransmitterProcess<T>::TunnelTransmitterProcess(threadSafeQueue<T>& toSendProcessQueue, threadSafeQueue<T>& sendQueue) 
    : toSendProcessQueue(toSendProcessQueue), sendQueue(sendQueue) { }

template <typename T>
void TunnelTransmitterProcess<T>::getPayload() {
    rawPacketBuffer_ = toSendProcessQueue.dequeue();
    if (rawPacketBuffer_.size() > HEADER_SIZE) {
        payloadBuffer_.assign(rawPacketBuffer_.begin() + HEADER_SIZE, rawPacketBuffer_.end());
    }
}

template <typename T>
void TunnelTransmitterProcess<T>::mainProcess() {
    /*
        Perform any necessary transformations on payloadBuffer_, then pass to the next queue.
        Data transformation or manipulation code goes here...
   */
    sendQueue.enqueue(payloadBuffer_);
}
