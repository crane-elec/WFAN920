// ---------------------------------------------------------------------------
// WFAN920_cmd.cpp
// This file is part of the WFAN920 library for Arduino.
// It provides command handling for the WFAN920 module.
// 
// このファイルは、ライブラリ提供時に .a ファイルとして隠蔽される
// 
// ---------------------------------------------------------------------------

#include "WFAN920_cmd.h"
#include <Arduino.h>

// ---------------------------------------------------------------------------
// defines

#define COMMAND_SUCCESS (0x00)
#define COMMAND_ERROR   (0x01)

#define SET_ECHO_OFF ("echo 0")
#define GET_MODULE_VERSION ("vernum")
#define GET_PARAMETER ("param")
#define SET_NETWORK_NAME ("netname ")
#define SET_PAN_ID ("pan ")

#define SET_MODE ("atstart ")
#define SET_MODE_BORDER_ROUTER ("1")
#define SET_MODE_ROUTER ("2")
#define SET_MODE_LEAF ("3")

#define SWITCH_ANTENNA ("rantsw ")
#define ANTENNA_INTERNAL (1)
#define ANTENNA_EXTERNAL (2)

#define SET_IP_ADDRESS_SPACE ("ip ")
#define DISPLAY_FAN_NODE ("fnode ")

#define PING ("ping start ")
#define PING_OPTION (" 16 1 3 0 1")  // Fixed: 16byte, 1sec, 3frame, ECHO reply, flags1

#define SEND_TCP ("tcps ")
#define SAVE_AND_RESET ("svrst")
// ---------------------------------------------------------------------------


WFAN920_CMD::WFAN920_CMD()
{
}
WFAN920_CMD::~WFAN920_CMD()
{
}

void WFAN920_CMD::begin(HardwareSerial* s)
{
    this->serial = s;
    memset(buf, 0, BUFFER_SIZE);
}

void WFAN920_CMD::SetEchoOff()
{
    this->serial->println(SET_ECHO_OFF);
    this->ClearSerialBuffer();
}

uint16_t WFAN920_CMD::GetModuleVersion()
{
    this->serial->println(GET_MODULE_VERSION);
    this->ReadSerialToBuffer();
    return COMMAND_SUCCESS;
}

uint16_t WFAN920_CMD::GetParamerter()
{
    this->serial->println(GET_PARAMETER);
    this->ReadSerialToBuffer();
    return COMMAND_SUCCESS;
}

uint16_t WFAN920_CMD::SetNetworkName(const char* name)
{
    if (name == nullptr || strlen(name) == 0 || 32 < strlen(name) )
    {
        return COMMAND_ERROR; // Invalid name length
    } 

    this->serial->print(SET_NETWORK_NAME);
    this->serial->print("\"");
    this->serial->print(name);
    this->serial->println("\"");
    this->ReadSerialToBuffer();
    return COMMAND_SUCCESS;
}
uint16_t WFAN920_CMD::SetPanId(const char* panid)
{
    if (panid == nullptr || strlen(panid) != 4)
    {
        return COMMAND_ERROR; // Invalid PAN-ID length
    }

    this->serial->print(SET_PAN_ID);
    this->serial->println(panid);
    this->ReadSerialToBuffer();
    return COMMAND_SUCCESS;
}
uint16_t WFAN920_CMD::SetModeBorderRouter()
{
    return this->SetMode(SET_MODE_BORDER_ROUTER);
}
uint16_t WFAN920_CMD::SetModeRouter()
{
    return this->SetMode(SET_MODE_ROUTER);
}
uint16_t WFAN920_CMD::SetModeLeaf()
{
    return this->SetMode(SET_MODE_LEAF);
}
uint16_t WFAN920_CMD::SetMode(const char* mode)
{
    this->serial->print(SET_MODE);
    this->serial->println(mode);
    this->ReadSerialToBuffer();
    return COMMAND_SUCCESS;
}
uint16_t WFAN920_CMD::SwitchAntenna(uint8_t ant)
{
    if (ant != ANTENNA_INTERNAL && ant != ANTENNA_EXTERNAL)
    {
        return COMMAND_ERROR; // Invalid antenna option
    }
    this->serial->print(SWITCH_ANTENNA);
    this->serial->println(ant);
    this->ReadSerialToBuffer();
    return COMMAND_SUCCESS;
}
uint16_t WFAN920_CMD::SetIpAddressSpace(const char* ip)
{
    if (ip == nullptr || strlen(ip) == 0)
    {
        return COMMAND_ERROR; // Invalid IP address space
    }
    this->serial->print(SET_IP_ADDRESS_SPACE);
    this->serial->println(ip);
    this->ReadSerialToBuffer();
    return COMMAND_SUCCESS;
}
uint16_t WFAN920_CMD::DisplayFanNode()
{
    this->serial->println(DISPLAY_FAN_NODE);
    this->ReadSerialToBuffer();
    return COMMAND_SUCCESS;
}
uint16_t WFAN920_CMD::SendPing(const char* ping)
{
    if (ping == nullptr || strlen(ping) == 0)
    {
        return COMMAND_ERROR; // Invalid ping command
    }
    this->serial->print(PING);
    this->serial->print(ping);
    this->serial->println(PING_OPTION);
    delay(1500);// wait for response
    this->ReadSerialToBuffer(2000);
    return COMMAND_SUCCESS;
}
uint16_t WFAN920_CMD::SendTcp(const char* ip, const char* data)
{
    return this->SendTcp(ip, data, 1000);
}
uint16_t WFAN920_CMD::SendTcp(const char* ip, const char* data, uint32_t timeout_ms = 1000)
{
    if (data == nullptr || strlen(data) == 0)
    {
        return COMMAND_ERROR; // Invalid TCP data
    }
    this->serial->print(SEND_TCP);
    this->serial->print(ip);
    this->serial->print(" ");
    this->serial->println(data);
    this->ReadSerialToBuffer(timeout_ms);
    return COMMAND_SUCCESS;
}
uint16_t WFAN920_CMD::SaveAndReset()
{
    this->serial->println(SAVE_AND_RESET);
    delay(5000);
    this->SetEchoOff();
    return COMMAND_SUCCESS;
}

