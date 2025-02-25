#pragma once

#include <iostream>
#include <vector>
#include <cstdint>

constexpr int PACKET_SIZE = 360;


class PacketAuthenticator {
public:
    static bool authenticatePacket(const std::vector<uint8_t>& packet);
    
private:
    static bool checkLegnth(const std::vector<uint8_t>& packet);
    static bool checkOpCode(const std::vector<uint8_t>& packet);
};