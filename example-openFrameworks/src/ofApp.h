#pragma once

#include "ofMain.h"
#include "ofxBinaryCommunicator.h"

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

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();

    void mouseMoved(int x, int y);
    void keyPressed(int key);

    // Callback methods for ofxBinaryCommunicator
    void onMessageReceived(const ofxBinaryPacket& packet);
    void onError(ofxBinaryCommunicator::ErrorType& errorType);

private:
    ofxBinaryCommunicator communicator;
    std::vector<SampleSensorData> receivedSensorData;
    std::string lastError;
};