uint16_t WFAN920_CMD::Response(uint8_t *buf, uint16_t maxlen)
{
    return this->GetFromBuffer(buf, maxlen); // bufに入れたデータ長を返す
}

void WFAN920_CMD::Write(const char* cmd)
{
    this->serial->println(cmd);
}

uint16_t WFAN920_CMD::Read(uint8_t* buf, uint16_t maxlen)
{
    this->ReadSerialToBuffer(10);
    return this->GetStringFromBuffer(buf, maxlen);
}


// データをバッファに追加
void WFAN920_CMD::BufferWrite(uint8_t data)
{
    uint16_t next = (head + 1) % BUFFER_SIZE;
    if (next != tail) { // バッファが満杯でなければ
        buf[head] = data;
        head = next;
    }
    // 満杯時は上書きしない
}
// バッファの長さを取得
uint16_t WFAN920_CMD::BufferLength()
{
    if (head >= tail)
        return head - tail;
    else
        return BUFFER_SIZE - tail + head;
}
// バッファからデータを取得
bool WFAN920_CMD::BufferRead(uint8_t* data)
{
    if (head == tail) return false; // 空
    *data = buf[tail];
    tail = (tail + 1) % BUFFER_SIZE;
    return true;
}
// シリアルバッファをクリア
uint16_t WFAN920_CMD::ClearSerialBuffer()
{
    uint16_t l = 0;
    while(0 < this->serial->available())
    {
        serial->read();
        l++;
    }
    return l;
}
// シリアルからバッファに読み込む
void WFAN920_CMD::ReadSerialToBuffer(uint32_t timeout_ms = 1000)
{
    uint32_t start = millis();
    while (millis() - start < timeout_ms)
    {
        while (serial->available())
        {
            char c = serial->read();
            BufferWrite(c);
        }
    }
}
// バッファから文字列として取り出す
uint16_t WFAN920_CMD::GetFromBuffer(uint8_t* out, uint16_t maxlen)
{
    uint16_t l = 0;
    uint8_t d;
    while (l < maxlen - 1 && BufferRead(&d))
    {
        out[l++] = d;
    }
    out[l] = '\0';
    return l;
}
// バッファから1行分文字列として取り出す
uint16_t WFAN920_CMD::GetStringFromBuffer(uint8_t* out, uint16_t maxlen)
{
    uint16_t l = 0;
    uint8_t d;
    while (l < maxlen - 1 && BufferRead(&d))
    {
        out[l++] = d;
        if (d == '\n') // 改行で終了
        {
            break;
        }
    }
    out[l] = '\0';
    return l;
}

// ---------------------------------------------------------------------------