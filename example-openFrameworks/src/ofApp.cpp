#include "ofApp.h"

void ofApp::setup() {
    ofSetFrameRate(60);
    communicator.setup("/dev/cu.usbmodem2101", 115200);  // Adjust port name as needed

    ofAddListener(communicator.onReceived, this, &ofApp::onMessageReceived);
    ofAddListener(communicator.onError, this, &ofApp::onError);
    ofAddListener(communicator.onEndPacket, this, &ofApp::onEndPacket);

}

void ofApp::update() {
    communicator.update();
}

void ofApp::draw() {
    ofBackground(0);
    ofSetColor(255);
    
    // Draw received sensor data
    ofDrawBitmapString("Received Sensor Data:", 20, 20);
    for (size_t i = 0; i < receivedSensorData.size() && i < 10; ++i) {
        const auto& data = receivedSensorData[receivedSensorData.size() - 1 - i];
        ofDrawBitmapString("Time: " + ofToString(data.timestamp) + ", Value: " + ofToString(data.sensorValue), 20, 40 + i * 20);
    }

    // Draw last error if any
    if (!lastError.empty()) {
        ofSetColor(255, 0, 0);
        ofDrawBitmapString("Last Error: " + lastError, 20, ofGetHeight() - 20);
    }

    // Draw instructions
    ofSetColor(255);
    ofDrawBitmapString("Move mouse or press keys to send data", 20, ofGetHeight() - 40);
}

void ofApp::mouseMoved(int x, int y) {
    SampleMouseData data;
    data.timestamp = ofGetElapsedTimeMillis();
    data.x = x;
    data.y = y;
    strncpy(data.message, "mouseMoved", sizeof(data.message) - 1);
    data.message[sizeof(data.message) - 1] = '\0';
    
    communicator.sendPacket<SampleMouseData>(1, data);
    
    ofLogNotice() << "Sent mouse data - X: " << data.x << ", Y: " << data.y;
}

void ofApp::keyPressed(int key) {
    SampleKeyData data;
    data.timestamp = ofGetElapsedTimeMillis();
    data.key = static_cast<char>(key);
    
    communicator.sendPacket<SampleKeyData>(2, data);
    
    ofLogNotice() << "Sent key data - Key: " << data.key;
}

void ofApp::onMessageReceived(const ofxBinaryPacket& packet) {
    switch (packet.topicId) {
        case 0: { // SampleSensorData
            SampleSensorData sensorData;
            if (ofxBinaryCommunicator::parse(packet, sensorData)) {
                receivedSensorData.push_back(sensorData);
                if (receivedSensorData.size() > 100) {
                    receivedSensorData.erase(receivedSensorData.begin());
                }
                ofLogNotice() << "Received sensor data - Time: " << sensorData.timestamp << ", Value: " << sensorData.sensorValue;
            }
            break;
        }

        case 1: { // SampleMouseData (echo back)
            SampleMouseData mouseData;
            if (ofxBinaryCommunicator::parse(packet, mouseData)) {
                ofLogNotice() << "Received mouse data (echo back)";
                ofLogNotice() << "Timestamp: " << mouseData.timestamp;
                ofLogNotice() << "X: " << mouseData.x;
                ofLogNotice() << "Y: " << mouseData.y;
                ofLogNotice() << "Message: " << mouseData.message;
            }
        } break;

        case 2: { // SampleKeyData (echo back)
            SampleKeyData keyData;
            if (ofxBinaryCommunicator::parse(packet, keyData)) {
                ofLogNotice() << "Received key data (echo back)";
                ofLogNotice() << "Timestamp: " << keyData.timestamp;
                ofLogNotice() << "Key: " << keyData.key;
            }
        } break;

        default:
            ofLogNotice() << "Received unknown topic: " << packet.topicId;
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

void ofApp::onEndPacket() {
    ofLogNotice() << "End packet received";
}
