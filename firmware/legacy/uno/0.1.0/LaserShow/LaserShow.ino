// See LICENSE file for details
// Copyright 2016 Florian Link (at) gmx.de

#include "Laser.h"
#include "FirmwareVersion.h"
#include "Drawing.h"

#include "Objects.h"

#include "CommandParser.h"

#include "NeopixelEncoderController.h"
#include <SoftwareSerial.h>


// Create a SoftwareSerial object with RX on pin 8 and TX on pin 9
SoftwareSerial mySerial(8, 9);  // RX, TX

NeopixelEncoderController controller;

const int chipSelect = 10;

float scale = 0.99;






bool readyToDraw = false;  // Флаг, что точки накоплены и готовы к отрисовке


void setup() {
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  pinMode(ENCODER_SW, INPUT_PULLUP);


  // Encoder/button are polled by controller.update(); no unused ISR counters.





  laser.init();

  strip.begin();
  strip.show();                  // Initialize all pixels to 'off'
  controller.startupSequence();  // Индикация включения

  // Инициализация Serial для отладки
  Serial.begin(19200);
  Serial.println(F("Sutum Uno " SUTUM_FIRMWARE_VERSION));

  // Initialize the SoftwareSerial port at 57600 baud (or your chosen baud rate)
  mySerial.begin(57600);

  // Setting scale at the initial state
  laser.setScale(scale);
}


// No unused ISR counters or demo caller; one mode handler per loop.
void loop() {
  static int previousMode=0;
  serviceCommandTimeouts();
  controller.update();
  if(controller.mode!=previousMode) {
    resetCommandReceiver();
    while(mySerial.available()) mySerial.read();
    previousMode=controller.mode;
  }
  if(controller.mode!=0) while(mySerial.available()) mySerial.read();
  laser.setScale(scale);
  controller.handleModes();
  // Optional diagnostics only; no automatic per-point USB traffic.
  if(Serial.available() && Serial.read()=='?') printCommandDiagnostics();
}
