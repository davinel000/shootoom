#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <Arduino.h>
#include <SoftwareSerial.h>

// Protocol markers as constants
extern const byte START_BYTE;
extern const byte END_BYTE;
extern SoftwareSerial mySerial;


// Enumeration for command parsing state
enum CommandState {
  WAITING_FOR_START,
  READING_COMMAND,
  PROCESSING_COMMAND,
  WAITING_FOR_END
};

// Declaration of the universal command processing function
void processIncomingCommands(Stream &in);

// Declaration of the command execution function
// (You can expand this function to handle multiple command types)
void executeCommand(byte command, byte* data, byte length);

#endif
