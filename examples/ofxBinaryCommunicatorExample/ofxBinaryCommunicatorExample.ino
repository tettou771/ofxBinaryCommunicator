#include <ofxBinaryCommunicator.h>

#define ERROR_LED_PIN 13  // Arduinoのビルトインバイナリが分かる場合に変更してください

struct SampleSensorData {
    static const uint8_t topicId = 0; // Should not conflict to other
    int32_t timestamp;
    int sensorValue;
};

struct SampleMouseData {
    static const uint8_t topicId = 1;
    int32_t timestamp;
    int x;
    int y;
};

struct SampleKeyData {
    static const uint8_t topicId = 2;
    int32_t timestamp;
    char key;
};

struct SampleMessageData {
    static const uint8_t topicId = 3;
    char message[30];
};

ofxBinaryCommunicator communicator;

void onMessageReceived(const ofxBinaryPacket& packet) {
  switch (packet.topicId) {
    case SampleMouseData::topicId: {
      SampleMessageData msgData;
      strcpy(msgData.message, "got mouse data");
      communicator.send(msgData);
    } break;
    case SampleKeyData::topicId: {
      SampleMessageData msgData;
      strcpy(msgData.message, "got key data");
      communicator.send(msgData);
    } break;
    case SampleMessageData::topicId: {
      SampleMessageData msgData;
      strcpy(msgData.message, "got message data");
      communicator.send(msgData);
    } break;
  }
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

  Serial.begin(115200);
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
    communicator.send(sensorData);
    lastSensorSend = millis();
  }
}