#pragma once

#include <iostream>
#include <vector>
#include "../ThreadSafeQueue/threadSafeQueue.hpp"

constexpr size_t RECEIVER_HEAER_SIZE = 8;

template <typename T>
class TunnelReceiverProcess {
    public:

        TunnelReceiverProcess(threadSafeQueue<T>& sendQueue, threadSafeQueue<T>& fromTunnelQueue);

    private:
        T rawPacketBuffer_;
        T payloadBuffer_;
        
        threadSafeQueue<T> sendQueue;
        threadSafeQueue<T> fromTunnelQueue;

        void getPayload();
        void mainProcess();

};

/*
    Note:
        You can add here processing of the must icd as known "*#M{info}"
*/

template <typename T>
TunnelReceiverProcess<T>::TunnelReceiverProcess(threadSafeQueue<T>& sendQueue, threadSafeQueue<T>& fromTunnelQueue) 
    : sendQueue(sendQueue), fromTunnelQueue(fromTunnelQueue) { }
template <typename T>
void TunnelReceiverProcess<T>::getPayload() {
    rawPacketBuffer_ = fromTunnelQueue.dequeue();
    if (rawPacketBuffer.size() > RECEIVER_HEAER_SIZE) {
        payloadBuffer.assign(rawPacketBuffer.begin() + RECEIVER_HEAER_SIZE, rawPacketBuffer.end());
    }

}

template <typename T>
void TunnelReceiverProcess<T>::mainProcess() {
    /*
        Perform any necessary transformations on payloadBuffer_, then pass to the next queue.
        Data transformation or manipulation code goes here...
    */
   sendQueue.enqueue(payloadBuffer_);
}
