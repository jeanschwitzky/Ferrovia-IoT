#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "env.h"

// --- CONFIGURAÇÕES DE PINOS ---
#define PIN_LDR 32
#define PIN_DHT 14
#define DHTTYPE DHT11

DHT dht(PIN_DHT, DHTTYPE);

// --- OBJETOS ---
WiFiClientSecure espClient;
PubSubClient client(espClient);

// --- FUNÇÃO DE CONEXÃO WI-FI ---
void setup_wifi() {
  Serial.print("Conectando-se à rede Wi-Fi: ");
  Serial.println(SSID);

  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ Wi-Fi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

// --- FUNÇÃO DE CONEXÃO AO BROKER MQTT ---
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT... ");
    if (client.connect("ESP32_S1", BROKER_USER_NAME, BROKER_USER_PASS)) {
      Serial.println("✅ Conectado ao HiveMQ Cloud!");
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

  delay(5000); // Envia a cada 5 segundos
}
