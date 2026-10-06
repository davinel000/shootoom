#ifndef COMMAND_ENCODER_H
#define COMMAND_ENCODER_H


#include <Arduino.h>

// Protocol constants
#define START_BYTE 0xFF
#define CMD_DRAW_TEXT 0x01
#define CMD_LASER_DIRECT 0x02
#define CMD_SYMBOL_END   0x03  // Новый идентификатор команды для окончания символа
#define DATA_LENGTH 0x04
#define END_BYTE   0xFE

// Encodes and sends a laser command packet using xData and yData.
// xData: 16-bit value for X (MSB may indicate laser ON)
// yData: 16-bit value for Y
void sendLaserPoint(unsigned short xData, unsigned short yData);

// Sends a termination command to indicate the end of a symbol.
void sendSymbolEnd();

// sends text command
void sendTextCommand(const char* text, byte repeat, byte rotation, unsigned int posX, unsigned int posY, float scale, byte speed);
byte scaleToByte(float scale);

// waits for handshake
void waitForDrawCompletion(unsigned long timeoutMs);

#endif
