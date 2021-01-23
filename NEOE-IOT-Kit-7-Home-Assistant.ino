/**********************************************************************************************************************************
  Arduino-Sketch für das NEOE-IOT-Kit-7, "Smart Home Digitaler Raumluftsensor mit MQ-135 und NodeMCU. Arduino-Programmierung. 
  MQTT-kompatibel zur Anbindung an Home Assistant."
  Dieser Arduino-Sketch wird in folgendem Tutorial verwendet:
  https://www.neoe.io/blogs/tutorials/digitaler-raumluftsensor-mqtt-kompatibel-aufbau-variante-breadboard
  Fragen und Anregungen bitte in unserer Facebook-Gruppe adressieren, damit die gesamte Community davon profitiert.
  https://www.facebook.com/groups/neoe.io/
  Datum der letzten Änderung: 16. Dezember, 2020
**********************************************************************************************************************************/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

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

// MQTT-Topic für Home Assistant MQTT Auto Discovery
const char* mqtt_config_topic = "homeassistant/binary_sensor/raumluft/config"; // Name des Zimmers bei Bedarf ändern
const char* mqtt_state_topic = "homeassistant/binary_sensor/raumluft/state"; // Name des Zimmers bei Bedarf ändern

// Speicher-Reservierung für JSON-Dokument, kann mithilfe von arduinojson.org/v6/assistant noch optimiert werden
StaticJsonDocument<512> doc_config;
StaticJsonDocument<512> doc_state;

char mqtt_config_data[512];
char mqtt_state_data[512];

bool configured = false;

WiFiClient espClient;
PubSubClient client(espClient);

// Funktion um CO2-Werte per MQTT zu übermitteln
void publishData(int pinValue) {
  doc_state["state"] = round(pinValue);
  serializeJson(doc_state, mqtt_state_data);
  client.publish(mqtt_state_topic, mqtt_state_data);
}

// Funktion zur WLAN-Verbindung
void setup_wifi() {
  delay(10);
  /* Mit WLAN verbinden */
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

// Funktion für Home Assistant MQTT Auto Discovery
void configMqtt() {
  doc_config["name"] = "Raumluft";  // Name des Zimmers bei Bedarf ändern
  doc_config["device_class"] = "gas";
  doc_config["state_topic"] = mqtt_state_topic;
  doc_config["payload_on"] = "0";
  doc_config["payload_off"] = "1";
  doc_config["value_template"] = "{{ value_json.state }}";
  serializeJson(doc_config, mqtt_config_data);
  client.publish(mqtt_config_topic, mqtt_config_data, true);
  delay(1000);
}

void setup() {
  analogWrite(PINgreen, 0);
  analogWrite(PINred, 0);
  analogWrite(PINblue, 1024);

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setBufferSize(512);
  if (!client.connected()) client.connect(mqtt_client, mqtt_user, mqtt_password);
  configMqtt();
}

void loop() {

  if (!client.connected()) client.connect(mqtt_client, mqtt_user, mqtt_password);

  pinValue = digitalRead(_pin);

  // Daten via MQTT zu übermitteln
  publishData(pinValue);

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
