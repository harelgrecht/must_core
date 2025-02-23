#include "UARTHandler.hpp"

UARTHandler::UARTHandler(const std::string& device, int baud_rate) : device(device), baud_rate(baud_rate), uart_fd(-1), run_recive_thread(false) { }

UARTHandler::~UARTHandler() {
    stop_recive_thread();
    close_uart();
}

bool UARTHandler::configure_uart() {
    uart_fd = open(device.c_str(), O_RDWR | O_NOCTTY);
    if (uart_fd == -1) {
        std::cerr << "Error opening UART device" << std::endl;
        return false;
    }
    struct termios options;
    tcgetattr(uart_fd, &options);
    cfsetispeed(&options, baud_rate);
    cfsetospeed(&options, baud_rate);

    options.c_cflag &= ~PARENB; // no parity
    options.c_cflag &= ~CSTOPB; // 1 stop bit
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8; // 8 data bit
    options.c_cflag |= CREAD | CLOCAL; //Enable reciver

    tcsetattr(uart_fd, TCSANOW, &options);

    return true;
}

bool UARTHandler::open_uart() {
    return configure_uart();
}

void UARTHandler::close_uart() {
    if (uart_fd != -1) {
        close(uart_fd);
        uart_fd = -1;
    }
}

bool UARTHandler::send_data(const std::string& data) {
    std::lock_guard<std::mutex> lock(send_mutex);
    if (uart_fd == -1) {
        std::cerr << "UART is not open" << std::endl;
        return false;
    }
    if (write(uart_fd, data.c_str(), data.length()) < 0 ) {
        std::cerr << "error sending data" << std::endl;
        return false;
    }
    return true;
}

void UARTHandler::set_recive_callback(std::function<void(const std::string&)> callback) {
    recive_callback = callback;
}

void UARTHandler::start_recive_thread() {
    if (!run_recive_thread.load()) {
        run_recive_thread.store(true);
        recive_thread = std::thread(&UARTHandler::recive_loop, this);
    }  
}

void UARTHandler::stop_recive_thread() {
    run_recive_thread.store(false);
    if(recive_thread.joinable()){
        recive_thread.join();
    }
}

void UARTHandler::recive_loop() {
    char buffer[UART_BUFFER_SIZE];
    while (run_recive_thread.load()) {
        int n = read(uart_fd, buffer, sizeof(buffer));
        if (n > 0) {
            buffer[n] = '\0';
            if (recive_callback) {
                recive_callback(std::string(buffer));
            }
        }
    }
}

