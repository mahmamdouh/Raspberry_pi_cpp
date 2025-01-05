#ifndef GPIO_H
#define GPIO_H

class GPIO {
private:
    int pin;       // GPIO pin number
    bool isOutput; // Direction: true = output, false = input

public:
    // Constructor
    GPIO(int gpioPin, bool output = true);

    // Destructor
    ~GPIO();

    // Configure the pin as input or output
    void configure(bool output);

    // Write a digital value to the pin (HIGH or LOW)
    void write(bool value);

    // Read a digital value from the pin (HIGH or LOW)
    bool readDigital();

    // Read an analog value from an ADC channel (0-1023 range for MCP3008)
    int readAnalog(int channel, int spiChannel = 0); // channel: ADC channel, spiChannel: SPI bus
};

#endif // GPIO_H
