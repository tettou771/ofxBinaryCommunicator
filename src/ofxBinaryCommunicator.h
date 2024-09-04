#pragma once

// You can overwrite this value
#ifndef MAX_PACKET_SIZE
#define MAX_PACKET_SIZE 256 // packet size max
#endif

#ifndef PacketHeader
#define PacketHeader 0x99
#endif

#ifndef PacketEscape
#define PacketEscape 0x98
#endif

#define PacketEnd 0x97

#include <stdint.h>
#include <string.h>

#if !defined(OF_VERSION_MAJOR) && !defined(ARDUINO)
    #include "ofMain.h"
#endif

#ifndef OF_VERSION_MAJOR
    #include <Arduino.h>
    #ifdef USBCON
        #include <SoftwareSerial.h>
    #endif
#endif

class ofxBinaryCommunicator {
public:
    enum class ErrorType {
        ChecksumMismatch,
        IncompletePacket,
        BufferOverflow,
        UnexpectedHeader
    };

    ofxBinaryCommunicator();
    ~ofxBinaryCommunicator();

    #ifdef OF_VERSION_MAJOR
    void setup(const string& port, int baudRate);
    #else
    void setup(HardwareSerial& serialDevice, int baudRate);
    void setup(Stream& serialDevice);
    #endif

    void update();
    void sendPacket(uint16_t topicId, const uint8_t* data, size_t length);
    void sendEndPacket();

    typedef void (*ReceivedCallback)(void* userData, uint16_t topicId, const uint8_t* data, size_t length);
    typedef void (*ErrorCallback)(void* userData, ErrorType errorType, const uint8_t* data, size_t length);
    typedef void (*EndPacketCallback)(void* userData);

    void setReceivedCallback(ReceivedCallback callback, void* userData) {
        onReceived = callback;
        receivedUserData = userData;
    }
    void setErrorCallback(ErrorCallback callback, void* userData) {
        onError = callback;
        errorUserData = userData;
    }
    void setEndPacketCallback(EndPacketCallback callback, void* userData) {
        onEndPacket = callback;
        endPacketUserData = userData;
    }

    bool isInitialized() const { return initialized; }

    // Helper template function for sending typed data
    template<typename T>
    void sendPacket(uint16_t topicId, const T& data) {
        sendPacket(topicId, reinterpret_cast<const uint8_t*>(&data), sizeof(T));
    }

    // Helper template function for parsing received data
    template<typename T>
    static bool parse(const uint8_t* data, size_t length, T& out) {
        if (length != sizeof(T)) return false;
        memcpy(&out, data, sizeof(T));
        return true;
    }

private:
    bool packetReceived();
    void sendByte(uint8_t byte);
    uint16_t calculateChecksum(const uint8_t* data, size_t length);

    #ifdef OF_VERSION_MAJOR
    ofSerial serial;
    #else
    Stream* serial;
    #endif
    bool initialized;

    enum class ReceiveState {
        WaitingForHeader,
        ReceivingChecksum,
        ReceivingTopicId,
        ReceivingLength,
        ReceivingData,
        ReceivingEscape
    };

    ReceiveState state;
    uint16_t receivedChecksum;
    uint16_t topicId;
    uint16_t packetLength;
    uint16_t receivedLength;
    uint8_t receivedData[MAX_PACKET_SIZE];

    ReceivedCallback onReceived;
    void* receivedUserData;
    ErrorCallback onError;
    void* errorUserData;
    EndPacketCallback onEndPacket;
    void* endPacketUserData;
};
