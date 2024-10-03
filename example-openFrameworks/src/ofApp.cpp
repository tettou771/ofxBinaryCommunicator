#include "ofApp.h"

void ofApp::setup() {
    ofSetFrameRate(60);
    // communicator.setup("COM3", 115200);
    communicator.setup("/dev/cu.usbmodem1101", 115200);  // Adjust port name as needed

    ofAddListener(communicator.onReceived, this, &ofApp::onMessageReceived);
    ofAddListener(communicator.onError, this, &ofApp::onError);
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
    
    communicator.send(data);
    
    ofLogNotice() << "Sent mouse data - X: " << data.x << ", Y: " << data.y;
}

void ofApp::keyPressed(int key) {
    SampleKeyData data;
    data.timestamp = ofGetElapsedTimeMillis();
    data.key = static_cast<char>(key);
    
    communicator.send(data);
    
    ofLogNotice() << "Sent key data - Key: " << data.key;
}

void ofApp::onMessageReceived(const ofxBinaryPacket& packet) {
    switch (packet.topicId) {
        case SampleSensorData::topicId: {
            SampleSensorData sensorData;
            if (packet.unpack(sensorData)) {
                receivedSensorData.push_back(sensorData);
                if (receivedSensorData.size() > 100) {
                    receivedSensorData.erase(receivedSensorData.begin());
                }
                ofLogNotice() << "Received sensor data - Time: " << sensorData.timestamp << ", Value: " << sensorData.sensorValue;
            }
            break;
        }

        case SampleMouseData::topicId: { // SampleMouseData (echo back)
            SampleMouseData mouseData;
            if (packet.unpack(mouseData)) {
                ofLogNotice() << "Received mouse data (echo back)";
                ofLogNotice() << "  Timestamp: " << mouseData.timestamp;
                ofLogNotice() << "  XY: " << mouseData.x << " " << mouseData.y;
            }
        } break;

        case SampleKeyData::topicId: { // SampleKeyData (echo back)
            SampleKeyData keyData;
            if (packet.unpack(keyData)) {
                ofLogNotice() << "Received key data (echo back)";
                ofLogNotice() << "  Timestamp: " << keyData.timestamp;
                ofLogNotice() << "  Key: " << keyData.key;
            }
        } break;
            
        case SampleMessageData::topicId: {
            SampleMessageData msgData;
            if (packet.unpack(msgData)) {
                ofLogNotice() << "Received Message Data";
                ofLogNotice() << "  Message: " << msgData.message;
            }
        } break;
            
            // OscLikeMessage is predefined struct.
        case OscLikeMessage::topicId: {
            OscLikeMessage msg;
            if (packet.unpack(msg)) {
                ofLogNotice() << "Received OscLikeMessage";
                ofLogNotice() << "address: " << msg.getAddressString() << " typestr: " << msg.getTypestrString();
                for (int i = 0; i < msg.getNumArgs(); ++i) {
                    switch (msg.getArgType(i)) {
                        case OscLikeMessage::OSCLIKE_TYPE_INT32:
                            ofLogNotice() << "int32: " << msg.getArgAsInt32(i);
                            break;
                        case OscLikeMessage::OSCLIKE_TYPE_UINT32:
                            ofLogNotice() << "uint32: " << msg.getArgAsUint32(i);
                            break;
                        case OscLikeMessage::OSCLIKE_TYPE_FLOAT:
                            ofLogNotice() << "float: " << msg.getArgAsFloat(i);
                            break;
                        case OscLikeMessage::OSCLIKE_TYPE_BOOL:
                            ofLogNotice() << "bool: " << (msg.getArgAsBool(i) ? "true" : "false");
                            break;
                        case OscLikeMessage::OSCLIKE_TYPE_CHAR:
                            ofLogNotice() << "char: " << msg.getArgAsChar(i);
                            break;
                        case OscLikeMessage::OSCLIKE_TYPE_CHAR4:
                        {
                            // 4byte charactors
                            char char4[4];
                            msg.getArgAsChar4(i, char4);
                            ofLogNotice() << "char4: " << string(char4, 4);
                        }
                            break;
                        case OscLikeMessage::OSCLIKE_TYPE_COLOR:
                        {
                            OscLikeMessage::Color color = msg.getArgAsColor(i);
                            ofLogNotice() << "color: (" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << ")";
                        }
                            break;
                        case OscLikeMessage::OSCLIKE_TYPE_NONE:
                            ofLogNotice() << "None";
                            break;
                    }
                }
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
