#include <WiFi.h>
#include <PubSubClient.h>
#include "env.h"

WiFiClient client;            //cria objeto p/ wifi
PubSubClient mqtt(client);    //cria objeto p/ mqtt usando WiFi

void setup() {
  Serial.begin(115200); //configura a placa para mostrar na tela
  WiFi.begin(SSID, PASS); //tenta conectar na rede
  Serial.println("Conectando no Wifi");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nConectado com sucesso");

  mqtt.setServer(BROKER_URL,BROKER_PORT);
  Serial.println("Conectando no Broker");
  String boardID = "Sensor";    //cria um nome que começa com "s1-"
  boardID += String(random(0xffff), HEX);

  while(!mqtt.connected()){
    mqtt.connect(boardID.c_str(),BROKER_USER_NAME,BROKER_USER_PASS);
    Serial.print(".");
    delay(2000);
  }
  mqtt.subscribe(TOPIC1);
  mqtt.setCallback(callback);
  Serial.println("Conectado com sucesso ao broker!");
  pinMode(2,OUTPUT);
}


void loop() {
    //String msg = "Arthur: Bezão";  //Informação que será enviada para o broker
    //String TOPIC1 = "AulaIoT/msg";
    //mqtt.publish(TOPIC1.c_str(),msg.c_str());
    //delay(2000);
    //mqtt.loop();

    String mensagem = "";
    if(Serial.available() > 0){
        mensagem = Serial.readStringUntil('\n');
        Serial.print("Mensagem digitada: ");
        Serial.println(mensagem);
        // mensagem = "Arthur: " + mensagem;
        mqtt.publish("Jean",mensagem.c_str()); //envia msg
    } 
    mqtt.loop(); //mantem a conexão
}

void callback(char* topic, byte* payload, unsigned long lenght){
    String mensagemRecebida = "";
    for(int i = 0; i < lenght; i++){
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