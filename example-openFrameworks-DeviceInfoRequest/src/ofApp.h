#pragma once

// This sample works with ofxBinaryCommunicatorExample-DeviceInfoRequest in Arduino sample

#include "ofMain.h"
#include "ofxBinaryCommunicator.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);

    // Callback methods for ofxBinaryCommunicator
    void onMessageReceived(const ofxBinaryPacket& packet);
    void onError(ofxBinaryCommunicator::ErrorType& errorType);

private:
    ofxBinaryCommunicator communicator;
    vector<string> receivedHistory;
    string lastError;
};
