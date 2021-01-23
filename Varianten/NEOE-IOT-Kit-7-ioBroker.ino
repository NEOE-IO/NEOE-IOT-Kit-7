/**********************************************************************************************************************************
  Arduino-Sketch für das NEOE-IOT-Kit-7, "Smart Home Digitaler Raumluftsensor mit MQ-135 und NodeMCU. Arduino-Programmierung. 
  MQTT-kompatibel zur Anbindung an ioBroker (experimentell)."
  Hardware-Aufbau siehe folgendes Tutorial:
  https://www.neoe.io/blogs/tutorials/digitaler-raumluftsensor-mqtt-kompatibel-aufbau-variante-breadboard
  Fragen und Anregungen bitte in unserer Facebook-Gruppe adressieren, damit die gesamte Community davon profitiert.
  https://www.facebook.com/groups/neoe.io/
  Datum der letzten Änderung: 23. Januar, 2021
**********************************************************************************************************************************/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// Alle zwei Sekunden eine MQTT-Nachricht senden (es gibt zwei Delays)
int delay_time = 1000;

// Digital PIN
uint8_t _pin = 5;
int pinValue;

// Parameter für die Indikator-LED
int PINblue = 12;
int PINgreen = 13;
int PINred = 15;

// WLAN-Zugangsdaten hier hinterlegen
const char* ssid = "NAME DES WLAN NETZWERKS"; // Anführungszeichen beibehalten
const char* password = "WLAN-PASSWORT"; // Anführungszeichen beibehalten, also z.B. so: "Geheim"

// Die für den MQTT-Server erforderlichen Daten hier hinterlegen
const char* mqtt_client = "NEOE-IOT-KIT-7-1"; // Wenn mehrere "NEOE-IOT-Kit-7" im Einsatz sind, einfach mit der letzten Zahl durchnummerieren
const char* mqtt_server = "IP-ADRESSE DES MQTT-SERVERS"; // Anführungszeichen beibehalten, also z.B. so: "192.168.0.123"
const uint16_t mqtt_port = 1883;
const char* mqtt_user = "BENUTZERNAME"; // Anführungszeichen beibehalten
const char* mqtt_password = "PASSWORT"; // Anführungszeichen beibehalten, also z.B. so: "Geheim"

// MQTT-Topic 
const char* mqtt_state_topic = "raumluft/state"; 

WiFiClient espClient;
PubSubClient client(espClient);

// Funktion zur WLAN-Verbindung
void setup_wifi() {
  delay(10);
  /* Mit WLAN verbinden */
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void setup() {
  analogWrite(PINgreen, 0);
  analogWrite(PINred, 0);
  analogWrite(PINblue, 1024);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setBufferSize(512);
  if (!client.connected()) client.connect(mqtt_client, mqtt_user, mqtt_password);
}

void loop() {

  if (!client.connected()) client.connect(mqtt_client, mqtt_user, mqtt_password);

  pinValue = digitalRead(_pin);

  // Daten via MQTT zu übermitteln
  // publishData(pinValue);
  client.publish(mqtt_state_topic, String(pinValue).c_str());

  // Indikator-LED
  if (pinValue == 0) {
    analogWrite(PINgreen, 0);
    analogWrite(PINred, 1024);
    analogWrite(PINblue, 0);
    delay(delay_time);
    analogWrite(PINgreen, 0);
    analogWrite(PINred, 0);
    analogWrite(PINblue, 0);
  }
  else
  {
    analogWrite(PINgreen, 1024);
    analogWrite(PINred, 0);
    analogWrite(PINblue, 0);
    delay(delay_time);
  }

  // Anzahl der Messungen reduzieren, da sonst zu viele MQTT-Meldungen versendet werden
  delay(delay_time);

}
