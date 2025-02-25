#ifndef ETHDEVICE_HPP
#define ETHDEVICE_HPP

#include <string>
#include <nlohmann/json.hpp>

// Forward declaration for SelfSearchConfig (defined below)
class SelfSearchConfig;

class EthDevice {
    public:
        // Role for the device
        enum class Role { SOURCE1, SOURCE2, SOURCE3, TUNNEL };

        // Constructors
        EthDevice();
        explicit EthDevice(const std::string& name);

        // Normal mode: apply the user-supplied settings
        void applySettings();

        // Check at hardware level if the cable is connected
        bool isCableConnected() const;

        // Get the device name (e.g., "eth1")
        const std::string& getName() const;

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

        // Helper methods for applying settings
        void setDeviceFlags();
        void setMTU();
        void addVirtualIpIfNeeded();
        void setInterfaceSpeed();
        void setSelfIP();
        void setDefaultGateway();
        void setSubnetMask();


        // Helper: convert a string to Role enum
        static Role parseRole(const std::string& roleStr);
};

#endif // ETHDEVICE_HPP
