#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "env.h"

void corLed(byte red, byte green, byte blue);

#define PIN_LDR 34
#define PIN_DHT 4
#define PIN_TRIGG 22
#define PIN_ECHO 23

const byte redPin = 14;
const byte greenPin = 26;
const byte bluePin = 25;

#define DHTTYPE DHT11

DHT dht(PIN_DHT, DHT11);

int distancia = 0;
bool presencaAnterior = false;

WiFiClientSecure espClient;

PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);

  espClient.setInsecure();

  // --- Conexão WiFi ---
  Serial.println("Conectando no WiFi...");
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nWiFi conectado!");

  // --- MQTT Broker ---
  client.setServer(BROKER_URL, BROKER_PORT);

  Serial.println("Conectando ao broker...");

  String clientId = "S1-" + String(random(0xffff), HEX);

  // Tenta conectar até conseguir
  while (!client.connected()) {
    client.connect(clientId.c_str(), BROKER_USER_NAME, BROKER_USER_PASS);
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nConectado ao broker!");

  // Inicia sensores
  pinMode(PIN_LDR, INPUT);
  dht.begin();

  pinMode(PIN_TRIGG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  ledcAttach(redPin, 5000, 8);
  ledcAttach(greenPin, 5000, 8);
  ledcAttach(bluePin, 5000, 8);

  corLed(255, 0, 0); 
}

void loop() {

  // Reconexão automática do MQTT caso caia
  if (!client.connected()) {
    String clientId = "S1-" + String(random(0xffff), HEX);
    client.connect(clientId.c_str(), BROKER_USER_NAME, BROKER_USER_PASS);
  }
  client.loop();

  // --- Leituras dos sensores ---
  int luminosidade = analogRead(PIN_LDR);
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  // --- Envio MQTT ---
  char msgLuz[10];
  sprintf(msgLuz, "%d", luminosidade);
  client.publish(TOPIC3, msgLuz);

  char msgTemp[10];
  dtostrf(temperatura, 4, 1, msgTemp); 
  client.publish(TOPIC1, msgTemp);

  char msgUmi[10];
  dtostrf(umidade, 4, 1, msgUmi);
  client.publish(TOPIC2, msgUmi);

  Serial.println("\n📡 Dados enviados:");
  Serial.print("LDR = "); Serial.println(luminosidade);
  Serial.print("Temp = "); Serial.println(temperatura);
  Serial.print("Umid = "); Serial.println(umidade);

  // --- Ultrassônico (mede distância em cm) ---
  distancia = readUltrassonic(PIN_ECHO, PIN_TRIGG);

  if (distancia > 0) {
    Serial.print("Distância: ");
    Serial.print(distancia);
    Serial.println(" cm");
  }

  // Considera presença até 30 cm do sensor
  bool presencaAtual = (distancia > 0 && distancia < 30);

  if (presencaAtual != presencaAnterior) {
    if (presencaAtual) {
      client.publish(TOPIC4, "1");
      Serial.println(">>> PRESENÇA DETECTADA");
      corLed(255, 255, 0);
    } else {
      client.publish(TOPIC4, "0");
      Serial.println(">>> PRESENÇA ENCERRADA");
      corLed(0, 255, 0); 
    }
    presencaAnterior = presencaAtual;
  }

  delay(5000);
}

// --- Cálculo de distância usando sensor ultrassônico ---
int readUltrassonic(byte echo_pin, byte trigg_pin) {

  digitalWrite(trigg_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigg_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigg_pin, LOW);

  unsigned long tempo = pulseIn(echo_pin, HIGH, 30000);
  if (tempo == 0) return -1;

  // Converte tempo do pulso → distância em cm
  return (tempo * 0.0343) / 2;
}

void corLed(byte red, byte green, byte blue) {
  ledcWrite(redPin, red);
  ledcWrite(greenPin, green);
  ledcWrite(bluePin, blue);
}
