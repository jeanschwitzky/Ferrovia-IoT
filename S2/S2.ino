#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "env.h"

WiFiClientSecure wifiClient;          //cria objeto p/ wifi
PubSubClient mqttClient(wifiClient);  //cria objeto p/ mqttClient usando WiFi


const byte redPin = 12;
const byte greenPin = 13;
const byte bluePin = 14;


const byte trigg_pin = 32;
const byte echo_pin = 35;
int distancia = 0;
unsigned long tempo = 0;


void setup() {
  ledcAttach(redPin, 5000, 8);
  ledcAttach(greenPin, 5000, 8);
  ledcAttach(bluePin, 5000, 8);
  corLed(255, 0, 0);
  Serial.begin(115200);  //configura a placa para mostrar na tela
  wifiClient.setInsecure();
  WiFi.begin(SSID, PASS);  // tenta conectar na rede
  Serial.println("Conectando no Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    corLed(0, 0, 255);
    delay(200);
  }
  Serial.println("Conectado com sucesso");
  corLed(0, 255, 0);

  mqttClient.setServer(BROKER_URL, BROKER_PORT);
  Serial.println("Conectando no Broker");
  String userId = "S2-";                  //cria um nome que começa com "S2-"
  userId += String(random(0xffff), HEX);  // junta o "S2-" com um número aleatório Hexadecimal

  while (!mqttClient.connected()) {  //Enqunato ão estiver conectado mostra "."
    mqttClient.connect(userId.c_str(), BROKER_USER_NAME, BROKER_USER_PASS);
    Serial.print(".");
    delay(200);
  }
  mqttClient.subscribe(TOPIC3);
  mqttClient.subscribe(TOPIC4);
  mqttClient.setCallback(callback);
  Serial.println("Conectado com sucesso ao broker!");
  pinMode(2, OUTPUT);
  pinMode(trigg_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
}

void loop() {
  //String msg = "Jean: Oi"; // Informação que será enviada para o broker
  //String TOPIC1 = "AulaIoT/msg";
  //mqttClient.publish(TOPIC1.c_str(), msg.c_str());
  //delay(2000);
  //mqttClient.loop();

  distancia = readUltrassonic(echo_pin, trigg_pin);
  delay(100);
  if (distancia > 100) {
    bool presenca = 1;
    if (Serial.available() > 0) {
      Serial.print(presenca);
      mqttClient.publish(TOPIC5, presenca);  //envia msg
    }
  }



  mqttClient.loop();  //mantem a conexão
}

void callback(char* TOPIC3, byte* payload, unsigned long length) {
  String mensagemRecebida = "";
  for (int i = 0; i < length; i++) {
    mensagemRecebida += (char)payload[i];
  }
  Serial.println(mensagemRecebida);
  if (mensagemRecebida == "1") {
    digitalWrite(2, HIGH);
    Serial.println("Ligando...");
  }
  if (mensagemRecebida == "0") {
    digitalWrite(2, LOW);
    Serial.println("Apagando...");
  }
}

void corLed(byte red, byte green, byte blue) {
  ledcWrite(redPin, red);
  ledcWrite(greenPin, green);
  ledcWrite(bluePin, blue);
}


int readUltrassonic(byte echo_pin, byte trigg_pin) {
  unsigned long tempo = 0;

  digitalWrite(trigg_pin, HIGH);
  delayMicroseconds(50);
  digitalWrite(trigg_pin, LOW);
  tempo = pulseIn(echo_pin, HIGH);
  return ((tempo * 340.29) / 2) / 10000;
}