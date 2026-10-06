#ifndef SYMBOL_SENDER_H
#define SYMBOL_SENDER_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include "CommandEncoder.h"  // содержит sendLaserPoint() и sendSymbolEnd()
#include "LineSubdivider.h"  // содержит drawLineSegmentMaster()

// Функция для отправки символа без субдивизирования.
template <size_t N>
void sendSymbol(const unsigned short (&symbol)[N], const char* symbolName, int delayMs = 5) {
  Serial.print("Sending symbol ");
  Serial.print(symbolName);
  Serial.print(", count = ");
  Serial.println(N);
  
  for (int i = 0; i < N; i += 2) {
    unsigned short xData = pgm_read_word(&symbol[i]);
    unsigned short yData = pgm_read_word(&symbol[i + 1]);
    sendLaserPoint(xData, yData);
    delay(delayMs);
  }
  // Выключаем лазер в конце.
  sendLaserPoint(0x0000, 0x0000);
  sendSymbolEnd();
}

// Функция для отправки символа с субдивизированием (для контура).
template <size_t N>
void sendSymbolSubdiv(const unsigned short (&symbol)[N], const char* symbolName, int steps, int delayMs) {
  Serial.print("Sending symbol (subdiv): ");
  Serial.print(symbolName);
  Serial.print(", total points = ");
  Serial.println(N / 2);

  bool haveOld = false;
  unsigned short oldX = 0;
  unsigned short oldY = 0;
  
  // Отправляем первую точку с выключенным лазером.
  if (N >= 2) {
    unsigned short rawX = pgm_read_word(&symbol[0]);
    unsigned short rawY = pgm_read_word(&symbol[1]);
    unsigned short startX = rawX & 0x7FFF;
    sendLaserPoint(startX, rawY);
    delay(delayMs);
    oldX = startX;
    oldY = rawY;
    haveOld = true;
  }
  
  for (int i = 2; i < N; i += 2) {
    unsigned short rawX = pgm_read_word(&symbol[i]);
    unsigned short rawY = pgm_read_word(&symbol[i+1]);
    bool laserOn = (rawX & 0x8000) != 0;
    unsigned short xCoord = rawX & 0x7FFF;
    
    drawLineSegmentMaster(oldX, oldY, xCoord, rawY, laserOn, steps, delayMs);
    oldX = xCoord;
    oldY = rawY;
  }
  
  // Отправляем финальную точку с выключенным лазером.
  sendLaserPoint(oldX & 0x7FFF, oldY);
  sendSymbolEnd();
}

// NEW: Функция для отправки символа с заливкой, где можно задать разные параметры для контура и заливки.
template <size_t N, size_t M>
void sendSymbolAndFillEx(const unsigned short (&outline)[N], const unsigned short (&fill)[M],
                         const char* symbolName,
                         int steps_outline, int delay_outline,
                         int steps_fill, int delay_fill) {
  Serial.print("Sending outline for ");
  Serial.println(symbolName);
  sendSymbolSubdiv(outline, symbolName, steps_outline, delay_outline);
  
  Serial.print("Sending fill for ");
  Serial.println(symbolName);
  // Если заливку хотим отрисовывать без субдивизирования, можно просто перебрать точки с указанной задержкой.
  for (int i = 0; i < M; i += 2) {
    unsigned short xData = pgm_read_word(&fill[i]);
    unsigned short yData = pgm_read_word(&fill[i+1]);
    sendLaserPoint(xData, yData);
    delay(delay_fill);
  }
  sendSymbolEnd();
}

// Макросы для удобства.
#define SEND_SYMBOL(symbol) sendSymbol(symbol, #symbol)
#define SEND_SYMBOL_SUBDIV(symbol, steps, delayMs) sendSymbolSubdiv(symbol, #symbol, steps, delayMs)
#define SEND_SYMBOL_AND_FILL_EX(outline, fill, steps_outline, delay_outline, steps_fill, delay_fill) \
    sendSymbolAndFillEx(outline, fill, #outline, steps_outline, delay_outline, steps_fill, delay_fill)

#endif
