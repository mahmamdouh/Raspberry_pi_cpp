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

    // Set the pin HIGH or LOW
    void write(bool value);

    // Read the state of the pin
    bool read();

    // Blink the pin (for testing purposes)
    void blink(int durationMs, int intervalMs);
};

#endif // GPIO_H
