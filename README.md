# Mô hình điều khiển giám sát làm mát trại chăn nuôi
### Tổng quan: 

​	Mô hình nuôi nấm rơm 2 chế độ tự động và bằng tay.

​	Mã được viết bằng editor "Visual studio code" kết hợp với extensions"PlatformIO IDE".

​	Phần code chính trong thư mục "src".

​	Phần minh họa và mô phỏng Proteus trong thư mục "Schematic".

​	Các thư viện chính của code đã được tích hợp sẵn.

​	Các thư viện dành cho mô phỏng Proteus được đặt trong thư mục "Proteus library".

### Mô tả họa động:

- Chế độ bằng tay:
  - Mỗi khi ấn một nút thì đầu ra tương ứng sẽ thay đổi trạng thái theo.
  - Chống dội mặc định của nút bấm là 200ms.
- Chế độ tự động:
  - Khi nhiệt độ cao hơn giới hạn trên, bộ phận làm mát sẽ được bật.
  - Khi nhiệt độ thấp hơn giới hạn dưới, bộ phận làm mát sẽ được tắt.
  - Các giới hạn trên và giới hạn dưới có thể tùy chỉnh được.
- Chế độ online và offline:
  - Khi không có kết nối internet, mô hình sẽ hoạt động ở trạng thái offline hoàn toàn, điều khiển bằng nút bấm và hiển thị trên LCD.
  - Khi kết nối thành công đến MQTT Blynk, mô hình sẽ được điều khiển và giám sát cả bằng nút bấm, LCD và Blynk (trên web và app).

### Mô tả phần cứng:

- Về phần cứng của mô hình, khối xử lý chính ở đây sẽ là Node MCU ESP8266.
- Các cảm biến được sử dụng là cảm biến nhiệt độ độ ẩm DHT 11.
- Các thiết bị ngoại vi gồm 4 nút nhấn, rơ le đầu ra, động cơ quạt.
- Về phần hiển thị, mô hình sử dụng module LCD + I2C (tối thiểu 16x2).

### Các vấn đề lỗi có thể gặp:

- Màn hình LCD không hiển thị: có thể do địa chỉ giao tiếp I2C sai, sửa trong code.

### Các phần mềm cần thiết:

- Visual studio code: https://code.visualstudio.com/download
- PlatformIO IDE: https://platformio.org/platformio-ide
- Git: https://git-scm.com/downloads
- Proteus (Tối thiểu 8.10 SP3): https://www.labcenter.com/
- Blynk: 
  - Blynk trên web: https://blynk.cloud/
  - Blynk trên Android: https://play.google.com/store/apps/details?id=cloud.blynk
  - Blynk trên IOS: https://apps.apple.com/us/app/blynk-iot/id1559317868
