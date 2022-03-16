// Các từ khóa cấu hình MQTT Blynk
#define BLYNK_TEMPLATE_ID "TMPL9mSW3KIs"
#define BLYNK_DEVICE_NAME "Project Trai Chan Nuoi"
#define BLYNK_AUTH_TOKEN "dxT0qHPQiBHxZ5DgBpmMhe7NWEn2t0vm"

#define BLYNK_PRINT Serial

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h> 
#include "BlynkSimpleEsp8266.h"
#include "DHT.h"
#include "Adafruit_Sensor.h"
#include "DHT_U.h"
#include "LiquidCrystal_I2C.h"

#pragma region Định nghĩa từ khóa

// Định nghĩa các chân đầu ra
#define DHTPIN 0
#define MODE_PIN 2
#define OK_PIN 12
#define ON_PIN 13
#define OFF_PIN 14
#define FAN_PIN 16

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

// Khởi tạo lcd có địa chỉ là 0x27, kích thước hiển thị 16*02
LiquidCrystal_I2C lcd(0x3f,16,2);

BlynkTimer timer;

// Tạo mảng đầu vào
byte Input_pin[] = {MODE_PIN, OK_PIN, ON_PIN, OFF_PIN};

// Khởi tạo các biến cần thiết để chống dội khi bấm nút
byte prevState = 1;
unsigned long lastChangeTime = 0;

// Khởi tạo các biến lưu dữ liệu
byte mode = 0;
byte fan_status = 0;
float t = 0, h = 0;
int upper_limit = 30;
int lower_limit = 25;

// Khởi tạo các biến lưu dữ liệu ảo
byte screen = 0;
float v_t = 0, v_h = 0;
int v_upper_limit = 30;
int v_lower_limit = 25;

// Các biến cần thiết để kiểm tra kết nối wifi
byte connect_wifi = 0;
unsigned long time_Reconnect = 0;

#pragma endregion

#pragma region Nguyên mẫu các hàm

// Kiểm tra trạng thái kết nối Wifi
// In ra tên Wifi và địa chỉ IP cục bộ
// Mỗi 10s tối đa kiểm tra 1 lần
void wifi_connect();

// Hàm đọc dữ liệu từ cảm biến DHT 11
void sendSensor();

// Hàm kiểm tra trạng thái chân
// Tham số truyền vào: số chân
// Tham số trả về: trạng thái bấm nút
// Phát hiện bấm nút: trả về 1
byte checkPin(byte pin);

// Tiến trình xử lý chính
void Process();

// Hàm hiển thị
// Tham số truyền vào: số màn hình và vị trí thay đổi
void display(byte screen, byte val);

#pragma endregion

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, pass);

  dht.begin();

  // Cứ mỗi 1000ms gửi thực hiện 1 lần hàm sendSensor
  timer.setInterval(1000L, sendSensor);

  // Thiết lập cho màn hình LCD
  Wire.begin(D2,D1);   // Thiết lập chân D2 = SDA, D1 = SCL
  lcd.init();           
  lcd.backlight();

  // Thiết lập chế độ vào ra cho các chân
  for (int i = 0; i < 4; i++)
    pinMode(Input_pin[i], INPUT);
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, fan_status);

  // In ra màn hình debug: Setup complete!
  Serial.println("Setup complete!");
  
  display(0,0);
}

void loop()
{
  wifi_connect();

  timer.run();

  Process();
}

#pragma region Các hàm con

void wifi_connect()
{
  if (connect_wifi == 1)
  {
    Blynk.run();
    return;
  }
  if ((unsigned long)(millis() - time_Reconnect) > 10000)
  {
    time_Reconnect = millis();
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("Đã kết nối wifi!");
      Serial.print("Tên Wifi: MYUI. Địa chỉ IP cục bộ: ");
      Serial.println(WiFi.localIP());

      connect_wifi = 1;
      Blynk.begin(auth, ssid, pass);
      return;
    }
    Serial.println("Đang kết nối tới wifi MYUI ...");
  }
}

void sendSensor()
{
  h = dht.readHumidity();
  t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
  Blynk.virtualWrite(V0, h);
  Blynk.virtualWrite(V1, t);

  if (h != v_h || t != v_t)
  {
    v_h = h;
    v_t = t;

    if (screen == 0)
      display (screen,2);
  }
}

