#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "env.h"

// Protótipos de função para evitar erros de compilação
void corLed(byte red, byte green, byte blue);

// --- CONFIGURAÇÕES DE PINOS ---
#define PIN_LDR 34 // Alterado para evitar conflito com o sensor ultrassônico
#define PIN_DHT 4
#define PIN_TRIGG 22 // Pino Trigger do Ultrassônico
#define PIN_ECHO 23 // Pino Echo do Ultrassônico

// --- CONFIGURAÇÕES DE LED RGB (Status) ---
const byte redPin = 14;
const byte greenPin = 26;
const byte bluePin = 25;
#define DHTTYPE DHT11

DHT dht(PIN_DHT, DHT11);
int distancia = 0;
bool presencaAnterior = false;

// --- OBJETOS ---
WiFiClientSecure espClient;
PubSubClient client(espClient);

// --- FUNÇÃO DE CONEXÃO WI-FI ---
void setup_wifi() {
  Serial.print("Conectando-se à rede Wi-Fi: ");
  Serial.println(SSID);
  corLed(0, 0, 255); // Azul: Conectando
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ Wi-Fi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
  corLed(0, 255, 0); // Verde: Conectado
}

// --- FUNÇÃO DE CONEXÃO AO BROKER MQTT ---
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT... ");
    if (client.connect("ESP32_S1", BROKER_USER_NAME, BROKER_USER_PASS)) {
      Serial.println("✅ Conectado ao HiveMQ Cloud!");
      corLed(0, 255, 0); // Verde: Conectado ao Broker
    } else {
      Serial.print("Falhou (rc=");
      Serial.print(client.state());
      Serial.println("). Tentando novamente em 5s...");
      delay(5000);
    }
  }
}

// --- SETUP ---
void setup() {
  Serial.begin(115200);
  pinMode(PIN_LDR, INPUT);
  dht.begin();
  pinMode(PIN_TRIGG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  // Configuração do LED RGB (Status)
  ledcAttach(redPin, 5000, 8);
  ledcAttach(greenPin, 5000, 8);
  ledcAttach(bluePin, 5000, 8);
  corLed(255, 0, 0); // Vermelho: Inicializando

  setup_wifi();

  // Conexão segura (TLS)
  espClient.setInsecure(); // ⚠️ Apenas para testes. Depois, use certificados.

  client.setServer(BROKER_URL, BROKER_PORT);
}

// --- LOOP ---
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Lê sensores
  int luminosidade = analogRead(PIN_LDR);
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();

  // Publica luminosidade
  char msgLuz[10];
  sprintf(msgLuz, "%d", luminosidade);
  client.publish(TOPIC3, msgLuz);

  // Publica temperatura
  char msgTemp[10];
  dtostrf(temperatura, 4, 1, msgTemp);
  client.publish(TOPIC1, msgTemp);

  // Publica umidade
  char msgUmi[10];
  dtostrf(umidade, 4, 1, msgUmi);
  client.publish(TOPIC2, msgUmi);

  // Log no Serial Monitor
  Serial.println("📡 Dados enviados ao broker HiveMQ Cloud:");
  Serial.print("Luminosidade: "); Serial.println(luminosidade);
  Serial.print("Temperatura: "); Serial.println(temperatura);
  Serial.print("Umidade: "); Serial.println(umidade);
  Serial.println("-------------------------------");

  // Lógica do Sensor Ultrassônico
  distancia = readUltrassonic(PIN_ECHO, PIN_TRIGG);
  if (distancia > 0) {
    Serial.print("Distância Ultrassônica: ");
    Serial.print(distancia);
    Serial.println(" cm");
  }

  bool presencaAtual = (distancia > 0 && distancia < 30); // Presença se a distância for menor que 30cm

  // Só envia se houve mudança de estado
  if (presencaAtual != presencaAnterior) {
    if (presencaAtual) {
      client.publish(TOPIC4, "1"); // Assumindo TOPIC5 para presença
      Serial.println(">>> PRESENÇA DETECTADA (enviado 1 para TOPIC4)");
      corLed(255, 255, 0); // Amarelo: Presença Detectada
    } else {
      client.publish(TOPIC4, "0"); // Assumindo TOPIC5 para presença
      Serial.println(">>> PRESENÇA ENCERRADA (enviado 0 para TOPIC4)");
      corLed(0, 255, 0); // Verde: Sem Presença
    }
    presencaAnterior = presencaAtual;
  }

  delay(5000); // Envia a cada 5 segundos
}

// --- FUNÇÃO DE LEITURA DO SENSOR ULTRASSÔNICO ---
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

// --- FUNÇÃO DE CONTROLE DO LED RGB ---
void corLed(byte red, byte green, byte blue) {
  ledcWrite(redPin, red);
  ledcWrite(greenPin, green);
  ledcWrite(bluePin, blue);
}
