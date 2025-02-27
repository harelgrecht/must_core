#include "GPIOHandler/gpioHandler.hpp"
#include "LogHandler/logger.hpp"
#include "NetworkManager/NetworkManager.hpp"
#include "packetSend/packet_send.hpp"
#include "PacketAuthentication/packet_authenticator.hpp"
#include "packetRouter/packet_router.hpp"
#include "RealTimeThread/real_time_thread.hpp"
#include "third_party/nlohmann/json.hpp"
#include "ThreadSafeQueue/threadSafeQueue.hpp"
#include "tunnel_receiver_process/tunnel_reciver_process.hpp"
#include "tunnel_transmitter_process/tunnel_transmitter_process.hpp"
#include <iostream>

constexpr int QueueSize = 100;


int main() {

    ThreadSafeQueue<std::vector<char>> reciveQueue(QueueSize); 
    ThreadSafeQueue<std::vector<char>> sendQueue(QueueSize);
    ThreadSafeQueue<std::vector<char>> fromTunnelQueue(QueueSize);  

    RealTimeThread receiverThread([&]() {
        NetworkManager networkConfig;
        
        networkConfig.parseSettings("user_settings.json", "self_seach_ips.json");
        networkConfig.applySettings();

    });

    RealTimeThread processThread([&]() {

    });

    RealTimeThread senderThread([&]() {
        
    });
    // Start the threads
    receiverThread.start();
    processingThread.start();
    senderThread.start();

    // Wait for threads to finish (in a real application, implement proper shutdown handling)
    receiverThread.join();
    processingThread.join();
    senderThread.join();

    return 0;
}
