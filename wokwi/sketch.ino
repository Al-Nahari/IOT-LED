#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// إعدادات الواي فاي
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// إعدادات قناة بيانات الحساس على ThingSpeak
const char* sensorApiKey = "device_commands";
const char* sensorChannelID = "2831041"; // قد لا تحتاجه عند التحديث عبر API

// إعدادات قناة الأوامر
const char* commandReadApiKey = "H2OM7NLFQNX8D0KU";
const char* commandChannelID = "2831041";

// معرف الجهاز الحالي (يجب أن يتطابق مع ما تستخدمه في لوحة التحكم)
const char* device_id = "ESP32_01";

// تعريف الدبابيس
#define LED_PIN 2
#define SENSOR_PIN 34  // مثال لحساس متصل بالدبوس 34

// لتخزين توقيت آخر أمر تم تنفيذه لتفادي التكرار
String lastCommandTime = "";

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);

  connectToWiFi();
}

void loop() {
  if(WiFi.status() == WL_CONNECTED) {
    sendSensorData();
    checkCommands();
  } else {
    Serial.println("WiFi not connected!");
  }
  delay(5000); // تأخير 5 ثواني بين الدورات
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
}

void sendSensorData() {
  // قراءة قيمة الحساس وتحويلها إلى فولت (تعديل المعادلة بحسب حساسك)
  float sensorValue = analogRead(SENSOR_PIN) * (3.3 / 4095.0);

  // بناء رابط التحديث لقناة الحساس (يُستخدم بروتوكول GET)
  String url = "https://api.thingspeak.com/update?api_key=";
  url += sensorApiKey;
  url += "&field1=";
  url += String(sensorValue, 2);

  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  if(httpCode == 200) {
    String payload = http.getString();
    Serial.print("Sensor data sent. Entry ID: ");
    Serial.println(payload);
  } else {
    Serial.print("Error sending sensor data. HTTP Code: ");
    Serial.println(httpCode);
  }
  http.end();
}

void checkCommands() {
  // رابط قراءة آخر سجل من قناة الأوامر
  String url = "https://api.thingspeak.com/channels/";
  url += commandChannelID;
  url += "/feeds/last.json?api_key=";
  url += commandReadApiKey;

  HTTPClient http;
  http.begin(url);
  int httpCode = http.GET();
  if(httpCode == 200) {
    String response = http.getString();
    Serial.println("Command response: " + response);

    // استخدام ArduinoJson لتحليل الاستجابة
    const size_t capacity = JSON_OBJECT_SIZE(5) + 200;
    DynamicJsonDocument doc(capacity);
    DeserializationError error = deserializeJson(doc, response);
    if(!error) {
      // نتوقع أن يحتوي الرد على:
      // field1: الأمر
      // field2: معرف الجهاز
      // created_at: وقت الإرسال
      const char* cmd = doc["field1"];
      const char* targetDevice = doc["field2"];
      String createdAt = doc["created_at"] | "";

      // التحقق من أن الأمر موجّه لهذا الجهاز وأنه جديد
      if(String(targetDevice) == String(device_id) && createdAt != "" && createdAt != lastCommandTime) {
        Serial.print("Received command: ");
        Serial.println(cmd);
        // تنفيذ الأمر
        if(String(cmd) == "LED_ON") {
          digitalWrite(LED_PIN, HIGH);
        }
        else if(String(cmd) == "LED_OFF") {
          digitalWrite(LED_PIN, LOW);
        }
        // حفظ توقيت الأمر لتفادي التكرار
        lastCommandTime = createdAt;
      }
    } else {
      Serial.println("JSON parsing error");
    }
  } else {
    Serial.print("Error checking commands. HTTP Code: ");
    Serial.println(httpCode);
  }
  http.end();
}
