#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>
#include <time.h>

// Konfigurasi Wi-Fi
const char* ssid = "NAMA_WIFI_ANDA";
const char* password = "PASSWORD_WIFI_ANDA";

// Konfigurasi Server (Sesuai dengan dashboard Vercel Anda)
const char* serverBaseUrl = "https://warung-angga.vercel.app/iot/";

// Konfigurasi Servo
Servo myServo;
const int servoPin = 18; // Sesuaikan pin GPIO yang Anda gunakan untuk servo

// Pengaturan Waktu (NTP)
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 25200;   // Zona Waktu WIB (GMT+7 -> 7 * 3600 = 25200)
const int daylightOffset_sec = 0;

unsigned long previousMillis = 0;
const long interval = 300000; // Selang waktu 5 menit dalam milidetik (5 * 60 * 1000 = 300000)

void setup() {
  Serial.begin(115200);
  
  // Inisialisasi Servo
  myServo.attach(servoPin);
  myServo.write(0); // Posisi awal servo (0 derajat)

  // Koneksi Wi-Fi
  Serial.print("Menghubungkan ke Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Terhubung!");

  // Sinkronisasi Waktu NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Menunggu sinkronisasi waktu internet...");
  struct tm timeinfo;
  while(!getLocalTime(&timeinfo)){
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWaktu berhasil disinkronkan.");
}

void loop() {
  unsigned long currentMillis = millis();

  // Eksekusi setiap 5 menit sekali
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // 1. Gerakkan Servo (Putar ke 90 derajat, lalu kembali ke 0 derajat)
    Serial.println("Menggerakkan servo...");
    myServo.write(90);
    delay(1000); // Tahan posisi selama 1 detik
    myServo.write(0);

    // 2. Ambil waktu saat ini dengan format ddmmyyhhmmss
    struct tm timeinfo;
    if(getLocalTime(&timeinfo)){
      char timeStringBuff[32];
      // Format: ddmmyyhhmmss (hari, bulan, tahun 2 digit, jam, menit, detik)
      strftime(timeStringBuff, sizeof(timeStringBuff), "%d%m%y%H%M%S", &timeinfo);
      String timestamp = String(timeStringBuff);

      // 3. Kirim data ke server (format: /iot/[timestamp]/servo-1)
      if(WiFi.status() == WL_CONNECTED){
        HTTPClient http;
        
        // Menggabungkan URL dasar, timestamp, dan lokasi (servo-1)
        String fullUrl = String(serverBaseUrl) + timestamp + "/servo-1";
        Serial.println("Mengirim request ke: " + fullUrl);

        http.begin(fullUrl);
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
          String response = http.getString();
          Serial.println("HTTP Response code: " + String(httpResponseCode));
          Serial.println("Response: " + response);
        } else {
          Serial.println("Error pada HTTP Request: " + String(httpResponseCode));
        }
        
        http.end();
      } else {
        Serial.println("Wi-Fi terputus!");
      }
    } else {
      Serial.println("Gagal mendapatkan waktu lokal");
    }
  }
}