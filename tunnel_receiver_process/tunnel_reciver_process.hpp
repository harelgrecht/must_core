#pragma once

#include <iostream>
#include <vector>
#include "../ThreadSafeQueue/threadSafeQueue.hpp"

constexpr size_t HEADER_SIZE = 8;


template <typename T>
class TunnelReceiverProcess {
    public:
        void tunnelReceiveHandler();

    private:
        T rawPacketBuffer_;
        T payloadBuffer_;

        void getPayload(threadSafeQueue<T>& fromTunnelQueue);
        void mainProcess(threadSafeQueue<T>& sendQueue);

};

template <typename T>
void TunnelReceiverProcess<T>::getPayload(threadSafeQueue<T>& fromTunnelQueue) {
    rawPacketBuffer_ = fromTunnelQueue.dequeue();
    if (rawPacketBuffer.size() > HEADER_SIZE) {
        payloadBuffer.assign(rawPacketBuffer.begin() + HEADER_SIZE, rawPacketBuffer.end());
    }

}

template <typename T>
void TunnelReceiverProcess<T>::mainProcess(threadSafeQueue<T>& sendQueue) {
    /*
        Perform any necessary transformations on payloadBuffer_, then pass to the next queue.
        Data transformation or manipulation code goes here...
    */
   sendQueue.enqueue(payloadBuffer_);
}

