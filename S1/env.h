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

//S1
//Inscreve
//Publica
#define TOPIC3 "S1/Luminosidade"
#define TOPIC1 "S1/Temperatura"
#define TOPIC2 "S1/Umidade"
#define TOPIC4 "S1/Presença1"

//S2
//Inscreve
#define TOPIC3 "S1/Luminosidade"
#define TOPIC4 "S1/Presença1"

//Publica
#define TOPIC5 "S2/Presença2"
#define TOPIC6 "S2/Presença4"
#define TOPIC7 "S2/Servo3"

//S3
//Inscreve
//#define TOPIC4 "S1/Presença1"
//#define TOPIC3 "S1/Luminosidade"
//#define TOPIC5 "S2/Presença2"
//#define TOPIC6 "S2/Presença4"
//Publica
//#define TOPIC8 "S3/Presença3"
//#define TOPIC9 "S3/Servo1"
//#define TOPIC10 "S3/Servo2"

//Trem
//Inscreve
//#define TOPIC11 "Trem/Trem"
//Publica


//#define TOPIC1 "S1/Temperatura"
//#define TOPIC2 "S1/Umidade"
//#define TOPIC3 "S1/Luminosidade"
//#define TOPIC4 "S1/Presença1"
//#define TOPIC5 "S2/Presença2"
//#define TOPIC6 "S2/Presença4"
//#define TOPIC7 "S2/Servo3"
//#define TOPIC8 "S3/Presença3"
//#define TOPIC9 "S3/Servo1"
//#define TOPIC10 "S3/Servo2"
//#define TOPIC11 "Trem/Trem"
#endif