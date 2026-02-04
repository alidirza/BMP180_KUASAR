/*
  BMP180_KUASAR Library
  
  Bu örnek BMP180 sensörünün temel kullanımını gösterir:
  - Sıcaklık okuma
  - Basınç okuma  
  - Yükseklik hesaplama
  - IIR filtre kullanımı
  - Hata kontrolü
  
  Bağlantı:
  - VCC -> 3.3V (5V de çalışır)
  - GND -> GND
  - SDA -> Pin 21 (ESP32) / A4 (Arduino Uno)
  - SCL -> Pin 22 (ESP32) / A5 (Arduino Uno)
  
  Yazan: Ali Rıza Öztürk
  Tarih: 05.02.2026
*/

#include <BMP180_KUASAR.h>

BMP180_KUASAR bmp;

//(ESP32)
#define SDA_PIN 21
#define SCL_PIN 22

void setup() {
  Serial.begin(115200);
  Serial.println("BMP180_KUASAR Library");
  Serial.println("=====================================");
  
  // I2C Sensör adresini değiştirmek isterseniz:
  // bmp.changeAddr(0x76);

  // Sensörü başlat
  // Parametreler: SDA pin, SCL pin, OSS (0-3, yüksek = daha hassas ama yavaş)
  if (bmp.begin(SDA_PIN, SCL_PIN, 3)) {
    Serial.println("BMP180 sensörü başarıyla başlatıldı!");
  } else {
    Serial.println("BMP180 sensörü başlatılamadı!");
    while(1); // Durur
  }
  
  // IIR filtre ayarla (0.01-1.0 arası, küçük = daha yumuşak)
  bmp.setIirFilter(0.2);  // Roket için ideal
  
  // Deniz seviyesi kalibrasyonu (isteğe bağlı)
  // Eğer bulunduğunuz yerin rakımını biliyorsanız:
  // bmp.calibrateSeaLevel(100.0);  // 100m rakımda olduğunuzu varsayar
  
  Serial.println("Setup tamamlandı. Ölçümler başlıyor...\n");
}

void loop() {
  // Sensörü güncelle (non-blocking)
  bmp.update();
  
  // Yeni veri hazır mı kontrol et
  if (bmp.dataReady()) {
    
    // Hata kontrolü
    if (bmp.checkError()) {
      Serial.println("⚠️ I2C Hatası tespit edildi!");
    } else {
      // Verileri oku
      float temperature = bmp.getTemperature();  // °C
      float pressure = bmp.getPressure();        // Pa
      float altitude = bmp.getAltitude();        // m
      
      // Sonuçları yazdır
      Serial.println("=== BMP180 Ölçümler ===");
      Serial.print("🌡️  Sıcaklık: ");
      Serial.print(temperature, 1);
      Serial.println(" °C");
      
      Serial.print("🔽 Basınç: ");
      Serial.print(pressure / 100.0, 2);  // hPa'ya çevir
      Serial.println(" hPa");
      
      Serial.print("📏 Yükseklik: ");
      Serial.print(altitude, 1);
      Serial.println(" m");
      
      Serial.println("========================\n");
    }
  }
  
  delay(100);  // 100ms bekle (sensör güncelleme döngüsü için)
}