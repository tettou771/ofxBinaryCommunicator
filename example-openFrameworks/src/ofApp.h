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

    void onMessageReceived(uint16_t topicId, const uint8_t* data, size_t length);

    static void staticOnMessageReceived(void* userData, uint16_t topicId, const uint8_t* data, size_t length);
    static void staticOnError(void* userData, ofxBinaryCommunicator::ErrorType errorType, const uint8_t* data, size_t length);

private:
    ofxBinaryCommunicator communicator;
};
