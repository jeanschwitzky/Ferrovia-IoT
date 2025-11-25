#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <ESP32Servo.h> 
#include "env.h"

WiFiClientSecure wifiClient;
PubSubClient mqtt(wifiClient);
Servo servo1;
Servo servo2;

#define PIN_TRIGG 22
#define PIN_ECHO 23

#define PIN_SERVO1 18 
#define PIN_SERVO2 21 

int distancia = 0;
bool presencaAnterior = false; 
const int LIMITE_DISTANCIA = 30; 

void callback(char* topic, uint8_t* payload, unsigned int length);
int readUltrassonic(byte echo_pin, byte trigg_pin);

void setup() {
  Serial.begin(115200);
  wifiClient.setInsecure();

  // --- Conexão WiFi ---
  WiFi.begin(SSID, PASS);
  Serial.println("Conectando no Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("Conectado com sucesso");

  // --- Conexão Broker MQTT ---
  mqtt.setServer(Broker_URL, Broker_PORT);
  Serial.println("Conectando no Broker");
  String userId = "S3-" + String(random(0xffff), HEX);
  while (!mqtt.connected()) {
    if (mqtt.connect(userId.c_str(), BROKER_USR_NAME, BROKER_USR_PASS)) {
        Serial.println("\nConectado com sucesso ao broker!");
    } else {
        Serial.print(".");
        delay(200);
    }
  }

  // --- Inscrição nos Tópicos e Callback ---
  mqtt.setCallback(callback);
  mqtt.subscribe(TOPIC4); 
  mqtt.subscribe(TOPIC5); 
  mqtt.subscribe(TOPIC8); 

  // --- Configuração dos Pinos ---
  pinMode(PIN_TRIGG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  // --- Configuração dos Servos ---
  servo1.attach(PIN_SERVO1);
  servo2.attach(PIN_SERVO2);
  servo1.write(90); 
  servo2.write(90); 
}

void loop() {
  mqtt.loop();
  distancia = readUltrassonic(PIN_ECHO, PIN_TRIGG);

  bool presencaAtual = (distancia > 0 && distancia < LIMITE_DISTANCIA);
  if (presencaAtual != presencaAnterior) {
    const char* msg = presencaAtual ? "1" : "0";
    mqtt.publish(TOPIC8, msg);
    Serial.print(">>> PRESENÇA S3 (TOPIC8) PUBLICADA: "); Serial.println(msg);
    presencaAnterior = presencaAtual;
  }

  Serial.print("Distância S3: "); Serial.print(distancia); Serial.print(" cm | ");
  Serial.print("Presença Atual: "); Serial.println(presencaAtual ? "SIM" : "NÃO");

  delay(500);
}

void callback(char* topic, uint8_t* payload, unsigned int length) {
  String mensagemRecebida = "";

  for (unsigned int i = 0; i < length; i++) {
    mensagemRecebida += (char)payload[i];
  }

  mensagemRecebida.trim();

  Serial.print("Mensagem recebida no tópico: ");
  Serial.println(topic);
  Serial.print("Conteúdo: ");
  Serial.println(mensagemRecebida);

  String topicString = String(topic);

  if (mensagemRecebida == "1") {
    if (topicString.equals(TOPIC5)) {
      servo1.write(90);
      Serial.println("Servo 1 -> 90 graus (Acionado por TOPIC5)");
    } else if (topicString.equals(TOPIC8)) {
      servo1.write(0);
      Serial.println("Servo 1 -> 0 graus (Acionado por TOPIC8)");
      servo2.write(90);
      Serial.println("Servo 2 -> 90 graus (Acionado por TOPIC8)");
    } else if (topicString.equals(TOPIC4)) {
      servo2.write(90);
      Serial.println("Servo 2 -> 90 graus (Acionado por TOPIC4)");
    }
  } else if (mensagemRecebida == "0") {
    if (topicString.equals(TOPIC5)) {
      servo1.write(0);
      Serial.println("Servo 1 -> 0 graus (Desacionado por TOPIC5)");
    } else if (topicString.equals(TOPIC8)) {
      servo2.write(0);
      Serial.println("Servo 2 -> 0 graus (Desacionado por TOPIC8)");
    } else if (topicString.equals(TOPIC4)) {
      servo2.write(0);
      Serial.println("Servo 2 -> 0 graus (Desacionado por TOPIC4)");
    }
  }
}

// --- FUNÇÃO DE LEITURA DO SENSOR ULTRASSÔNICO ---
int readUltrassonic(byte echo_pin, byte trigg_pin) {
  digitalWrite(trigg_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigg_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigg_pin, LOW);

  unsigned long tempo = pulseIn(echo_pin, HIGH, 50000);

  if (tempo == 0) return -1;

  return (tempo * 0.0343) / 2;
}