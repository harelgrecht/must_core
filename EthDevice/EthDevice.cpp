#include "EthDevice.hpp"
#include "SelfSearchConfig.hpp"  // needed for applySelfSearchConfig
#include <iostream>

// Default constructor
EthDevice::EthDevice() : name(""), role(Role::SOURCE1) {}

// Constructor with name (e.g. "eth1")
EthDevice::EthDevice(const std::string& name) : name(name), role(Role::SOURCE1) {}

// In normal mode, apply the configuration read from user_setting.json
void EthDevice::applySettings() {
    std::cout << "[EthDevice] Applying settings to " << name << ":\n"
              << "  IP Address: " << ipAddress << "\n"
              << "  Destination IP: " << destIpAddress << "\n"
              << "  Default Gateway: " << defaultGateway << "\n"
              << "  Subnet Mask: " << subnetMask << "\n"
              << "  Remote IP: " << remoteIp << "\n"
              << "  Remote IP Destination: " << remoteIpDestination << "\n"
              << "  Role: " << static_cast<int>(role) << "\n";
    setDeviceFlags();
    setMTU();
    addVirtualIpIfNeeded();
    setInterfaceSpeed();
}

// Simulate checking for cable connectivity at the hardware level.
// In real code, this might query the OS or NIC driver.
bool EthDevice::isCableConnected() const {
    std::cout << "[EthDevice] Checking cable connectivity on " << name << std::endl;
    return true; // For simulation, assume cable is connected.
}

const std::string& EthDevice::getName() const {
    return name;
}

// In self-search mode, update the device settings from the SelfSearchConfig.
void EthDevice::applySelfSearchConfig(const SelfSearchConfig& config) {
    ipAddress           = config.selfIp;           // use selfIp as the new IP address
    destIpAddress       = config.destIpAddress;
    defaultGateway      = config.defaultGateway;
    subnetMask          = config.subnetMask;
    remoteIp            = config.remoteIp;
    remoteIpDestination = config.remoteIpDestination;
    role                = parseRole(config.role);
    
    std::cout << "[EthDevice] Applied self-search configuration on " << name << ":\n"
              << "  Self IP: " << ipAddress << "\n"
              << "  Destination IP: " << destIpAddress << "\n"
              << "  Default Gateway: " << defaultGateway << "\n"
              << "  Subnet Mask: " << subnetMask << "\n";
}

// Helper methods (placeholders)
void EthDevice::setDeviceFlags() {
    std::cout << "[EthDevice] Setting device flags for " << name << std::endl;
}
void EthDevice::setMTU() {
    std::cout << "[EthDevice] Setting MTU for " << name << std::endl;
}
void EthDevice::addVirtualIpIfNeeded() {
    std::cout << "[EthDevice] Adding virtual IP if needed for " << name << std::endl;
}
void EthDevice::setInterfaceSpeed() {
    std::cout << "[EthDevice] Setting interface speed for " << name << std::endl;
}

EthDevice::Role EthDevice::parseRole(const std::string& roleStr) {
    if (roleStr == "source1") return Role::SOURCE1;
    if (roleStr == "source2") return Role::SOURCE2;
    if (roleStr == "source3") return Role::SOURCE3;
    if (roleStr == "tunnel")  return Role::TUNNEL;
    return Role::SOURCE1;
}

// JSON deserialization: maps JSON keys to EthDevice fields.
void from_json(const nlohmann::json& j, EthDevice& ethDevice) {
    ethDevice.ipAddress           = j.value("ipAddress", "");
    ethDevice.destIpAddress       = j.value("DestIpAddress", "");
    ethDevice.defaultGateway      = j.value("DefaultGateway", "");
    ethDevice.subnetMask          = j.value("SubnetMask", "255.255.255.0");
    ethDevice.remoteIp            = j.value("RemoteIp", "");
    ethDevice.remoteIpDestination = j.value("RemoteIpDestination", "");
    std::string roleStr           = j.value("role", "source1");
    ethDevice.role                = EthDevice::parseRole(roleStr);
}
