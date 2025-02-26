#pragma once

#include <string>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include "../ThreadSafeQueue/ThreadSafeQueue.hpp"
#include "../NetworkManager/NetworkManager.hpp"

// Define a struct to encapsulate these variables
typedef struct {
    uint8_t PROTOCOL_TYPE_INDEX;
    uint16_t FLAGS_FRAGMENT;
    uint8_t DEFAULT_TOS;      // Type of Service
    uint16_t PACKET_CHECKSUM;   // UDP checksum is optional for IPv4
    int INVALID_SOCKET;
    uint16_t INITIAL_IP_CHECKSUM;
} SendConfig;

template <typename T>
class PacketSender {
public:
    PacketSender(threadSafeQueue<T>& queue);
    ~PacketSender();

    void runSender();

private:
    int packetID_ = 0;
    T payloadBuffer_;
    T packetBuffer_;
    SendConfig senderConfig_;
    threadSafeQueue<T> toTunnelQueue_;
    EthDevice ethDevice_;
    
    constexpr int ICD_INDEX = 4;
    constexpr int CONVENTION_MASK = 64;
    constexpr int CONVENTION_OFFSET = 6;

    void sendPacket();
    void assemblePacket();
    void createHeader(struct udphdr* udpHeader, uint16_t payloadSize);
    void createIpHeader(struct iphdr* ipHeader, uint16_t totalPacketSize);
    uint16_t computeChecksum(T buf, int len);
    EthDevice extract_device(T buffer);
    void loadConfig()
};

template <typename T>
PacketSender<T>::PacketSender(threadSafeQueue<T>& queue) : toTunnelQueue_(queue) {
    loadConfig();
}

template <typename T>
void PacketSender<T>::runSender() {
    while(true) {
        payloadBuffer_ = inputQueue.dequeue();
        ethDevice_ = extract_device(payloadBuffer_);
        assemblePacket();
        sendPacket();
    }
}

template <typename T>
void PacketSender<T>::loadConfig() {
    std::ifstream configFile("sender_setting.json");
    if (!configFile.is_open()) {
        throw std::runtime_error("Failed to open configuration file: " + configFilePath);
    }
    json jsonConfig;
    configFile >> jsonConfig;

    // Extract values from the JSON object
    senderConfig_.PACKET_HEADER_SIZE_ = jsonConfig.at("HEADER_SIZE").get<size_t>();
    senderConfig_.IPV4_HEADER_SIZE_ = jsonConfig.at("IPV4_HEADER_SIZE").get<size_t>();
    senderConfig_.IP_VERSION_ = jsonConfig.at("IP_VERSION").get<uint8_t>();
    senderConfig_.IHL_ = jsonConfig.at("IHL").get<uint8_t>();
    senderConfig_.DEFAULT_TTL_ = jsonConfig.at("DEFAULT_TTL").get<uint8_t>();
    senderConfig_.PROTOCOL_TYPE_INDEX_ = jsonConfig.at("PROTOCOL_TYPE_INDEX").get<uint8_t>();
    senderConfig_.FLAGS_FRAGMENT_ = jsonConfig.at("FLAGS_FRAGMENT").get<uint16_t>();
    senderConfig_.DEFAULT_TOS_ = jsonConfig.at("DEFAULT_TOS").get<uint8_t>();
    senderConfig_.PACKET_CHECKSUM_ = jsonConfig.at("CHECKSUM").get<uint16_t>();
    senderConfig_.INVALID_SOCKET_ = jsonConfig.at("INVALID_SOCKET").get<int>();
    senderConfig_.INITIAL_IP_CHECKSUM_ = jsonConfig.at("INITIAL_IP_CHECKSUM").get<uint16_t>();
}

template <typename T>
void PacketSender<T>::createHeader(struct udphdr* udpHeader, uint16_t payloadSize) {
    udpHeader->source = htons(ethDevice_.getSourcePort());
    udpHeader->dest = htons(ethDevice_.getDestPort());
    udpHeader->len = htons(UDP_HEADER_SIZE + payloadSize);
    udpHeader->check = CHECKSUM;
}

