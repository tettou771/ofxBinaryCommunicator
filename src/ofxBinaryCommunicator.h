#pragma once

#ifndef PacketHeader
#define PacketHeader 0x99
#endif

#ifndef PacketEscape
#define PacketEscape 0x98
#endif

#define PacketEnd 0x97

#include <cstdint>
#include <functional>

#ifdef OF_VERSION_MAJOR
#include "ofMain.h"
#else
#include <Arduino.h>
#include <SoftwareSerial.h>
#endif

template<typename T>
union ofxBinaryPacket {
    T data;
    uint8_t bytes[1];
};

template<typename T>
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
    void setup(const string& port, int baudRate) {
        serial.setup(port, baudRate);
        initialized = serial.isInitialized();
    }
    #else
    void setup(HardwareSerial& serialDevice, int baudRate) {
        serial = &serialDevice;
        serialDevice.begin(baudRate);
        initialized = true;
    }

    void setup(SoftwareSerial& serialDevice, int baudRate) {
        serial = &serialDevice;
        serialDevice.begin(baudRate);
        initialized = true;
    }

    void setup(Stream& serialDevice, int baudRate) {
        serial = &serialDevice;
        initialized = true;
        // Stream クラスには begin メソッドがないので、何もしない
    }
    #endif

    void update();
    void sendPacket(const ofxBinaryPacket<T>& packet);
    void sendPacket(const ofxBinaryPacket<T>& packet, size_t totalSize);
    void sendEndPacket();

    std::function<void(const ofxBinaryPacket<T>&, size_t)> onReceived;
    std::function<void()> onBinaryEnd;
    std::function<void(ErrorType, const uint8_t*, size_t)> onError;

    bool isInitialized() {
        return initialized;
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
    bool initialized = false;

    enum class ReceiveState {
        WaitingForHeader,
        ReceivingChecksum,
        ReceivingLength,
        ReceivingData,
        ReceivingEscape
    };

    ReceiveState state;
    uint16_t receivedChecksum;
    uint16_t packetLength;
    uint16_t receivedLength;
    static const size_t MAX_PACKET_SIZE = sizeof(T) + 100;
    uint8_t receivedData[MAX_PACKET_SIZE];
};

// テンプレートの実装
template<typename T>
ofxBinaryCommunicator<T>::ofxBinaryCommunicator() : state(ReceiveState::WaitingForHeader), receivedChecksum(0), packetLength(0), receivedLength(0) {}

template<typename T>
ofxBinaryCommunicator<T>::~ofxBinaryCommunicator() {
    #ifdef OF_VERSION_MAJOR
    if (serial.isInitialized()) {
        serial.close();
    }
    #endif
}

template<typename T>
void ofxBinaryCommunicator<T>::update() {
#ifdef OF_VERSION_MAJOR
    while (serial.available()) {
        uint8_t incomingByte = serial.readByte();
#else
    while (serial->available()) {
        uint8_t incomingByte = serial->read();
#endif
            
        switch (state) {
            case ReceiveState::WaitingForHeader:
                if (incomingByte == PacketHeader) {
                    state = ReceiveState::ReceivingChecksum;
                    receivedChecksum = 0;
                    receivedLength = 0;
                } else if (incomingByte == PacketEnd) {
                    if (onBinaryEnd) onBinaryEnd();
                } else {
                    if (onError) onError(ErrorType::UnexpectedHeader, &incomingByte, 1);
                }
                break;
                
            case ReceiveState::ReceivingChecksum:
                receivedChecksum = (receivedChecksum << 8) | incomingByte;
                if (receivedLength == 1) {
                    state = ReceiveState::ReceivingLength;
                    packetLength = 0;
                    receivedLength = 0;
                } else {
                    receivedLength++;
                }
                break;
                
            case ReceiveState::ReceivingLength:
                packetLength = (packetLength << 8) | incomingByte;
                if (receivedLength == 1) {
                    state = ReceiveState::ReceivingData;
                    receivedLength = 0;
                    if (packetLength > MAX_PACKET_SIZE) {
                        if (onError) onError(ErrorType::BufferOverflow, receivedData, receivedLength);
                        state = ReceiveState::WaitingForHeader;
                    }
                } else {
                    receivedLength++;
                }
                break;
                
            case ReceiveState::ReceivingData:
                if (incomingByte == PacketEscape) {
                    state = ReceiveState::ReceivingEscape;
                } else {
                    receivedData[receivedLength++] = incomingByte;
                    if (receivedLength == packetLength) {
                        packetReceived();
                        state = ReceiveState::WaitingForHeader;
                    } else if (receivedLength > packetLength) {
                        if (onError) onError(ErrorType::BufferOverflow, receivedData, receivedLength);
                        state = ReceiveState::WaitingForHeader;
                    }
                }
                break;
                
            case ReceiveState::ReceivingEscape:
                receivedData[receivedLength++] = incomingByte;
                if (receivedLength == packetLength) {
                    packetReceived();
                    state = ReceiveState::WaitingForHeader;
                } else if (receivedLength > packetLength) {
                    if (onError) onError(ErrorType::BufferOverflow, receivedData, receivedLength);
                    state = ReceiveState::WaitingForHeader;
                } else {
                    state = ReceiveState::ReceivingData;
                }
                break;
        }
    }
};

template<typename T>
void ofxBinaryCommunicator<T>::sendPacket(const ofxBinaryPacket<T>& packet) {
    sendPacket(packet, sizeof(T));
}

template<typename T>
void ofxBinaryCommunicator<T>::sendPacket(const ofxBinaryPacket<T>& packet, size_t totalSize) {
    sendByte(PacketHeader);

    uint16_t checksum = calculateChecksum(packet.bytes, totalSize);
    sendByte(checksum >> 8);
    sendByte(checksum & 0xFF);

    sendByte(totalSize >> 8);
    sendByte(totalSize & 0xFF);

    for (size_t i = 0; i < totalSize; ++i) {
        if (packet.bytes[i] == PacketHeader || packet.bytes[i] == PacketEscape || packet.bytes[i] == PacketEnd) {
            sendByte(PacketEscape);
        }
        sendByte(packet.bytes[i]);
    }
}

template<typename T>
void ofxBinaryCommunicator<T>::sendEndPacket() {
    sendByte(PacketEnd);
}

template<typename T>
bool ofxBinaryCommunicator<T>::packetReceived() {
    uint16_t calculatedChecksum = calculateChecksum(receivedData, packetLength);
    if (calculatedChecksum == receivedChecksum) {
        ofxBinaryPacket<T> receivedPacket;
        memcpy(&receivedPacket, receivedData, std::min(sizeof(T), (size_t)packetLength));
        if (onReceived) onReceived(receivedPacket, packetLength);
        return true;
    } else {
        if (onError) onError(ErrorType::ChecksumMismatch, receivedData, receivedLength);
        return false;
    }
}

template<typename T>
void ofxBinaryCommunicator<T>::sendByte(uint8_t byte) {
    #ifdef OF_VERSION_MAJOR
    serial.writeByte(byte);
    #else
    serial->write(byte);
    #endif
}

template<typename T>
uint16_t ofxBinaryCommunicator<T>::calculateChecksum(const uint8_t* data, size_t length) {
    uint16_t checksum = 0;
    for (size_t i = 0; i < length; ++i) {
        checksum += data[i];
    }
    return checksum;
}
