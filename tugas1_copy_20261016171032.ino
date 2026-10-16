#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Servo.h>#include <WiFi.h>
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
const int servoPin = 18; // Sesuaikan pin GPIO untuk servo

// Konfigurasi Relay
const int relayPin = 23; // Sesuaikan pin GPIO untuk relay

// Pengaturan Waktu (NTP)
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 25200;    // Zona Waktu WIB (GMT+7 -> 7 * 3600 = 25200)
const int daylightOffset_sec = 0;

// Pengaturan Waktu / Timer
unsigned long previousMillisRelay = 0;
const long relayInterval = 3600000; // Selang waktu 1 jam dalam milidetik (1 * 3600 * 1000 = 3600000)

void setup() {
  Serial.begin(115200);
  
  // Inisialisasi Servo
  myServo.attach(servoPin);
  myServo.write(0); // Posisi awal servo (0 derajat)

  // Inisialisasi Relay
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW); // Pastikan relay mati di awal (ubah ke HIGH jika modul active-LOW)

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

  // Eksekusi setiap 1 jam sekali
  if (currentMillis - previousMillisRelay >= relayInterval) {
    previousMillisRelay = currentMillis;

    Serial.println("\n--- Memulai Siklus 1 Jam ---");

    // 1. Hidupkan Relay
    Serial.println("Menghidupkan Relay...");
    digitalWrite(relayPin, HIGH); // Ubah ke LOW jika modul relay Anda tipe active-LOW
    delay(500); // Beri jeda sejenak

    // 2. Untuk mengaktifkan Servo (Putar ke 90 derajat, lalu kembali ke 0 derajat)
    Serial.println("Menggerakkan servo...");
    myServo.write(90);
    delay(1000); // Tahan posisi selama 1 detik
    myServo.write(0);

    // 3. Matikan Kembali Relay (Opsional, matikan setelah proses selesai)
    Serial.println("Mematikan Relay...");
    digitalWrite(relayPin, LOW); // Ubah ke HIGH jika modul relay Anda tipe active-LOW

    // 4. Ambil waktu saat ini dengan format ddmmyyhhmmss dan kirim ke server
    struct tm timeinfo;
    if(getLocalTime(&timeinfo)){
      char timeStringBuff[32];
      // Format: ddmmyyhhmmss (hari, bulan, tahun 2 digit, jam, menit, detik)
      strftime(timeStringBuff, sizeof(timeStringBuff), "%d%m%y%H%M%S", &timeinfo);
      String timestamp = String(timeStringBuff);

      // 5. Kirim data ke server (format: /iot/[timestamp]/servo-1)
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
    
    Serial.println("--- Siklus Selesai ---\n");
  }
}
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