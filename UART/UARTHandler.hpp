#pragma once

#include <string>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

constexpr int UART_BUFFER_SIZE = 256;

class UARTHandler {
public:
    UARTHandler(const std::string& device, int baud_rate);
    ~UARTHandler();

    
    

    bool sendData(const std::string& data);

    void setReceiveCallback(std::function<void(const std::string&)> callback);

    void startReceiveThread();
    void stopReceiveThread();

private:
    std::string device_;
    int baudRate_;
    int uartFD_;
    std::mutex send_mutex_;
    std::thread receive_thread_;
    std::atomic<bool> run_receive_thread_;
    std::function<void(const std::string&)> receive_callback_;

    bool configureUART();
    void receiveLoop();
};
