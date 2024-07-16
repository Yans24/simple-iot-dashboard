#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <DallasTemperature.h>
#include <OneWire.h>

#define FIREBASE_HOST "wahyu-b55e8-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "VQRjG2UnOoiHyFZajEn68o1c5EuPdvA7cDaA7xon"

const char* ssid = "Yans2";
const char* password = "00000000";

#define TRIGPIN D5
#define ECHOPIN D6

#define oneWireBus D0
OneWire oneWire(oneWireBus);
DallasTemperature sensorDS(&oneWire);

FirebaseData firebaseData;
FirebaseAuth firebaseAuth;
FirebaseConfig firebaseConfig;

#define RELAY1 D3
#define RELAY2 D4
#define RELAY3 D7
#define RELAY4 D8

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi with IP: ");
  Serial.println(WiFi.localIP());

  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);

  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);

  pinMode(RELAY1, OUTPUT);
  pinMode(RELAY2, OUTPUT);
  pinMode(RELAY3, OUTPUT);
  pinMode(RELAY4, OUTPUT);

  sensorDS.begin();
}

void loop() {
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);

  long duration = pulseIn(ECHOPIN, HIGH);
  float distance = duration / 2 * 0.0343; 
  Serial.print("Distance: ");
  Serial.println(distance);

  sensorDS.requestTemperatures();
  float waterTemperature = sensorDS.getTempCByIndex(0);
  Serial.print("Water Temperature: ");
  Serial.println(waterTemperature);

  if (Firebase.setFloat(firebaseData, "/Water_Level", distance)) {
    Serial.println("Water Level updated");
  } else {
    Serial.print("Failed to update Water Level: ");
    Serial.println(firebaseData.errorReason());
  }

  if (Firebase.setFloat(firebaseData, "/Water_Temperature", waterTemperature)) {
    Serial.println("Water Temperature updated");
  } else {
    Serial.print("Failed to update water temperature: ");
    Serial.println(firebaseData.errorReason());
  }

  if (Firebase.getInt(firebaseData, "/Relay1")) {
    digitalWrite(RELAY1, firebaseData.intData() ? HIGH : LOW);
  }
  if (Firebase.getInt(firebaseData, "/Relay2")) {
    digitalWrite(RELAY2, firebaseData.intData() ? HIGH : LOW);
  }
  if (Firebase.getInt(firebaseData, "/Relay3")) {
    digitalWrite(RELAY3, firebaseData.intData() ? HIGH : LOW);
  }
  if (Firebase.getInt(firebaseData, "/Relay4")) {
    digitalWrite(RELAY4, firebaseData.intData() ? HIGH : LOW);
  }

  delay(2000);
}

