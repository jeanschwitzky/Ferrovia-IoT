#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "env.h"

void corLed(byte red, byte green, byte blue);


#define PIN_LDR 34
#define PIN_DHT 4
#define PIN_TRIGG 22 
#define PIN_ECHO 23 


const byte redPin = 14;
const byte greenPin = 26;
const byte bluePin = 25;
#define DHTTYPE DHT11

DHT dht(PIN_DHT, DHT11);
int distancia = 0;
bool presencaAnterior = false;


WiFiClientSecure espClient;
PubSubClient client(espClient);

// --- FUNÇÃO DE CONEXÃO WI-FI ---
void setup_wifi() {
  Serial.print("Conectando-se à rede Wi-Fi: ");
  Serial.println(SSID);
  corLed(0, 0, 255); 
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ Wi-Fi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
  corLed(0, 255, 0); 
}

// --- FUNÇÃO DE CONEXÃO AO BROKER MQTT ---
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT... ");
    if (client.connect("ESP32_S1", BROKER_USER_NAME, BROKER_USER_PASS)) {
      Serial.println("✅ Conectado ao HiveMQ Cloud!");
      corLed(0, 255, 0); 
    } else {
      Serial.print("Falhou (rc=");
      Serial.print(client.state());
      Serial.println("). Tentando novamente em 5s...");
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(PIN_LDR, INPUT);
  dht.begin();
  pinMode(PIN_TRIGG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);


  ledcAttach(redPin, 5000, 8);
  ledcAttach(greenPin, 5000, 8);
  ledcAttach(bluePin, 5000, 8);
  corLed(255, 0, 0); 

  setup_wifi();
  espClient.setInsecure(); 

  client.setServer(BROKER_URL, BROKER_PORT);
}

// --- LOOP ---
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  int luminosidade = analogRead(PIN_LDR);
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  char msgLuz[10];
  sprintf(msgLuz, "%d", luminosidade);
  client.publish(TOPIC3, msgLuz);

  char msgTemp[10];
  dtostrf(temperatura, 4, 1, msgTemp);
  client.publish(TOPIC1, msgTemp);

  char msgUmi[10];
  dtostrf(umidade, 4, 1, msgUmi);
  client.publish(TOPIC2, msgUmi);

  Serial.println("📡 Dados enviados ao broker HiveMQ Cloud:");
  Serial.print("Luminosidade: "); Serial.println(luminosidade);
  Serial.print("Temperatura: "); Serial.println(temperatura);
  Serial.print("Umidade: "); Serial.println(umidade);
  Serial.println("-------------------------------");

  distancia = readUltrassonic(PIN_ECHO, PIN_TRIGG);
  if (distancia > 0) {
    Serial.print("Distância Ultrassônica: ");
    Serial.print(distancia);
    Serial.println(" cm");
  }

  bool presencaAtual = (distancia > 0 && distancia < 30); 


  if (presencaAtual != presencaAnterior) {
    if (presencaAtual) {
      client.publish(TOPIC4, "1"); 
      Serial.println(">>> PRESENÇA DETECTADA (enviado 1 para TOPIC4)");
      corLed(255, 255, 0); 
    } else {
      client.publish(TOPIC4, "0"); 
      Serial.println(">>> PRESENÇA ENCERRADA (enviado 0 para TOPIC4)");
      corLed(0, 255, 0);
    }
    presencaAnterior = presencaAtual;
  }

  delay(100); 
}


int readUltrassonic(byte echo_pin, byte trigg_pin) {
  digitalWrite(trigg_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigg_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigg_pin, LOW);

  unsigned long tempo = pulseIn(echo_pin, HIGH, 30000); // timeout 30ms
  if (tempo == 0) return -1; // sem leitura válida
  return (tempo * 0.0343) / 2;
}


void corLed(byte red, byte green, byte blue) {
  ledcWrite(redPin, red);
  ledcWrite(greenPin, green);
  ledcWrite(bluePin, blue);
}
