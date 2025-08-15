// ---------------------------------------------------------------------------
// WFAN920_cmd.h
// This file is part of the WFAN920 library for Arduino.
// It provides command handling for the WFAN920 module.
// 
// ---------------------------------------------------------------------------

#ifndef _LIBWFAN920_CMD_H_
#define _LIBWFAN920_CMD_H_

#include <Arduino.h>

class WFAN920_CMD
{
public:

    WFAN920_CMD();
    ~WFAN920_CMD();

    void begin(HardwareSerial* s);
    void SetEchoOff();
    uint16_t GetModuleVersion();
    uint16_t GetParamerter();
    uint16_t SetNetworkName(const char* name);
    uint16_t SetPanId(const char* panid);
    uint16_t SetModeBorderRouter();
    uint16_t SetModeRouter();
    uint16_t SetModeLeaf();
    uint16_t SetMode(const char* mode);
    uint16_t SetIpAddressSpace(const char* ip);
    uint16_t DisplayFanNode();
    uint16_t SendPing(const char* ping);
    uint16_t SendTcp(const char* ip, const char* data);
    uint16_t SaveAndReset();

    uint16_t Response(uint8_t *buf, uint16_t maxlen);
    void Write(const char* cmd);
    uint16_t Read(uint8_t* buf, uint16_t maxlen);

private:
    HardwareSerial* serial;
    const static uint16_t BUFFER_SIZE = 2048;
    uint8_t buf[BUFFER_SIZE];
    uint16_t head = 0;
    uint16_t tail = 0;

    void BufferWrite(uint8_t data);
    uint16_t BufferLength();
    bool BufferRead(uint8_t* data);
    uint16_t ClearSerialBuffer();
    void ReadSerialToBuffer(uint32_t timeout_ms = 1000);
    uint16_t GetFromBuffer(uint8_t* out, uint16_t maxlen);
    uint16_t GetStringFromBuffer(uint8_t* out, uint16_t maxlen);

};
#endif
