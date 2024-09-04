#include <ofxBinaryCommunicator.h>

#define ERROR_LED_PIN 13 // Arduinoのビルトインバイナリが分かる場合に変更してください

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

void onMessageReceived(const ofxBinaryPacket& packet) {
    // Echo back all received data
    communicator.sendBinaryPacket(packet);
}

void onError(ofxBinaryCommunicator::ErrorType errorType) {
    // Flash LED to indicate error
    for (int i = 0; i < 5; i++) {
        digitalWrite(ERROR_LED_PIN, HIGH);
        delay(100);
        digitalWrite(ERROR_LED_PIN, LOW);
        delay(100);
    }
}

void onEndPacket() {
    // Do nothing for end packet
}

void setup() {
    pinMode(ERROR_LED_PIN, OUTPUT);
    
    communicator.setup(Serial);

    communicator.setReceivedCallback(onMessageReceived);
    communicator.setErrorCallback(onError);
    communicator.setEndPacketCallback(onEndPacket);
}

void loop() {
    communicator.update();

    // Send sensor data every second
    static unsigned long lastSensorSend = 0;
    if (millis() - lastSensorSend > 1000) {
        SampleSensorData sensorData;
        sensorData.timestamp = millis();
        sensorData.sensorValue = analogRead(A0);
        communicator.sendPacket<SampleSensorData>(0, sensorData);
        lastSensorSend = millis();
    }
}