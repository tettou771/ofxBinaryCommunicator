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
    uint8_t topicId;
    uint16_t length;
    const uint8_t* data;
    
    ofxBinaryPacket(uint8_t _topicId, uint16_t _length, const uint8_t* _data)
    : topicId(_topicId), length(_length), data(_data) {}
    
    // Helper template function for convert typed data
    template<typename T>
    ofxBinaryPacket(const T& data, decltype(T::topicId)* = 0) 
    : topicId(T::topicId), length(sizeof(T)), data(reinterpret_cast<const uint8_t*>(&data)) {}

    // Helper template function for deserialize received data
    template<typename T>
    bool unpack(T& out, decltype(T::topicId)* = 0) const {
        if (T::topicId != topicId) return false;
        if (length != sizeof(T)) return false;
        memcpy(&out, data, sizeof(T));
        return true;
    }
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
#else
    // callback for Arduino
    typedef void (*ReceivedCallback)(const ofxBinaryPacket& packet);
    typedef void (*ErrorCallback)(ErrorType errorType);

    // Arduino specific methods to set callbacks
    void setReceivedCallback(ReceivedCallback callback) { onReceived = callback; }
    void setErrorCallback(ErrorCallback callback) { onError = callback; }
#endif
    
    bool isInitialized() const { return initialized; }

    void sendPacket(const ofxBinaryPacket& packet);
    template<typename T>
    void send(const T& data, decltype(T::topicId)* = 0) {
        ofxBinaryPacket packet(data);
        sendPacket(packet);
    }

private:

    #ifdef OF_VERSION_MAJOR
    ofSerial* serial = nullptr;
    #else
    Stream* serial;

    // Arduino specific callback function pointers
    ReceivedCallback onReceived;
    ErrorCallback onError;
    #endif
    
    // Private methods to handle different aspects of communication
    void processIncomingByte(uint8_t incomingByte);
    bool packetReceived();
    void sendByte(uint8_t byte);
    uint8_t calculateChecksum(const uint8_t* data, uint16_t length);

    // Methods to notify callbacks/events (implementation differs between platforms)
    void notifyReceived(const ofxBinaryPacket& packet);
    void notifyError(ErrorType errorType);
    
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
    uint8_t receivedChecksum;
    uint8_t topicId;
    uint16_t packetLength;
    uint16_t receivedLength;
    uint8_t receivedData[MAX_PACKET_SIZE];
};
