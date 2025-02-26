#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <iostream>
#include <net/if.h>  // for IFF_UP
#include <iostream>
#include <chrono>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/ip_icmp.h>

extern "C" {
    #include <netlink/netlink.h>
    #include <netlink/msg.h>
    #include <netlink/route/link.h>
    #include <netlink/route/addr.h>
    #include <netlink/route/rtnl.h>
    #include <netlink/route/route.h>
    #include <netlink/route/nexthop.h>
}
#include "SelfSearchConfig.hpp" 
#include "LogHandler/logger.hpp"

class EthDevice {
    public:
        // Role for the device
        enum class Role { SOURCE1, SOURCE2, SOURCE3, TUNNEL };

        // Constructors
        EthDevice();
        explicit EthDevice(const std::string& name);

        // Destructor
        EthDevice::~EthDevice();
        // Normal mode: apply the user-supplied settings
        void applySettings();

        // Check at hardware level if the cable is connected
        bool isCableConnected() const;

        // Get the device name (e.g., "eth1")
        const std::string& getName() const;

        // Get the device role
        const Role& getRole() const;

        // In self-search mode: update the device configuration from a SelfSearchConfig object
        void applySelfSearchConfig(const SelfSearchConfig& config);

        // JSON deserialization for normal (user) settings from user_setting.json
        friend void from_json(const nlohmann::json& j, EthDevice& ethDevice);

        // Pinging to given IP address.
        bool pingIP(const std::string& ip);

    private:
        std::string name;
        std::string ipAddress;
        std::string destIpAddress;
        std::string defaultGateway;
        std::string subnetMask;
        std::string remoteIp;
        std::string remoteIpDestination;
        Role role;

        struct nl_sock* sock_;
        void initSocket();
        void closeSocket();

        uint16_t compute_checksum(void* buf, int len);

        // Helper methods for applying settings
        void setDeviceFlags();
        void setMTU();
        void addVirtualIpIfNeeded();
        void setSelfIP();
        void setDefaultGateway();
       
        // Helper: convert a string to Role enum
        static Role parseRole(const std::string& roleStr);
};

#endif // ETHDEVICE_HPP
