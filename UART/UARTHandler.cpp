#include "UARTHandler.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

UARTHandler::UARTHandler(const std::string& device, int baud_rate) 
    : device_(device), baud_rate_(baud_rate), uart_fd_(-1), run_receive_thread_(false) { }

UARTHandler::~UARTHandler() {
    stop_receive_thread();
    close_uart();
}

bool UARTHandler::configure_uart() {
    uart_fd_ = open(device.c_str(), O_RDWR | O_NOCTTY);
    if (uart_fd_ == -1) {
        std::cerr << "[UARTHandler] Error opening UART device: " << device << std::endl;
        return false;
    }

    struct termios options;
    if (tcgetattr(uart_fd_, &options) != 0) {
        std::cerr << "[UARTHandler] Error getting UART attributes" << std::endl;
        close(uart_fd_);
        uart_fd_ = -1;
        return false;
    }

    cfsetispeed(&options, baud_rate_);
    cfsetospeed(&options, baud_rate_);

    // Configure control options
    options.c_cflag &= ~PARENB;        // No parity
    options.c_cflag &= ~CSTOPB;        // 1 stop bit
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;            // 8 data bits
    options.c_cflag &= ~CRTSCTS;       // No hardware flow control
    options.c_cflag |= CREAD | CLOCAL; // Enable receiver, local mode

    // Configure input options: disable software flow control
    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    // Configure local options: set raw input (non-canonical, no echo, no signals)
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // Configure output options: raw output (no post-processing)
    options.c_oflag &= ~OPOST;

    // Set read timeout and minimum characters to read
    options.c_cc[VTIME] = 1; // Timeout in deciseconds (0.1 seconds)
    options.c_cc[VMIN] = 0;

    if (tcsetattr(uart_fd_, TCSANOW, &options) != 0) {
        std::cerr << "[UARTHandler] Failed to apply UART device settings" << std::endl;
        close(uart_fd_);
        uart_fd_ = -1;
        return false;
    }
    return true;
}

bool UARTHandler::open_uart() {
    return configure_uart();
}

void UARTHandler::close_uart() {
    if (uart_fd_ != -1) {
        close(uart_fd_);
        uart_fd_ = -1;
    }
}

bool UARTHandler::send_data(const std::string& data) {
    std::lock_guard<std::mutex> lock(send_mutex_);
    if (uart_fd_ == -1) {
        std::cerr << "[UARTHandler] UART is not open" << std::endl;
        return false;
    }
    ssize_t bytes_written = write(uart_fd_, data.c_str(), data.length());
    if (bytes_written < 0) {
        std::cerr << "[UARTHandler] Error sending data" << std::endl;
        return false;
    }
    return true;
}

void UARTHandler::set_receive_callback(std::function<void(const std::string&)> callback) {
    receive_callback = callback;
}

void UARTHandler::start_receive_thread() {
    if (!run_receive_thread_.load()) {
        run_receive_thread_.store(true);
        receive_thread_ = std::thread(&UARTHandler::receive_loop, this);
    }  
}

void UARTHandler::stop_receive_thread() {
    run_receive_thread_.store(false);
    if(receive_thread_.joinable()){
        receive_thread_.join();
    }
}

void UARTHandler::receive_loop() {
    char buffer[UART_BUFFER_SIZE];
    while (run_receive_thread_.load()) {
        int readBytes = read(uart_fd_, buffer, sizeof(buffer));
        if (readBytes > 0) {
            buffer[readBytes] = '\0';
            if (receive_callback_) {
                receive_callback(std::string(buffer));
            }
        }
    }
}

