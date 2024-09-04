#include "ofApp.h"

void ofApp::setup() {
    ofSetFrameRate(60);
    communicator.setup("/dev/cu.usbmodem2101", 115200);  // Adjust port name as needed

    communicator.onReceived = [this](const ofxBinaryPacket<SampleData>& packet, size_t size) {
        ofLogNotice() << "Received packet:";
        ofLogNotice() << "Timestamp: " << packet.data.timestamp;
        ofLogNotice() << "X: " << packet.data.x;
        ofLogNotice() << "Y: " << packet.data.y;
        ofLogNotice() << "Message: " << packet.data.message;
    };

    communicator.onError = [](ofxBinaryCommunicator<SampleData>::ErrorType errorType, const uint8_t* data, size_t length) {
        ofLogError() << "Error occurred: ";
        switch (errorType) {
            case ofxBinaryCommunicator<SampleData>::ErrorType::ChecksumMismatch:
                ofLogError() << "Checksum mismatch";
                break;
            case ofxBinaryCommunicator<SampleData>::ErrorType::IncompletePacket:
                ofLogError() << "Incomplete packet";
                break;
            case ofxBinaryCommunicator<SampleData>::ErrorType::BufferOverflow:
                ofLogError() << "Buffer overflow";
                break;
            case ofxBinaryCommunicator<SampleData>::ErrorType::UnexpectedHeader:
                ofLogError() << "Unexpected header";
                break;
        }
    };
}

void ofApp::update() {
    communicator.update();
}

void ofApp::draw() {
    ofBackground(0);
    ofSetColor(255);
    ofDrawBitmapString("ofxBinaryCommunicator Example", 20, 20);

    if (communicator.isInitialized()) {
        ofDrawBitmapString("Move mouse or click to send data", 20, 40);
    }
    else {
        ofSetColor(255, 50, 50);
        ofDrawBitmapString("Serial device is not initialized", 20, 40);
    }
}

void ofApp::mouseMoved(int x, int y) {
    sendMessage(x, y, "mouseMoved");
}

void ofApp::mousePressed(int x, int y, int button) {
    sendMessage(x, y, "mousePressed");
}

void ofApp::sendMessage(int x, int y, const char* msg) {
    ofxBinaryPacket<SampleData> packet;
    packet.data.timestamp = ofGetElapsedTimeMillis();
    packet.data.x = x;
    packet.data.y = y;
    strncpy(packet.data.message, msg, sizeof(packet.data.message) - 1);
    packet.data.message[sizeof(packet.data.message) - 1] = '\0';  // Ensure null-termination
    
    communicator.sendPacket(packet);
    
    ofLogNotice() << "Sent packet:";
    ofLogNotice() << "Timestamp: " << packet.data.timestamp;
    ofLogNotice() << "X: " << packet.data.x;
    ofLogNotice() << "Y: " << packet.data.y;
    ofLogNotice() << "Message: " << packet.data.message;
}
