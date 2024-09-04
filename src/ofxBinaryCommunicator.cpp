#include "ofxBinaryCommunicator.h"

ofxBinaryCommunicator::ofxBinaryCommunicator()
    : state(ReceiveState::WaitingForHeader), receivedChecksum(0), topicId(0), packetLength(0), receivedLength(0),
      onReceived(nullptr), onError(nullptr), onEndPacket(nullptr), initialized(false) {}

ofxBinaryCommunicator::~ofxBinaryCommunicator() {
    #ifdef OF_VERSION_MAJOR
    if (serial.isInitialized()) {
        serial.close();
    }
    #endif
}

#ifdef OF_VERSION_MAJOR
void ofxBinaryCommunicator::setup(const string& port, int baudRate) {
    serial.setup(port, baudRate);
    initialized = serial.isInitialized();
}
#else
void ofxBinaryCommunicator::setup(HardwareSerial& serialDevice, int baudRate) {
    serial = &serialDevice;
    serialDevice.begin(baudRate);
    initialized = true;
}

void ofxBinaryCommunicator::setup(Stream& serialDevice) {
    serial = &serialDevice;
    initialized = true;
}
#endif

void ofxBinaryCommunicator::update() {
    while (initialized && serial.available()) {
        int incomingByte;
        #ifdef OF_VERSION_MAJOR
        incomingByte = serial.readByte();
        if (incomingByte == OF_SERIAL_NO_DATA) break;
        #else
        incomingByte = serial->read();
        #endif

        uint8_t byte = static_cast<uint8_t>(incomingByte);

        switch (state) {
            case ReceiveState::WaitingForHeader:
                if (incomingByte == PacketHeader) {
                    state = ReceiveState::ReceivingChecksum;
                    receivedChecksum = 0;
                    receivedLength = 0;
                } else if (incomingByte == PacketEnd) {
                    if (onEndPacket) {
                        onEndPacket(endPacketUserData);
                    }
                } else {
                    if (onError) {
                        if (onError) {
                            onError(errorUserData, ErrorType::BufferOverflow, receivedData, receivedLength);
                        }
                    }
                }
                break;

            case ReceiveState::ReceivingChecksum:
                receivedChecksum = (receivedChecksum << 8) | byte;
                if (receivedLength == 1) {
                    state = ReceiveState::ReceivingTopicId;
                    topicId = 0;
                    receivedLength = 0;
                } else {
                    receivedLength++;
                }
                break;

            case ReceiveState::ReceivingTopicId:
                topicId = (topicId << 8) | byte;
                if (receivedLength == 1) {
                    state = ReceiveState::ReceivingLength;
                    packetLength = 0;
                    receivedLength = 0;
                } else {
                    receivedLength++;
                }
                break;

            case ReceiveState::ReceivingLength:
                packetLength = (packetLength << 8) | byte;
                if (receivedLength == 1) {
                    state = ReceiveState::ReceivingData;
                    receivedLength = 0;
                    if (packetLength > MAX_PACKET_SIZE) {
                        if (onError) {
                            onError(errorUserData, ErrorType::BufferOverflow, receivedData, receivedLength);
                        }
                        state = ReceiveState::WaitingForHeader;
                    }
                } else {
                    receivedLength++;
                }
                break;

            case ReceiveState::ReceivingData:
                if (byte == PacketEscape) {
                    state = ReceiveState::ReceivingEscape;
                } else {
                    receivedData[receivedLength++] = byte;
                    if (receivedLength == packetLength) {
                        packetReceived();
                        state = ReceiveState::WaitingForHeader;
                    } else if (receivedLength > packetLength) {
                        if (onError) {
                            onError(errorUserData, ErrorType::BufferOverflow, receivedData, receivedLength);
                        }
                        state = ReceiveState::WaitingForHeader;
                    }
                }
                break;

            case ReceiveState::ReceivingEscape:
                receivedData[receivedLength++] = byte;
                if (receivedLength == packetLength) {
                    packetReceived();
                    state = ReceiveState::WaitingForHeader;
                } else if (receivedLength > packetLength) {
                    if (onError) {
                        onError(errorUserData, ErrorType::BufferOverflow, receivedData, receivedLength);
                    }
                    state = ReceiveState::WaitingForHeader;
                } else {
                    state = ReceiveState::ReceivingData;
                }
                break;
        }
    }
}

void ofxBinaryCommunicator::sendPacket(uint16_t topicId, const uint8_t* data, size_t length) {
    sendByte(PacketHeader);

    uint16_t checksum = calculateChecksum(data, length);
    sendByte(checksum >> 8);
    sendByte(checksum & 0xFF);

    sendByte(topicId >> 8);
    sendByte(topicId & 0xFF);

    sendByte(length >> 8);
    sendByte(length & 0xFF);

    for (size_t i = 0; i < length; ++i) {
        if (data[i] == PacketHeader || data[i] == PacketEscape || data[i] == PacketEnd) {
            sendByte(PacketEscape);
        }
        sendByte(data[i]);
    }
}

void ofxBinaryCommunicator::sendEndPacket() {
    sendByte(PacketEnd);
}

bool ofxBinaryCommunicator::packetReceived() {
    uint16_t calculatedChecksum = calculateChecksum(receivedData, packetLength);
    if (calculatedChecksum == receivedChecksum) {
        if (onReceived) {
            onReceived(receivedUserData, topicId, receivedData, packetLength);
        }
        return true;
    } else {
        if (onError) {
            onError(errorUserData, ErrorType::ChecksumMismatch, receivedData, receivedLength);
        }
        return false;
    }
}

void ofxBinaryCommunicator::sendByte(uint8_t byte) {
    #ifdef OF_VERSION_MAJOR
    serial.writeByte(byte);
    #else
    serial->write(byte);
    #endif
}

uint16_t ofxBinaryCommunicator::calculateChecksum(const uint8_t* data, size_t length) {
    uint16_t checksum = 0;
    for (size_t i = 0; i < length; ++i) {
        checksum += data[i];
    }
    return checksum;
}
