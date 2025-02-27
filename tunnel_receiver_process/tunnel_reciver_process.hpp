#pragma once

#include <iostream>
#include <vector>
#include "../ThreadSafeQueue/threadSafeQueue.hpp"

constexpr size_t RECEIVER_HEAER_SIZE = 8;

template <typename T>
class TunnelReceiverProcess {
    public:

        TunnelReceiverProcess(ThreadSafeQueue<T>& sendQueue, ThreadSafeQueue<T>& fromTunnelQueue);

        
    private:
        T rawPacketBuffer_;
        T payloadBuffer_;
        
        ThreadSafeQueue<T> sendQueue_;
        ThreadSafeQueue<T> fromTunnelQueue_;

        void getPayload();
        void mainProcess();

};

/*
    Note:
        You can add here processing of the must icd as known "*#M{info}"
*/

template <typename T>
TunnelReceiverProcess<T>::TunnelReceiverProcess(ThreadSafeQueue<T>& sendQueue, ThreadSafeQueue<T>& fromTunnelQueue) 
    : sendQueue_(sendQueue), fromTunnelQueue_(fromTunnelQueue) { }
template <typename T>
void TunnelReceiverProcess<T>::getPayload() {
    rawPacketBuffer_ = fromTunnelQueue_.dequeue();
    if (rawPacketBuffer_.size() > RECEIVER_HEAER_SIZE) {
        payloadBuffer_.assign(rawPacketBuffer_.begin() + RECEIVER_HEAER_SIZE, rawPacketBuffer_.end());
    }

}

template <typename T>
void TunnelReceiverProcess<T>::mainProcess() {
    /*
        Perform any necessary transformations on payloadBuffer_, then pass to the next queue.
        Data transformation or manipulation code goes here...
    */
   sendQueue_.enqueue(payloadBuffer_);
}
