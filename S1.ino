#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include "env.h"

WiFiClientSecure wifiClient;          //cria objeto p/ wifi
PubSubClient mqttClient(wifiClient);  //cria objeto p/ mqtt usando WiFi

const byte ldr_pin = 32;
int val_ldr = 0;

void setup() {
  Serial.begin(115200);    //configura a placa para mostrar na tela
  WiFi.begin(SSID, PASS);  //tenta conectar na rede
  wifiClient.setInsecure();
  Serial.println("Conectando no Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nConectado com sucesso");

  mqtt.setServer(BROKER_URL, BROKER_PORT);
  Serial.println("Conectando no Broker");
  String boardID = "Sensor";  //cria um nome que começa com "s1-"
  boardID += String(random(0xffff), HEX);

  while (!mqtt.connected()) {
    mqtt.connect(boardID.c_str(), BROKER_USER_NAME, BROKER_USER_PASS);
    Serial.print(".");
    delay(2000);
  }
  mqtt.setCallback(callback);
  Serial.println("Conectado com sucesso ao broker!");
  pinMode(2, OUTPUT);
  pinMode(ldr_pin, INPUT);
}


void loop() {
  //String msg = "Arthur: Bezão";  //Informação que será enviada para o broker
  //String TOPIC1 = "AulaIoT/msg";
  //mqtt.publish(TOPIC1.c_str(),msg.c_str());
  //delay(2000);
  //mqtt.loop();

  val_ldr = analogRead(ldr_pin);
  Serial.println(val_ldr);
  delay(100);
  if ( val_ldr > 0) {
    mqtt.publish(TOPIC3, 1);  //envia msg
  }
  mqtt.loop();  //mantem a conexão
}

void callback(char* topic, byte* payload, unsigned long lenght) {
  String mensagemRecebida = "";
  for (int i = 0; i < lenght; i++) {
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


int ler_ldr(byte pino) {
  return analogRead(byte);
}
