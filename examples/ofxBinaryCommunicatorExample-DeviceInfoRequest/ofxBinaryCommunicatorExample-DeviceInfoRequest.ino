#include <ofxBinaryCommunicator.h>

#define ERROR_LED_PIN 13

ofxBinaryCommunicator communicator;

// The device info
DeviceInfoResponse deviceInfo;

void setDeviceId(uint16_t id) {
  deviceInfo.deviceId = id;

  // send back response
  SetDeviceIdResponse res;
  res.deviceId = id;
  res.succeeded = true;
  communicator.send(res);

  // Write to EEPROM here, if you wanna
}

void onMessageReceived(const ofxBinaryPacket& packet) {
  switch (packet.topicId) {
    case DeviceInfoRequest::topicId:
      {
        // Send back my data to PC
        communicator.send(deviceInfo);
      }
      break;
    case SetDeviceIdRequest::topicId:
      {
        SetDeviceIdRequest req;
        if (packet.unpack(req)) {
          setDeviceId(req.deviceId);
        }
      }
      break;
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
  deviceInfo.deviceId = 0xFFFF;
  strncpy(deviceInfo.deviceName, "TestDevice", 32);
  strncpy(deviceInfo.version, "0.1.0", 32);

  pinMode(ERROR_LED_PIN, OUTPUT);

  Serial.begin(115200);
  communicator.setup(Serial);

  communicator.setReceivedCallback(onMessageReceived);
  communicator.setErrorCallback(onError);
}

void loop() {
  communicator.update();
}