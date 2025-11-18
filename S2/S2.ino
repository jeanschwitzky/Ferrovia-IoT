#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "env.h"

// Protótipos de função para evitar erros de compilação
void corLed(byte red, byte green, byte blue);
int readUltrassonic(byte echo_pin, byte trigg_pin);

WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

// --- CONFIGURAÇÕES DE PINOS ---

// Ultrassônico 1 (Presença 2 - TOPIC5)
const byte trigg_pin1 = 22;
const byte echo_pin1 = 23;

// Ultrassônico 2 (Presença 4 - TOPIC6)
const byte trigg_pin2 = 12;
const byte echo_pin2 = 13;

// LED RGB (Status)
const byte redPin = 14;
const byte greenPin = 26;
const byte bluePin = 25;

int distancia1 = 0;
int distancia2 = 0;
bool presencaAnterior1 = false;  // guarda último estado de presença (Ultrassônico 1)
bool presencaAnterior2 = false;  // guarda último estado de presença (Ultrassônico 2)

void setup() {
  Serial.begin(115200);

  // LED RGB (status)
  ledcAttach(redPin, 5000, 8);
  ledcAttach(greenPin, 5000, 8);
  ledcAttach(bluePin, 5000, 8);
  corLed(255, 0, 0);

  wifiClient.setInsecure();

  // --- Configuração de Pinos ---
  pinMode(trigg_pin1, OUTPUT);
  pinMode(echo_pin1, INPUT);
  pinMode(trigg_pin2, OUTPUT);
  pinMode(echo_pin2, INPUT);

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
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    reconnectWiFi();
  }

  if (!mqttClient.connected()) {
    conectarBroker();
  }

  mqttClient.loop();

  // --- LEITURA DE SENSORES ---

  // Ultrassônico 1 (TOPIC5)
  distancia1 = readUltrassonic(echo_pin1, trigg_pin1);
  // Ultrassônico 2 (TOPIC6)
  distancia2 = readUltrassonic(echo_pin2, trigg_pin2);

  // --- LÓGICA DE PUBLICAÇÃO ---

  // 1. Ultrassônico 1 (TOPIC5)
  bool presencaAtual1 = (distancia1 > 0 && distancia1 < 30);
  if (presencaAtual1 != presencaAnterior1) {
    const char* msg = presencaAtual1 ? "1" : "0";
    mqttClient.publish(TOPIC5, msg);
    Serial.print(">>> PRESENÇA 1 (TOPIC5): "); Serial.println(msg);
    presencaAnterior1 = presencaAtual1;
  }

  // 2. Ultrassônico 2 (TOPIC6)
  bool presencaAtual2 = (distancia2 > 0 && distancia2 < 30);
  if (presencaAtual2 != presencaAnterior2) {
    const char* msg = presencaAtual2 ? "1" : "0";
    mqttClient.publish(TOPIC6, msg);
    Serial.print(">>> PRESENÇA 2 (TOPIC6): "); Serial.println(msg);
    presencaAnterior2 = presencaAtual2;
  }

  // --- LÓGICA DE LED RGB (Status) ---
  if (presencaAtual1 || presencaAtual2) {
    corLed(255, 255, 0); // Amarelo: Presença Detectada em qualquer um
  } else {
    corLed(0, 255, 0); // Verde: Sem Presença
  }

  // --- LOG ---
  Serial.println("-------------------------------");
  Serial.print("Distância 1 (TOPIC5): "); Serial.print(distancia1); Serial.println(" cm");
  Serial.print("Distância 2 (TOPIC6): "); Serial.print(distancia2); Serial.println(" cm");
  Serial.println("-------------------------------");

  delay(1000); // Envia a cada 5 segundos
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
