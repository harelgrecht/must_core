#include "EthDevice.hpp"

// Default constructor
EthDevice::EthDevice() : name_(""), role_(Role::SOURCE1), sock_(-1) {
    initSocket();
}

// Constructor with name (e.g. "eth1")
EthDevice::EthDevice(const std::string& name) : name_(name), role_(Role::SOURCE1) {
    initSocket();
}

EthDevice::~EthDevice() {
    closeSocket();
}

bool EthDevice::initSocket() {
    sock_ = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock_ < 0) {
        throw std::runtime_error("Failed to create sock_et for device: " + name_);
    }

    strncpy(ifr.ifr_name, name_.c_str(), IFNAMSIZ - 1); 

    sockaddr_in addrLocal = {};
    addrLocal.sin_family = AF_INET;
    addrLocal.sin_port = htons(0);
    addrLocal.sin_addr.s_addr = INADDR_ANY; // use all available adapters on the local host

    int flags = fcntl(sock_, F_GETFL, 0);
    if (flags == -1) {
        closeSocket();
        return false;
    }
    if (fcntl(sock_, F_SETFL, flags | O_NONBLOCK) == -1) {
        closeSocket();
        return false;
    }
    if (bind(sock_, reinterpret_cast<struct sockaddr*>(&addrLocal), sizeof(addrLocal)) == -1) {
        closeSocket();
        return false;
    }

    int ifaceNameSize = name_.size() + 1; // +1 for null terminator
    if (setsockopt(sock_, SOL_SOCKET, SO_BINDTODEVICE, &ifr, ifaceNameSize) < 0) {
        closeSocket();
        return false;
    }
}

void EthDevice::closeSocket() {
    if(sock_ >= 0) {
        close(sock_);
        sock_ = -1;
    }
}

void EthDevice::setDeviceFlags() {
    if(ioctl(sock_, SIOCGIFFLAGS, &ifr) == -1) {
        throw std::runtime_error("Failed to read interface flags to device: " + name_);
    }
    updateFlag(ifr.ifr_flags, IFF_PROMISC, promisc_);
    updateFlag(ifr.ifr_flags, IFF_NOARP, noArp_);
    updateFlag(ifr.ifr_flags, IFF_MULTICAST, multicast_);
    updateFlag(ifr.ifr_flags, IFF_DEBUG, debug_);
    updateFlag(ifr.ifr_flags, IFF_DYNAMIC, dynamic_);
    updateFlag(ifr.ifr_flags, IFF_NOTRAILERS, notrailers_);
    updateFlag(ifr.ifr_flags, IFF_BROADCAST, broadcast_);

    if (mtu_ > 0) {
        setMTU();
    }
}

void updateFlag(short& flags, unsigned int flag, bool enable) {
    if (enable) {
        flags |= flag;
    } else {
        flags &= ~flag;
    }
}

void EthDevice::setMTU() {
    struct ifreq mtu_ifr = ifr;
    mtu_ifr.ifr_mtu = mtu_;
    if(ioctl(sock_, SIOCSIFMTU, &mtu_ifr) < 0) {
        throw std::runtime_error("Failed to set MTU for interface: " + name_);
    }
}

//after a success ping we apllay the settings
void EthDevice::applySettings() {
    std::cout << "[EthDevice] Applying settings to " << name_ << ":\n"
              << "  IP Address: " << ipAddress_ << "\n"
              << "  Destination IP: " << destIpAddress_ << "\n"
              << "  Default Gateway: " << defaultGateway_ << "\n"
              << "  Subnet Mask: " << subnetMask_ << "\n"
              << "  Remote IP: " << remoteIp_ << "\n"
              << "  Remote IP Destination: " << remoteIpDestination_ << "\n"
              << "  Source port: " << srcPort_ << "\n"
              << "  Destination port: " << destPort_ << "\n"
              << "  MTU: " << mtu_ << "\n"
              << "  Role: " << static_cast<int>(role_) << "\n";
    setDeviceFlags();
    setMTU();
    addVirtualIpIfNeeded();
}


void EthDevice::applySelfSearchConfig(const SelfSearchConfig& config) {
    ipAddress_           = config.selfIp_;           // use selfIp as the new IP address
    destIpAddress_      = config.destIpAddress_;
    defaultGateway_      = config.defaultGateway_;
    subnetMask_          = config.subnetMask_;
    remoteIp_            = config.remoteIp_;
    remoteIpDestination_ = config.remoteIpDestination_;
    role_                = parseRole(config.role_);
    
    setSelfIP();
    setDefaultGateway();

    std::cout << "[EthDevice] Applied self-search configuration on " << name_ << ":\n"
              << "  Self IP: " << ipAddress_ << "\n"
              << "  Destination IP: " << destIpAddress_ << "\n"
              << "  Default Gateway: " << defaultGateway_ << "\n"
              << "  Subnet Mask: " << subnetMask_ << "\n";
}

