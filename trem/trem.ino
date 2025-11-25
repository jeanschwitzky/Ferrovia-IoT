#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h> 
#include "env.h"

WiFiClientSecure wifiClient;          
	PubSubClient mqttClient(wifiClient);  
	
	const int LED_VERDE = 18; 
	const int LED_VERMELHO = 19; 
	
	void setup() {
  Serial.begin(115200);    
  wifiClient.setInsecure();  
  WiFi.begin(SSID, PASS);  
  Serial.println("Conectando no Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("Conectado com sucesso");

  mqttClient.setServer(BROKER_URL, BROKER_PORT);
  Serial.println("Conectando no Broker");
  String userID = "trem-";               
  userID += String(random(0xffff), HEX);  

  while (!mqttClient.connected()) { 
    mqttClient.connect(userID.c_str(), BROKER_USR_NAME, BROKER_URS_PASS); 
    Serial.print(".");
    delay(200);
  }
  Serial.println("Conectado com sucesso ao broker!");
  mqttClient.subscribe(TOPIC11);
  mqttClient.setCallback(callback);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_VERMELHO, LOW);

}

void loop() {
  String mensagem = "";
  if (Serial.available() > 0) {
    mensagem = Serial.readStringUntil('\n');
    Serial.print("Mensagem digitada: ");
    Serial.println(mensagem);
    mqttClient.publish("bezinho", mensagem.c_str());
    
  }
  mqttClient.loop();
}

void callback(char* topic, byte* payload, unsigned long length){
  String mensagemRecebida = "";
  for(int i = 0; i < length; i++){
    mensagemRecebida += (char) payload[i];
  }
  mensagemRecebida.trim(); 
  Serial.println(mensagemRecebida);
  int velocidade = mensagemRecebida.toInt();

  Serial.print("Velocidade Recebida: ");
  Serial.println(velocidade);

  if (velocidade == 0) {
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_VERMELHO, LOW);
    Serial.println("Trem parado. LEDs apagados.");
  } else if (velocidade > 0) {
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_VERMELHO, LOW);
    Serial.println("Trem para frente. LED Verde aceso.");
  } else { 
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_VERMELHO, HIGH);
    Serial.println("Trem para trás. LED Vermelho aceso.");
  }
}