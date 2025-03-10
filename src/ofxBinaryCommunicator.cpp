#include "ofxBinaryCommunicator.h"

// Constructor
ofxBinaryCommunicator::ofxBinaryCommunicator() : serial(nullptr) {
    state = ReceiveState::WaitingForHeader;
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
    initialized = serial->isInitialized();
}
#else
void ofxBinaryCommunicator::setup(Stream& serialStream) {
    serial = &serialStream;
    initialized = true;
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

void ofxBinaryCommunicator::sendPacket(const ofxBinaryPacket& packet) {
    sendByte(PacketHeader);

    uint16_t checksum = calculateChecksum(packet.data, packet.length);
    // 2 bytes
    sendByte(checksum >> 8);
    sendByte(checksum & 0xFF);

    sendByte(packet.topicId);

    // 2 bytes
    sendByte(packet.length >> 8);
    sendByte(packet.length & 0xFF);

    for (uint16_t i = 0; i < packet.length; ++i) {
        if (packet.data[i] == PacketHeader || packet.data[i] == PacketEscape) {
            sendByte(PacketEscape);
        }
        sendByte(packet.data[i]);
    }
}

// Process each incoming byte
void ofxBinaryCommunicator::processIncomingByte(uint8_t byte) {
    switch (state) {
        case ReceiveState::WaitingForHeader:
            if (byte == PacketHeader) {
                state = ReceiveState::ReceivingChecksum;
                receivedChecksum = 0;
                receivedLength = 0;
            } else {
                // 無視してゴミbyteを捨てる
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
            topicId = byte;
            state = ReceiveState::ReceivingLength;
            packetLength = 0;
            receivedLength = 0;
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
            } else if (byte == PacketHeader) {
                // 未エスケープのPacketHeaderを受信した場合
                // 今読んでいたパケットは不完全で捨てる(エラーとして扱うなら notifyError も呼ぶ)
                notifyError(ErrorType::UnexpectedHeader);

                // 新しいパケットの先頭(ヘッダ)が来たとみなして、最初から受信やり直し
                state = ReceiveState::ReceivingChecksum;
                receivedChecksum = 0;
                receivedLength   = 0;
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
            if (byte == PacketHeader || byte == PacketEscape) {
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
            } else {
                // 不正なエスケープシーケンス
                notifyError(ErrorType::UnknownError);
                state = ReceiveState::WaitingForHeader;
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

uint16_t ofxBinaryCommunicator::calculateChecksum(const uint8_t* data, uint16_t length) {
    // 16bit Fletcher's Checksum
    uint8_t sum1 = 0xff;
    uint8_t sum2 = 0xff;
    
    while (length--) {
        sum1 += *data++;
        sum2 += sum1;
    }
    
    return (sum2 << 8) | sum1;
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