bool EthDevice::pingIP(const std::string& ip) {
    std::cout << "[Ping] Pinging " << ip << "...\n";
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    if(inet_pton(AF_INET, destIpAddress_.c_str(), &addr.sin_addr) != 1){
        std::cerr << "Invalid destIpAddress_ IP: " << destIpAddress_ << "\n";
        close(sock_);
        return false;
    }

    char sendbuf[64];
    memset(sendbuf, 0, sizeof(sendbuf));
    icmphdr* icmp_hdr = reinterpret_cast<icmphdr*>(sendbuf);
    icmp_hdr->type = ICMP_ECHO;
    icmp_hdr->code = 0;
    icmp_hdr->un.echo.id = htons(1);
    icmp_hdr->un.echo.sequence = htons(1);
    icmp_hdr->checksum = 0;
    icmp_hdr->checksum = compute_checksum(sendbuf, sizeof(sendbuf));

    auto start = std::chrono::steady_clock::now();
    ssize_t sent = sendto(sock_, sendbuf, sizeof(sendbuf), 0, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
    if (sent < 0) {
        perror("sendto");
        close(sock_);
        return false;
    }

    char recvbuf[1024];
    socklen_t addr_len = sizeof(addr);
    ssize_t received = recvfrom(sock_, recvbuf, sizeof(recvbuf), 0, reinterpret_cast<struct sockaddr*>(&addr), &addr_len);
    if (received < 0) {
        perror("recvfrom");
        close(sock_);
        return false;
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Ping response from " << destIpAddress_ << " in " << diff.count()*1000 << " ms\n";

    close(sock_);
    return true;
}

const EthDevice::Role& EthDevice::getRole() const {
    return role_;
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
    ethDevice.ipAddress_           = j.value("ipAddress", "");
    ethDevice.destIpAddress_       = j.value("DestIpAddress", "");
    ethDevice.defaultGateway_      = j.value("DefaultGateway", "");
    ethDevice.subnetMask_          = j.value("SubnetMask", "255.255.255.0");
    ethDevice.remoteIp_            = j.value("RemoteIp", "");
    ethDevice.remoteIpDestination_ = j.value("RemoteIpDestination", "");
    ethDevice.srcPort_             = j.value("SourcePort", "");
    ethDevice.destPort_            = j.value("DestPort", "");
    ethDevice.promisc_             = j.value("Promisc",false);
    ethDevice.noArp_               = j.value("noArp", false);
    ethDevice.multicast_           = j.value("multicast", true);
    ethDevice.debug_               = j.value("debug", false);
    ethDevice.dynamic_             = j.value("dynamic", true);
    ethDevice.notrailers_          = j.value("notrailers", false);
    ethDevice.broadcast_           = j.value("broadcast", true);
    ethDevice.mtu_                 = j.value("mtu", 1500);

    std::string roleStr_           = j.value("role", "source1");
    ethDevice.role_                = EthDevice::parseRole(roleStr_);
}

uint16_t compute_checksum(void* buf, int len) {
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

void EthDevice::addVirtualIpIfNeeded() {
    // Check if we need to add a virtual IP (for example, if remoteIp_ is set and different from ipAddress_)
    if (remoteIp_.empty() || remoteIp_ == ipAddress_) {
        return; // No virtual IP needed
    }

    std::cout << "[EthDevice] Adding virtual IP " << remoteIp_ << " to interface " << name_ << std::endl;

    struct ifreq virt_ifr = ifr;
    struct sockaddr_in* addr = reinterpret_cast<struct sockaddr_in*>(&virt_ifr.ifr_addr);
    
    // Set interface name
    strncpy(virt_ifr.ifr_name, name_.c_str(), IFNAMSIZ - 1);
    
    // Set address family
    addr->sin_family = AF_INET;
    
    // Convert IP string to network binary format
    if (inet_pton(AF_INET, remoteIp_.c_str(), &addr->sin_addr) != 1) {
        throw std::runtime_error("Failed to convert remote IP address: " + remoteIp_);
    }
    
    // Add the virtual IP address to the interface
    if (ioctl(sock_, SIOCSIFADDR, &virt_ifr) < 0) {
        throw std::runtime_error("Failed to add virtual IP address to interface: " + name_);
    }
    
    // Get current flags to preserve them
    if (ioctl(sock_, SIOCGIFFLAGS, &virt_ifr) < 0) {
        throw std::runtime_error("Failed to get interface flags for virtual IP: " + name_);
    }
    
    // Ensure the interface is up with the virtual IP
    virt_ifr.ifr_flags |= IFF_UP;
    
    if (ioctl(sock_, SIOCSIFFLAGS, &virt_ifr) < 0) {
        throw std::runtime_error("Failed to set interface flags for virtual IP: " + name_);
    }
    
    std::cout << "[EthDevice] Successfully added virtual IP " << remoteIp_ << " to interface " << name_ << std::endl;
}

void EthDevice::setDefaultGateway() {
    if (defaultGateway_.empty()) {
        std::cerr << "[EthDevice] Default gateway is empty. Skipping configuration.\n";
        return;
    }
    struct rtentry route;
    memset(&route, 0, sizeof(route));

    struct sockaddr_in* addr = (struct sockaddr_in*)&route.rt_gateway;
    addr->sin_family = AF_INET;
    if (inet_pton(AF_INET, defaultGateway_.c_str(), &addr->sin_addr) != 1) {
        close(sock_);
        throw std::runtime_error("[EthDevice] Invalid default gateway IP address.");
    }

    addr = (struct sockaddr_in*)&route.rt_dst;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY; // Default route (0.0.0.0)

    addr = (struct sockaddr_in*)&route.rt_genmask;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY; // Default mask (0.0.0.0)

    route.rt_flags = RTF_UP | RTF_GATEWAY;
    route.rt_dev = const_cast<char*>(name_.c_str()); // Interface name

    if (ioctl(sock_, SIOCADDRT, &route) < 0) {
        closeSocket();
        throw std::runtime_error("[EthDevice] Failed to set default gateway: " + defaultGateway_);
    }

    std::cout << "[EthDevice] Default gateway set successfully: " << defaultGateway_ << " on interface: " << name_ << std::endl;
}

void EthDevice::setSelfIP() {
    if (ipAddress_.empty()) {
        std::cerr << "[EthDevice] Self IP address is empty. Skipping configuration.\n";
        return;
    }

    if (sock_ < 0) {
        throw std::runtime_error("[EthDevice] Invalid socket descriptor.");
    }

    struct ifreq selfIpIfr = {};
    
    // Use safer string copy
    std::strncpy(selfIpIfr.ifr_name, name_.c_str(), IFNAMSIZ - 1);
    selfIpIfr.ifr_name[IFNAMSIZ - 1] = '\0';  // Ensure null termination

    // Set address family to AF_INET (IPv4)
    selfIpIfr.ifr_addr.sa_family = AF_INET;
    auto* addr = reinterpret_cast<struct sockaddr_in*>(&selfIpIfr.ifr_addr);

    if (inet_pton(AF_INET, ipAddress_.c_str(), &addr->sin_addr) <= 0) {
        closeSocket();
        throw std::runtime_error("[EthDevice] Invalid self IP address: " + ipAddress_);
    }

    // Set the IP address of the interface
    if (ioctl(sock_, SIOCSIFADDR, &selfIpIfr) < 0) {
        closeSocket();
        throw std::runtime_error("[EthDevice] Failed to set self IP address: " + ipAddress_);
    }

    // Get current flags safely
    if (ioctl(sock_, SIOCGIFFLAGS, &selfIpIfr) < 0) {
        throw std::runtime_error("[EthDevice] Failed to get interface flags: " + name_);
    }

    // Ensure the interface is up with the new IP
    selfIpIfr.ifr_flags |= IFF_UP;

    if (ioctl(sock_, SIOCSIFFLAGS, &selfIpIfr) < 0) {
        throw std::runtime_error("[EthDevice] Failed to set interface flags: " + name_);
    }

    std::cout << "[EthDevice] Self IP address set successfully: " << ipAddress_
              << " on interface: " << name_ << std::endl;
}

std::string EthDevice::getSelfIp() const {
    return ipAddress_;
}

std::string EthDevice::getDestIp() const {
    return destIpAddress_;
}

std::string EthDevice::getSrcPort() const {
    return srcPort_;
}

std::string EthDevice::getDestPort() const {
    return destPort_;
}

int EthDevice::getSocketFd() const {
    return sock_;
}

std::string EthDevice::getName() const {
    return name_;
}