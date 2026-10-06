// See LICENSE file for details
// Copyright 2016 Florian Link (at) gmx.de

#include "Laser.h"
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


  // Подключаем внешнее прерывание к пину ENCODER_CLK (который теперь 2)
  attachInterrupt(digitalPinToInterrupt(ENCODER_CLK), encoderISR, CHANGE);

  attachInterrupt(digitalPinToInterrupt(ENCODER_SW), buttonISR, FALLING);


  laser.init();

  strip.begin();
  strip.show();                  // Initialize all pixels to 'off'
  controller.startupSequence();  // Индикация включения

  // Инициализация Serial для отладки
  Serial.begin(19200);

  // Initialize the SoftwareSerial port at 57600 baud (or your chosen baud rate)
  mySerial.begin(57600);

  // Setting scale at the initial state
  laser.setScale(scale);
}


void encoderISR() {
  bool aState = digitalRead(ENCODER_CLK);
  // Если состояние изменилось:
  if (aState != lastAState) {
    // Сравниваем с состоянием второго канала
    if (digitalRead(ENCODER_DT) != aState) {
      encoderPos++;  // вращение в одну сторону
    } else {
      encoderPos--;  // вращение в другую сторону
    }
    lastAState = aState;
  }
}

void buttonISR() {
  // Проверяем, прошло ли достаточно времени для дебаунса (например, 200 мс)
  if (millis() - lastButtonPressTime > 200) {
    buttonFlag = true;
    lastButtonPressTime = millis();
  }
}



void runArduino() {
  String str = "ARDUINO";
  int w = Drawing::stringAdvance(str);
  laser.setScale(0.5);
  laser.setOffset(1024, 1024);
  int count = 360 / 4;
  int angle = 45;
  for (int i = 0; i < count; i++) {
    Matrix3 world;
    world = Matrix3::rotateX(angle % 360);
    laser.setEnable3D(true);
    laser.setMatrix(world);
    laser.setZDist(2000);
    Drawing::drawString(str, -w / 2, -500, 1);
    angle += 8;
  }
  laser.setEnable3D(false);
}





//// LOOP

void loop() {
    laser.setScale(scale);


  // Обработка остальных режимов, например:
  controller.handleModes();


  // laser.setSpeed(200);
  // runArduino();

  // laser.setOffset(0, 0);

  // Drawing::drawObject(draw_stone, sizeof(draw_stone) / 4, 0, 0);
  controller.handleModes();
  controller.update();  // Обновление состояния энкодера и управления Neopixel
}