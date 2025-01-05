#include "GPIO.h"
#include <iostream>

int main() {
    try {
        // Create GPIO object for the LED pin (e.g., GPIO 17)
        GPIO led(17); // GPIO 17, configured as output by default

        std::cout << "Blinking LED..." << std::endl;

        // Blink the LED for 10 seconds, with a 500ms interval
        led.blink(10000, 500);

        std::cout << "Done!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
