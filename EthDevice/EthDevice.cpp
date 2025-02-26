#include "EthDevice.hpp"

// Default constructor
EthDevice::EthDevice() : name(""), role(Role::SOURCE1), sock_(nullptr) {
    initSocket();
}

// Constructor with name (e.g. "eth1")
EthDevice::EthDevice(const std::string& name) : name(name), role(Role::SOURCE1) {
    initSocket();
}

EthDevice::~EthDevice() {
    closeSocket();
}

void EthDevice::initSocket() {
    sock_ = nl_socket_alloc();
    if (!sock_) {
        std::cerr << "Failed to allocate netlink socket for " << name << "\n";
        return;
    }
    int err = nl_connect(sock_, NETLINK_ROUTE);
    if (err < 0) {
        std::cerr << "Failed to connect netlink socket for " << name << ": " << nl_geterror(err) << "\n";
        nl_socket_free(sock_);
        sock_ = nullptr;
    }
}

// Close the persistent netlink socket
void EthDevice::closeSocket() {
    if (sock_) {
        nl_close(sock_);
        nl_socket_free(sock_);
        sock_ = nullptr;
    }
}

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

// Checking for cable connectivity at the hardware level.
bool EthDevice::isCableConnected() const {
    std::cout << "[EthDevice] Checking cable connectivity on " << name << std::endl;
    // Create a socket for ioctl calls
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return false;
    }

    // Prepare the ifreq structure
    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    std::strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ - 1);

    // Prepare an ethtool_value structure for the ETHTOOL_GLINK command
    struct ethtool_value edata;
    edata.cmd = ETHTOOL_GLINK;
    ifr.ifr_data = reinterpret_cast<char*>(&edata);

    // Call ioctl to get the link status
    if (ioctl(sock, SIOCETHTOOL, &ifr) < 0) {
        perror("ioctl(ETHTOOL_GLINK)");
        close(sock);
        return false;
    }

    close(sock);
    // edata.data is 1 if the link (cable) is connected, 0 otherwise.
    return (edata.data == 1);
    return true; // For simulation, assume cable is connected.
}

