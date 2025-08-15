#ifndef _LIBWFAN920_H_
#define _LIBWFAN920_H_

#include <Arduino.h>
#include "WFAN920_cmd.h"

#define PORT_WFAN_PWR_D19   (19) // default power on/off port.
#define PORT_WFAN_PWR_D20   (20) // if chnaged mounting from R6 to R5 jumper register.
#define PORT_WFAN_PWR_ON    (PORT_WFAN_PWR_D19) // default reset port.

#define PORT_WFAN_RESET     (21) // Default: reset port is not use. To using reset, mount a R15.
#define PORT_WFAN_SERIAL_OE (26) // serial enable

#define HW_RESET_TIME_US    (2)
#define STARTUP_TIME_MS     (3000)
#define SERIAL_BAUDRATE     (115200)

class WFAN920
{
private:
    uint8_t ser = 0;
	uint8_t pwr = 0;
    uint8_t power_pin = 0; // power pin number
    HardwareSerial* serial;

public:

    WFAN920_CMD *command;
	
    WFAN920()
    {
        this->serial = nullptr;
        this->ser = 0;
        this->pwr = 0;
        command = new WFAN920_CMD();
    }
    ~WFAN920()
    {
    }

    void begin(HardwareSerial *s, uint8_t pp = PORT_WFAN_PWR_ON)
    {
        this->serial = s;
        this->power_pin = pp;
        //setting power port
        pinMode(power_pin, OUTPUT);
        digitalWrite(power_pin, LOW);
        delayMicroseconds(HW_RESET_TIME_US);
    	//setting serial enable port
        pinMode(PORT_WFAN_SERIAL_OE, OUTPUT);
        digitalWrite(PORT_WFAN_SERIAL_OE, LOW);

        this->powerOn();
        this->serialEnable();

        serial->begin(SERIAL_BAUDRATE, SERIAL_8N1); // Spresense <--> Wi-SUN add-on
        this->command->begin(s);

        delay(STARTUP_TIME_MS); 
    }
    void end()
    {
        this->serialDisable();
        this->powerOff();
        if (command)
        {
            delete command;
            command = nullptr;
        }
        this->serial->end();
    }
    void powerOn()
    {
        digitalWrite(power_pin, HIGH);
        this->pwr = 1;
    }
    void powerOff()
    {
        digitalWrite(power_pin, LOW);
        this->pwr = 0;
    }
    int isPower()
    {
    	return this->pwr;
	}
    
    void serialEnable()
    {
        digitalWrite(PORT_WFAN_SERIAL_OE, HIGH);
        this->ser = 1;
    }
    void serialDisable()
    {
        digitalWrite(PORT_WFAN_SERIAL_OE, LOW);
        this->ser = 0;
    }
    int isSerial()
    {
	    return this->ser;
    }
};

#endif
