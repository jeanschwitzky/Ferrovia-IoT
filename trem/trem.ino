#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient client; //cria objeto p/ wifi
PubSubClient mqtt(client); //cria objeto p/ mqtt usando WiFi

const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

const String brokerURL = "test.mosquitto.org";
const int brokerPort = 1883;

const String brokerUser = ""; //variavel para o user do broker
const String brokerPass = ""; //variavel para senha do broker  
void setup() {
  Serial.begin(115200); //configura a placa para mostrar na tela
  WiFi.begin(SSID,PASS); //tenta conectar na rede 
  Serial.println("Conectando no WiFi");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("Conectado com sucesso");
  mqtt.setServer(brokerURL.c_str(),brokerPort);
  Serial.println("conectando no Broker");
//Cria um nome que comeca com principal
  String boardID = "Principal-";
//junta o "Principal" com um numero aleatorio hexadecimal
boardID += String(random(0xffff),HEX);

  while(mqtt.connect(boardID.c_str())){
    Serial.print(".");
    delay(200);
  }
  Serial.println("Conectado  com sucesso ao broker");
}

void loop() {
  // put your main code here, to run repeatedly:

}