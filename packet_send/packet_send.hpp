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
#include "../ThreadSafeQueue/threadSafeQueue.hpp"
#include "../NetworkManager/NetworkManager.hpp"

// Packet = UDP Header + IP Header + Payload
constexpr size_t UDP_HEADER_SIZE = 8;
constexpr size_t IPV4_HEADER_SIZE = 20;
constexpr uint8_t IP_VERSION = 4;
constexpr uint8_t IHL = 5; // Internet Header Length (5 * 4 bytes = 20 bytes)
constexpr uint8_t DEFAULT_TTL = 64;
constexpr uint8_t PROTOCOL_UDP = 17;
constexpr uint16_t FLAGS_FRAGMENT = 0;
constexpr uint8_t DEFAULT_TOS = 0;    // Type of Service
constexpr uint16_t UDP_CHECKSUM = 0;  // UDP checksum is optional for IPv4
constexpr int INVALID_SOCKET = -1;
constexpr uint16_t INITIAL_IP_CHECKSUM = 0;

template <typename T>
class PacketSender {
public:
    void sendPacket(const EthDevice& destDevice);
    void assemblePacket(threadSafeQueue<T>& inputQueue, const EthDevice& destDevice);

private:
    void createUdpHeader(struct udphdr* udpHeader, const EthDevice& destDevice, uint16_t payloadSize);
    void createIpHeader(struct iphdr* ipHeader, const EthDevice& destDevice, uint16_t totalPacketSize);
    uint16_t computeChecksum(std::span<const uint8_t> buffer);

    int packetID = 0;
    T payloadBuffer;
    T packetBuffer;
};

template <typename T>
void PacketSender<T>::createUdpHeader(struct udphdr* udpHeader, const EthDevice& destDevice, uint16_t payloadSize) {
    udpHeader->source = htons(destDevice.getSourcePort());
    udpHeader->dest = htons(destDevice.getDestPort());
    udpHeader->len = htons(UDP_HEADER_SIZE + payloadSize);
    udpHeader->check = UDP_CHECKSUM;
}

template <typename T>
void PacketSender<T>::createIpHeader(struct iphdr* ipHeader, const EthDevice& destDevice, uint16_t totalPacketSize) {
    ipHeader->ihl = IHL;
    ipHeader->version = IP_VERSION;
    ipHeader->tos = DEFAULT_TOS;
    ipHeader->tot_len = htons(totalPacketSize);
    ipHeader->id = htons(packetID++);
    ipHeader->frag_off = FLAGS_FRAGMENT;
    ipHeader->ttl = DEFAULT_TTL;
    ipHeader->protocol = PROTOCOL_UDP;
    ipHeader->check = INITIAL_IP_CHECKSUM;
    ipHeader->saddr = inet_addr(destDevice.getSourceIpAddress().c_str());
    ipHeader->daddr = inet_addr(destDevice.getDestIpAddress().c_str());

    // Compute checksum for the IP header
    ipHeader->check = computeChecksum(std::span<const uint8_t>(
        reinterpret_cast<const uint8_t*>(ipHeader), IPV4_HEADER_SIZE));
}

template <typename T>
void PacketSender<T>::assemblePacket(threadSafeQueue<T>& inputQueue, const EthDevice& destDevice) {
    payloadBuffer = inputQueue.dequeue();
    uint16_t payloadSize = payloadBuffer.size();
    uint16_t totalPacketSize = IPV4_HEADER_SIZE + UDP_HEADER_SIZE + payloadSize;
    packetBuffer.resize(totalPacketSize);
    struct iphdr* ipHeader = reinterpret_cast<struct iphdr*>(packetBuffer.data());
    struct udphdr* udpHeader = reinterpret_cast<struct udphdr*>(packetBuffer.data() + IPV4_HEADER_SIZE);
    uint8_t* packetPayload = packetBuffer.data() + IPV4_HEADER_SIZE + UDP_HEADER_SIZE;
    createIpHeader(ipHeader, destDevice, totalPacketSize);
    createUdpHeader(udpHeader, destDevice, payloadSize);
    std::memcpy(packetPayload, payloadBuffer.data(), payloadSize);
}

template <typename T>
void PacketSender<T>::sendPacket(const EthDevice& destDevice) {
    if (packetBuffer.empty()) {
        std::cerr << "[PacketSend] Error: No packet assembled. Call assemblePacket() first." << std::endl;
        return;
    }

    struct sockaddr_in destAddr{};
    destAddr.sin_family = AF_INET;
    destAddr.sin_port = htons(destDevice.getDestPort());
    destAddr.sin_addr.s_addr = inet_addr(destDevice.getDestIpAddress().c_str());

    if (sendto(destDevice.getSocketFd(), packetBuffer.data(), packetBuffer.size(), 0,
               (struct sockaddr*)&destAddr, sizeof(destAddr)) < 0) {
        perror("[PacketSend] Failed to send packet");
    }
}

uint16_t PacketSender<uint8_t>::computeChecksum(std::span<const uint8_t> buffer) {
    uint32_t sum = 0;

    // Process the data as 16-bit words
    std::span<const uint16_t> words(reinterpret_cast<const uint16_t*>(buffer.data()), buffer.size() / 2);
    for (uint16_t word : words) {
        sum += word;
    }

    // Handle the odd byte if present
    if (buffer.size() % 2) {
        sum += static_cast<uint16_t>(buffer.back() << 8);
    }

    // Fold 32-bit sum into 16-bit
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return static_cast<uint16_t>(~sum);
}

