#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "env.h"

void corLed(byte red, byte green, byte blue);

<<<<<<< HEAD
#define PIN_LDR 34
#define PIN_DHT 4
#define PIN_TRIGG 22
#define PIN_ECHO 23

=======

#define PIN_LDR 34
#define PIN_DHT 4
#define PIN_TRIGG 22 
#define PIN_ECHO 23 


>>>>>>> 51941a78411ec264b0bc51f330aba4ca8156305a
const byte redPin = 14;
const byte greenPin = 26;
const byte bluePin = 25;

#define DHTTYPE DHT11

DHT dht(PIN_DHT, DHT11);

int distancia = 0;
bool presencaAnterior = false;

<<<<<<< HEAD
=======

>>>>>>> 51941a78411ec264b0bc51f330aba4ca8156305a
WiFiClientSecure espClient;

PubSubClient client(espClient);

<<<<<<< HEAD
=======
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


>>>>>>> 51941a78411ec264b0bc51f330aba4ca8156305a
void setup() {
  Serial.begin(115200);

  espClient.setInsecure();

  // --- Conexão WiFi ---
  Serial.println("Conectando no WiFi...");
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nWiFi conectado!");

  // --- MQTT Broker ---
  client.setServer(BROKER_URL, BROKER_PORT);

  Serial.println("Conectando ao broker...");

  String clientId = "S1-" + String(random(0xffff), HEX);

  // Tenta conectar até conseguir
  while (!client.connected()) {
    client.connect(clientId.c_str(), BROKER_USER_NAME, BROKER_USER_PASS);
    Serial.print(".");
    delay(200);
  }
  Serial.println("\nConectado ao broker!");

  // Inicia sensores
  pinMode(PIN_LDR, INPUT);
  dht.begin();

  pinMode(PIN_TRIGG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

<<<<<<< HEAD
  ledcAttach(redPin, 5000, 8);
  ledcAttach(greenPin, 5000, 8);
  ledcAttach(bluePin, 5000, 8);

  corLed(255, 0, 0); 
=======

  ledcAttach(redPin, 5000, 8);
  ledcAttach(greenPin, 5000, 8);
  ledcAttach(bluePin, 5000, 8);
  corLed(255, 0, 0); 

  setup_wifi();
  espClient.setInsecure(); 

  client.setServer(BROKER_URL, BROKER_PORT);
>>>>>>> 51941a78411ec264b0bc51f330aba4ca8156305a
}

void loop() {

  // Reconexão automática do MQTT caso caia
  if (!client.connected()) {
    String clientId = "S1-" + String(random(0xffff), HEX);
    client.connect(clientId.c_str(), BROKER_USER_NAME, BROKER_USER_PASS);
  }
  client.loop();

<<<<<<< HEAD
  // --- Leituras dos sensores ---
=======
>>>>>>> 51941a78411ec264b0bc51f330aba4ca8156305a
  int luminosidade = analogRead(PIN_LDR);
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

<<<<<<< HEAD
  // --- Envio MQTT ---
=======
>>>>>>> 51941a78411ec264b0bc51f330aba4ca8156305a
  char msgLuz[10];
  sprintf(msgLuz, "%d", luminosidade);
  client.publish(TOPIC3, msgLuz);

  char msgTemp[10];
  dtostrf(temperatura, 4, 1, msgTemp); 
  client.publish(TOPIC1, msgTemp);

  char msgUmi[10];
  dtostrf(umidade, 4, 1, msgUmi);
  client.publish(TOPIC2, msgUmi);

<<<<<<< HEAD
  Serial.println("\n📡 Dados enviados:");
  Serial.print("LDR = "); Serial.println(luminosidade);
  Serial.print("Temp = "); Serial.println(temperatura);
  Serial.print("Umid = "); Serial.println(umidade);

  // --- Ultrassônico (mede distância em cm) ---
=======
  Serial.println("📡 Dados enviados ao broker HiveMQ Cloud:");
  Serial.print("Luminosidade: "); Serial.println(luminosidade);
  Serial.print("Temperatura: "); Serial.println(temperatura);
  Serial.print("Umidade: "); Serial.println(umidade);
  Serial.println("-------------------------------");

>>>>>>> 51941a78411ec264b0bc51f330aba4ca8156305a
  distancia = readUltrassonic(PIN_ECHO, PIN_TRIGG);

  if (distancia > 0) {
    Serial.print("Distância: ");
    Serial.print(distancia);
    Serial.println(" cm");
  }

<<<<<<< HEAD
  // Considera presença até 30 cm do sensor
  bool presencaAtual = (distancia > 0 && distancia < 30);

  if (presencaAtual != presencaAnterior) {
    if (presencaAtual) {
      client.publish(TOPIC4, "1");
      Serial.println(">>> PRESENÇA DETECTADA");
      corLed(255, 255, 0);
    } else {
      client.publish(TOPIC4, "0");
      Serial.println(">>> PRESENÇA ENCERRADA");
      corLed(0, 255, 0); 
=======
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
>>>>>>> 51941a78411ec264b0bc51f330aba4ca8156305a
    }
    presencaAnterior = presencaAtual;
  }

<<<<<<< HEAD
  delay(5000);
}

// --- Cálculo de distância usando sensor ultrassônico ---
=======
  delay(100); 
}


>>>>>>> 51941a78411ec264b0bc51f330aba4ca8156305a
int readUltrassonic(byte echo_pin, byte trigg_pin) {

  digitalWrite(trigg_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigg_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigg_pin, LOW);

  unsigned long tempo = pulseIn(echo_pin, HIGH, 30000);
  if (tempo == 0) return -1;

  // Converte tempo do pulso → distância em cm
  return (tempo * 0.0343) / 2;
}

<<<<<<< HEAD
=======

>>>>>>> 51941a78411ec264b0bc51f330aba4ca8156305a
void corLed(byte red, byte green, byte blue) {
  ledcWrite(redPin, red);
  ledcWrite(greenPin, green);
  ledcWrite(bluePin, blue);
}
