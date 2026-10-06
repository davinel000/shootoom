// See LICENSE file for details
// Copyright 2016 Florian Link (at) gmx.de
#include "Drawing.h"
#include "Font.h"




// void Drawing::drawString(String text, int x, int y, int count) {
//   for (int loop = 0; loop < count; loop++) {
//     int i = 0;
//     int x1 = x;
//     while (text.charAt(i) != '\0') {
//       x1 += drawLetter(text.charAt(i), x1, y);
//       i++;
//     }
//   }
// }

// Функция для поворота текста
void applyRotation(long& x, long& y, int rotation) {
  if (rotation != 0) {
    float rad = radians(rotation);
    long tempX = x;
    x = x * cos(rad) - y * sin(rad);
    y = tempX * sin(rad) + y * cos(rad);
  }
}

void Drawing::drawStringDetail(String text, float scale, int x, int y, int rotation, int speed, int count) {
  float letterSpacing = 5 * scale;  // Масштабируемое расстояние между буквами
  Matrix3 world;
  world = Matrix3::rotateZ(rotation % 360);

  laser.setEnable3D(true);
  laser.setMatrix(world);
  laser.setZDist(1);
  laser.setSpeed(speed);

  // Оставляем оффсет фиксированным
  laser.setOffset(x, y);  // Устанавливаем оффсет для текста

  for (int loop = 0; loop < count; loop++) {
    int i = 0;
    long x1 = 0;  // Начальная позиция в локальной системе координат
    long y1 = 0;

    // Цикл по символам
    while (text.charAt(i) != '\0') {
      x1 += drawLetterDetail(text.charAt(i), x1, y1, scale);  // Рисуем каждую букву с учетом масштаба
      x1 += letterSpacing;                                    // Добавляем расстояние между буквами
      i++;
    }
  }

  laser.setEnable3D(false);
}


long Drawing::stringAdvance(String text) {
  long adv = 0;
  int i = 0;
  while (text.charAt(i) != '\0') {
    adv += advance(text.charAt(i));
    i++;
  }
  return adv;
}

long Drawing::advance(byte letter) {  // IMPORTANT - COMPENSATION OF DISTANCES FOR I AND W
  long adv = 850;
  if (letter == 'I') {
    adv = 850;  //200
  } else if (letter == 'W') {
    adv = 850;  //1000
  }
  return adv;
}