// Ping to the given IP address.
bool EthDevice::pingIP(const std::string& ip) {
    std::cout << "[Ping] Pinging " << ip << "...\n";
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock < 0) {
        perror("socket");
        return false;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    if(inet_pton(AF_INET, target.c_str(), &addr.sin_addr) != 1){
        std::cerr << "Invalid target IP: " << target << "\n";
        close(sock);
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
    ssize_t sent = sendto(sock, sendbuf, sizeof(sendbuf), 0, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr));
    if (sent < 0) {
        perror("sendto");
        close(sock);
        return false;
    }

    char recvbuf[1024];
    socklen_t addr_len = sizeof(addr);
    ssize_t received = recvfrom(sock, recvbuf, sizeof(recvbuf), 0, reinterpret_cast<struct sockaddr*>(&addr), &addr_len);
    if (received < 0) {
        perror("recvfrom");
        close(sock);
        return false;
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Ping response from " << target << " in " << diff.count()*1000 << " ms\n";

    close(sock);
    return true;
}

const std::string& EthDevice::getName() const {
    return name;
}

const EthDevice::Role& EthDevice::getRole() const {
    return role;
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
    
    setSelfIP();
    setDefaultGateway();

    std::cout << "[EthDevice] Applied self-search configuration on " << name << ":\n"
              << "  Self IP: " << ipAddress << "\n"
              << "  Destination IP: " << destIpAddress << "\n"
              << "  Default Gateway: " << defaultGateway << "\n"
              << "  Subnet Mask: " << subnetMask << "\n";
}

//-------------------
// Set IP Address (and subnet mask)
// Combines ipAddress and subnetMask fields into CIDR notation.
// Example: ipAddress="192.168.1.10", subnetMask="255.255.255.0" --> "192.168.1.10/24"
//-------------------
void EthDevice::setSelfIP() {
    if (!sock_) return;
    if(ipAddress.empty() || subnetMask.empty()){
       std::cerr << "IP address or subnet mask is empty for " << name << "\n";
       return;
    }
    
    // Convert subnet mask to prefix length (e.g., "255.255.255.0" -> 24)
    unsigned int a, b, c, d;
    int prefix = 0;
    if(sscanf(subnetMask.c_str(), "%u.%u.%u.%u", &a, &b, &c, &d) == 4){
        unsigned int mask = (a << 24) | (b << 16) | (c << 8) | d;
        while(mask & 0x80000000) {
            prefix++;
            mask <<= 1;
        }
    } else {
        std::cerr << "Invalid subnet mask format: " << subnetMask << "\n";
        return;
    }
    
    std::string cidr = ipAddress + "/" + std::to_string(prefix);
    
    struct nl_addr* nladdr = nullptr;
    int err = nl_addr_parse(cidr.c_str(), AF_INET, &nladdr);
    if(err < 0){
       std::cerr << "Failed to parse IP address " << cidr << " for " << name 
                 << ": " << nl_geterror(err) << "\n";
       return;
    }
    
    int ifindex = rtnl_link_get_by_name(sock_, name.c_str());
    if(ifindex < 0){
       std::cerr << "Interface " << name << " not found in setIpAddress.\n";
       nl_addr_put(nladdr);
       return;
    }
    
    struct rtnl_addr* addr = rtnl_addr_alloc();
    if(!addr){
       std::cerr << "Failed to allocate rtnl_addr for " << name << "\n";
       nl_addr_put(nladdr);
       return;
    }
    
    rtnl_addr_set_ifindex(addr, ifindex);
    rtnl_addr_set_local(addr, nladdr);
    
    err = rtnl_addr_add(sock_, addr, 0);
    if(err < 0)
      std::cerr << "Failed to add IP address " << cidr << " to " << name 
                << ": " << nl_geterror(err) << "\n";
    else
      std::cout << "IP address " << cidr << " set on " << name << "\n";
    
    nl_addr_put(nladdr);
    rtnl_addr_put(addr);
}

//-------------------
// Set Default Gateway
// Uses libnl route APIs to add a default route (0.0.0.0/0) via defaultGateway field.
//-------------------
void EthDevice::setDefaultGateway() {
    if (!sock_) return;
    if(defaultGateway.empty()){
       std::cerr << "Default gateway is empty for " << name << "\n";
       return;
    }
    
    int ifindex = rtnl_link_get_by_name(sock_, name.c_str());
    if(ifindex < 0){
       std::cerr << "Interface " << name << " not found in setDefaultGateway.\n";
       return;
    }
    
    struct rtnl_route* route = rtnl_route_alloc();
    if(!route){
       std::cerr << "Failed to allocate route object for " << name << "\n";
       return;
    }
    
    rtnl_route_set_family(route, AF_INET);
    
    // Set destination to default route "0.0.0.0/0"
    struct nl_addr* dst = nullptr;
    int err = nl_addr_parse("0.0.0.0/0", AF_INET, &dst);
    if(err < 0){
       std::cerr << "Failed to parse default destination: " << nl_geterror(err) << "\n";
       rtnl_route_put(route);
       return;
    }
    rtnl_route_set_dst(route, dst);
    
    // Create nexthop for gateway
    struct rtnl_nexthop* nh = rtnl_route_nh_alloc();
    if(!nh){
       std::cerr << "Failed to allocate nexthop object for " << name << "\n";
       nl_addr_put(dst);
       rtnl_route_put(route);
       return;
    }
    rtnl_route_nh_set_ifindex(nh, ifindex);
    
    struct nl_addr* gw = nullptr;
    err = nl_addr_parse(defaultGateway.c_str(), AF_INET, &gw);
    if(err < 0){
       std::cerr << "Failed to parse default gateway " << defaultGateway << ": " << nl_geterror(err) << "\n";
       rtnl_route_nh_free(nh);
       nl_addr_put(dst);
       rtnl_route_put(route);
       return;
    }
    rtnl_route_nh_set_gateway(nh, gw);
    rtnl_route_add_nexthop(route, nh);
    
    err = rtnl_route_add(sock_, route, 0);
    if(err < 0)
       std::cerr << "Failed to add default gateway " << defaultGateway << " for " << name 
                 << ": " << nl_geterror(err) << "\n";
    else
       std::cout << "Default gateway " << defaultGateway << " set for " << name << "\n";
    
    nl_addr_put(gw);
    nl_addr_put(dst);
    rtnl_route_put(route);
}

// Helper methods
void EthDevice::setDeviceFlags() {
    std::cout << "[EthDevice] Setting device flags for " << name << std::endl;
    if (!sock_) return;
    int ifindex = rtnl_link_get_by_name(sock_, name.c_str());
    if (ifindex < 0) {
        std::cerr << "Interface " << name << " not found in setDeviceFlags.\n";
        return;
    }
    struct rtnl_link* link = nullptr;
    int err = rtnl_link_get_kernel(sock_, ifindex, &link);
    if (err < 0 || !link) {
        std::cerr << "Failed to get link for " << name << ": " << nl_geterror(err) << "\n";
        return;
    }
    int flags = rtnl_link_get_flags(link);
    flags |= IFF_UP;
    rtnl_link_set_flags(link, flags);
    err = rtnl_link_change(sock_, link, link, 0);
    if (err < 0)
        std::cerr << "Failed to change link flags for " << name << ": " << nl_geterror(err) << "\n";
    else
        std::cout << "Interface " << name << " is now up.\n";
    rtnl_link_put(link);
}

void EthDevice::setMTU() {
    std::cout << "[EthDevice] Setting MTU for " << name << std::endl;
    if (!sock_) return;
    int new_mtu = 1500;
    int ifindex = rtnl_link_get_by_name(sock_, name.c_str());
    if (ifindex < 0) {
        std::cerr << "Interface " << name << " not found in setMTU.\n";
        return;
    }
    struct rtnl_link* link = nullptr;
    int err = rtnl_link_get_kernel(sock_, ifindex, &link);
    if (err < 0 || !link) {
        std::cerr << "Failed to get link for " << name << " in setMTU: " << nl_geterror(err) << "\n";
        return;
    }
    rtnl_link_set_mtu(link, new_mtu);
    err = rtnl_link_change(sock_, link, link, 0);
    if (err < 0)
        std::cerr << "Failed to set MTU for " << name << ": " << nl_geterror(err) << "\n";
    else
        std::cout << "MTU set to " << new_mtu << " for " << name << "\n";
    rtnl_link_put(link);
}

void EthDevice::addVirtualIpIfNeeded() {
    std::cout << "[EthDevice] Adding virtual IP if needed for " << name << std::endl;
    if (!sock_) return;
    const char* virtual_ip = "192.168.1.101/24";
    int ifindex = rtnl_link_get_by_name(sock_, name.c_str());
    if (ifindex < 0) {
        std::cerr << "Interface " << name << " not found in addVirtualIpIfNeeded.\n";
        return;
    }
    struct rtnl_addr* addr = rtnl_addr_alloc();
    if (!addr) {
        std::cerr << "Failed to allocate rtnl_addr in addVirtualIpIfNeeded.\n";
        return;
    }
    rtnl_addr_set_ifindex(addr, ifindex);
    struct nl_addr* nladdr = nullptr;
    int err = nl_addr_parse(virtual_ip, AF_INET, &nladdr);
    if (err < 0) {
        std::cerr << "Failed to parse virtual IP in addVirtualIpIfNeeded: " << nl_geterror(err) << "\n";
        rtnl_addr_put(addr);
        return;
    }
    rtnl_addr_set_local(addr, nladdr);
    err = rtnl_addr_add(sock_, addr, 0);
    if (err < 0)
        std::cerr << "Failed to add virtual IP " << virtual_ip << " to " << name << ": " << nl_geterror(err) << "\n";
    else
        std::cout << "Virtual IP " << virtual_ip << " added to " << name << "\n";
    nl_addr_put(nladdr);
    rtnl_addr_put(addr);
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