#include "ofApp.h"

void ofApp::setup() {
    ofSetFrameRate(60);
    communicator.setup("/dev/cu.usbmodem2101", 115200);  // Adjust port name as needed

    communicator.setReceivedCallback(staticOnMessageReceived, this);
    communicator.setErrorCallback(staticOnError, this);
}

void ofApp::staticOnMessageReceived(void* userData, uint16_t topicId, const uint8_t* data, size_t length) {
    ofApp* app = static_cast<ofApp*>(userData);
    app->onMessageReceived(topicId, data, length);
}

void ofApp::staticOnError(void* userData, ofxBinaryCommunicator::ErrorType errorType, const uint8_t* data, size_t length) {
    ofLogError() << "Error occurred: " << static_cast<int>(errorType);
}

void ofApp::update() {
    communicator.update();
}

void ofApp::draw() {
    ofBackground(0);
    ofSetColor(255);
    ofDrawBitmapString("ofxBinaryCommunicator Example", 20, 20);

    if (communicator.isInitialized()) {
        ofDrawBitmapString("Move mouse or press keys to send data", 20, 40);
    }
    else {
        ofSetColor(255, 50, 50);
        ofDrawBitmapString("Serial device is not initialized", 20, 40);
    }
}

void ofApp::mouseMoved(int x, int y) {
    SampleMouseData data;
    data.timestamp = ofGetElapsedTimeMillis();
    data.x = x;
    data.y = y;
    strncpy(data.message, "mouseMoved", sizeof(data.message) - 1);
    data.message[sizeof(data.message) - 1] = '\0';
    
    communicator.sendPacket(1, data);
    
    ofLogNotice() << "Sent mouse data:";
    ofLogNotice() << "Timestamp: " << data.timestamp;
    ofLogNotice() << "X: " << data.x;
    ofLogNotice() << "Y: " << data.y;
    ofLogNotice() << "Message: " << data.message;
}

void ofApp::keyPressed(int key) {
    SampleKeyData data;
    data.timestamp = ofGetElapsedTimeMillis();
    data.key = static_cast<char>(key);
    
    communicator.sendPacket(2, data);
    
    ofLogNotice() << "Sent key data:";
    ofLogNotice() << "Timestamp: " << data.timestamp;
    ofLogNotice() << "Key: " << data.key;
}

void ofApp::onMessageReceived(uint16_t topicId, const uint8_t* data, size_t length) {
    switch (topicId) {
        case 0: { // SampleSensorData
            SampleSensorData sensorData;
            if (ofxBinaryCommunicator::parse(data, length, sensorData)) {
                ofLogNotice() << "Received sensor data:";
                ofLogNotice() << "Timestamp: " << sensorData.timestamp;
                ofLogNotice() << "Sensor value: " << sensorData.sensorValue;
            }
            break;
        }
        // Handle other topicIds if needed
    }
}
