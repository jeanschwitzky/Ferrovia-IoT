#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient client;            //cria objeto p/ wifi
PubSubClient mqtt(client);    //cria objeto p/ mqtt usando WiFi

const String SSID = "FIESC_IOT_EDU";
const String PASS = "8120gv08";

const String brokerURL = "test.mosquitto.org";
const int brokerPort = 1883 ;

const String brokerUser = "";   //variável para o user do broker
const String brokerPass = "";   //variável para a senha do broker

void setup() {
  Serial.begin(115200); //configura a placa para mostrar na tela
  WiFi.begin(SSID, PASS); //tenta conectar na rede
  Serial.println("Conectando no Wifi");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nConectado com sucesso");

  mqtt.setServer(brokerURL.c_str(),brokerPort);
  Serial.println("Conectando no Broker");

  String boardID = "Sensor";    //cria um nome que começa com "s1-"
  boardID += String(random(0xffff),HEX); 

  //Enquanto não estiver conectado mostra "."
  while(!mqtt.connect(boardID.c_str())){
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nConectado com sucesso no broker!");
}

void loop() {
    String msg = "Arthur: Salve!";  //Informação que será enviada para o broker
    String topico = "AulaIoT/msg";
    mqtt.publish(topico.c_str(),msg.c_str());
    delay(2000);
    mqtt.loop();

}

//void callback(char* topic, byte* payload, usigned Long lenght){