/*
   Smartmeter decoder for MBUS Slave Feather wing and MBUS Slave MKR shield
   
   Version 1.0
   Copyright (C) 2023  Hartmut Wendt  www.zihatec.de

   This application will decode the data by an dlms smartmeter like Kaifa MA309M
   or SAGEMCOM T210-D

   You will need a special key for your meter by your electricity supplier.

   Additional libraries: Arduino Crypto Library by Rhys Weatherley 
                         https://rweather.github.io/arduinolibs/crypto.html


   
   Based on ESP32/8266 Smartmeter decoder for Kaifa MA309M code by FKW9, EIKSEUand others
   https://github.com/FKW9/esp-smartmeter-netznoe/


   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <Arduino.h>
#include "dlms.h"
#include "obis.h"

#include "key.h"
#include <Crypto.h>
#include <AES.h>
#include <GCM.h>


// enable the following line for Adafruit feather boards with TinyUSB support like nRF52840 
//#include <Adafruit_TinyUSB.h> // for Serial



uint32_t last_read = 0;                      // Timestamp when data was last read
uint16_t receive_buffer_index = 0;           // Current position in the receive buffer
uint8_t receive_buffer[RECEIVE_BUFFER_SIZE]; // Stores the received data

#define packet_size 256 // Sagemcom

GCM<AES128> *gcmaes128 = 0;

uint32_t swap_uint32(uint32_t val);
uint16_t swap_uint16(uint16_t val);

void setup()
{   
  // Debug port
  Serial.begin(9600); 

  // init decryption
  gcmaes128 = new GCM<AES128>();

  // MBus input from MBus Wing
  Serial1.begin(2400, SERIAL_8E1);
  Serial1.setTimeout(2);

}


void loop()
{
    uint32_t current_time = millis();

    // Read while data is available
    while (Serial1.available())
    {
        if (receive_buffer_index >= RECEIVE_BUFFER_SIZE)
        {
            Serial.println("Buffer overflow!");
            receive_buffer_index = 0;
        }

        receive_buffer[receive_buffer_index++] = Serial1.read();

        last_read = current_time;
    }

    if (receive_buffer_index > 0 && current_time - last_read > READ_TIMEOUT)
    {
        if (receive_buffer_index < packet_size)
        {
            Serial.println("Received packet with invalid size!");
            Serial.println(receive_buffer_index);
            receive_buffer_index = 0;
            return;
        }

        /**
         * @TODO: ADD ROUTINE TO DETERMINE PAYLOAD LENGTHS AUTOMATICALLY
         */

        uint16_t payload_length = 243;
        uint16_t payload_length_msg1 = 228;
        uint16_t payload_length_msg2 = payload_length - payload_length_msg1;

        uint8_t iv[12]; // Initialization vector

        memcpy(&iv[0], &receive_buffer[DLMS_SYST_OFFSET], DLMS_SYST_LENGTH); // Copy system title to IV
        memcpy(&iv[8], &receive_buffer[DLMS_IC_OFFSET], DLMS_IC_LENGTH);     // Copy invocation counter to IV

        uint8_t ciphertext[payload_length];
        memcpy(&ciphertext[0], &receive_buffer[DLMS_HEADER1_LENGTH], payload_length_msg1);
        memcpy(&ciphertext[payload_length_msg1], &receive_buffer[DLMS_HEADER2_OFFSET + DLMS_HEADER2_LENGTH], payload_length_msg2);

        // Start decrypting
        uint8_t plaintext[payload_length];
        
        gcmaes128->setKey(KEY, gcmaes128->keySize());
        gcmaes128->setIV(iv, 12);
        gcmaes128->decrypt(plaintext, ciphertext, payload_length);
    
        if (plaintext[0] != 0x0F || plaintext[5] != 0x0C)
        {
            Serial.println("Packet was decrypted but data is invalid!");
            receive_buffer_index = 0;
            return;
        }

        // Decode data
        uint16_t current_position = DECODER_START_OFFSET;
        time_t unix_timestamp = -1;

        do
        {
            if (plaintext[current_position + OBIS_TYPE_OFFSET] != DATA_OCTET_STRING)
            {
                Serial.println("Unsupported OBIS header type!");
                receive_buffer_index = 0;
                return;
            }

            uint8_t data_length = plaintext[current_position + OBIS_LENGTH_OFFSET];

            if (data_length != 0x06)
            {
                // read timestamp
                if ((data_length == 0x0C) && (current_position == DECODER_START_OFFSET))
                {
                    uint8_t dateTime[data_length];
                    memcpy(&dateTime[0], &plaintext[current_position + 2], data_length);

                    uint16_t year;
                    uint8_t month;
                    uint8_t day;
                    uint8_t hour;
                    uint8_t minute;
                    uint8_t second;

                    year = (plaintext[current_position + 2] << 8) + plaintext[current_position + 3];
                    month = plaintext[current_position + 4];
                    day = plaintext[current_position + 5];
                    hour = plaintext[current_position + 7];
                    minute = plaintext[current_position + 8];
                    second = plaintext[current_position + 9];

                    char timeStamp[21];
                    sprintf(timeStamp, "%02u.%02u.%04u %02u:%02u:%02u", day, month, year, hour, minute, second);

/*
                    // convert to unix timestamp for graphite
                    struct tm tm;
                    if (strptime(timeStamp, "%d.%m.%Y %H:%M:%S", &tm) != NULL)
                    {
                        unix_timestamp = mktime(&tm) - 7200;
                        Serial.print("Unix Time: ");
                        Serial.println(unix_timestamp);
                    }
                    else
                    {
                        Serial.println("Invalid Timestamp");
                        receive_buffer_index = 0;
                        return;
                    }
*/
                    Serial.print("Timestamp: ");
                    Serial.println(timeStamp);

                    current_position = 34;
                    data_length = plaintext[current_position + OBIS_LENGTH_OFFSET];
                }
                else if ((data_length == 0x0C) && (current_position > 225))
                {
                    uint8_t meterNumber[data_length];
                    memcpy(&meterNumber[0], &plaintext[current_position + 2], data_length);

                    // THIS IS THE END OF THE PACKET
                    break;
                }
                else
                {
                    Serial.println("Unsupported OBIS header length");
                    receive_buffer_index = 0;
                    return;
                }
            }

            uint8_t obis_code[data_length];
            memcpy(&obis_code[0], &plaintext[current_position + OBIS_CODE_OFFSET], data_length); // Copy OBIS code to array

            current_position += data_length + 2; // Advance past code, position and type

            uint8_t obis_data_type = plaintext[current_position];
            current_position++; // Advance past data type

            uint8_t obis_data_length = 0x00;
            uint8_t code_type = TYPE_UNKNOWN;

            if (obis_code[OBIS_A] == 0x01)
            {
                // Compare C and D against code
                if (memcmp(&obis_code[OBIS_C], OBIS_VOLTAGE_L1, 2) == 0)
                {
                    code_type = TYPE_VOLTAGE_L1;
                }
                else if (memcmp(&obis_code[OBIS_C], OBIS_VOLTAGE_L2, 2) == 0)
                {
                    code_type = TYPE_VOLTAGE_L2;
                }
                else if (memcmp(&obis_code[OBIS_C], OBIS_VOLTAGE_L3, 2) == 0)
                {
                    code_type = TYPE_VOLTAGE_L3;
                }

                else if (memcmp(&obis_code[OBIS_C], OBIS_CURRENT_L1, 2) == 0)
                {
                    code_type = TYPE_CURRENT_L1;
                }
                else if (memcmp(&obis_code[OBIS_C], OBIS_CURRENT_L2, 2) == 0)
                {
                    code_type = TYPE_CURRENT_L2;
                }
                else if (memcmp(&obis_code[OBIS_C], OBIS_CURRENT_L3, 2) == 0)
                {
                    code_type = TYPE_CURRENT_L3;
                }

                else if (memcmp(&obis_code[OBIS_C], OBIS_ACTIVE_POWER_PLUS, 2) == 0)
                {
                    code_type = TYPE_ACTIVE_POWER_PLUS;
                }
                else if (memcmp(&obis_code[OBIS_C], OBIS_ACTIVE_POWER_MINUS, 2) == 0)
                {
                    code_type = TYPE_ACTIVE_POWER_MINUS;
                }

                else if (memcmp(&obis_code[OBIS_C], OBIS_ACTIVE_ENERGY_PLUS, 2) == 0)
                {
                    code_type = TYPE_ACTIVE_ENERGY_PLUS;
                }
                else if (memcmp(&obis_code[OBIS_C], OBIS_ACTIVE_ENERGY_MINUS, 2) == 0)
                {
                    code_type = TYPE_ACTIVE_ENERGY_MINUS;
                }

                else if (memcmp(&obis_code[OBIS_C], OBIS_POWER_FACTOR, 2) == 0)
                {
                    code_type = TYPE_POWER_FACTOR;
                }
                else
                {
                    Serial.println("Unsupported OBIS code");
                }
            }
            else
            {
                Serial.println("Unsupported OBIS medium");
                receive_buffer_index = 0;
                return;
            }

            uint16_t uint16_value;
            uint32_t uint32_value;
            float float_value;

            switch (obis_data_type)
            {
            case DATA_LONG_DOUBLE_UNSIGNED:
                obis_data_length = 4;

                memcpy(&uint32_value, &plaintext[current_position], 4); // Copy uint8_ts to integer
                uint32_value = swap_uint32(uint32_value);               // Swap uint8_ts

                float_value = uint32_value; // Ignore decimal digits for now

                switch (code_type)
                {
                case TYPE_ACTIVE_POWER_PLUS:
                    Serial.print("ActivePowerPlus ");
                    Serial.println(float_value);           
                    break;

                case TYPE_ACTIVE_POWER_MINUS:
                    Serial.print("ActivePowerMinus ");
                    Serial.println(float_value);
                    break;

                case TYPE_ACTIVE_ENERGY_PLUS:
                    Serial.print("ActiveEnergyPlus ");
                    Serial.println(float_value);
                    break;

                case TYPE_ACTIVE_ENERGY_MINUS:
                    Serial.print("ActiveEnergyMinus ");
                    Serial.println(float_value);
                    break;
                }
                break;

            case DATA_LONG_UNSIGNED:
                obis_data_length = 2;

                memcpy(&uint16_value, &plaintext[current_position], 2); // Copy uint8_ts to integer
                uint16_value = swap_uint16(uint16_value);               // Swap uint8_ts

                if (plaintext[current_position + 5] == SCALE_TENTHS)
                    float_value = uint16_value / 10.0;
                else if (plaintext[current_position + 5] == SCALE_HUNDREDTHS)
                    float_value = uint16_value / 100.0;
                else if (plaintext[current_position + 5] == SCALE_THOUSANDS)
                    float_value = uint16_value / 1000.0;
                else
                    float_value = uint16_value;

                switch (code_type)
                {
                case TYPE_VOLTAGE_L1:
                    Serial.print("VoltageL1 ");
                    Serial.println(float_value);
                    break;

                case TYPE_VOLTAGE_L2:
                    Serial.print("VoltageL2 ");
                    Serial.println(float_value);
                    break;

                case TYPE_VOLTAGE_L3:
                    Serial.print("VoltageL3 ");
                    Serial.println(float_value);
                    break;

                case TYPE_CURRENT_L1:
                    Serial.print("CurrentL1 ");
                    Serial.println(float_value);
                    break;

                case TYPE_CURRENT_L2:
                    Serial.print("CurrentL2 ");
                    Serial.println(float_value);
                     break;

                case TYPE_CURRENT_L3:
                    Serial.print("CurrentL3 ");
                    Serial.println(float_value);
                    break;

                case TYPE_POWER_FACTOR:
                    Serial.print("PowerFactor ");
                    Serial.println(float_value);
                    break;
                }
                break;

            case DATA_OCTET_STRING:
                obis_data_length = plaintext[current_position];
                current_position++; // Advance past string length
                break;

            default:
                Serial.println("Unsupported OBIS data type");
                receive_buffer_index = 0;
                return;
                break;
            }

            current_position += obis_data_length; // Skip data length

            current_position += 2; // Skip pause after data

            if (plaintext[current_position] == 0x0F)  // There is still additional data for this type, skip it
                current_position += 4;                // Skip additional data and additional break; this will jump out of bounds on last frame
        } while (current_position <= payload_length); // Loop until end of packet

        receive_buffer_index = 0;
        Serial.println("Received valid data!");

    }
}

uint16_t swap_uint16(uint16_t val)
{
    return (val << 8) | (val >> 8);
}

uint32_t swap_uint32(uint32_t val)
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
    return (val << 16) | (val >> 16);
} 
