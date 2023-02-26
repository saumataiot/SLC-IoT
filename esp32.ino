#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#include <addons/TokenHelper.h>

#include <addons/RTDBHelper.h>

#define WIFI_SSID "SAUMATA TEKNOSAINS GLOBAL"
#define WIFI_PASSWORD "11022022"
#define API_KEY "API_KEY"
#define DATABASE_URL "URL"
#define USER_EMAIL "USER_EMAIL"
#define USER_PASSWORD "USER_PASSWORD"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;

#define RELAY1 25
#define RELAY2 26
#define RELAY3 27
#define RELAY4 14
#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  unsigned long ms = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  config.token_status_callback = tokenStatusCallback;
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Firebase.setDoubleDigits(5);

  config.timeout.serverResponse = 10 * 1000;

}

void loop() {
  float lembab = dht.readHumidity();
  float suhu = dht.readTemperature();

  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  else {
    Serial.print(F("Kelembaban: "));
    Serial.print(lembab);
    Serial.print(F("%  Suhu: "));
    Serial.print(suhu);
    Serial.print(F("°C "));

    if (Firebase.ready() && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0))
    {
      sendDataPrevMillis = millis();
      Serial.printf("Set suhu.... %s\n", Firebase.RTDB.setFloat(&fbdo, F("Suhu/"), suhu) ? "ok" : fbdo.errorReason().c_str());
      Serial.printf("Set lembab.. %s\n", Firebase.RTDB.setFloat(&fbdo, F("Lembab/"), lembab) ? "ok" : fbdo.errorReason().c_str());

      bool relVal1, relVal2, relVal3, relVal4;
      Serial.printf("Get bool rel 1... %s\n", Firebase.RTDB.getBool(&fbdo, F("/Relay/Relay1"), &relVal1) ? relVal1 ? "true" : "false" : fbdo.errorReason().c_str());
      Serial.printf("Get bool rel 2... %s\n", Firebase.RTDB.getBool(&fbdo, F("/Relay/Relay2"), &relVal2) ? relVal2 ? "true" : "false" : fbdo.errorReason().c_str());
      Serial.printf("Get bool rel 3... %s\n", Firebase.RTDB.getBool(&fbdo, F("/Relay/Relay3"), &relVal3) ? relVal3 ? "true" : "false" : fbdo.errorReason().c_str());
      Serial.printf("Get bool rel 4... %s\n", Firebase.RTDB.getBool(&fbdo, F("/Relay/Relay4"), &relVal4) ? relVal4 ? "true" : "false" : fbdo.errorReason().c_str());

      if (relVal1 == "true") {
        digitalWrite(RELAY1, LOW);
      }
      else if (relVal1 == "false") {
        digitalWrite(RELAY1, HIGH);
      }

      if (relVal2 == "true") {
        digitalWrite(RELAY2, LOW);
      }
      else if (relVal2 == "false") {
        digitalWrite(RELAY2, HIGH);
      }

      if (relVal3 == "true") {
        digitalWrite(RELAY3, LOW);
      }
      else if (relVal3 == "false") {
        digitalWrite(RELAY3, HIGH);
      }

      if (relVal4 == "true") {
        digitalWrite(RELAY4, LOW);
      }
      else if (relVal4 == "false") {
        digitalWrite(RELAY4, HIGH);
      }
    }
  }
}
