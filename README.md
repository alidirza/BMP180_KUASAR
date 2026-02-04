# BMP180_KUASAR Library

![Arduino](https://img.shields.io/badge/Arduino-00979D?style=flat&logo=Arduino&logoColor=white)
![ESP32](https://img.shields.io/badge/ESP32-E7352C?style=flat&logo=Espressif&logoColor=white)
![License](https://img.shields.io/badge/License-MIT-blue.svg)

BMP180 barometrik basınç sensörü için geliştirilmiş, non-blocking (engellemesiz) çalışan Arduino kütüphanesi. Roket ve İHA projelerinde yüksek performans için optimize edilmiştir.

## 🚀 Özellikler

- **Non-blocking İşlem**: Ana döngüyü engellemez
- **IIR Filtre**: Gürültü azaltma ve yumuşak geçiş
- **Hata Kontrolü**: I2C iletişim hatalarını algılar
- **Esnek Konfigürasyon**: OSS ayarları ve I2C adresi değiştirilebilir
- **Deniz Seviyesi Kalibrasyonu**: Bilinen yükseklik ile kalibrasyon
- **Arduino & ESP32 Uyumlu**: Çoklu platform desteği

## 📦 Kurulum

### Arduino IDE ile:
1. Bu repoyu indirin veya ZIP olarak kaydedin
2. Arduino IDE → Sketch → Include Library → Add .ZIP Library
3. İndirdiğiniz ZIP dosyasını seçin

### Manuel Kurulum:
```bash
git clone https://github.com/alidirza/BMP180_KUASAR.git
```
Arduino libraries klasörüne kopyalayın: `~/Documents/Arduino/libraries/`

## 🔌 Bağlantı Şeması

| BMP180 | Arduino Uno | ESP32 |
|--------|-------------|-------|
| VCC    | 3.3V/5V     | 3.3V  |
| GND    | GND         | GND   |
| SDA    | A4          | GPIO21|
| SCL    | A5          | GPIO22|

## 💡 Temel Kullanım

```cpp
#include <BMP180_KUASAR.h>

BMP180_KUASAR bmp;

void setup() {
    Serial.begin(115200);
    
    // Sensörü başlat (SDA, SCL, OSS)
    if (bmp.begin(21, 22, 3)) {
        Serial.println("BMP180 başarıyla başlatıldı!");
    }
    
    // IIR filtre ayarla (0.01-1.0)
    bmp.setIirFilter(0.2);
}

void loop() {
    // Non-blocking güncelleme
    bmp.update();
    
    // Veri hazır mı kontrol et
    if (bmp.dataReady()) {
        float temp = bmp.getTemperature();    // °C
        float pressure = bmp.getPressure();   // Pa
        float altitude = bmp.getAltitude();   // m
        
        Serial.print("Sıcaklık: "); Serial.print(temp);
        Serial.print(" °C, Basınç: "); Serial.print(pressure/100.0);
        Serial.print(" hPa, Yükseklik: "); Serial.print(altitude);
        Serial.println(" m");
    }
    
    delay(100);
}
```

## 📚 API Referansı

### Başlatma
```cpp
bool begin(uint8_t sda, uint8_t scl, uint8_t oss = 3)
```
- `sda`: SDA pin numarası
- `scl`: SCL pin numarası  
- `oss`: Oversampling ayarı (0-3, yüksek = daha hassas)

### Veri Okuma
```cpp
void update()                    // Non-blocking güncelleme
bool dataReady()                 // Yeni veri hazır mı?
float getTemperature()           // Sıcaklık (°C)
float getPressure()              // Basınç (Pa)
float getAltitude()              // Yükseklik (m)
```

### Konfigürasyon
```cpp
void setIirFilter(float alpha)                          // IIR filtre katsayısı (0.01-1.0)
void calibrateSeaLevel(float knownAltitudeMeters)       // Deniz seviyesi kalibrasyonu
void changeAddr(uint8_t address)                        // I2C adresi değiştir
bool checkError()                                       // Hata kontrolü
```

## ⚙️ IIR Filtre Kılavuzu

IIR filtre, sensör verilerindeki ani değişimleri yumuşatır:

- **0.01-0.1**: Çok yumuşak, yavaş tepki
- **0.2**: Roket/İHA için ideal
- **0.5**: Orta filtreleme
- **0.8-1.0**: Minimum filtreleme, hızlı tepki

## 🎯 Roket/İHA Projeleri İçin Öneriler

```cpp
void setup() {
    // Yüksek hassasiyet için OSS=3
    bmp.begin(SDA_PIN, SCL_PIN, 3);
    
    // Roket için ideal filtre ayarı
    bmp.setIirFilter(0.2);
    
    // Fırlatma alanının rakımını girin
    bmp.calibrateSeaLevel(120.0); // örnek: 120m rakım
}

void loop() {
    bmp.update();
    
    if (bmp.dataReady()) {
        if (!bmp.checkError()) {
            float altitude = bmp.getAltitude();
            
            // Apogee tespiti, paraşüt kontrolü vb.
            // altitude verisi ile çalışın
        }
    }
    
    // Çok kısa döngü süresi için
    delayMicroseconds(100);
}
```

## 🔧 Sorun Giderme

**Problem**: Sensör başlamıyor
- I2C bağlantılarını kontrol edin
- 3.3V beslemesi kullanın
- Pull-up dirençlerinin olduğundan emin olun

**Problem**: Veri okumuyor
- `checkError()` ile I2C hatalarını kontrol edin
- `dataReady()` metodunu kullanmayı unutmayın

**Problem**: Gürültülü veri
- IIR filtre değerini düşürün (örn: 0.1)
- OSS ayarını yükseltin

## 📄 Lisans

Bu proje MIT lisansı altında dağıtılmaktadır. Detaylar için [LICENSE](LICENSE) dosyasını inceleyebilirsiniz.

## 👨‍💻 Geliştirici

**Ali Rıza Öztürk**  
📧 [alidirza@gmail.com]  
🌐 [GitHub](https://github.com/alidirza)

## 🤝 Katkıda Bulunma

1. Bu repoyu fork edin
2. Feature branch oluşturun (`git checkout -b feature/AmazingFeature`)
3. Değişikliklerinizi commit edin (`git commit -m 'Add some AmazingFeature'`)
4. Branch'inizi push edin (`git push origin feature/AmazingFeature`)
5. Pull Request oluşturun

## ⭐ Bu projeyi beğendiyseniz yıldızlamayı unutmayın!
