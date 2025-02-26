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
#include "NetworkManager/NetworkManager.hpp"
#include "ThreadSafeQueue/threadSafeQueue.hpp"

struct PacketInfo {
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
        uint16_t GUI_PAYLOAD_ICD;

        EthDevice tunnelDevice; // Member variable to store the tunnel device

        bool isFromTunnel(const std::string& srcIP, const std::string& destIP);
        bool isFromGui(PacketInfo& udpPkt);
        PacketInfo PacketRouter<T>::parsePacket(const T& packet);
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
        IPV4_HEADER_SIZE = j.at("IPV4_HEADER_SIZE").get<int>();
        PACKET_HEADER_SIZE = j.at("PACKET_HEADER_SIZE").get<int>();
        MIN_PACKET_SIZE = j.at("MIN_PACKET_SIZE").get<int>();
        SRC_IP_OFFSET = j.at("SRC_IP_OFFSET").get<int>();
        DEST_IP_OFFSET = j.at("DEST_IP_OFFSET").get<int>();
        SRC_PORT_OFFSET = j.at("SRC_PORT_OFFSET").get<int>();
        DEST_PORT_OFFSET = j.at("DEST_PORT_OFFSET").get<int>();
        PAYLOAD_OFFSET = j.at("PAYLOAD_OFFSET").get<int>();
        GUI_PAYLOAD_ICD = j.at("guiPayloadICD").get<uint16_t>();

        tunnelDevice = NetworkManager::getDeviceByRole(EthDevice::Role::TUNNEL);
}

template <typename T>
void PacketRouter<T>::packetRouteHandler(threadSafeQueue<T>& receiveQueue, threadSafeQueue<T>& toSendQueue, threadSafeQueue<T>& fromTunnelQueue) {
    T rawPacket;
    while(true) {
        T rawPacket = receiveQueue.dequeue();
        PacketInfo packet = parsePacket(rawPacket, udpPkt)
        if(!packet) continue;
        
        if (isFromTunnel(packet.srcIP, packet.destIP)) { 
            if(isFromGui(packet)) {
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
    return (srcIP == tunnelDevice.getDestIP()) && (destIP == tunnelDevice.getSelfIP());
}

template <typename T>
bool PacketRouter<T>::isFromGui(PacketInfo& packet) {
    return (packet.payload[0] == GUI_PAYLOAD_ICD) && (packet.payload[packet.payload.size() - 2] == GUI_PAYLOAD_ICD);
}

template <typename T>
PacketInfo PacketRouter<T>::parsePacket(const T& packet) {
    PacketInfo packet;
    char ipStr[INET_ADDRSTRLEN];

    uint32_t srcIp = ntohl(*((uint32_t*)(packet.data() + SRC_IP_OFFSET)));
    inet_ntop(AF_INET, &srcIp, ipStr, sizeof(ipStr));
    packet.srcIP = std::string(ipStr);

    uint32_t destIp = ntohl(*((uint32_t*)(packet.data() + DEST_IP_OFFSET)));
    inet_ntop(AF_INET, &destIp, ipStr, sizeof(ipStr));
    packet.destIP = std::string(ipStr);

    if (packet.size() > UDP_PAYLOAD_OFFSET) {
        packet.payload.assign(packet.begin() + PAYLOAD_OFFSET, packet.end());
    }

    return packet;
}