byte checkPin(byte pin)
{
    byte state = digitalRead(pin);
  byte i = 0;

  // Debounce mechanism
  unsigned long t = millis();
 
  if (t - lastChangeTime > 200) 
  {
    if (state != prevState)
    { 
      lastChangeTime = t;
      if (state != 1) 
      {
        i = 1;
      }
      prevState = state;
    }
  }
  return i;
}

void Process()
{
  // Lần lượt kiểm tra trạng thái từng nút bấm, nếu phát hiện bấm nút thì xử lý trường hợp
  if (checkPin(MODE_PIN) == 1)        // Bấm nút mode - cancel
  {
    if (screen == 0)
    {
      if (mode == 0)
        mode = 1;
      else
        mode = 0;
      
      display(screen,1);
      Blynk.virtualWrite(V5, mode);
    }
    else
    {
      screen = 0;
      v_upper_limit = upper_limit;
      v_lower_limit = lower_limit;
      display(screen,0);
    }
  }
  else if (checkPin(OK_PIN) == 1)     // Bấm nút Setting - ok
  {
    if (screen == 0)
      screen = 1;
    else if (screen == 1)
    {
      screen = 2;
      upper_limit = v_upper_limit;
      Blynk.virtualWrite(V6, upper_limit);
    }
    else if (screen == 2)
    {
      screen = 0;
      lower_limit = v_lower_limit;
      Blynk.virtualWrite(V7, lower_limit);
    }

    // Thay đổi màn hình
    display(screen,0);
  }
  else if (checkPin(ON_PIN) == 1)     // Bấm nút On - up
  {
    if (mode == 0 && screen == 0)
    {
      fan_status = 1;
      digitalWrite(FAN_PIN, fan_status);
      Blynk.virtualWrite(V10, fan_status);
      Blynk.virtualWrite(V8, fan_status);
    }
    else if (screen == 1)
      v_upper_limit++;
    else if (screen == 2)
      v_lower_limit++;

    display(screen,1);
  }
  else if (checkPin(OFF_PIN) == 1)    // Bấm nút Off - down
  {
    if (mode == 0 && screen == 0)
    {
      fan_status = 0;
      digitalWrite(FAN_PIN, fan_status);
      Blynk.virtualWrite(V10, fan_status);
      Blynk.virtualWrite(V8, fan_status);
    }
    else if (screen == 1)
      v_upper_limit--;
    else if (screen == 2)
      v_lower_limit--;

    display(screen,1);
  }

  // Chế độ auto
  if (mode == 1)
  {
    if (t >= upper_limit)
    {
      fan_status = 1;
      Blynk.virtualWrite(V10, fan_status);
      Blynk.virtualWrite(V8, fan_status);
      digitalWrite(FAN_PIN, fan_status);
    }
    else if (t <= lower_limit)
    {
      fan_status = 0;
      Blynk.virtualWrite(V10, fan_status);
      Blynk.virtualWrite(V8, fan_status);
      digitalWrite(FAN_PIN, fan_status);
    }
  }
}

