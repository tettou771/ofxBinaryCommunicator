# ofxBinaryCommunicator

ofxBinaryCommunicatorは、ArduinoとopenFrameworksの間で効率的なバイナリ通信を行うためのライブラリです。カスタム定義された構造体データをシリアル通信で送受信し、データの整合性を検証する機能を備えています。

目的は、OSCやMQTTのようにいつかの型を含んだパッケージを送受信できるようにすることです。
事前に定義した構造体のみを通信で使えるので、柔軟性は犠牲になっていますがオーバーヘッドは小さいです。
マイコンで毎秒数百回のデータ送受信をする場合など、送信できるbyte数に制限がある場合にも適しています。

## 特徴

- カスタム定義された構造体の直接送受信
- ArduinoとopenFrameworksの両方に対応
- topicIdを使用した複数のデータ型のサポート
- checksumを使用したデータ整合性の検証
- エラーハンドリング機能

## インストール方法

### openFrameworks

1. このリポジトリを`openFrameworks/addons/`フォルダにクローンします。
2. openFrameworksのプロジェクトジェネレーターを使用して、ofxBinaryCommunicatorをプロジェクトに追加します。

### Arduino

1. このリポジトリをZIPファイルとしてダウンロードします。
2. Arduino IDEで、"スケッチ" → "ライブラリをインクルード" → "ZIP形式のライブラリをインストール..." を選択し、ダウンロードしたZIPファイルを選択します。

## 使用方法

1. 送受信したいデータの構造体を定義します：

```cpp
// SampleSensorDataという構造体を定義
// Arduino -> openFrameworks を想定
TOPIC_STRUCT_MAKER(SampleSensorData, 0, 
    int32_t timestamp;
    int sensorValue;
)

// SampleMouseDataという構造体を定義
// openFrameworks -> Arduino を想定
TOPIC_STRUCT_MAKER(SampleMouseData, 1,
    int32_t timestamp;
    int x;
    int y;
)
```

2. ofxBinaryCommunicatorのインスタンスを作成します：

```cpp
ofxBinaryCommunicator communicator;
```

3. communicatorをセットアップします：

```cpp
// openFrameworks
communicator.setup("/dev/ttyUSB0", 115200);

// Arduino
communicator.setup(Serial);
```

4. データを送信します：

```cpp
SampleSensorData data;
data.timestamp = ofGetElapsedTimeMillis(); // または millis()
data.sensorValue = analogRead(A0);
communicator.send(data);
```

5. データを受信します：

```cpp
// openFrameworks
ofAddListener(communicator.onReceived, this, &YourClass::onMessageReceived);

void YourClass::onMessageReceived(const ofxBinaryPacket& packet) {
	// topicIdはuint8_tなので、switch文を使うと高速に振り分けができます
    switch (packet.topicId) {
        case SampleSensorData::topicId: {
            SampleSensorData sensorData;
            if (packet.unpack(sensorData)) {
                // 受信データを処理
            }
            break;
        }
        // 他のトピックIDを処理...
    }
}

// Arduino
communicator.setReceivedCallback(onMessageReceived);

void onMessageReceived(const ofxBinaryPacket& packet) {
    switch (packet.topicId) {
        case SampleMouseData::topicId: {
            SampleMouseData mouseData;
            if (packet.unpack(mouseData)) {
                // 受信データを処理
            }
            break;
        }
        // 他のトピックIDを処理...
    }
}
```

6. エラーハンドリングをセットアップします(必要なら)：

```cpp
// openFrameworks
ofAddListener(communicator.onError, this, &YourClass::onError);

void YourClass::onError(ofxBinaryCommunicator::ErrorType& errorType) {
    // エラーを処理
}

// Arduino
communicator.setErrorCallback(onError);

void onError(ofxBinaryCommunicator::ErrorType errorType) {
    // エラーを処理
}
```

7. データを処理するために定期的に`update()`を呼び出します：

```cpp
void update() { // Arduinoの場合は void loop()
    communicator.update();
}
```

## サンプル

リポジトリには3種類のサンプルが含まれています。Arduino用（`ofxBinaryCommunicatorExample-xxx.ino`）とopenFrameworks用（`example-openFrameworks-xxx`）のサンプルコードが含まれています。これらのサンプルは、セットアップ方法、様々な種類のデータの送信方法、受信メッセージの処理方法を示しています。

それぞれ、対応するソースをArduino側に書き込んだ上でopenFraemwroks側のソースをビルドすると、通信ができます。

### basic

基本的な使い方について説明しています。

通信したい内容に即した構造体を定義して、それをシリアライズして通信するという使い方のサンプルです。

### OscLikeMessage

事前に定義されている OscLikeMessage という構造体で送受信する方法です。

オーバーヘッドが大きい代わりに、OSCに似た使い方で簡単にデータを送受信できます。

ただし、構造体が固定長であることに起因して、stringなどの型は扱えません。int32, float, Color(独自定義)などをデータに含めることができます。

### DeviceInfoRequest

PCに多くのデバイスがつながっていると、COMポートの番号だけではデバイスを同定できなくて困ることがあります。

そこで、あらかじめデバイスに名前とIDをアサインしてリクエストできると、どのデバイスがつながっているか特定できるので便利です。

このサンプルは、そういった使い方を想定しています。


## カスタマイズ

ライブラリをincludeする前に`MAX_PACKET_SIZE`を定義することで、最大パケットサイズを調整できます。

送受信する構造体が大きい場合はサイズを大きく、使用メモリを削減したい場合は小さくしてください。

```cpp
#define MAX_PACKET_SIZE 512
#include "ofxBinaryCommunicator.h"
```

## ライセンス

このライブラリはMITライセンスの下で公開されています。

## 作者

tettou771

## 貢献

バグ報告、機能リクエスト、プルリクエストはGitHub Issuesで歓迎します。
