/*
#include "WFAN920.h"

void WFAN920::begin(void)
{
    Serial2.begin(DEF_BAUD, SERIAL_8N1 | SERIAL_RTSCTS);


    return;
}
void WFAN920::reset(void)
{
    pinMode(D21_WSUN_RST, OUTPUT);
    digitalWrite(D21_WSUN_RST, LOW);
    delay(1000);
    digitalWrite(D21_WSUN_RST, HIGH);
    return;
}
*/