void display(byte screen, byte val)
{
  if (screen == 0 && val == 0)        // Hiển thị toàn bộ screen 1
  {
    lcd.clear();

    // Hiển thị chế độ
    lcd.setCursor(0,0);
    if (mode == 0)
      lcd.print("Manual");
    else
      lcd.print("Auto  ");

    // Hiển thị trạng thái quạt
    lcd.setCursor(8,0);
    lcd.print("Fan: ");
    lcd.setCursor(13,0);
    if (fan_status == 0)
      lcd.print("off");
    else
      lcd.print(" on");

    // Hiển thị nhiệt độ
    lcd.setCursor(1,0);
    lcd.print("Tem:");
    lcd.setCursor(1,4);
    lcd.print(int(t));
    lcd.setCursor(1,7);
    lcd.print(" C");

    // Hiển thị độ ẩm
    lcd.setCursor(1,9);
    lcd.print("Hum:");
    lcd.setCursor(1,13);
    lcd.print(int(h));
    lcd.setCursor(1,15);
    lcd.print("%");

    // Debug LCD
    Serial.println("           ************************           ");
    Serial.println("==============================================");
    Serial.println("           ------------------------           ");    
    Serial.println("==============================================");
    Serial.println("           ************************           ");
    if (mode == 0)
      Serial.print("Manual  Fan: ");
    else
      Serial.print("Auto    Fan: ");
    if (fan_status == 0)
      Serial.println("off");
    else
      Serial.println(" on");
    Serial.print("Tem:");
    Serial.print(int(t));
    Serial.print(" C ");
    Serial.print("Hum:");
    Serial.print(int(h));
    Serial.println("%");
  }
  else if (screen == 0 && val == 1)   // Hiển thị sự thay đổi chế độ
  {
    lcd.setCursor(0,0);
    if (mode == 0)
      lcd.print("Manual");
    else
      lcd.print("Auto  ");

    // Debug LCD
    Serial.println("           ------------------------           ");
    if (mode == 0)
      Serial.print("Manual  Fan: ");
    else
      Serial.print("Auto    Fan: ");
    if (fan_status == 0)
      Serial.println("off");
    else
      Serial.println(" on");
    Serial.print("Tem:");
    Serial.print(int(t));
    Serial.print(" C ");
    Serial.print("Hum:");
    Serial.print(int(h));
    Serial.println("%");
  }
  else if (screen == 0 && val == 2)   // Hiển thị sự thay đổi nhiệt độ, độ ẩm
  {
    lcd.setCursor(1,4);
    lcd.print(int(t));

    lcd.setCursor(1,13);
    lcd.print(int(h));

    // Debug LCD
    Serial.println("           ------------------------           ");
    if (mode == 0)
      Serial.print("Manual  Fan: ");
    else
      Serial.print("Auto    Fan: ");
    if (fan_status == 0)
      Serial.println("off");
    else
      Serial.println(" on");
    Serial.print("Tem:");
    Serial.print(int(t));
    Serial.print(" C ");
    Serial.print("Hum:");
    Serial.print(int(h));
    Serial.println("%");
  }
  else if (screen == 1 && val == 0)   // Hiển thị màn hình thứ 2
  {
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("Select Tem max");
    lcd.setCursor(1,7);
    lcd.print(v_upper_limit);
    lcd.setCursor(1,9);
    lcd.print(".00 C");

    // Debug LCD
    Serial.println("           ************************           ");
    Serial.println("==============================================");
    Serial.println("           ------------------------           ");    
    Serial.println("==============================================");
    Serial.println("           ************************           ");
    Serial.println("Select Tem max");
    Serial.print("       ");
    Serial.print(v_upper_limit);
    Serial.println(".00 C  ");
  }
  else if (screen == 1 && val == 1)   // Hiển thị sự thay đổi của giới hạn trên nhiệt độ
  {
    lcd.setCursor(1,7);
    lcd.print(v_upper_limit);

    Serial.println("           ------------------------           ");    
    Serial.println("Select Tem max");
    Serial.print("       ");
    Serial.print(v_upper_limit);
    Serial.println(".00 C  ");
  }
  else if (screen == 2 && val == 0)   // Hiển thị màn hình thứ 3
  {
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.print("Select Tem min");
    lcd.setCursor(1,7);
    lcd.print(v_lower_limit);
    lcd.setCursor(1,9);
    lcd.print(".00 C");

    Serial.println("           ************************           ");
    Serial.println("==============================================");
    Serial.println("           ------------------------           ");    
    Serial.println("==============================================");
    Serial.println("           ************************           ");
    Serial.println("Select Tem min");
    Serial.print("       ");
    Serial.print(v_lower_limit);
    Serial.println(".00 C  ");
  }
  else if (screen == 2 && val == 1)   // Hiển thị sự thay đổi của giới hạn dưới nhiệt độ
  {
    lcd.setCursor(1,7);
    lcd.print(v_lower_limit);

    Serial.println("           ------------------------           ");    
    Serial.println("Select Tem min");
    Serial.print("       ");
    Serial.print(v_lower_limit);
    Serial.println(".00 C  ");
  }
}

BLYNK_WRITE(V5)
{
  int pinValue = param.asInt();
  mode = (byte)pinValue;

  display(0,1);
}

BLYNK_WRITE(V6)
{
  upper_limit = param.asInt();

  if (screen == 0)
    v_upper_limit = upper_limit;
}

BLYNK_WRITE(V7)
{
  lower_limit = param.asInt();

  if (screen == 0)
    v_lower_limit = lower_limit;
}

BLYNK_WRITE(V8)
{
  int pinValue = param.asInt();
  fan_status = (byte)pinValue;

  digitalWrite(FAN_PIN, fan_status);
  Blynk.virtualWrite(V10, fan_status);

  if (screen == 0)
    display(0,1);
}

BLYNK_CONNECTED()
{
  Blynk.syncVirtual(V5, V6, V7, V8);
}

#pragma endregion