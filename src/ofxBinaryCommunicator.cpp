#include "ofxBinaryCommunicator.h"

// Constructor
ofxBinaryCommunicator::ofxBinaryCommunicator() : serial(nullptr) {
    // Initialize other members if needed
}

// Destructor
ofxBinaryCommunicator::~ofxBinaryCommunicator() {
    #ifdef OF_VERSION_MAJOR
    if (serial != nullptr) {
        delete serial;
        serial = nullptr;
    }
    #endif
}

// Setup method
#ifdef OF_VERSION_MAJOR
void ofxBinaryCommunicator::setup(const std::string& portName, int baudRate) {
    if (serial == nullptr) {
        serial = new ofSerial();
    }
    serial->setup(portName, baudRate);
}
#else
void ofxBinaryCommunicator::setup(Stream& serialStream) {
    serial = &serialStream;
}
#endif

// Update method to process incoming data
void ofxBinaryCommunicator::update() {
    #ifdef OF_VERSION_MAJOR
    if (serial != nullptr && serial->isInitialized()) {
        while (serial->available() > 0) {
            uint8_t incomingByte = serial->readByte();
            processIncomingByte(incomingByte);
        }
    }
    #else
    while (serial->available() > 0) {
        uint8_t incomingByte = serial->read();
        processIncomingByte(incomingByte);
    }
    #endif
}

void ofxBinaryCommunicator::sendEndPacket() {
    sendByte(PacketEnd);
}

void ofxBinaryCommunicator::sendBinaryPacket(const ofxBinaryPacket& packet) {
    sendByte(PacketHeader);

    uint16_t checksum = calculateChecksum(packet.data, packet.length);
    sendByte(checksum >> 8);
    sendByte(checksum & 0xFF);

    sendByte(topicId >> 8);
    sendByte(topicId & 0xFF);

    sendByte(packet.length >> 8);
    sendByte(packet.length & 0xFF);

    for (size_t i = 0; i < packet.length; ++i) {
        if (packet.data[i] == PacketHeader || packet.data[i] == PacketEscape || packet.data[i] == PacketEnd) {
            sendByte(PacketEscape);
        }
        sendByte(packet.data[i]);
    }
}

// Process each incoming byte
void ofxBinaryCommunicator::processIncomingByte(uint8_t incomingByte) {
    uint8_t byte = static_cast<uint8_t>(incomingByte);

    switch (state) {
        case ReceiveState::WaitingForHeader:
            if (incomingByte == PacketHeader) {
                state = ReceiveState::ReceivingChecksum;
                receivedChecksum = 0;
                receivedLength = 0;
            } else if (incomingByte == PacketEnd) {
                notifyEndPacket();
            } else {
                notifyError(ErrorType::BufferOverflow);
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
                    notifyError(ErrorType::BufferOverflow);
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
                    notifyError(ErrorType::BufferOverflow);
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
                notifyError(ErrorType::BufferOverflow);
                state = ReceiveState::WaitingForHeader;
            } else {
                state = ReceiveState::ReceivingData;
            }
            break;
    }
}

// Handle a fully received packet
bool ofxBinaryCommunicator::packetReceived() {
    uint16_t calculatedChecksum = calculateChecksum(receivedData, packetLength);
    if (calculatedChecksum == receivedChecksum) {
        notifyReceived(ofxBinaryPacket(topicId, receivedLength, receivedData));
        return true;
    } else {
        notifyError(ErrorType::ChecksumMismatch);
        return false;
    }}

// Send a single byte
void ofxBinaryCommunicator::sendByte(uint8_t byte) {
    #ifdef OF_VERSION_MAJOR
    if (serial != nullptr && serial->isInitialized()) {
        serial->writeByte(byte);
    }
    #else
    serial->write(byte);
    #endif
}

// Calculate checksum for data integrity
uint16_t ofxBinaryCommunicator::calculateChecksum(const uint8_t* data, size_t length) {
    // TODO: Implement your checksum calculation here
    // This is a placeholder implementation. Replace with your actual checksum algorithm.
    uint16_t checksum = 0;
    for (size_t i = 0; i < length; i++) {
        checksum += data[i];
    }
    return checksum;
}

// Notify methods for platform-specific callback/event handling
void ofxBinaryCommunicator::notifyReceived(const ofxBinaryPacket& packet) {
#ifdef OF_VERSION_MAJOR
    ofNotifyEvent(onReceived, packet);
#else
    if (onReceived) {
        onReceived(packet);
    }
#endif
}

void ofxBinaryCommunicator::notifyError(ErrorType errorType) {
#ifdef OF_VERSION_MAJOR
    ofNotifyEvent(onError, errorType);
#else
    if (onError) {
        onError(errorType);
    }
#endif
}

void ofxBinaryCommunicator::notifyEndPacket() {
    #ifdef OF_VERSION_MAJOR
    ofNotifyEvent(onEndPacket);
    #else
    if (onEndPacket) {
        onEndPacket();
    }
    #endif
}
