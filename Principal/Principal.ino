#include <WiFi.h>
#include <PubSubClient.h>
#include "env.h"
WiFiClient client;          //cria objeto p/ wifi
PubSubClient mqtt(client);  //cria objeto p/ mqtt usando WiFi




void setup() {
  Serial.begin(115200);    //configura a placa para mostrar na tela
  WiFi.begin(SSID, PASS);  // tenta conectar na rede
  Serial.println("Conectando no Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("Conectado com sucesso");

  mqtt.setServer(Broker_URL, Broker_PORT);
  Serial.println("Conectando no Broker");
  String userId = "Motor-";               //cria um nome que começa com "Motor-"
  userId += String(random(0xffff), HEX);  // junta o "Motor-" com um número aleatório Hexadecimal

  while (!mqtt.connected()){
    mqtt.connect(userId.c_str(),BROKER_USR_NAME,BROKER_USR_PASS);  //Enqunato ão estiver conectado mostra "."
    Serial.print(".");
    delay(200);
  }
  Serial.println("Conectado com sucesso ao broker!");
  mqtt.subscribe(TOPIC1);
  mqtt.setCallback(callback);
  pinMode(2,OUTPUT);
}

void loop() {
  //String msg = "Jean: Oi"; // Informação que será enviada para o broker
  //String TOPIC1 = "AulaIoT/msg";
  //mqtt.publish(TOPIC1.c_str(), msg.c_str());
  //delay(2000);
  //mqtt.loop();

  String mensagem = "";
  if (Serial.available() > 0) {
    mensagem = Serial.readStringUntil('\n');
    Serial.print("Mensagem digitada: ");
    Serial.println(mensagem);
    mqtt.publish("Bezao", mensagem.c_str()); //envia msg
    
  }
  mqtt.loop(); //mantem a conexão
}

void callback(char* topic, byte* payload, unsigned long length){
  String mensagemRecebida = "";
  for(int i = 0; i < length; i++){
    mensagemRecebida += (char) payload[i];
  }
  Serial.println(mensagemRecebida);
  if(mensagemRecebida == "1") {
    digitalWrite(2,HIGH);
    Serial.println("Ligando...");
  }
  if(mensagemRecebida == "0") {
    digitalWrite(2,LOW);
    Serial.println("Apagando...");
  }
}