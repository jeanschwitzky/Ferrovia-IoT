#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h> 
#include "env.h"

WiFiClientSecure wifiClient;          //cria objeto p/ wifi
	PubSubClient mqttClient(wifiClient);  //cria objeto p/ mqttClient usando WiFi
	
	// Pinos dos LEDs
	const int LED_VERDE = 18; // Exemplo de pino para o LED Verde
	const int LED_VERMELHO = 19; // Exemplo de pino para o LED Vermelho
	
	void setup() {
  Serial.begin(115200);    //configura a placa para mostrar na tela
  wifiClient.setInsecure();  
  WiFi.begin(SSID, PASS);  // tenta conectar na rede
  Serial.println("Conectando no Wifi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println("Conectado com sucesso");

  mqttClient.setServer(BROKER_URL, BROKER_PORT);
  Serial.println("Conectando no Broker");
  String userID = "Motor-";               //cria um nome que começa com "Motor-"
  userID += String(random(0xffff), HEX);  // junta o "Motor-" com um número aleatório Hexadecimal

  while (!mqttClient.connected()) {  //Enqunato ão estiver conectado mostra "."
    mqttClient.connect(userID.c_str(), BROKER_USR_NAME, BROKER_URS_PASS); 
    Serial.print(".");
    delay(200);
  }
  Serial.println("Conectado com sucesso ao broker!");
  mqttClient.subscribe(TOPIC11);
  mqttClient.setCallback(callback);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  // Inicializa com os dois LEDs apagados
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_VERMELHO, LOW);
  // O pino 2 estava sendo usado para um LED, vou removê-lo
}

void loop() {
  // Removido código de exemplo e comentários desnecessários
  // O loop principal deve ser o mais limpo possível


  String mensagem = "";
  if (Serial.available() > 0) {
    mensagem = Serial.readStringUntil('\n');
    Serial.print("Mensagem digitada: ");
    Serial.println(mensagem);
    mqttClient.publish("bezinho", mensagem.c_str()); //envia msg
    
  }
  mqttClient.loop(); //mantem a conexão
}

void callback(char* topic, byte* payload, unsigned long length){
  String mensagemRecebida = "";
  for(int i = 0; i < length; i++){
    mensagemRecebida += (char) payload[i];
  }
  mensagemRecebida.trim(); // Limpa espaços em branco e caracteres de nova linha
  Serial.println(mensagemRecebida);
  // Converte a mensagem recebida para um número inteiro (velocidade)
  int velocidade = mensagemRecebida.toInt();

  Serial.print("Velocidade Recebida: ");
  Serial.println(velocidade);

  if (velocidade == 0) {
    // Velocidade = 0: Ambos os LEDs apagados
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_VERMELHO, LOW);
    Serial.println("Trem parado. LEDs apagados.");
  } else if (velocidade > 0) {
    // Velocidade > 0: LED Verde aceso, Vermelho apagado
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_VERMELHO, LOW);
    Serial.println("Trem para frente. LED Verde aceso.");
  } else { // velocidade < 0
    // Velocidade < 0: LED Vermelho aceso, Verde apagado
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_VERMELHO, HIGH);
    Serial.println("Trem para trás. LED Vermelho aceso.");
  }
}