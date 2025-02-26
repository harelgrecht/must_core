#pragma once

#include <iostream>
#include <vector>
#include "../ThreadSafeQueue/ThreadSafeQueue.hpp"

constexpr size_t HEADER_SIZE = 8;

template <typename T>
class TunnelTransmitterProcess {
    public:
        TunnelTransmitterProcess(ThreadSafeQueue<T>& toSendProcessQueue_, ThreadSafeQueue<T>& sendQueue_);

    private:
        T rawPacketBuffer_;
        T payloadBuffer_;
        
        ThreadSafeQueue<T> toSendProcessQueue_;
        ThreadSafeQueue<T> sendQueue_;

        void getPayload();
        void mainProcess();

};

template <typename T>
TunnelTransmitterProcess<T>::TunnelTransmitterProcess(ThreadSafeQueue<T>& toSendProcessQueue_, ThreadSafeQueue<T>& sendQueue_) 
    : toSendProcessQueue_(toSendProcessQueue_), sendQueue_(sendQueue_) { }

template <typename T>
void TunnelTransmitterProcess<T>::getPayload() {
    rawPacketBuffer_ = toSendProcessQueue_.dequeue();
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
    sendQueue_.enqueue(payloadBuffer_);
}
