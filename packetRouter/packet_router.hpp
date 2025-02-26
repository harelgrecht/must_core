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
#include "ThreadSafeQueue/threadSafeQueue.hpp"

struct udpPacket {
    std::string srcIP;
    std::string destIP;
    std::vector<uint8_t> payload;
};

template <typename T>
class PacketRouter {
    public:
        PacketRouter();
        
        void packetRouteHandler(threadSafeQueue<T>& receiveQueue, threadSafeQueue<T>& processQueue, threadSafeQueue<T>& fromTunnelQueue);

    private:
        int IPV4_HEADER_SIZE;
        int PACKET_HEADER_SIZE;
        int MIN_PACKET_SIZE;
        int SRC_IP_OFFSET;
        int DEST_IP_OFFSET;
        int SRC_PORT_OFFSET;
        int DEST_PORT_OFFSET;
        int PAYLOAD_OFFSET;
        uint16_t guiPayloadICD;

        bool isFromTunnel(const std::string& srcIP, const std::string& destIP);
        bool isFromGui(udpPacket& udpPkt);
        bool parseUdpPacket(const T& packet, udpPacket& udpPkt);
};

template <typename T>
PacketRouter<T>::PacketRouter() {
    std::ifstream routeConfig("route_settings.json");
    if (!routeConfig.is_open()) {
        throw std::runtime_error("Failed to open user settings file: " + userSettingsFile);
    }
    nlohmann::json jsonConfig;
    jsonConfig >> routeConfig;

    // Extract values from the JSON object
        int IPV4_HEADER_SIZE = j.at("IPV4_HEADER_SIZE").get<int>();
        int UDP_HEADER_SIZE = j.at("PACKET_HEADER_SIZE").get<int>();
        int MIN_PACKET_SIZE = j.at("MIN_PACKET_SIZE").get<int>();
        int SRC_IP_OFFSET = j.at("SRC_IP_OFFSET").get<int>();
        int DEST_IP_OFFSET = j.at("DEST_IP_OFFSET").get<int>();
        int SRC_PORT_OFFSET = j.at("SRC_PORT_OFFSET").get<int>();
        int DEST_PORT_OFFSET = j.at("DEST_PORT_OFFSET").get<int>();
        int PAYLOAD_OFFSET = j.at("PAYLOAD_OFFSET").get<int>();
        uint16_t guiPayloadICD = j.at("guiPayloadICD").get<uint16_t>();
}

template <typename T>
void PacketRouter<T>::packetRouteHandler(threadSafeQueue<T>& receiveQueue, threadSafeQueue<T>& toSendQueue, threadSafeQueue<T>& fromTunnelQueue) {
    T rawPacket;
    while(true) {
        T rawPacket = receiveQueue.dequeue();
        udpPacket udpPkt = parseUdpPacket(rawPacket, udpPkt)
        if(!) continue;
        
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


