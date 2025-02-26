#pragma once

#include <iostream>
#include <vector>
#include "../ThreadSafeQueue/threadSafeQueue.hpp"

constexpr size_t UDP_HEADER_SIZE = 8;


template <typename T>
class TunnelReceiverProcess {
    public:
        void tunnelReceiveHandler();

    private:
        void getPayload(threadSafeQueue<T>& fromTunnelQueue);
        void mainProcess();
        T rawPacketBuffer;
        T payloadBuffer;

};

template <typename T>
void TunnelReceiverProcess<T>::getPayload(threadSafeQueue<T>& fromTunnelQueue) {
    rawPacketBuffer = fromTunnelQueue.dequeue();
    if (rawPacketBuffer.size() > UDP_HEADER_SIZE) {
        payloadBuffer.assign(rawPacketBuffer.begin() + UDP_HEADER_SIZE, rawPacketBuffer.end());
    }

}

template <typename T>
void TunnelReceiverProcess<T>::mainProcess() {
    //Place holder implementation will be different in each must project
    // can be a compression or decompression or what ever the project needs
    return;
}

