#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""

#include <WiFi.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>


#define BLYNK_PRINT Serial 
BlynkTimer timer;

// Thông tin WiFi và Blynk
char auth[] = "your_blynk_auth";
char ssid[] = "your_name_wifi";
char pass[] = "your_password_wifi";

LiquidCrystal_I2C lcd(0x27, 16, 2);
#define Led_light 25
#define Led_green 26
#define Sensor 34
#define Buzzer 12
#define Fan_pin 27

WidgetLED LED(V1);  // Khai báo WidgetLED ngoài setup()


// fan setup runtime

unsigned long fan_start_time=0;
bool fan_running = false;
const unsigned long fan_time = 10000;

void setup() {
  Serial.begin(921600);
  lcd.init();
  lcd.backlight();

  WiFi.begin(ssid, pass);
  Serial.println(" ");
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println("\nConnected to WiFi!");

  Blynk.begin(auth, ssid, pass);
  pinMode(Led_light, OUTPUT);
  pinMode(Led_green, OUTPUT);
  pinMode(Buzzer, OUTPUT);
  pinMode(Sensor, INPUT);
  pinMode(Fan_pin,OUTPUT);

  Serial.println("Warming up the MQ9 sensor");
  delay(30000);  // wait for the MQ9 to warm up

  timer.setInterval(2000L, GasSmoke); // Chạy GasSmoke mỗi 2s
}

void GasSmoke() {
  

  int value = analogRead(Sensor);
  value = map(value, 0, 4095, 0, 100);
  
  Serial.print("Gas Level: ");
  Serial.println(value);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GAS Level: ");
  lcd.print(value);
  lcd.print(" ");

  if (value >= 40) {
    digitalWrite(Buzzer, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("Warning gas!!! ");
    digitalWrite(Led_light, HIGH);
    digitalWrite(Led_green, LOW);

   Blynk.logEvent("gas_detected");
    
    if (!fan_running) {
      digitalWrite(Fan_pin, HIGH);
      fan_running = true;
      fan_start_time = millis(); // Record the start time
      Serial.println("Fan started");
    }

    
    LED.on();
  } else {
    digitalWrite(Buzzer, LOW);
    lcd.setCursor(0, 1);                        
    lcd.print("Normal ");
    digitalWrite(Led_light, LOW);
    digitalWrite(Led_green, HIGH);
    if (!fan_running) {
      digitalWrite(Fan_pin, LOW);
    }
    LED.off();
  }
  Blynk.virtualWrite(V0, value);
}

void checkFanTimer() {
  // If fan is running and 10 seconds have passed, turn it off
  if (fan_running && (millis() - fan_start_time >= fan_time)) {
    digitalWrite(Fan_pin, LOW);
    fan_running = false;
    Serial.println("Fan stopped after 10 seconds");
  }
}


void loop() {
  Blynk.run();
  timer.run();  // Thay vì delay(2000)
  checkFanTimer();
}
