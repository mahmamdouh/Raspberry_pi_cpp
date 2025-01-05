#include "dht11.h"
#include <pigpio.h>
#include <stdexcept>
#include <chrono>
#include <thread>

// Constructor: Initialize GPIO pin for DHT11
Dht11::Dht11(int pin) : gpioPin(pin) {
    if (gpioInitialise() < 0) {
        throw std::runtime_error("Failed to initialize pigpio");
    }
    gpioSetMode(gpioPin, PI_OUTPUT);
    gpioWrite(gpioPin, PI_HIGH);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Allow DHT11 to stabilize
}

// Destructor: Cleanup GPIO
Dht11::~Dht11() {
    gpioTerminate();
}

// Read raw data from DHT11
long long Dht11::read() {
    int count = 0;
    long long raw = 0;

    // Step 1: Send start signal
    gpioSetMode(gpioPin, PI_OUTPUT);
    gpioWrite(gpioPin, PI_LOW);
    std::this_thread::sleep_for(std::chrono::milliseconds(18)); // Hold LOW for 18ms
    gpioWrite(gpioPin, PI_HIGH);
    std::this_thread::sleep_for(std::chrono::microseconds(40)); // Pull HIGH for 20-40us
    gpioSetMode(gpioPin, PI_INPUT);

    // Step 2: Wait for sensor response
    if (!waitForSignal(PI_LOW, 80)) return TRANSMISSION_ERROR; // Wait for LOW signal (~80us)
    if (!waitForSignal(PI_HIGH, 80)) return TRANSMISSION_ERROR; // Wait for HIGH signal (~80us)

    // Step 3: Read 40 bits of data
    for (int i = 0; i < 40; i++) {
        if (!waitForSignal(PI_LOW, 50)) return TRANSMISSION_ERROR; // Wait for LOW signal (~50us)
        
        auto start = std::chrono::high_resolution_clock::now();
        if (!waitForSignal(PI_HIGH, 70)) return TRANSMISSION_ERROR; // Wait for HIGH signal

        auto end = std::chrono::high_resolution_clock::now();
        auto pulseWidth = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

        raw <<= 1; // Shift left to make space for the next bit
        if (pulseWidth > THRESHOLD) { // If HIGH duration > threshold, it's a 1
            raw |= 1;
        }
    }

    // Step 4: Validate checksum
    if (!validateChecksum(raw)) return TRANSMISSION_ERROR;

    return raw;
}

// Read temperature from DHT11
double Dht11::readT() {
    long long raw = read();
    if (raw == TRANSMISSION_ERROR) return TRANSMISSION_ERROR;

    int tempInt = (raw & TEMP_INT_MASK) >> 16;
    int tempDec = (raw & TEMP_DEC_MASK) >> 8;

    return tempInt + 0.1 * tempDec;
}

// Read humidity from DHT11
double Dht11::readRH() {
    long long raw = read();
    if (raw == TRANSMISSION_ERROR) return TRANSMISSION_ERROR;

    int rhInt = (raw & RH_INT_MASK) >> 32;
    int rhDec = (raw & RH_DEC_MASK) >> 24;

    return rhInt + 0.1 * rhDec;
}

// Read temperature and humidity together
void Dht11::readRHT(double* temp, double* rh) {
    long long raw = read();
    if (raw == TRANSMISSION_ERROR) {
        *temp = *rh = TRANSMISSION_ERROR;
        return;
    }

    int tempInt = (raw & TEMP_INT_MASK) >> 16;
    int tempDec = (raw & TEMP_DEC_MASK) >> 8;
    int rhInt = (raw & RH_INT_MASK) >> 32;
    int rhDec = (raw & RH_DEC_MASK) >> 24;

    *temp = tempInt + 0.1 * tempDec;
    *rh = rhInt + 0.1 * rhDec;
}

// Wait for a signal level (HIGH/LOW) with timeout
bool Dht11::waitForSignal(int level, int timeoutUs) {
    auto start = std::chrono::high_resolution_clock::now();
    while (gpioRead(gpioPin) != level) {
        auto now = std::chrono::high_resolution_clock::now();
        if (std::chrono::duration_cast<std::chrono::microseconds>(now - start).count() > timeoutUs) {
            return false;
        }
    }
    return true;
}

// Validate checksum from the raw data
bool Dht11::validateChecksum(long long raw) {
    int checksum = raw & CHECKSUM_MASK;
    int sum = ((raw & RH_INT_MASK) >> 32) +
              ((raw & RH_DEC_MASK) >> 24) +
              ((raw & TEMP_INT_MASK) >> 16) +
              ((raw & TEMP_DEC_MASK) >> 8);
    return (sum & 0xFF) == checksum;
}
