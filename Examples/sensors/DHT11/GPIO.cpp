#include "GPIO.h"
#include <pigpio.h>
#include <stdexcept> // For exception handling
#include <thread>    // For delays
#include <iostream>  // For debugging (optional)

// Constructor
GPIO::GPIO(int gpioPin, bool output) : pin(gpioPin), isOutput(output) {
    if (gpioInitialise() < 0) {
        throw std::runtime_error("Failed to initialize pigpio");
    }
    configure(output);
}

// Destructor
GPIO::~GPIO() {
    gpioTerminate(); // Cleanup pigpio
}

// Configure the pin as input or output
void GPIO::configure(bool output) {
    isOutput = output;
    gpioSetMode(pin, output ? PI_OUTPUT : PI_INPUT);
}

// Write a digital value to the pin (HIGH or LOW)
void GPIO::write(bool value) {
    if (!isOutput) {
        throw std::runtime_error("Cannot write to an input pin");
    }
    gpioWrite(pin, value ? PI_HIGH : PI_LOW);
}

// Read a digital value from the pin (HIGH or LOW)
bool GPIO::readDigital() {
    if (isOutput) {
        throw std::runtime_error("Cannot read from an output pin");
    }
    return gpioRead(pin);
}

// Read an analog value from an ADC channel (MCP3008 via SPI)
int GPIO::readAnalog(int channel, int spiChannel) {
    if (channel < 0 || channel > 7) {
        throw std::invalid_argument("Channel must be between 0 and 7 for MCP3008");
    }

    // Open SPI channel
    int handle = spiOpen(spiChannel, 50000, 0); // 50kHz, mode 0
    if (handle < 0) {
        throw std::runtime_error("Failed to open SPI channel");
    }

    // MCP3008 SPI communication (3 bytes)
    char tx[3] = {1, static_cast<char>((8 + channel) << 4), 0}; // Start bit + Single/Diff + Channel
    char rx[3] = {0};

    spiXfer(handle, tx, rx, 3);

    // Close SPI channel
    spiClose(handle);

    // Combine the result bytes into a 10-bit value
    int result = ((rx[1] & 3) << 8) + rx[2];

    return result; // Analog value (0-1023)
}
