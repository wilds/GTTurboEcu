#ifndef GTTURBOECU_OBDSerialComm_h
#define GTTURBOECU_OBDSerialComm_h

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "definitions.h"


class OBDSerialComm {
public:
    // ODB parameters
    enum STATUS {
        IDLE = 0, READY = 1
    };

    OBDSerialComm(uint32_t baudRate, uint8_t rxPin, uint8_t txPin);

    ~OBDSerialComm();

    void writeEndOK();

    void writeEndERROR();

	/**
	* example no response in time from command send from elm to ECU, respond no data to user
	*/
    void writeEndNoData();

    /**
     * incomplete command, input not understand and no action taken
     * respond to:  incomplete messages, incorrect AT or invalid HEX digit
     */
    void writeEndUnknown();


    void setToDefaults();

    String readData();

    void writeTo(uint8_t cChar);

    void writeTo(char cChar);

    void writeTo(char const *string);

    void writeTo(String string);

    void setEcho(bool echo);

    void writeEnd();

    bool isEchoEnable();

    void setLineFeeds(bool status);

    void setMemory(bool status);

    void setWhiteSpaces(bool status);

    void setHeaders(bool status);

    void setStatus(STATUS status);

    void writeEndPidTo(uint8_t const *string, uint8_t len);

private:

    // Serial parameters
    SoftwareSerial *serial; // lib to communicate with bluetooth
    uint32_t boudRate; // Serial Boud Rate



    STATUS status; // Operation status
    bool echoEnable; // echoEnable command after received
    bool lineFeedEnable;
    bool memoryEnabled;
    bool whiteSpacesEnabled;
    bool headersEnabled;

    const uint8_t ECUaddr = 0x11;
    const uint8_t MyAddr = 0xF1;

    void setBaudRate(uint32_t rate);

    long getBaudRate();

    void addSpacesToResponse(const char *response, char string[]);

    char * OBDSerialComm::toHexString(uint8_t *buffer, uint8_t len);
    uint8_t calcChecksum(uint8_t *data, uint8_t len);
};


#endif
