#pragma once

#include "ofMain.h"
#include "ofxBinaryCommunicator.h"

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
    void onEndPacket();

private:
    ofxBinaryCommunicator communicator;
    std::vector<SampleSensorData> receivedSensorData;
    std::string lastError;
};
