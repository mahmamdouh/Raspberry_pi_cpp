#include "GPIO.h"
#include <iostream>
#include <thread> // For delays
#include <chrono> // For chrono::milliseconds

int main() {
    try {
        // Configure GPIO pin 17 as output
        GPIO led(17, true);

        // Blink the LED
        std::cout << "Blinking LED on GPIO 17..." << std::endl;
        for (int i = 0; i < 10; ++i) {
            led.write(true); // Turn on
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            led.write(false); // Turn off
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // Configure GPIO pin 18 as input
        GPIO button(18, false);
        std::cout << "Waiting for button press on GPIO 18..." << std::endl;

        for (int i = 0; i < 10; ++i) {
            bool buttonState = button.readDigital();
            std::cout << "Button state: " << (buttonState ? "PRESSED" : "NOT PRESSED") << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }

        // Read analog value from MCP3008 (channel 0)
        GPIO adc(0, false); // The pin here doesn't matter for SPI communication
        std::cout << "Reading analog value from ADC channel 0..." << std::endl;
        int analogValue = adc.readAnalog(0);
        std::cout << "Analog value: " << analogValue << std::endl;

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
