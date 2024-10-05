#include "ofApp.h"

// This sample works with ofxBinaryCommunicatorExample-DeviceInfoRequest in Arduino sample

/*
This is an example of retrieving device information using DeviceInfoRequest.
The Arduino side needs to have information such as deviceName and ID.
It is intended to be used to identify devices when multiple devices are connected to the same PC during serial communication.
*/

void ofApp::setup() {
    ofSetWindowTitle("example DeviceInfoRequest");
    ofSetFrameRate(60);
    // communicator.setup("COM3", 115200);
    communicator.setup("/dev/cu.usbmodem101", 115200);  // Adjust port name as needed

    ofAddListener(communicator.onReceived, this, &ofApp::onMessageReceived);
    ofAddListener(communicator.onError, this, &ofApp::onError);
}

void ofApp::update() {
    communicator.update();
}

void ofApp::draw() {
    ofBackground(0);
    ofSetColor(255);
    
    // erase over 30 count history
    while (receivedHistory.size() > 30) {
        receivedHistory.erase(receivedHistory.begin());
    }
    
    // Draw received data
    ofDrawBitmapString("Received:", 20, 20);
    for (int i = receivedHistory.size() - 1; i >= 0; --i) {
        ofDrawBitmapString(receivedHistory[i], 20, 40 + i * 20);
    }

    // Draw last error if any
    if (!lastError.empty()) {
        ofSetColor(255, 0, 0);
        ofDrawBitmapString("Last Error: " + lastError, 20, ofGetHeight() - 20);
    }

    // Draw instructions
    ofSetColor(255);
    ofDrawBitmapString("Num key to set device id \nOther key to request DeviceInfo", 20, ofGetHeight() - 60);
}


void ofApp::keyPressed(int key) {
    // Num key, set device id
    if ('0' <= key && key <= '9') {
        // keycode -> number
        int num = key - '0';
        
        SetDeviceIdRequest req;
        req.deviceId = num;
        communicator.send(req);
        
        receivedHistory.push_back("Device ID Set request: " + ofToString(num));
    }

    // Other key, ask DeviceInfo to device over Serial.
    else {
        DeviceInfoRequest req;
        communicator.send(req);
    }
}

void ofApp::onMessageReceived(const ofxBinaryPacket& packet) {
    switch (packet.topicId) {
        case DeviceInfoResponse::topicId: {
            DeviceInfoResponse res;
            if (packet.unpack(res)) {
                stringstream ss;
                ss << "  Name: " << ofToString(res.deviceName);
                ss << ", ID: " + ofToString(res.deviceId);
                ss << ", Ver: " + ofToString(res.version);
                receivedHistory.push_back(ss.str());
            }
        } break;
        
        case SetDeviceIdResponse::topicId: {
            SetDeviceIdResponse res;
            if (packet.unpack(res)) {
                string result = res.succeeded ? " succeeded." : " fault.";
                receivedHistory.push_back("Device ID " + ofToString(res.deviceId) + result);
            }
        } break;

        default:
            ofLogNotice() << "  Received unknown topic: " << packet.topicId;
            break;
    }
}

void ofApp::onError(ofxBinaryCommunicator::ErrorType& errorType) {
    switch (errorType) {
        case ofxBinaryCommunicator::ErrorType::ChecksumMismatch:
            lastError = "Checksum mismatch";
            break;
        case ofxBinaryCommunicator::ErrorType::IncompletePacket:
            lastError = "Incomplete packet";
            break;
        case ofxBinaryCommunicator::ErrorType::BufferOverflow:
            lastError = "Buffer overflow";
            break;
        case ofxBinaryCommunicator::ErrorType::UnexpectedHeader:
            lastError = "Unexpected header";
            break;
        default:
            lastError = "Unknown error";
            break;
    }
    ofLogError() << "Error occurred: " << lastError;
}
