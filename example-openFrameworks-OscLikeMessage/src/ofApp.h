#pragma once

// This sample works with ofxBinaryCommunicatorExample-OscLikeMessage in Arduino sample

#include "ofMain.h"
#include "ofxBinaryCommunicator.h"

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
    vector<pair<uint32_t, float>> receivedSensorData;
    string lastError;
};
