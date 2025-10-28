#ifndef ENV_H
#define ENV_H

//Variáveis do WIfi
#define SSID "FIESC_IOT_EDU"
#define PASS "8120gv08"
//Variáveis do Broker
#define Broker_URL "cd85d77f392340d88bf57f7e27f9d9b9.s1.eu.hivemq.cloud"
#define Broker_PORT 8883

//Variáveis do user do Broker 
#define BROKER_USR_NAME "Carlos"
#define BROKER_USR_PASS "Bolinha123"

//Variáveis dos Tópicos 
//S3
//Inscreve
#define TOPIC4 "S1/Presença1"
#define TOPIC3 "S1/Luminosidade"
#define TOPIC5 "S2/Presença2"
#define TOPIC6 "S2/Presença4"
//Publica
#define TOPIC8 "S3/Presença3"
#define TOPIC9 "S3/Servo1"
#define TOPIC10 "S3/Servo2"



#endif