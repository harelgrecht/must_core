#include "GPIOHandler/GPIOHandler.HPP"

int main() {
    gpioHandler::setConfigPath("config.json");

    gpioHandler myGpio1(17, "out");
    gpioHandler myGpio2(18, "out");

    // Use gpioHandler methods here

    return 0;
}