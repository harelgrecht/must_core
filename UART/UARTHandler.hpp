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


int constexpr const UART_BUFFER_SIZE 256

class UARTHandler {

    public:
        UARTHandler(const std::string& device, int baud_rate);
    
        ~UARTHandler();

        bool open_uart();
        void close_uart();
    
        bool send_data(const std::string& data);
    
        void set_recive_callback(std::function<void(const std::string&)> callback);
    
        void start_recive_thread();
        void stop_recive_thread();
    
    private:
        std::string device_;
        int baud_rate_;
        int uart_fd_;
        std::mutex send_mutex_;
        std::thread recive_thread_;
        std::atomic<bool> run_recive_thread_;
        std::function<void(const std::string&)> recive_callback;

        void recive_loop();
        bool configure_uart();
};

#endif