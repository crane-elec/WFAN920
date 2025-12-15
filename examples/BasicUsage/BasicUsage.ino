// ---------------------------------------------------------------------------
// Basic usage example for the WFAN920 library
// This example demonstrates how to use the WFAN920 library to communicate
// with the WFAN920 module.

#include <WFAN920.h>
#include "jpeg50.h" // 50KB JPEG image data

WFAN920 fan;

void setup()
{
    pinMode(LED0, OUTPUT);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    Serial.begin(115200, SERIAL_8N1); // PC <--> Spresene

    fan.begin(&Serial2); // Use Serial2 to communicate with the WFAN920 module.
    fan.command->SetEchoOff();

    Serial.println("WFAN920 BasicUsage.");
    
}

void loop()
{
    const uint16_t maxlen = 8;
    uint8_t res[maxlen];
    uint16_t len = 0;

    digitalWrite(LED0, LOW);
    digitalWrite(LED1, LOW);

    // Menu for user input
    Serial.println("\033[32m");
    Serial.println("\nPress a key to select command: ");
    Serial.println("  1: get module version.");
    Serial.println("  2: get parameter.");
    Serial.println("  3: set network name.(\"WFAN920\")");
    Serial.println("  4: set PAN-ID.(\"12ef\") only Border Router mode.");
    Serial.println("  5: set Border Router mode.");
    Serial.println("  6: set Router mode.");
    Serial.println("  7: set Leaf mode.");
    Serial.println("  a: switch to internal ANT1.");
    Serial.println("  b: switch to external ANT2.");
    Serial.println("  i: set IP address space. (\"2001:db8::1/48\")");
    Serial.println("  n: display FAN node.");
    Serial.println("  p: send ping. (\"2001:db8::1\")");
    Serial.println("  q: send ping. (\"2001:db8::2\")");
    Serial.println("  s: save and reset. must be executed after set mode.");
    Serial.println("  t: send a packet over TCP.(\"2001:db8::1\"), \"00112233445566778899AABBCCDDEEFF\")");
    Serial.println("  u: send a packet over TCP.(\"2001:db8::2\"), \"00112233445566778899AABBCCDDEEFF\")");
    Serial.println("  x: send large data test over TCP.(\"2001:db8::2\"), 50KB JPEG image data");

    Serial.println("  0: exit.");
    Serial.println("\033[0m");

    // Wait for user input
    while (Serial.available() == 0) {
        // Output interrupt message
        uint16_t len;
        while (0 < (len = fan.command->Read(res, maxlen))) {
            Serial.write(res, len);
        }
        delay(1); // for CPU load reduction
    }

    // Read user input
    digitalWrite(LED1, HIGH);
    char c = Serial.read();
    switch (c)
    {
        case '1':
            // Get module version
            fan.command->GetModuleVersion();
            break;
        case '2':
            // Get parameter
            len = fan.command->GetParamerter();
            break;
        case '3':
            // Set network name
            len = fan.command->SetNetworkName("WFAN920");
            break;
        case '4':
            // Set PAN-ID
            len = fan.command->SetPanId("12ef");
            break;
        case '5':
            // Set Border Router mode
            len = fan.command->SetModeBorderRouter();
            break;
        case '6':
            // Set Router mode
            len = fan.command->SetModeRouter();
            break;
        case '7':
            // Set Leaf mode
            len = fan.command->SetModeLeaf();
            break;
        case 'a':
            // Switch to internal antenna ANT1
            len = fan.command->SwitchAntenna(1);
            break;
        case 'b':
            // Switch to external antenna ANT2
            len = fan.command->SwitchAntenna(2);
            break;
        case 'i':
            // Set IP address space
            len = fan.command->SetIpAddressSpace("2001:db8::1/48");
            break;
        case 'n':
            // Display FAN node
            len = fan.command->DisplayFanNode();
            break;
        case 'p':
            // Send ping
            len = fan.command->SendPing("2001:db8::1");
            break;
        case 'q':
            // Send ping
            len = fan.command->SendPing("2001:db8::2");
            break;
        case 's':
            // Save and reset
            Serial.println("Configuration saved and module will reset. Wait for 8 seconds to reboot.");
            len = fan.command->SaveAndReset();
            break;
        case 't':
            // Send a message over TCP
            len = fan.command->SendTcp("2001:db8::1", "00112233445566778899AABBCCDDEEFF");
            break;
        case 'u':
            // Send a message over TCP
            len = fan.command->SendTcp("2001:db8::2", "00112233445566778899AABBCCDDEEFF");
            break;
        case 'x':
            // Send large data test over TCP
            {
                const char* ip = "2001:db8::2";
                const uint8_t* img = jpeg50;
                const uint32_t img_size = sizeof(jpeg50);
                const uint8_t header_size = 4; // "JPG" + chunk number(00~FF)
                const uint16_t chunk_size = 256; // Send in 256-byte chunks
                const uint16_t total_blocks = (img_size + chunk_size - 1) / chunk_size;
                uint32_t block_num = 0;

                while (block_num < total_blocks)
                {
                    char data[(header_size + chunk_size) * 2 + 1]; // Each byte as two hex chars + null terminator
                    memset(data, '0', sizeof(data)); // padding '00'
                    sprintf(&data[0], "%02X%02X%02X%02X", 'J', 'P', 'G', block_num % 256); // Header with chunk number
                    uint16_t data_len = 0;
                    for (uint16_t i = 0; i < chunk_size && (block_num * chunk_size + i) < img_size; i++)
                    {
                        sprintf(&data[header_size * 2 + data_len], "%02X", *(img + block_num * chunk_size + i));
                        data_len += 2;
                    }
                    data[header_size * 2 + data_len] = '\0';

                    len = fan.command->SendTcp(ip, data, 400);
                    Serial.printf("(%03d/%03d) %s\n", block_num + 1, total_blocks, data);
                    // Serial.printf("(%03d/%03d)\n", block_num, total_blocks);
                    block_num++;
                }
            }
            break;

        default:
            // Not implemented or unsupported command
            Serial.println("Command not implemented or unsupported.");
            break;
    }

    // If a command was executed, read the response
    while(0 < (len = fan.command->Response(res, maxlen)))
    {
        Serial.write(res, len);
    }

    /* FOR DEBUG
    fan.command->Write("echo 1");
    delay(1000);

    fan.command->Write("echo 0");
    delay(1000);
    */

    // Turn off LED after operation
    delay(500);
    Serial.println("");
    digitalWrite(LED1, LOW);

}
