#pragma once

#include <iostream>
#include <vector>
#include "../ThreadSafeQueue/threadSafeQueue.hpp"

constexpr size_t UDP_HEADER_SIZE = 8;

template <typename T>
class TunnelTransmitterProcess {
    public:
        void tunnelTransmitterProcess();

    private:
        void getPayload(threadSafeQueue<T>& toSendTunnelQueue);
        void mainProcess(threadSafeQueue<T>& sendQueue);
        T rawPacketBuffer_;
        T payloadBuffer_;

};

template <typename T>
void TunnelTransmitterProcess<T>::getPayload(threadSafeQueue<T>& toSendTunnelQueue) {
    rawPacketBuffer_ = toSendTunnelQueue.dequeue();
    if (rawPacketBuffer_.size() > UDP_HEADER_SIZE) {
        payloadBuffer_.assign(rawPacketBuffer_.begin() + UDP_HEADER_SIZE, rawPacketBuffer_.end());
    }
}

template <typename T>
void TunnelTransmitterProcess<T>::mainProcess(threadSafeQueue<T>& sendQueue) {
    /*
        Perform any necessary transformations on payloadBuffer_, then pass to the next queue.
        Data transformation or manipulation code goes here...
   */
    sendQueue.enqueue(payloadBuffer_);
}