long Drawing::drawLetterDetail(byte letter, long translateX, long translateY, float scale) {
  long adv = advance(letter) * scale;  // Масштабируемое смещение для следующей буквы

  // Отрисовка буквы с учетом масштаба
  switch (letter) {
    case 'A': drawObjectDetail(draw_A, sizeof(draw_A) / 4, translateX, translateY, scale); break;
    case 'B': drawObjectDetail(draw_B, sizeof(draw_B) / 4, translateX, translateY, scale); break;
    case 'C': drawObjectDetail(draw_C, sizeof(draw_C) / 4, translateX, translateY, scale); break;
    case 'D': drawObjectDetail(draw_D, sizeof(draw_D) / 4, translateX, translateY, scale); break;
    case 'E': drawObjectDetail(draw_E, sizeof(draw_E) / 4, translateX, translateY, scale); break;
    case 'F': drawObjectDetail(draw_F, sizeof(draw_F) / 4, translateX, translateY, scale); break;
    case 'G': drawObjectDetail(draw_G, sizeof(draw_G) / 4, translateX, translateY, scale); break;
    case 'H': drawObjectDetail(draw_H, sizeof(draw_H) / 4, translateX, translateY, scale); break;
    case 'I': drawObjectDetail(draw_I, sizeof(draw_I) / 4, translateX, translateY, scale); break;
    case 'J': drawObjectDetail(draw_J, sizeof(draw_J) / 4, translateX, translateY, scale); break;
    case 'K': drawObjectDetail(draw_K, sizeof(draw_K) / 4, translateX, translateY, scale); break;
    case 'L': drawObjectDetail(draw_L, sizeof(draw_L) / 4, translateX, translateY, scale); break;
    case 'M': drawObjectDetail(draw_M, sizeof(draw_M) / 4, translateX, translateY, scale); break;
    case 'N': drawObjectDetail(draw_N, sizeof(draw_N) / 4, translateX, translateY, scale); break;
    case 'O': drawObjectDetail(draw_O, sizeof(draw_O) / 4, translateX, translateY, scale); break;
    case 'P': drawObjectDetail(draw_P, sizeof(draw_P) / 4, translateX, translateY, scale); break;
    case 'Q': drawObjectDetail(draw_Q, sizeof(draw_Q) / 4, translateX, translateY, scale); break;
    case 'R': drawObjectDetail(draw_R, sizeof(draw_R) / 4, translateX, translateY, scale); break;
    case 'S': drawObjectDetail(draw_S, sizeof(draw_S) / 4, translateX, translateY, scale); break;
    case 'T': drawObjectDetail(draw_T, sizeof(draw_T) / 4, translateX, translateY, scale); break;
    case 'U': drawObjectDetail(draw_U, sizeof(draw_U) / 4, translateX, translateY, scale); break;
    case 'V': drawObjectDetail(draw_V, sizeof(draw_V) / 4, translateX, translateY, scale); break;
    case 'W': drawObjectDetail(draw_W, sizeof(draw_W) / 4, translateX, translateY, scale); break;
    case 'X': drawObjectDetail(draw_X, sizeof(draw_X) / 4, translateX, translateY, scale); break;
    case 'Y': drawObjectDetail(draw_Y, sizeof(draw_Y) / 4, translateX, translateY, scale); break;
    case 'Z': drawObjectDetail(draw_Z, sizeof(draw_Z) / 4, translateX, translateY, scale); break;

    case '0': drawObjectDetail(draw_0, sizeof(draw_0) / 4, translateX, translateY, scale); break;
    case '1': drawObjectDetail(draw_1, sizeof(draw_1) / 4, translateX, translateY, scale); break;
    case '2': drawObjectDetail(draw_2, sizeof(draw_2) / 4, translateX, translateY, scale); break;
    case '3': drawObjectDetail(draw_3, sizeof(draw_3) / 4, translateX, translateY, scale); break;
    case '4': drawObjectDetail(draw_4, sizeof(draw_4) / 4, translateX, translateY, scale); break;
    case '5': drawObjectDetail(draw_5, sizeof(draw_5) / 4, translateX, translateY, scale); break;
    case '6': drawObjectDetail(draw_6, sizeof(draw_6) / 4, translateX, translateY, scale); break;
    case '7': drawObjectDetail(draw_7, sizeof(draw_7) / 4, translateX, translateY, scale); break;
    case '8': drawObjectDetail(draw_8, sizeof(draw_8) / 4, translateX, translateY, scale); break;
    case '9': drawObjectDetail(draw_9, sizeof(draw_9) / 4, translateX, translateY, scale); break;
    case '!': drawObjectDetail(draw_exclam, sizeof(draw_exclam) / 4, translateX, translateY, scale); break;
    case '?': drawObjectDetail(draw_question, sizeof(draw_question) / 4, translateX, translateY, scale); break;
    case '.': drawObjectDetail(draw_dot, sizeof(draw_dot) / 4, translateX, translateY, scale); break;
    case ' ':
      adv = 400 * scale;  // Пробел с учетом масштаба
      break;
  }

  return adv;  // Возвращаем значение для смещения к следующей букве
}


void Drawing::drawObjectDetail(const unsigned short* data, int size, long translateX, long translateY, float scale) {
  const unsigned short* d = data;
  unsigned short posX;
  unsigned short posY;
  laser.off();
  while (size > 0) {
    posX = pgm_read_word(d);
    d++;
    posY = pgm_read_word(d);
    d++;
    size--;

    if (posX & 0x8000) {
      laser.on();
    } else {
      laser.off();
    }

    // Применение масштаба к позициям, оффсет уже фиксированный
    long scaledX = (posX & 0x7fff) * scale + translateX;
    long scaledY = posY * scale + translateY;
    
    laser.sendto(scaledX, scaledY, true);  // Отправляем координаты с коррекцией
  }
  laser.off();


}



long SIN(unsigned int angle);
long COS(unsigned int angle);


void Drawing::calcObjectBox(const unsigned short* data, int size, long& centerX, long& centerY, long& width, long& height) {
  const unsigned short* d = data;
  unsigned short posX;
  unsigned short posY;
  unsigned short x0 = 4096;
  unsigned short y0 = 4096;
  unsigned short x1 = 0;
  unsigned short y1 = 0;
  while (size > 0) {
    posX = pgm_read_word(d) & 0x7fff;
    d++;
    posY = pgm_read_word(d);
    d++;
    size--;
    if (posX < x0) x0 = posX;
    if (posY < y0) y0 = posY;
    if (posX > x1) x1 = posX;
    if (posY > y1) y1 = posY;
  }
  centerX = (x0 + x1) / 2;
  centerY = (y0 + y1) / 2;
  width = x1 - x0;
  height = y1 - y0;
}



void Drawing::processSinglePoint(unsigned short xData, unsigned short yData) {
  // Проверка бита для включения/выключения лазера
  if (xData & 0x8000) {
    laser.on();  // Включаем лазер
  } else {
    laser.off();  // Выключаем лазер
  }

  // Отправляем данные на гальванометр
  // Удаляем бит, отвечающий за лазер, с помощью побитовой операции AND
  unsigned short correctedX = xData & 0x7FFF;
  unsigned short correctedY = yData;

  // Передаем координаты в функцию перемещения лазера
  laser.sendto(correctedX, correctedY, 1);
}


