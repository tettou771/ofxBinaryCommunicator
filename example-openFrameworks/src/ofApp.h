#pragma once

#include "ofMain.h"
#include "ofxBinaryCommunicator.h"

struct SampleData {
    int32_t timestamp;
    int x;
    int y;
    char message[30];
};

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    
    void mouseMoved(int x, int y);
    void mousePressed(int x, int y, int button);
    
private:
    ofxBinaryCommunicator<SampleData> communicator;
    void sendMessage(int x, int y, const char* msg);
};
