#pragma once

#include <vector>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <arpa/inet.h> 
#include <string>
#include <utility> 
#include <thread>
#include <chrono>
#include "../ThreadSafeQueue/threadSafeQueue.hpp"

constexpr char airbornePayloadIP[] = "192.168.1.1";
constexpr char groundPayloadIP[] = "192.168.1.1";

constexpr int IPV4_HEADER_SIZE = 20;
constexpr int UDP_HEADER_SIZE = 8;
constexpr int MIN_PACKET_SIZE = IPV4_HEADER_SIZE + UDP_HEADER_SIZE;

constexpr int SRC_IP_OFFSET = 12;
constexpr int DEST_IP_OFFSET = 16;
constexpr int SRC_PORT_OFFSET = 20;
constexpr int DEST_PORT_OFFSET = 22;
constexpr int UDP_PAYLOAD_OFFSET = IPV4_HEADER_SIZE + UDP_HEADER_SIZE;
constexpr uint16_t guiPayloadICD = 0x6464;

struct udpPacket {
    std::string srcIP;
    std::string destIP;
    std::vector<uint8_t> payload;
};

template <typename T>
class PacketRouter {
    public:
        void packetRouteHandler(threadSafeQueue<T>& receiveQueue, threadSafeQueue<T>& processQueue, threadSafeQueue<T>& fromTunnelQueue);

    private:
        bool isFromTunnel(const std::string& srcIP, const std::string& destIP);
        bool isFromGui(udpPacket& udpPkt);
        bool parseUdpPacket(const T& packet, udpPacket& udpPkt);
};

template <typename T>
void PacketRouter<T>::packetRouteHandler(threadSafeQueue<T>& receiveQueue, threadSafeQueue<T>& toSendQueue, threadSafeQueue<T>& fromTunnelQueue) {
    T rawPacket;
    while(true) {
        T rawPacket = receiveQueue.dequeue();
        udpPacket udpPkt;
        if(!parseUdpPacket(rawPacket, udpPkt)) continue;
        
        if (isFromTunnel(udpPkt.srcIP, udpPkt.destIP)) { 
            fromTunnelQueue.enqueue(rawPacket);
            if(isFromGui(udpPkt)) {
                guiQueue.enqueue(rawPacket);
            } else
                fromTunnelQueue.enqueue(rawPacket);
        } else {
            toSendQueue.enqueue(rawPacket);
        }   
    } 
    std::this_thread::sleep_for(THREAD_SLEEP_DURATION);
}

template <typename T>
bool PacketRouter<T>::isFromTunnel(const std::string& srcIP, const std::string& destIP) {
    if ((srcIP == airbornePayloadIP) && (destIP == groundPayloadIP)) {
        // from air to ground
        return true;
    }else if((srcIP == groundPayloadIP) && (destIP == airbornePayloadIP)) {
        // from ground to air
        return false;
    }
}

template <typename T>
bool PacketRouter<T>::isFromGui(udpPacket& udpPkt) {
    return (udpPkt.payload[0] == guiPayloadICD) && (udpPkt.payload[udpPkt.payload.size() - 2] == guiPayloadICD);
}

template <typename T>
bool PacketRouter<T>::parseUdpPacket(const T& packet, udpPacket& udpPkt) {
    char ipStr[INET_ADDRSTRLEN];

    uint32_t srcIp = ntohl(*((uint32_t*)(packet.data() + SRC_IP_OFFSET)));
    inet_ntop(AF_INET, &srcIp, ipStr, sizeof(ipStr));
    udpPkt.srcIP = std::string(ipStr);

    uint32_t destIp = ntohl(*((uint32_t*)(packet.data() + DEST_IP_OFFSET)));
    inet_ntop(AF_INET, &destIp, ipStr, sizeof(ipStr));
    udpPkt.destIP = std::string(ipStr);

    if (packet.size() > UDP_PAYLOAD_OFFSET) {
        udpPkt.payload.assign(packet.begin() + UDP_PAYLOAD_OFFSET, packet.end());
    }

    return true;
}


