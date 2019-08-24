#include "OBDSerialComm.h"


OBDSerialComm::OBDSerialComm(uint32_t baudRate, uint8_t rxPin, uint8_t txPin) {
    setBaudRate(baudRate);
    serial = new SoftwareSerial(rxPin, txPin);
    serial->begin(getBaudRate());
    serial->setTimeout(SERIAL_READ_TIMEOUT);
    setToDefaults();
}

OBDSerialComm::~OBDSerialComm() {
    operator delete(serial);
}


void OBDSerialComm::writeEnd() {

    // 1 - write carriage return
    //    writeTo(0x0D);
    writeTo('\n');
    //    writeTo(13);


    // 2- (optional ) write linefeed
    if (lineFeedEnable) {
        writeTo('\r');
        // writeTo(10);
    }

    // 3 - Write prompt
    //    writeTo(0x3E);
    writeTo('>');

    serial->flush();
};


void OBDSerialComm::writeEndOK() {
    writeTo(F("OK"));
    writeEnd();
}

void OBDSerialComm::writeEndERROR() {
    writeTo(F("ERROR"));
    writeEnd();
}

void OBDSerialComm::writeEndNoData() {
    writeTo(F("NO DATA"));
    writeEnd();
}

void OBDSerialComm::writeEndUnknown() {
    writeTo(F("?"));
    writeEnd();
}

void OBDSerialComm::setToDefaults() {
    setEcho(true);
    setStatus(READY);
    setWhiteSpaces(true);
    setHeaders(false);
    setLineFeeds(true);
    setMemory(false);
}

void OBDSerialComm::writeTo(uint8_t byte) {
	//DEBUGW(byte);
    serial->write(byte);
}

void OBDSerialComm::writeTo(char cChar) {
	//DEBUGW(cChar);
    serial->write(cChar);
}

void OBDSerialComm::writeTo(char const *response) {
	//DEBUGW(response);
    serial->write(response);
}

void OBDSerialComm::writeTo(String response) {
    //DEBUGW(response.c_str());
    serial->write(response.c_str());
    /*for (uint8_t i = 0; i < response.length(); i++) {
        //DEBUGW(response[i]);
        // Push each char 1 by 1 on each loop pass
        serial->write(response[i]);
    }*/
}

void OBDSerialComm::writeEndPidTo(uint8_t const *response, uint8_t dataLen) {
    uint8_t respLen;
    char *resp;
    if(this->headersEnabled)
    {
        //uint8_t len = strlen(response);
        respLen = 4  + dataLen + 1;
        uint8_t data[respLen];
        data[0] = 0x80;                
        data[1] = this->MyAddr;
        data[2] = this->ECUaddr;      
        data[3] = dataLen;
        memcpy(data + 4, response, dataLen);
        memcpy(data + 4 + dataLen, calcChecksum(data, 4 + dataLen), sizeof(uint8_t));
        resp = toHexString(data, respLen);
    } else {
        respLen = dataLen;
        resp = toHexString(response, dataLen);
    }

    if (whiteSpacesEnabled) {
        char spacedResponse[respLen + respLen / 2 + 1];
        addSpacesToResponse(resp, spacedResponse);
        writeTo(resp);
    } else {
        writeTo(resp);
    }
    free(resp);
    writeEnd();
}

String OBDSerialComm::readData() {
    serial->flush(); // temp remove this
    String rxData = serial->readStringUntil(SERIAL_END_CHAR);
	DEBUG("RX < " + rxData);
    if (isEchoEnable()) {
        writeTo(rxData);
    }
    return rxData;
}

void OBDSerialComm::setBaudRate(uint32_t rate) {
    this->boudRate = rate;
}

long OBDSerialComm::getBaudRate() {
    return boudRate;
}

bool OBDSerialComm::isEchoEnable() {
    return echoEnable;
}

void OBDSerialComm::setEcho(bool echo) {
    this->echoEnable = echo;
}

void OBDSerialComm::setStatus(STATUS status) {
    this->status = status;
}

void OBDSerialComm::setLineFeeds(bool status) {
    this->lineFeedEnable = status;
}

void OBDSerialComm::setMemory(bool status) {
    this->memoryEnabled = status;
}

void OBDSerialComm::setWhiteSpaces(bool status) {
    this->whiteSpacesEnabled = status;
}

void OBDSerialComm::setHeaders(bool status) {
    this->headersEnabled = status;
}

void OBDSerialComm::addSpacesToResponse(const char *response, char spacedRes[]) {
    uint8_t len = strlen(response);
    int j = 0;
    for (int i = 0; i < len;) {
        *(spacedRes + j++) = *(response + i++);
        *(spacedRes + j++) = *(response + i++);
        if (i < len) {
            *(spacedRes + j++) = 0x20;
        }
    }
    *(spacedRes + j) = '\0';
}

// Checksum is simply the sum of all data bytes modulo 0xFF
// (same as being truncated to one byte)
uint8_t OBDSerialComm::calcChecksum(uint8_t *data, uint8_t len)
{
  uint8_t crc = 0;

  for (uint8_t i = 0; i < len; i++)
  {
    crc = crc + data[i];
  }
  return crc;
}

char * OBDSerialComm::toHexString(uint8_t *buffer, uint8_t len)
{
    char * asta = (char*)calloc(2 * len * sizeof(uint8_t) + 1, sizeof(char));
    //char asta[2 * len * sizeof(uint8_t) + 1];
    for (int k = 0; k < len; k++)
    {
        sprintf(&asta[2 * k],"%02X", buffer[k]);
    }
    asta[2 * len * sizeof(uint8_t)] = '\0';
    return asta;
}