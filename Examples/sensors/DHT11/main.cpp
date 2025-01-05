#include "../../blink/GPIO.h" // GPIO header relative to the main.cpp location

#include <iostream>
#include <thread> // For delays

int main() {
    try {
        // Initialize DHT11 sensor on GPIO pin 4
        DHT11 dht11(4);

        // Read and print temperature and humidity every 2 seconds
        while (true) {
            dht11.printValues();
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }

    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
