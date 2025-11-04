#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "env.h"

WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

const byte redPin = 12;
const byte greenPin = 13;
const byte bluePin = 14;

const byte trigg_pin = 32;
const byte echo_pin = 35;

int distancia = 0;
bool presencaAnterior = false;  // guarda último estado de presença

void setup() {
  Serial.begin(115200);

  // LED RGB (status)
  ledcAttach(redPin, 5000, 8);
  ledcAttach(greenPin, 5000, 8);
  ledcAttach(bluePin, 5000, 8);
  corLed(255, 0, 0);

  wifiClient.setInsecure();

  // --- Conexão WiFi ---
  WiFi.begin(SSID, PASS);
  Serial.print("Conectando no WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    corLed(0, 0, 255);
    delay(300);
  }
  Serial.println("\nConectado ao WiFi!");
  corLed(0, 255, 0);

  // --- Conexão Broker MQTT ---
  mqttClient.setServer(BROKER_URL, BROKER_PORT);
  conectarBroker();

  pinMode(trigg_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    reconnectWiFi();
  }

  if (!mqttClient.connected()) {
    conectarBroker();
  }

  mqttClient.loop();

  distancia = readUltrassonic(echo_pin, trigg_pin);
  if (distancia > 0) {
    Serial.print("Distância: ");
    Serial.print(distancia);
    Serial.println(" cm");
  }

  bool presencaAtual = (distancia > 0 && distancia < 30);

  // Só envia se houve mudança de estado
  if (presencaAtual != presencaAnterior) {
    if (presencaAtual) {
      mqttClient.publish(TOPIC5, "1");
      Serial.println(">>> PRESENÇA DETECTADA (enviado 1)");
      corLed(255, 255, 0);  // amarelo
    } else {
      mqttClient.publish(TOPIC5, "0");
      Serial.println(">>> PRESENÇA ENCERRADA (enviado 0)");
      corLed(0, 255, 0);  // verde
    }
    presencaAnterior = presencaAtual;
  }

  delay(200);
}

// --------------------- Funções auxiliares ------------------------

void conectarBroker() {
  Serial.print("Conectando ao broker MQTT...");
  String userId = "S2-" + String(random(0xffff), HEX);
  while (!mqttClient.connected()) {
    if (mqttClient.connect(userId.c_str(), BROKER_USER_NAME, BROKER_USER_PASS)) {
      Serial.println("\nConectado com sucesso ao broker!");
      corLed(0, 255, 0);
    } else {
      Serial.print(".");
      delay(500);
    }
  }
}

void reconnectWiFi() {
  Serial.println("Reconectando WiFi...");
  WiFi.disconnect();
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nWiFi reconectado!");
}

void corLed(byte red, byte green, byte blue) {
  ledcWrite(redPin, red);
  ledcWrite(greenPin, green);
  ledcWrite(bluePin, blue);
}

int readUltrassonic(byte echo_pin, byte trigg_pin) {
  digitalWrite(trigg_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigg_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigg_pin, LOW);

  unsigned long tempo = pulseIn(echo_pin, HIGH, 30000); // timeout 30ms
  if (tempo == 0) return -1; // sem leitura válida
  return (tempo * 0.0343) / 2;
}