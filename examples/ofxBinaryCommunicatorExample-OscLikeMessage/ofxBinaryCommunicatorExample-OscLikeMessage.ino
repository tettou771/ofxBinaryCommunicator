#include <ofxBinaryCommunicator.h>

#define ERROR_LED_PIN 13

ofxBinaryCommunicator communicator;

void onMessageReceived(const ofxBinaryPacket& packet) {
  switch (packet.topicId) {
    case OscLikeMessage::topicId: {
      OscLikeMessage msg;
      if (packet.unpack(msg)) {
        if (strcmp(msg.getAddress(), "/control/mouse") == 0) {
          // echo message
          OscLikeMessage response;
          response.setAddress("/response");
          response.addChar4Arg("MOUS");
          response.addInt32Arg(msg.getArgAsInt32(0));
          response.addInt32Arg(msg.getArgAsInt32(1));
        }
        else if (strcmp(msg.getAddress(), "/control/key") == 0) {
          // do anything
        }
      }
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
    // predefined struct: OscLikeMessage (High overhead)
    OscLikeMessage msg;
    msg.setAddress("/sensor/value"); // Max 32 char
    msg.addInt32Arg(millis());
    msg.addFloatArg(analogRead(A0) / 1024.0);
    communicator.send(msg);

    lastSensorSend = millis();
  }
}