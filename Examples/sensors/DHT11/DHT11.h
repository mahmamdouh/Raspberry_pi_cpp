#ifndef DHT11_H
#define DHT11_H

#include "../../blink/GPIO.h" // Update the include path for GPIO

#include <stdexcept> // For exception handling
#include <utility>   // For std::pair

class DHT11 {
private:
    GPIO gpio;        // GPIO instance for communication
    int pin;          // GPIO pin connected to the DHT11 sensor

    // Private helper functions
    bool waitForSignal(bool level, int timeoutUs);

    // Checksum validation
    bool validateChecksum(const uint8_t data[5]);

public:
    // Constructor
    DHT11(int gpioPin);

    // Destructor
    ~DHT11();

    // Read temperature and humidity
    std::pair<float, float> read(); // Returns temperature and humidity as a pair

    // Utility to print debug information (optional)
    void printValues();
};

#endif // DHT11_H
