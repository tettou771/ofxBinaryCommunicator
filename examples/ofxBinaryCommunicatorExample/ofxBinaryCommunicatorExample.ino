#include <ofxBinaryCommunicator.h>

#define ERROR_LED_PIN 13

//---- Common definision for Arduino and openFrameworks ----//
TOPIC_STRUCT_MAKER(SampleSensorData, 0, // Should not conflict to other
    int32_t timestamp;
    int sensorValue;
)

TOPIC_STRUCT_MAKER(SampleMouseData, 1,
    int32_t timestamp;
    int x;
    int y;
)

TOPIC_STRUCT_MAKER(SampleKeyData, 2,
    int32_t timestamp;
    char key;
)

TOPIC_STRUCT_MAKER( SampleMessageData, 3,
    char message[30];
)
//---- Common definision for Arduino and openFrameworks end ----//

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

void setup() {
  pinMode(ERROR_LED_PIN, OUTPUT);

  Serial.begin(115200);
  communicator.setup(Serial);

  communicator.setReceivedCallback(onMessageReceived);
  communicator.setErrorCallback(onError);
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

    // predefined struct: OscLikeMessage (High overhead)
    OscLikeMessage msg;
    msg.setAddress("/sensor/value"); // Max 32 char
    msg.addInt32Arg(millis());
    msg.addFloatArg(analogRead(A0) / 1024.0);
    msg.addCharArg('A'); // 1byte char data
    msg.addChar4Arg("SENS"); // 4byte char data
    OscLikeMessage::Color color;
    color.r = 0xC0;
    color.g = 0xFF;
    color.b = 0xEE;
    color.a = 0xFF;
    msg.addColorArg(color);
    msg.addBoolArg(true);
    communicator.send(msg);

    lastSensorSend = millis();
  }
}