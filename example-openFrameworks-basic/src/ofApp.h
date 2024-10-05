#pragma once

// This sample works with ofxBinaryCommunicatorExample-basic in Arduino sample

#include "ofMain.h"
#include "ofxBinaryCommunicator.h"

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
    vector<SampleSensorData> receivedSensorData;
    string lastError;
};
