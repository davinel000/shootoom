#include "CommandEncoder.h"
#include <ctype.h>

void sendLaserPoint(unsigned short xData, unsigned short yData) {
  // Packet structure: START_BYTE, CMD_LASER_DIRECT, DATA_LENGTH,
  // 2 bytes for xData, 2 bytes for yData, END_BYTE.
  Serial1.write(START_BYTE);
  Serial1.write(CMD_LASER_DIRECT);
  Serial1.write(DATA_LENGTH);
  Serial1.write((xData >> 8) & 0xFF);
  Serial1.write(xData & 0xFF);
  Serial1.write((yData >> 8) & 0xFF);
  Serial1.write(yData & 0xFF);
  Serial1.write(END_BYTE);
}

void sendSymbolEnd() {
  // Special packet with command ID CMD_SYMBOL_END and zero-length payload.
  Serial1.write(START_BYTE);
  Serial1.write(CMD_SYMBOL_END);
  Serial1.write((uint8_t)0x00);  // No data
  Serial1.write(END_BYTE);
}


byte scaleToByte(float scale) {
  // Предположим, диапазон scale: [0.05, 2.0]. Можно использовать формулу:
  return round(((scale - 0.05f) / 1.95f) * 195.0f + 5.0f);
}

// String toUpperCase(const char* input) {
//   String result = "";
//   for (int i = 0; input[i] != '\0'; i++) {
//     result += (char)toupper(input[i]);
//   }
//   return result;
// }

void sendTextCommand(const char* text, byte repeat, byte rotation, unsigned int posX, unsigned int posY, float scale, byte speed) {
  byte textLength = strlen(text);
  // Размер полезной нагрузки: 1 (длина текста) + текст + 1 (repeat) + 1 (rotation) + 2 (posX) + 2 (posY) + 1 (scale_byte) + 1 (speed)
  byte payloadLength = textLength + 9;
  byte scale_byte = scaleToByte(scale);
  
  
  Serial1.write(START_BYTE);      // Отправка стартового байта
  Serial1.write(CMD_DRAW_TEXT);     // Команда "Draw text" (0x01)
  Serial1.write(payloadLength);     // Длина payload
  Serial1.write(textLength);        // Длина текста
  
  // Отправляем текст (без нуль-терминатора)
  for (byte i = 0; i < textLength; i++) {
    Serial1.write(text[i]);
  }
  
  Serial1.write(repeat);            // Количество повторов
  Serial1.write(rotation);          // Угол поворота
  Serial1.write((posX >> 8) & 0xFF);  // Высокий байт posX
  Serial1.write(posX & 0xFF);         // Низкий байт posX
  Serial1.write((posY >> 8) & 0xFF);  // Высокий байт posY
  Serial1.write(posY & 0xFF);         // Низкий байт posY
  Serial1.write(scale_byte);        // Значение scale, преобразованное в байт
  Serial1.write(speed);             // Скорость
  Serial1.write(END_BYTE);          // Концевой байт

  Serial.print("Sent text command: ");
  Serial.println(text);
  
  waitForDrawCompletion(9000);
}

void waitForDrawCompletion(unsigned long timeoutMs = 5000) {
  unsigned long startTime = millis();
  while (millis() - startTime < timeoutMs) {
    if (Serial1.available() > 0) {
      byte b = Serial1.read();
      if (b == 0xAC) {
        Serial.println("command received ");
        return;  // Получили подтверждение
      }
    }
    delay(1);
  }
  Serial.println("Warning: draw completion not confirmed!");
}