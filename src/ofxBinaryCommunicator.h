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

// Packet data struct
struct ofxBinaryPacket {
    uint16_t topicId;
    size_t length;
    const uint8_t* data;

    ofxBinaryPacket(uint16_t _topicId, size_t _length, const uint8_t* _data)
        : topicId(_topicId), length(_length), data(_data) {}
};

class ofxBinaryCommunicator {
public:
    // Error types that can occur during communication
    enum class ErrorType {
        ChecksumMismatch,
        IncompletePacket,
        BufferOverflow,
        UnexpectedHeader
    };

    ofxBinaryCommunicator();
    ~ofxBinaryCommunicator();

    // Setup method to initialize the communicator
    #ifdef OF_VERSION_MAJOR
    void setup(const string& port, int baudRate);
    #else
    void setup(HardwareSerial& serialDevice, int baudRate);
    void setup(Stream& serialDevice);
    #endif

    void update();

#ifdef OF_VERSION_MAJOR
    // callback for openFrameworks
    ofEvent<const ofxBinaryPacket> onReceived;
    ofEvent<ErrorType> onError;
    ofEvent<void> onEndPacket;
#else
    // callback for Arduino
    typedef void (*ReceivedCallback)(const ofxBinaryPacket& packet);
    typedef void (*ErrorCallback)(ErrorType errorType);
    typedef void (*EndPacketCallback)();

    // Arduino specific methods to set callbacks
    void setReceivedCallback(ReceivedCallback callback) { onReceived = callback; }
    void setErrorCallback(ErrorCallback callback) { onError = callback; }
    void setEndPacketCallback(EndPacketCallback callback) { onEndPacket = callback; }
#endif
    
    bool isInitialized() const { return initialized; }

    void sendEndPacket();
    void sendPacket(const ofxBinaryPacket& packet);

    // Helper template function for sending typed data
    template<typename T>
    void sendPacket(uint16_t topicId, const T& data) {
        sendPacket(ofxBinaryPacket(topicId, sizeof(T), reinterpret_cast<const uint8_t*>(&data)));
    }

    // Helper template function for parsing received data
    template<typename T>
    static bool parse(const ofxBinaryPacket packet, T& out) {
        if (packet.length != sizeof(T)) return false;
        memcpy(&out, packet.data, sizeof(T));
        return true;
    }

private:

    #ifdef OF_VERSION_MAJOR
    ofSerial* serial = nullptr;
    #else
    Stream* serial;

    // Arduino specific callback function pointers
    ReceivedCallback onReceived;
    ErrorCallback onError;
    EndPacketCallback onEndPacket;
    #endif
    
    // Private methods to handle different aspects of communication
    void processIncomingByte(uint8_t incomingByte);
    bool packetReceived();
    void sendByte(uint8_t byte);
    uint16_t calculateChecksum(const uint8_t* data, size_t length);

    // Methods to notify callbacks/events (implementation differs between platforms)
    void notifyReceived(const ofxBinaryPacket& packet);
    void notifyError(ErrorType errorType);
    void notifyEndPacket();
    
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
};
