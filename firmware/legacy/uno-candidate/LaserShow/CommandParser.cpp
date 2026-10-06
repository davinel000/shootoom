#include "CommandParser.h"
#include "Laser.h"
#include "Drawing.h"

// Create laser instance (with laser pointer connected to digital pin 5)
Laser laser(5);

// Define protocol markers
const byte START_BYTE = 0xFF;
const byte END_BYTE = 0xFE;

// Global state machine variables (declared as static to limit scope to this file)
static CommandState currentState = WAITING_FOR_START;
static byte currentCommand;
static byte buffer[64];  // Buffer for incoming data
static byte bufferIndex = 0;
static byte totalLength = 0;
static bool debugMode = false;  // Set to true for debugging

// Universal function to process incoming commands from any Stream (Serial or SoftwareSerial)
void processIncomingCommands(Stream& in) {
  while (in.available() > 0) {
    byte incomingByte = in.read();

    switch (currentState) {
      case WAITING_FOR_START:
        // Check for the start byte
        if (incomingByte == START_BYTE) {
          currentState = READING_COMMAND;
          bufferIndex = 0;
          if (debugMode) Serial.println("Start byte received.");
        }
        break;

      case READING_COMMAND:
        // Read the command byte
        currentCommand = incomingByte;
        if (debugMode) {
          Serial.print("Received command: 0x");
          Serial.println(currentCommand, HEX);
        }
        currentState = PROCESSING_COMMAND;
        break;

      case PROCESSING_COMMAND:
        // Read the payload length
        totalLength = incomingByte;
        if (debugMode) {
          Serial.print("Expected payload length: ");
          Serial.println(totalLength);
        }
        if (totalLength > 0 && totalLength <= sizeof(buffer)) {
          bufferIndex = 0;
          // Read the payload bytes (this loop is blocking)
          while (bufferIndex < totalLength) {
            while (!in.available()) {
              delay(1);  // Wait for data
            }
            buffer[bufferIndex++] = in.read();
          }
          currentState = WAITING_FOR_END;
        } else {
          if (debugMode) Serial.println("Invalid payload length.");
          currentState = WAITING_FOR_START;
        }
        break;

      case WAITING_FOR_END:
        // Expect the end byte
        if (incomingByte == END_BYTE) {
          if (debugMode) Serial.println("End byte received.");
          // Execute the parsed command
          executeCommand(currentCommand, buffer, totalLength);
        } else {
          if (debugMode) Serial.println("Error: End byte not received.");
        }
        // Reset state for the next packet
        currentState = WAITING_FOR_START;
        break;
    }
  }
}

// Command execution

void executeCommand(byte command, byte* data, byte length) {
  byte textLength = 0;
  char text[100];  // Increased size to avoid buffer overflow
  byte rotation = 0;
  unsigned int posX = 0, posY = 0;
  float scale = 0;
  byte speed = 0;
  byte count = 0;
  byte scale_byte = 0;
  int offset = -1950;

  switch (command) {
    case 0x02:  // Direct laser control
      if (debugMode) Serial.println("Direct laser control:");
      if (length == 4) {
        unsigned short xData = (data[0] << 8) | data[1];
        unsigned short yData = (data[2] << 8) | data[3];
        laser.setOffset(0, 0);
        Drawing::processSinglePoint(xData, yData);
        if (debugMode) Serial.println("Laser command executed.");
      } else {
        if (debugMode) Serial.println("Error: Incorrect data length for laser command.");
      }
      break;

    case 0x03:  // Display grid
      if (debugMode) Serial.println("Displaying grid:");
      if (length == 2) {
        count = data[0];
        scale_byte = data[1];
        scale = ((scale_byte - 5) / 195.0) * (2.0 - 0.05) + 0.05;
        laser.setSpeed(10);
        if (debugMode) {
          Serial.print("Number of cycles: ");
          Serial.println(count);
          Serial.print("Scale: ");
          Serial.println(scale);
        }
        if (count > 0 && scale >= 0.05 && scale <= 2.0) {
          laser.drawGridPar(5, 5, 30, 1, 1);
          if (debugMode) Serial.println("Grid successfully drawn.");
        } else {
          if (debugMode) Serial.println("Error in command parameters.");
        }
      } else {
        if (debugMode) Serial.println("Error: Incorrect data length for grid command.");
      }
      break;

    case 0x01:  // Draw text
      if (debugMode) Serial.println("Drawing text");

      textLength = data[0];
      if (textLength >= sizeof(text)) textLength = sizeof(text) - 1;  // Prevent buffer overflow

      memcpy(text, &data[1], textLength);
      text[textLength] = '\0';  // Null-terminate string

      count = data[1 + textLength];
      rotation = data[2 + textLength];
      posX = (data[3 + textLength] << 8) | data[4 + textLength];
      posY = (data[5 + textLength] << 8) | data[6 + textLength];
      scale_byte = data[7 + textLength];
      scale = ((scale_byte - 5) / 195.0) * (2.0 - 0.05) + 0.05;
      speed = data[8 + textLength];

      if (debugMode) {
        Serial.print("Repeats: ");
        Serial.println(count);
        Serial.print("Rotation: ");
        Serial.println(rotation);
        Serial.print("Position X: ");
        Serial.println(posX);
        Serial.print("Position Y: ");
        Serial.println(posY);
        Serial.print("Scale: ");
        Serial.println(scale);
        Serial.print("Speed: ");
        Serial.println(speed);
      }
      

      Drawing::drawStringDetail(text, scale, posX + offset, posY + offset, rotation, speed, count);
      mySerial.write(0xAC);  // код подтверждения завершения отрисовки
      if (debugMode) Serial.println("String done ");
      break;

    default:
      if (debugMode) Serial.println("Unknown command");
      break;
  }
}