template <typename T>
void PacketSender<T>::createIpHeader(struct iphdr* ipHeader, uint16_t totalPacketSize) {
    ipHeader->ihl = IHL;
    ipHeader->version = IP_VERSION;
    ipHeader->tos = DEFAULT_TOS;
    ipHeader->tot_len = htons(totalPacketSize);
    ipHeader->id = htons(packetID++);
    ipHeader->frag_off = FLAGS_FRAGMENT;
    ipHeader->ttl = DEFAULT_TTL;
    ipHeader->protocol = PROTOCOL_UDP;
    ipHeader->check = INITIAL_IP_CHECKSUM;
    ipHeader->saddr = inet_addr(ethDevice_.getSourceIpAddress().c_str());
    ipHeader->daddr = inet_addr(ethDevice_.getDestIpAddress().c_str());

    // Compute checksum for the IP header
    ipHeader->check = computeChecksum(std::span<const uint8_t>(
        reinterpret_cast<const uint8_t*>(ipHeader)), IPV4_HEADER_SIZE);
}

template <typename T>
void PacketSender<T>::assemblePacket() {
    uint16_t payloadSize = payloadBuffer_.size();
    uint16_t totalPacketSize = IPV4_HEADER_SIZE + UDP_HEADER_SIZE + payloadSize;
    packetBuffer_.resize(totalPacketSize);

    struct iphdr* ipHeader = reinterpret_cast<struct iphdr*>(packetBuffer_.data());
    struct udphdr* udpHeader = reinterpret_cast<struct udphdr*>(packetBuffer_.data() + IPV4_HEADER_SIZE);
    uint8_t* packetPayload = packetBuffer_.data() + IPV4_HEADER_SIZE + UDP_HEADER_SIZE;
    
    createIpHeader(ipHeader, ethDevice_, totalPacketSize);
    createUdpHeader(udpHeader, ethDevice_, payloadSize);
    std::memcpy(packetPayload, payloadBuffer_.data(), payloadSize);
}

template <typename T>
void PacketSender<T>::sendPacket() {
    if (packetBuffer_.empty()) {
        Logger.getInstance().Log(LogLevel.ERROR, "[PacketSend] Error: No packet assembled. Call assemblePacket() first.");
        return;
    }

    struct sockaddr_in destAddr{};
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(ethDevice_.getDestPort());
    destAddr.sin_addr.s_addr = inet_addr(ethDevice_.getDestIpAddress().c_str());

    if (sendto(ethDevice_.getSocketFd(), packetBuffer_.data(), packetBuffer_.size(), 0,
               (struct sockaddr*)&destAddr, sizeof(destAddr)) < 0) {
        perror("[PacketSend] Failed to send packet");
    }
}

template <typename T>
EthDevice PacketSender<T>::extract_device(T buffer) {
    int device_index = (buffer[ICD_INDEX] & CONVENTION_MASK) >> CONVENTION_OFFSET;
    return NetworkManager.getDeviceByRole(EthDevice::Role(device_index));
}

uint16_t computeChecksum(void* buf, int len) {
    uint16_t* data = reinterpret_cast<uint16_t*>(buf);
    uint32_t sum = 0;
    while (len > 1) {
        sum += *data++;
        len -= 2;
    }
    if (len)
        sum += *(uint8_t*)data;
    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);
    return static_cast<uint16_t>(~sum);
}

//This is the ICD:
/*
struct ICD_BIT_BUILD icd;
    icd.EOP = (ICD & 128) >> 7;
    icd.IS_COMP = (ICD & 64) >> 6;
    icd.CONV = (ICD & 48) >> 4;
    icd.SEQ_NUM = (ICD & 12) >> 2;
    icd.MSG_ID = (ICD & 3);
    return icd;

*/