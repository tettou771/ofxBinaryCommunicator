#include <ofxBinaryCommunicator.h>

struct SampleMouseData {
    int32_t timestamp;
    int x;
    int y;
    char message[30];
};

struct SampleKeyData {
    int32_t timestamp;
    char key;
};

struct SampleSensorData {
    int32_t timestamp;
    int sensorValue;
};

ofxBinaryCommunicator communicator;

void onMessageReceived(void* userData, uint16_t topicId, const uint8_t* data, size_t length) {
    // Echo back the received data
    communicator.sendPacket(topicId, data, length);

    // You can also parse and use the data if needed
    switch (topicId) {
        case 1: { // SampleMouseData
            SampleMouseData mouseData;
            if (ofxBinaryCommunicator::parse(data, length, mouseData)) {
                Serial.print("Received mouse data - X: ");
                Serial.print(mouseData.x);
                Serial.print(", Y: ");
                Serial.println(mouseData.y);
            }
            break;
        }
        case 2: { // SampleKeyData
            SampleKeyData keyData;
            if (ofxBinaryCommunicator::parse(data, length, keyData)) {
                Serial.print("Received key press: ");
                Serial.println(keyData.key);
            }
            break;
        }
    }
}

void onError(void* userData, ofxBinaryCommunicator::ErrorType errorType, const uint8_t* data, size_t length) {
    Serial.print("Error occurred: ");
    Serial.println(static_cast<int>(errorType));
}

void onEndPacket(void* userData) {
    Serial.println("End packet received");
}

void setup() {
    Serial.begin(115200);
    communicator.setup(Serial);

    communicator.setReceivedCallback(onMessageReceived, nullptr);
    communicator.setErrorCallback(onError, nullptr);
    communicator.setEndPacketCallback(onEndPacket, nullptr);
}

void loop() {
    communicator.update();

    // Send sensor data every second
    static unsigned long lastSensorSend = 0;
    if (millis() - lastSensorSend > 1000) {
        SampleSensorData sensorData;
        sensorData.timestamp = millis();
        sensorData.sensorValue = analogRead(A0);
        uint16_t topicId = 0; // sensor data topic
        communicator.sendPacket<SampleSensorData>(topicId, sensorData);
        lastSensorSend = millis();

        Serial.print("Sent sensor data: ");
        Serial.println(sensorData.sensorValue);
    }
}