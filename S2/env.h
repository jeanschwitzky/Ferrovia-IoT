#ifndef ENV_H
#define ENV_H

// Variáveis do WiFi
#define SSID "FIESC_IOT_EDU"
#define PASS "8120gv08"
// Variáveis do Broker
#define BROKER_URL "cd85d77f392340d88bf57f7e27f9d9b9.s1.eu.hivemq.cloud"
#define BROKER_PORT 8883
// Variáveis do user do Broker
#define BROKER_USER_NAME "Jeans"
#define BROKER_USER_PASS "Bolinha123"
// Variáveis dos Tópicos

//S2
//Inscreve
// Removido: #define TOPIC3 "S1/Luminosidade"
// Removido: #define TOPIC4 "S1/Presença1"

//Publica
#define TOPIC5 "S2/Presença2"
#define TOPIC6 "S2/Presença4"
// Removido: #define TOPIC7 "S2/Servo3"

#endif