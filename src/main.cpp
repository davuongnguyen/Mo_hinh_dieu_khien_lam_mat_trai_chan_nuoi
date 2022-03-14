// Các từ khóa cấu hình MQTT Blynk
#define BLYNK_TEMPLATE_ID "TMPL9mSW3KIs"
#define BLYNK_DEVICE_NAME "Project Trai Chan Nuoi"
#define BLYNK_AUTH_TOKEN "dxT0qHPQiBHxZ5DgBpmMhe7NWEn2t0vm"

#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "BlynkSimpleEsp8266.h"
#include "DHT.h"
#include "Adafruit_Sensor.h"
#include "DHT_U.h"

#pragma region Định nghĩa từ khóa

// Định nghĩa các chân đầu ra
#define DHTPIN 4

// Định nghĩa loại cảm biến DHT
#define DHTTYPE DHT11

#pragma endregion

#pragma region Khởi tạo

// Khởi tạo chuỗi chứa thông số kết nối MQTT Blynk
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "MYUI";
char pass[] = "22222222";

// Khởi tạo cảm biến DHT
DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

#pragma endregion

#pragma region Nguyên mẫu các hàm

// Hàm đọc dữ liệu từ cảm biến DHT 11
void sendSensor();

#pragma endregion

void setup()
{
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass);

  dht.begin();

  timer.setInterval(1000L, sendSensor);

  Serial.print("Setup complete!");
}

void loop()
{
  Blynk.run();
  timer.run();
}

#pragma region Các hàm con

void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // In ra giá trị và gửi dữ liệu lên máy chủ
  Serial.print(t);
  Serial.print(" *C, ");
  Serial.print(h);
  Serial.println(" H");

  Blynk.virtualWrite(V0, h);
  Blynk.virtualWrite(V1, t);
}

#pragma endregion