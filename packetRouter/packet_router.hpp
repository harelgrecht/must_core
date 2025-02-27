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
#include "../NetworkManager/NetworkManager.hpp"
#include "../ThreadSafeQueue/threadSafeQueue.hpp"

struct PacketInfo {
    std::string srcIP;
    std::string destIP;
    std::vector<uint8_t> payload;
};

template <typename T>
class PacketRouter {
    public:
        PacketRouter(ThreadSafeQueue<T>& receiveQueue, ThreadSafeQueue<T>& processQueue, ThreadSafeQueue<T>& fromTunnelQueue, ThreadSafeQueue<T> guiQueue);
        
        void packetRouteHandler();

    private:
        int IPV4_HEADER_SIZE_;
        int PACKET_HEADER_SIZE_;
        int MIN_PACKET_SIZE_;
        int SRC_IP_OFFSET_;
        int DEST_IP_OFFSET_;
        int SRC_PORT_OFFSET_;
        int DEST_PORT_OFFSET_;
        int PAYLOAD_OFFSET_;
        uint16_t GUI_PAYLOAD_ICD_;

        EthDevice tunnelDevice_; // Member variable to store the tunnel device
        ThreadSafeQueue<T> processQueue;
        ThreadSafeQueue<T> receiveQueue;
        ThreadSafeQueue<T> fromTunnelQueue;
        ThreadSafeQueue<T> guiQueue;


        bool isFromTunnel(const std::string& srcIP, const std::string& destIP);
        bool isFromGui(PacketInfo& udpPkt);
        PacketInfo parsePacket(const T& packet);
};

template <typename T>
PacketRouter<T>::PacketRouter(ThreadSafeQueue<T>& receiveQueue, ThreadSafeQueue<T>& processQueue, ThreadSafeQueue<T>& fromTunnelQueue, ThreadSafeQueue<T> guiQueue) 
    : receiveQueue(receiveQueue), processQueue(processQueue), fromTunnelQueue(fromTunnelQueue) {
    std::ifstream routeConfig("route_settings.json");
    if (!routeConfig.is_open()) {
        throw std::runtime_error("Failed to open user settings file: " + std::to_string(route_settings.json));
    }
    nlohmann::json jsonConfig;
    routeConfig >> jsonConfig;
    
    // Extract values from the JSON object
        IPV4_HEADER_SIZE_ = jsonConfig.at("IPV4_HEADER_SIZE").get<int>();
        PACKET_HEADER_SIZE_ = jsonConfig.at("PACKET_HEADER_SIZE").get<int>();
        MIN_PACKET_SIZE_ = jsonConfig.at("MIN_PACKET_SIZE").get<int>();
        SRC_IP_OFFSET_ = jsonConfig.at("SRC_IP_OFFSET").get<int>();
        DEST_IP_OFFSET_ = jsonConfig.at("DEST_IP_OFFSET").get<int>();
        SRC_PORT_OFFSET_ = jsonConfig.at("SRC_PORT_OFFSET").get<int>();
        DEST_PORT_OFFSET_ = jsonConfig.at("DEST_PORT_OFFSET").get<int>();
        PAYLOAD_OFFSET_ = jsonConfig.at("PAYLOAD_OFFSET").get<int>();
        GUI_PAYLOAD_ICD_ = jsonConfig.at("guiPayloadICD").get<uint16_t>();

        tunnelDevice_ = NetworkManager::getDeviceByRole(EthDevice::Role::TUNNEL);
}

template <typename T>
void PacketRouter<T>::packetRouteHandler() {
    T rawPacket;    while(true) {
        T rawPacket = receiveQueue.dequeue();
        PacketInfo packet = parsePacket(rawPacket);
        
        if (isFromTunnel(packet.srcIP, packet.destIP)) { 
            if(isFromGui(packet)) {
                guiQueue.enqueue(rawPacket);
            } else
                fromTunnelQueue.enqueue(rawPacket);
        } else {
            processQueue.enqueue(rawPacket);
        }   
    } 
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

template <typename T>
bool PacketRouter<T>::isFromTunnel(const std::string& srcIP, const std::string& destIP) {
    return (srcIP == tunnelDevice_.getDestIp()) && (destIP == tunnelDevice_.getSelfIp());
}

template <typename T>
bool PacketRouter<T>::isFromGui(PacketInfo& packet) {
    return (packet.payload[0] == GUI_PAYLOAD_ICD_) && (packet.payload[packet.payload.size() - 2] == GUI_PAYLOAD_ICD_);
}

template <typename T>
PacketInfo PacketRouter<T>::parsePacket(const T& packet) {
    PacketInfo parsedPacket;
    char ipStr[INET_ADDRSTRLEN];

    uint32_t srcIp = ntohl(*((uint32_t*)(packet.data() + SRC_IP_OFFSET_)));
    inet_ntop(AF_INET, &srcIp, ipStr, sizeof(ipStr));
    parsedPacket.srcIP = std::string(ipStr);

    uint32_t destIp = ntohl(*((uint32_t*)(packet.data() + DEST_IP_OFFSET_)));
    inet_ntop(AF_INET, &destIp, ipStr, sizeof(ipStr));
    parsedPacket.destIP = std::string(ipStr);

    if (packet.size() > PAYLOAD_OFFSET_) {
        parsedPacket.payload.assign(packet.begin() + PAYLOAD_OFFSET_, packet.end());
    }

    return parsedPacket;
}
