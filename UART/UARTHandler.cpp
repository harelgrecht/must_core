#include "UARTHandler.hpp"
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

UARTHandler::UARTHandler(const std::string& device, int baud_rate)
    : device_(device), baudRate_(baud_rate), uartFD_(-1), run_receive_thread_(false) {}

UARTHandler::~UARTHandler() {
    stopReceiveThread();
    close(uartFD_);
}

bool UARTHandler::configureUART() {
    uartFD_ = open(device_.c_str(), O_RDWR | O_NOCTTY);
    if (uartFD_ == -1) {
        std::cerr << "[UARTHandler] Error opening UART device: " << device_ << std::endl;
        return false;
    }

    struct termios options;
    if (tcgetattr(uartFD_, &options) != 0) {
        std::cerr << "[UARTHandler] Error getting UART attributes" << std::endl;
        close(uartFD_);
        return false;
    }

    cfsetispeed(&options, baudRate_);
    cfsetospeed(&options, baudRate_);

    // Configure control options
    options.c_cflag &= ~PARENB;  // No parity
    options.c_cflag &= ~CSTOPB;  // 1 stop bit
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;      // 8 data bits
    options.c_cflag &= ~CRTSCTS; // No hardware flow control
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

    if (tcsetattr(uartFD_, TCSANOW, &options) != 0) {
        std::cerr << "[UARTHandler] Failed to apply UART device settings" << std::endl;
        close(uartFD_);
        return false;
    }
    return true;
}

bool UARTHandler::sendData(const std::string& data) {
    std::lock_guard<std::mutex> lock(send_mutex_);
    if (uartFD_ == -1) {
        std::cerr << "[UARTHandler] UART is not open" << std::endl;
        return false;
    }
    ssize_t bytes_written = write(uartFD_, data.c_str(), data.length());
    if (bytes_written < 0) {
        std::cerr << "[UARTHandler] Error sending data" << std::endl;
        return false;
    }
    return true;
}

void UARTHandler::setReceiveCallback(std::function<void(const std::string&)> callback) {
    receive_callback_ = callback;
}

void UARTHandler::startReceiveThread() {
    if (!run_receive_thread_.load()) {
        run_receive_thread_.store(true);
        receive_thread_ = std::thread(&UARTHandler::receiveLoop, this);
    }
}

void UARTHandler::stopReceiveThread() {
    run_receive_thread_.store(false);
    if (receive_thread_.joinable()) {
        receive_thread_.join();
    }
}

void UARTHandler::receiveLoop() {
    char buffer[UART_BUFFER_SIZE];
    while (run_receive_thread_.load()) {
        ssize_t readBytes = read(uartFD_, buffer, sizeof(buffer) - 1);
        if (readBytes > 0) {
            buffer[readBytes] = '\0';
            if (receive_callback_) {
                receive_callback_(std::string(buffer));
            }
        } else if (readBytes < 0 && errno != EAGAIN) {
            std::cerr << "[UARTHandler] Error reading data" << std::endl;
            break;
        }
    }
}
