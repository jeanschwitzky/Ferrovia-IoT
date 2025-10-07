#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient client; //cria objeto p/ wifi
PubSubClient mqtt(client); //cria objeto p/ mqtt usando WiFi

const String SSID = "FIESC_IOT_EDU"; 
const String PASS = "8120gv08";

const String brokerURL = "test.mosquitto.org";
const int brokerPort = 1883;

const String brokerUser = ""; //variável para o user do broker
const String brokerPass = ""; //variável para a senha do broker

void setup() {
  Serial.begin(115200); //configura a placa para mostrar na tela
  WiFi.begin(SSID, PASS); // tenta conectar na rede
  Serial.println("Conectando no Wifi");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("Conectado com sucesso");

  mqtt.setServer(brokerURL.c_str(),brokerPort);
  Serial.println("Conectando no Broker");

  String boardID = "Motor-"; //cria um nome que começa com "Motor-"
  boardID += String(random(0xffff),HEX); // junta o "Motor-" com um número aleatório Hexadecimal
  
  while(!mqtt.connect(boardID.c_str())){ //Enqunato ão estiver conectado mostra "."
    Serial.print(".");
    delay(200);
  }
  Serial.println("Conectado com sucesso ao broker!");
}

void loop() {
  String msg = "Jean: Oi"; // Informação que será enviada para o broker
  String topico = "AulaIoT/msg";
  mqtt.publish(topico.c_str(), msg.c_str());
  delay(2000);
}

//void callback(char* topic, byte* payload, usigned Long lenght){}