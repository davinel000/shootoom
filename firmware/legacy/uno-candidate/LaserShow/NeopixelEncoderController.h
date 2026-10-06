#include <Adafruit_NeoPixel.h>

#include <SoftwareSerial.h>
#include "Drawing.h"
#include <EEPROM.h>


#define NEOPIXEL_PIN 6  // Пин, к которому подключен DIN Neopixel
#define NUMPIXELS 12    // Количество светодиодов в Neopixel модуле

#define ENCODER_CLK 2  // Пин для CLK сигнала энкодера
#define ENCODER_DT 4   // Пин для DT сигнала энкодера
#define ENCODER_SW 3   // Пин для кнопки энкодера


// NeopixelEncoderController.h

#include "Laser.h"
#include "Drawing.h"

extern Laser laser;
extern float scale;
extern SoftwareSerial mySerial;

extern void processSerialCommands();           // Объявляем функцию для обработки команд
extern void drawGrid(int count, float scale);  // Объявляем функцию для отрисовки сетки

// для прерываний
volatile int encoderPos = 0;
volatile bool lastAState = HIGH;  // предполагаем, что по умолчанию подтянут к HIGH
volatile bool buttonFlag = false;
unsigned long lastButtonPressTime = 0;  // для программного дебаунсинга

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

class NeopixelEncoderController {
private:
  int currentState = 0;       // Текущее состояние энкодера (1-84)
  int currentPixel = 0;       // Текущий пиксель (0-11)
  int currentColorIndex = 0;  // Индекс текущего цвета
  int lastClkState;


  const uint32_t colors[7] = {
    strip.Color(255, 0, 0),    // Красный
    strip.Color(255, 127, 0),  // Оранжевый
    strip.Color(255, 255, 0),  // Желтый
    strip.Color(0, 255, 0),    // Зеленый
    strip.Color(0, 0, 255),    // Синий
    strip.Color(75, 0, 130),   // Индиго
    strip.Color(148, 0, 211)   // Фиолетовый
  };

  const uint32_t red = strip.Color(30, 0, 0);      // Тусклый красный
  const uint32_t green = strip.Color(0, 30, 0);    // Тусклый зеленый
  const uint32_t blue = strip.Color(0, 0, 30);     // Тусклый синий
  const uint32_t violet = strip.Color(20, 0, 20);  // Violet

  void showColor(int pixel, int colorIndex) {
    strip.setPixelColor(pixel, colors[colorIndex]);
    strip.show();
  }

  void clearStrip() {
    for (int i = 0; i < NUMPIXELS; i++) {
      strip.setPixelColor(i, 0);  // Очищаем все пиксели
    }
    strip.show();
  }

  void showModeIndicator() {
    clearStrip();
    switch (mode) {
      case 0:
        strip.setPixelColor(0, red);  // Режим приёма данных — красный
        break;
      case 1:
        strip.setPixelColor(0, green);  // Режим отображения сетки — зеленый
        break;
      case 2:
        strip.setPixelColor(0, blue);  // Режим паузы — синий
        break;
      case 3:
        strip.setPixelColor(0, violet);  // Режим паузы — синий
        break;
    }
    strip.show();
  }

public:
  int mode = 0;

  NeopixelEncoderController() {
    pinMode(ENCODER_CLK, INPUT);
    pinMode(ENCODER_DT, INPUT);
    pinMode(ENCODER_SW, INPUT_PULLUP);  // Кнопка энкодера с подтяжкой
    lastClkState = digitalRead(ENCODER_CLK);
  }

  void startupSequence(int times = 5, int r = 255, int g = 0, int b = 3, int millis = 100) {
    for (int i = 0; i < times; i++) {
      for (int j = 0; j < NUMPIXELS; j++) {
        strip.setPixelColor(j, strip.Color(r, g, b));  // Красный
      }
      strip.show();
      delay(millis);
      clearStrip();
      delay(millis);
    }
    showModeIndicator();  // Показать текущий режим после старта
  }


  int direction = 0;

  void update() {
    int clkState = digitalRead(ENCODER_CLK);
    int dtState = digitalRead(ENCODER_DT);
    direction = 0;

    // Если обнаружили переход с LOW на HIGH
    if (lastClkState == LOW && clkState == HIGH) {
      delay(2);  // дебаунс
      // Повторное считывание для надежности:
      clkState = digitalRead(ENCODER_CLK);
      dtState = digitalRead(ENCODER_DT);
      if (lastClkState == LOW && clkState == HIGH) {
        if (dtState != clkState) {
          // Поворот по часовой стрелке
          currentState = (currentState + 1) % 84;
          direction = 1;
        } else {
          // Поворот против часовой стрелки
          currentState = (currentState - 1 + 84) % 84;
          direction = -1;
        }
        //scale = controlValue(scale, direction, 0.1, 0.9, 0.05);
        //Serial.print("scale ");
        //Serial.println(scale);
        //laser.setScale(scale);
        // Обновляем отображение
        currentPixel = currentState % NUMPIXELS;
        currentColorIndex = currentState / NUMPIXELS;

        clearStrip();
        showColor(currentPixel, currentColorIndex % 7);

        //Serial.print("Current state: ");
        //Serial.println(currentState);
        //Serial.print("Direction: ");
        //Serial.println(direction);
      }
    }

    // Обновляем предыдущее состояние clk
    lastClkState = clkState;

    // Обработка кнопки остаётся без изменений...
    if (digitalRead(ENCODER_SW) == LOW) {
      delay(50);
      while (digitalRead(ENCODER_SW) == LOW)
        ;
      mode = (mode + 1) % 4;
      Serial.print("Mode changed to: ");
      Serial.println(mode);
      showModeIndicator();
    }
  }

  // Универсальная функция для регулировки значения:
  // currentValue – текущее значение,
  // encoderDelta – обычно +1 (вперед) или -1 (назад),
  // minVal, maxVal – границы,
  // step – шаг изменения.

  void adjustScale(bool increase) {
    // Предположим, что scale определена глобально или как член класса:
    scale = constrain(scale + (increase ? 0.05f : -0.05f), 0.1f, 1.0f);
    Serial.print("New scale: ");
    Serial.println(scale, 2);
  }

  float controlValue(float currentValue, int encoderDelta, float minVal, float maxVal, float step) {
    float newVal = currentValue + encoderDelta * step;
    if (newVal < minVal) newVal = minVal;
    if (newVal > maxVal) newVal = maxVal;
    Serial.println(newVal);
    return newVal;
  }


  // Обработка режимов работы
  void handleModes() {
    switch (mode) {
      case 0:                               // Режим приема данных
        processIncomingCommands(mySerial);  // Продолжаем обрабатывать команды

        break;
      case 1:  // Режим отображения сетки
        laser.drawGridPar(4, 4, 30, 1, 1);

        break;
      case 2:
        laser.off();




        break;
      case 3:  // Laser off


        Drawing::drawObjectDetail(draw_stone, sizeof(draw_stone) / 4, 0, 0, 1);


        break;
    }
  }
};
