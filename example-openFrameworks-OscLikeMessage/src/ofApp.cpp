#include "ofApp.h"

// This sample works with ofxBinaryCommunicatorExample-OscLikeMessage in Arduino sample

/*
This is a sample that communicates between structures called OscLikeMessage.
Like Osc, you can specify an address and send values ​​such as Int32 or float. However, there are some types, such as strings, that cannot be sent. This is because the OscLikeMessage structure is fixed length.
As you can see from the definition of OscLikeMessage, it itself becomes a relatively large binary (an instance is 192 bytes), so in order to achieve efficient sending and receiving, it is recommended to define a small structure like in the basic example.
*/

void ofApp::setup() {
    ofSetWindowTitle("example OscLikeMessage");
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
    
    // Draw received sensor data
    ofDrawBitmapString("Received Sensor Data:", 20, 20);
    for (size_t i = 0; i < receivedSensorData.size() && i < 10; ++i) {
        const auto& data = receivedSensorData[receivedSensorData.size() - 1 - i];
        ofDrawBitmapString("Time: " + ofToString(data.first) + " Value: " + ofToString(data.second), 20, 40 + i * 20);
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
    OscLikeMessage msg;
    msg.setAddress("/input/mouse");
    msg.addInt32Arg(x);
    msg.addInt32Arg(y);
    communicator.send(msg);
    
    ofLogNotice() << "Sent mouse data - X: " << x << ", Y: " << y;
}

void ofApp::keyPressed(int key) {
    OscLikeMessage msg;
    msg.setAddress("/input/key");
    msg.addCharArg(key);
    communicator.send(msg);
    
    ofLogNotice() << "Sent key data :" << key;
}

void ofApp::onMessageReceived(const ofxBinaryPacket& packet) {
    switch (packet.topicId) {
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
                
                // If msg is sensor data, add to history
                if (msg.getAddressString() == "/sensor/value") {
                    pair<uint32_t, float> newData;
                    
                    // Check arg type
                    if (msg.getArgType(0) == OscLikeMessage::OSCLIKE_TYPE_INT32) {
                        newData.first = msg.getArgAsInt32(0); // get arg
                    }
                    if (msg.getArgType(1) == OscLikeMessage::OSCLIKE_TYPE_FLOAT) {
                        newData.second = msg.getArgAsFloat(1);
                    }
                    receivedSensorData.push_back(newData);
                    
                    // history max is 30
                    while (receivedSensorData.size() > 30) {
                        receivedSensorData.erase(receivedSensorData.begin());
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
