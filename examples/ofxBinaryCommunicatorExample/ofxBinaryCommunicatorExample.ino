#include <ofxBinaryCommunicator.h>

struct SampleData {
    int32_t timestamp;
    int x;
    int y;
    char message[30];
};

ofxBinaryCommunicator<SampleData> communicator;

void sendMessage(int x, int y, const char* msg) {
    ofxBinaryPacket<SampleData> packet;
    packet.data.timestamp = millis();
    packet.data.x = x;
    packet.data.y = y;
    strncpy(packet.data.message, msg, sizeof(packet.data.message) - 1);
    packet.data.message[sizeof(packet.data.message) - 1] = '\0';  // Ensure null-termination

    communicator.sendPacket(packet);
}

void setup() {
    Serial.begin(115200);
    communicator.setup(Serial, 115200);

    communicator.onReceived = [](const ofxBinaryPacket<SampleData>& packet, size_t size) {
        SampleData data = packet.data;
        
        // Echo back
        sendMessage(data.x, data.y, data.message);
    };

    communicator.onError = [](ofxBinaryCommunicator<SampleData>::ErrorType errorType, const uint8_t* data, size_t length) {
        const char* errorMsg;
        switch (errorType) {
            case ofxBinaryCommunicator<SampleData>::ErrorType::ChecksumMismatch:
                errorMsg = "Checksum mismatch";
                break;
            case ofxBinaryCommunicator<SampleData>::ErrorType::IncompletePacket:
                errorMsg = "Incomplete packet";
                break;
            case ofxBinaryCommunicator<SampleData>::ErrorType::BufferOverflow:
                errorMsg = "Buffer overflow";
                break;
            case ofxBinaryCommunicator<SampleData>::ErrorType::UnexpectedHeader:
                errorMsg = "Unexpected header";
                break;
            default:
                errorMsg = "Unknown error";
                break;
        }
        sendMessage(0, 0, errorMsg);
    };
}

void loop() {
    communicator.update();
}