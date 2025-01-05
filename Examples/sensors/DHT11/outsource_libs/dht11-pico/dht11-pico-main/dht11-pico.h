#ifndef DHT11_H
#define DHT11_H

#include <cstdint>

#define RH_INT_MASK     0xFF00000000
#define RH_DEC_MASK     0x00FF000000
#define TEMP_INT_MASK   0x0000FF0000
#define TEMP_DEC_MASK   0x000000FF00
#define CHECKSUM_MASK   0x00000000FF
#define TRANSMISSION_ERROR -999.0
#define POLLING_LIMIT   10000
#define THRESHOLD       30

class Dht11 {
private:
    int gpioPin;

    // Helper functions
    bool waitForSignal(int level, int timeoutUs);
    bool validateChecksum(long long raw);

public:
    Dht11(int pin);
    ~Dht11();

    long long read();
    double readT();
    double readRH();
    void readRHT(double* temp, double* rh);
};

#endif // DHT11_H
