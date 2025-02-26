#pragma once

#include <string>
#include "../third_party/nlohmann/json.hpp"
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
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdexcept>
#include <netinet/in.h>
#include <net/route.h>

#include "../SelfSearchConfig/SelfSearchConfig.hpp" 
#include "../LogHandler/logger.hpp"

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

        // Get the device role
        const Role& getRole() const;

        // Get the device role
        const Role& getRole() const;

        // In self-search mode: update the device configuration from a SelfSearchConfig object
        void applySelfSearchConfig(const SelfSearchConfig& config);

        // JSON deserialization for normal (user) settings from user_setting.json
        friend void from_json(const nlohmann::json& j, EthDevice& ethDevice);

        // Pinging to given IP address.
        bool pingIP(const std::string& ip);

        std::string getSelfIp() const;

        std::string getDestIp() const;

        std::string getSrcPort() const;
        
        std::string getDestPort() const;

        int getSocketFd() const;

        std::string getName() const;
    private:
        std::string name_;
        std::string ipAddress_;
        std::string destIpAddress_;
        std::string defaultGateway_;
        std::string subnetMask_;
        std::string remoteIp_;
        std::string remoteIpDestination_;
        std::string srcPort_;
        std::string destPort_;
        bool promisc_;
        bool noArp_;
        bool multicast_;
        bool debug_;
        bool dynamic_;
        bool notrailers_;
        bool broadcast_;
        int mtu_;
        Role role_;

        struct ifreq ifr;

        int  sock_;
        int sockOptStatus;
        bool initSocket();
        void closeSocket();

        uint16_t compute_checksum(void* buf, int len);

        // Helper methods for applying settings
        void setDeviceFlags();
        void updateFlag(short& flags, unsigned int flag, bool enable);
        void setMTU();
        void addVirtualIpIfNeeded();
        void setSelfIP();
        void setDefaultGateway();
       
        // Helper: convert a string to Role enum
        static Role parseRole(const std::string& roleStr);
};

