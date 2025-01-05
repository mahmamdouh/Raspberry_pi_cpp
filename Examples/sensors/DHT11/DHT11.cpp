#include "DHT11.h"
#include <iostream>  // For debugging
#include <chrono>    // For time measurement
#include <thread>    // For delays

// Constructor
DHT11::DHT11(int gpioPin) : gpio(gpioPin, false), pin(gpioPin) {}

// Destructor
DHT11::~DHT11() {}

// Wait for a signal level (HIGH/LOW) for a maximum timeout (in microseconds)
bool DHT11::waitForSignal(bool level, int timeoutUs) {
    auto start = std::chrono::high_resolution_clock::now();
    while (gpio.readDigital() != level) {
        auto now = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - start).count();
        if (elapsed > timeoutUs) {
            return false;
        }
    }
    return true;
}

// Validate checksum of the received data
bool DHT11::validateChecksum(const uint8_t data[5]) {
    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    return checksum == data[4];
}

// Read temperature and humidity from DHT11
std::pair<float, float> DHT11::read() {
    uint8_t data[5] = {0}; // Array to hold the 40-bit response (5 bytes)
    
    // Step 1: Send start signal (low for 18ms, then high for 20-40us)
    gpio.configure(true); // Set pin as output
    gpio.write(false);
    std::this_thread::sleep_for(std::chrono::milliseconds(18));
    gpio.write(true);
    std::this_thread::sleep_for(std::chrono::microseconds(40));

    // Step 2: Set pin as input and wait for DHT11 response
    gpio.configure(false); // Set pin as input
    if (!waitForSignal(false, 80)) {
        throw std::runtime_error("DHT11 failed to respond (no LOW signal).");
    }
    if (!waitForSignal(true, 90)) {
        throw std::runtime_error("DHT11 failed to respond (no HIGH signal).");
    }

    // Step 3: Read 40 bits of data (5 bytes)
    for (int i = 0; i < 40; ++i) {
        // Wait for the start of each bit (LOW signal)
        if (!waitForSignal(false, 50)) {
            throw std::runtime_error("Timeout while waiting for the start of bit signal.");
        }

        // Measure the length of the HIGH signal to determine bit value
        auto start = std::chrono::high_resolution_clock::now();
        if (!waitForSignal(true, 70)) {
            throw std::runtime_error("Timeout while waiting for HIGH signal during bit read.");
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto pulseWidth = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        // Store bit value (0 if < 30us, 1 if > 30us)
        data[i / 8] <<= 1;
        if (pulseWidth > 30) {
            data[i / 8] |= 1;
        }
    }

    // Step 4: Validate checksum
    if (!validateChecksum(data)) {
        throw std::runtime_error("Checksum validation failed.");
    }

    // Step 5: Extract temperature and humidity
    float humidity = data[0] + data[1] * 0.1;
    float temperature = data[2] + data[3] * 0.1;

    return {humidity, temperature};
}

// Utility function to print temperature and humidity
void DHT11::printValues() {
    try {
        auto [humidity, temperature] = read();
        std::cout << "Humidity: " << humidity << "%, Temperature: " << temperature << "�C" << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
