#include "GPIO.h"
#include <pigpio.h>
#include <stdexcept> // For exception handling
#include <thread>    // For delays
#include <chrono>    // For chrono::milliseconds

// Constructor
GPIO::GPIO(int gpioPin, bool output) : pin(gpioPin), isOutput(output) {
    if (gpioInitialise() < 0) {
        throw std::runtime_error("Failed to initialize pigpio");
    }

    // Set pin mode
    if (isOutput) {
        gpioSetMode(pin, PI_OUTPUT);
    } else {
        gpioSetMode(pin, PI_INPUT);
    }
}

// Destructor
GPIO::~GPIO() {
    gpioTerminate(); // Cleanup pigpio
}

// Write to the pin (HIGH or LOW)
void GPIO::write(bool value) {
    if (!isOutput) {
        throw std::runtime_error("Cannot write to an input pin");
    }
    gpioWrite(pin, value ? PI_HIGH : PI_LOW);
}

// Read the pin state
bool GPIO::read() {
    if (isOutput) {
        throw std::runtime_error("Cannot read from an output pin");
    }
    return gpioRead(pin);
}

// Blink the pin
void GPIO::blink(int durationMs, int intervalMs) {
    if (!isOutput) {
        throw std::runtime_error("Cannot blink an input pin");
    }
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::steady_clock::now() - start)
               .count() < durationMs) {
        write(true);
        std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
        write(false);
        std::this_thread::sleep_for(std::chrono::milliseconds(intervalMs));
    }
}
