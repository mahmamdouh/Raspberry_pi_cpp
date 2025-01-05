#include "dht11.h"
#include <iostream>
#include <thread>

int main() {
    try {
        // Initialize DHT11 sensor on GPIO pin 27
        Dht11 sensor(27);

        while (true) {
            double temperature, humidity;
            sensor.readRHT(&temperature, &humidity);

            if (temperature == TRANSMISSION_ERROR || humidity == TRANSMISSION_ERROR) {
                std::cerr << "Failed to read from DHT11 sensor." << std::endl;
            } else {
                std::cout << "Temperature: " << temperature << " °C, Humidity: " << humidity << " %" << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
