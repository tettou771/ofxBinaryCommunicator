#pragma once

class ofxBinaryCommunicatorTool {
#ifdef OF_VERSION_MAJOR
    // PCに接続されているデバイスの中から、DeviceInfoRequestを使ってデバイス名で探し出すメソッド。
    // 戻り値がそのポート名で、見つからなければ空で返す。
    // deviceIdを指定するとその番号に一致するものを返し、未指定（-1）の場合はどの番号でもいいから見つかれば返す
public:
    static string findDeviceByDeviceInfo(int baudRate, string deviceName, int deviceId = -1){
        string port = "";
        ofxBinaryCommunicator com;
        
        vector<ofSerialDeviceInfo> deviceList = ofSerial().getDeviceList();
        
        for (auto& device : deviceList) {
            bool deviceFounded = false;
            com.setup(device.getDevicePath() , baudRate);
            
            // Set lambda as listener
            ofEventListener listener = com.onReceived.newListener([&](const ofxBinaryPacket& packet) {
                if (packet.topicId == DeviceInfoResponse::topicId) {
                    DeviceInfoResponse res;
                    if (packet.unpack(res)) {
                        // デバイス名の一致を確認
                        if (strcmp(res.deviceName, deviceName.c_str()) == 0) {
                            // deviceId指定が -1 なら、名前だけで合致判定
                            if (deviceId == -1) deviceFounded = true;
                            
                            // deviceIdが一致するか確認
                            else if (deviceId == res.deviceId) deviceFounded = true;
                        }
                    }
                }
            });
            
            // Send a DeviceInfoRequest
            // Wait for a response (this is a simplified example, you might need to handle this asynchronously)
            
            float startTime = ofGetElapsedTimef();
            float timeout = 0.5; // sec
            while (ofGetElapsedTimef() - startTime < 0.5) {
                com.send(DeviceInfoRequest());
                ofSleepMillis(100);
                com.update();
                if (deviceFounded) break;
            }
            
            // com.close();
            
            // Check if the received device name matches the target
            if (deviceFounded) {
                port = device.getDevicePath();
                break;
            }
        }
        
        return port;
    };
#endif
};
