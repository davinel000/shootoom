// Sutum Uno 0.1.0: framing and validation replaced; legacy drawing retained.
#include "CommandParser.h"
#include "PacketDecoder.h"
#include "FirmwareVersion.h"
#include "Laser.h"
#include "Drawing.h"
Laser laser(5);
static Sutum::PacketDecoder decoder;
static bool debugMode = false;
static bool pointOn = false;
static uint32_t lastPointAt = 0;
static uint32_t packets=0, rejected=0, timeouts=0, idleStops=0, overflows=0;
static void blankNow() { laser.offImmediate(); pointOn=false; }
void resetCommandReceiver() { decoder.reset(); blankNow(); }
void serviceCommandTimeouts() {
  const uint32_t now=millis();
  if(decoder.expire(now)) { ++timeouts; blankNow(); }
  if(pointOn && uint32_t(now-lastPointAt)>=Sutum::POINT_IDLE_TIMEOUT_MS) {
    ++idleStops; resetCommandReceiver();
  }
}
void processIncomingCommands(Stream& in) {
  serviceCommandTimeouts();
  if(mySerial.overflow()) {
    ++overflows; resetCommandReceiver();
    while(in.available()) in.read();
    return;
  }
  for(byte budget=0; budget<64 && in.available()>0; ++budget) {
    serviceCommandTimeouts();
    const Sutum::PacketDecoder::Result result=decoder.feed(byte(in.read()),millis());
    if(result==Sutum::PacketDecoder::Invalid) { ++rejected; blankNow(); }
    else if(result==Sutum::PacketDecoder::Complete) {
      ++packets;
      pointOn=false; // synchronous text/grid must not trip the point idle timer
      executeCommand(decoder.command,decoder.payload,decoder.length);
      if(decoder.command==2) {
        pointOn=(Sutum::read16(decoder.payload)&0x8000)!=0;
        lastPointAt=millis(); // start timeout AFTER synchronous movement
      }
    }
  }
}
void printCommandDiagnostics() {
  Serial.print(F("Sutum Uno " SUTUM_FIRMWARE_VERSION " packets=")); Serial.print(packets);
  Serial.print(F(" rejected=")); Serial.print(rejected);
  Serial.print(F(" timeout=")); Serial.print(timeouts);
  Serial.print(F(" idleOff=")); Serial.print(idleStops);
  Serial.print(F(" overflow=")); Serial.println(overflows);
}

// Command execution

void executeCommand(byte command, byte* data, byte length) {
  if (!Sutum::validPayload(command,data,length)) { ++rejected; blankNow(); return; }
  byte textLength = 0;
  char text[56];  // Maximum validated text is 55 bytes plus terminator.
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
        unsigned short xData = Sutum::read16(data);
        unsigned short yData = Sutum::read16(data+2);
        laser.setOffset(0, 0);
        Drawing::processSinglePoint(xData, yData);
        if (debugMode) Serial.println("Laser command executed.");
      } else {
        if (debugMode) Serial.println("Error: Incorrect data length for laser command.");
      }
      break;

    case 0x04:  // Explicit off, zero payload
      blankNow();
      break;
    case 0x03:  // Zero payload is player symbol-end; two bytes are legacy grid
      if (length == 0) { blankNow(); break; }
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
      posX = Sutum::read16(data+3+textLength);
      posY = Sutum::read16(data+5+textLength);
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
