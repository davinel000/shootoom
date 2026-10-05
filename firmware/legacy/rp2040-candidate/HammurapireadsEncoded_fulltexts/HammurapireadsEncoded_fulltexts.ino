#include <Arduino.h>
#include <avr/pgmspace.h>
#include "Cuneiform.h"       // Contains arrays such as draw_25, draw_21, draw_24, etc.
#include "CommandEncoder.h"  // Contains sendLaserPoint() and sendSymbolEnd()
#include "SymbolSender.h"    // Template function sendSymbol()
#include "LineSubdivider.h"

void setup() {
  Serial.begin(19200);
  Serial1.begin(57600);
  delay(1000);
  Serial.println("Starting symbol transmission...");
  stone(1, 15);
}


int randomX = 0;
int randomY = 0;
int randomC = 1;

const char* count = "symbol";

int pauseTime = 6;
int interp = 5;

// int pauseTime = 5;
// int interp = 4;

int pauseTime1 = 1;
int interp1 = 1;

int interp_outline = 6;
int pause_outline = 5;
int interp_fill = 1;  // например, без субдивизирования заливки (или с минимальным количеством подшагов)
int pause_fill = 1;

void stone(int pause, int cycles) {
  for (int i = 0; i < cycles; i++) {
    sendSymbolSubdiv(draw_stone_0, "stone", 1, pause);
  }
}

void loop() {

  //stone();
  // ---------------------------
  // Law 196
  // ---------------------------
  Serial.println("Law 196, row 1: 26, 22, 25, 24, 23");
  SEND_SYMBOL_SUBDIV(draw_law_196_26, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_22, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_25, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_24, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_23, interp, pauseTime);

  law196();

  delay(3000);

  Serial.println("Law 196, row 2: 18, 15, 20, 16, 17, 19, 21");
  SEND_SYMBOL_SUBDIV(draw_law_196_18, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_15, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_20, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_16, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_17, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_19, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_21, interp, pauseTime);

  law196();

  delay(4000);

  Serial.println("Law 196, row 3: 9, 14, 11, 13, 12, 10");
  SEND_SYMBOL_SUBDIV(draw_law_196_9, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_14, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_11, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_13, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_12, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_10, interp, pauseTime);

  law196();

  delay(4000);

  Serial.println("Law 196, row 4: 6, 7, 8, 5");
  SEND_SYMBOL_SUBDIV(draw_law_196_6, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_7, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_8, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_5, interp, pauseTime);

  law196();

  delay(4000);

  Serial.println("Law 196, row 5: 0, 2, 3, 4, 1");
  SEND_SYMBOL_SUBDIV(draw_law_196_0, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_2, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_3, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_4, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_196_1, interp, pauseTime);




  law196();
  delay(7000);

  // ---------------------------
  // Law 197
  // ---------------------------
  Serial.println("Law 197, row 1: 28, 25, 24, 26, 27");
  SEND_SYMBOL_SUBDIV(draw_law_197_28, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_25, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_24, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_26, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_27, interp, pauseTime);


  law197();

  delay(4000);

  Serial.println("Law 197, row 2: 22, 23, 21");
  SEND_SYMBOL_SUBDIV(draw_law_197_22, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_23, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_21, interp, pauseTime);


  law197();

  delay(4000);

  Serial.println("Law 197, row 3: 20, 17, 18, 19");
  SEND_SYMBOL_SUBDIV(draw_law_197_20, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_17, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_18, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_19, interp, pauseTime);


  law197();

  delay(4000);

  Serial.println("Law 197, row 4: 6, 10, 11, 12, 5, 7, 15, 16, 13, 14, 8, 9");
  SEND_SYMBOL_SUBDIV(draw_law_197_6, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_10, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_11, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_12, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_5, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_7, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_15, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_16, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_13, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_14, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_8, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_9, interp, pauseTime);

  law197();

  delay(4000);

  Serial.println("Law 197, row 5: 3, 0, 1, 2, 4");
  SEND_SYMBOL_SUBDIV(draw_law_197_3, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_0, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_1, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_2, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_197_4, interp, pauseTime);


  sendTextCommand("BROKEN", 80, 0, 1300, 200, 0.3, 180);  // Segment 74

  delay(4000);


  law197();
  delay(7000);

  // ---------------------------
  // Law 200
  // ---------------------------
  Serial.println("Law 200, row 1: 38, 40, 34, 39, 36, 35, 37");
  SEND_SYMBOL_SUBDIV(draw_law_200_38, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_40, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_34, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_39, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_36, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_35, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_37, interp, pauseTime);

  law200();
  delay(4000);


  Serial.println("Law 200, row 2: 22, 30, 32, 29, 27, 24, 26, 31, 33, 28, 25, 23");
  SEND_SYMBOL_SUBDIV(draw_law_200_22, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_30, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_32, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_29, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_27, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_24, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_26, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_31, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_33, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_28, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_25, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_23, interp, pauseTime);

  law200();

  delay(4000);


  Serial.println("Law 200, row 3: 20, 21, 19");
  SEND_SYMBOL_SUBDIV(draw_law_200_20, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_21, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_19, interp, pauseTime);

  law200();

  delay(4000);


  Serial.println("Law 200, row 4: 15, 16, 18, 17, 14");
  SEND_SYMBOL_SUBDIV(draw_law_200_15, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_16, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_18, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_17, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_14, interp, pauseTime);

  law200();

  delay(4000);


  Serial.println("Law 200, row 5: 12, 11, 13");
  SEND_SYMBOL_SUBDIV(draw_law_200_12, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_11, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_13, interp, pauseTime);

  law200();

  delay(4000);


  Serial.println("Law 200, row 6: 8, 6, 10, 9, 5, 7");
  SEND_SYMBOL_SUBDIV(draw_law_200_8, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_6, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_10, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_9, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_5, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_7, interp, pauseTime);

  law200();

  delay(4000);


  Serial.println("Law 200, row 7: 1, 2, 0, 4, 3");
  SEND_SYMBOL_SUBDIV(draw_law_200_1, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_2, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_0, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_4, interp, pauseTime);
  SEND_SYMBOL_SUBDIV(draw_law_200_3, interp, pauseTime);

  law200();

  // Пауза между полными циклами хореографии
  delay(7000);
}


void law196() {
  sendTextCommand("IF  A  MAN", 12, 0, 1300, 3300, 0.2, 30);      // Segment 6
  sendTextCommand("HAS  BLINDED", 12, 0, 900, 3000, 0.25, 30);    // Segment 7
  sendTextCommand("THE  EYE", 12, 0, 400, 1200, 0.12, 50);        // Segment 15
  sendTextCommand("OF  THE  MAN", 12, 0, 400, 900, 0.12, 50);     // Segment 16
  sendTextCommand("OF  HIGH  RANK", 12, 0, 900, 3000, 0.25, 30);  // Segment 22
  sendTextCommand("HIS  EYE", 20, 0, 3250, 1200, 0.13, 50);       // Segment 27
  sendTextCommand("WILL  BE", 20, 0, 3250, 900, 0.13, 50);        // Segment 28
  sendTextCommand("BLINDED", 30, 0, 1300, 200, 0.3, 150);         // Segment 34
}

void law197() {
  // Full text 197 again
  sendTextCommand("IF  HE", 16, 0, 1300, 3300, 0.2, 30);       // Segment 51
  sendTextCommand("HAS  BROKEN", 16, 0, 900, 3000, 0.25, 30);  // Segment 52
  sendTextCommand("A  BONE", 16, 0, 400, 1200, 0.12, 25);      // Segment 62
  sendTextCommand("OF  A  MAN", 16, 0, 400, 900, 0.12, 25);    // Segment 63
  sendTextCommand("HIS  BONE", 12, 0, 900, 3000, 0.25, 10);    // Segment 66
  sendTextCommand("WILL  BE", 12, 0, 3250, 1200, 0.13, 10);    // Segment 73
  sendTextCommand("BROKEN", 30, 0, 1300, 200, 0.3, 150);       // Segment 74
}

void law200() {
  sendTextCommand("IF  A  MAN", 20, 0, 1300, 3300, 0.2, 20);         // Segment 94
  sendTextCommand("HAS  KNOCKED  OUT", 20, 0, 500, 3000, 0.25, 30);  // Segment 95
  sendTextCommand("THE  TOOTH", 16, 0, 250, 1200, 0.12, 20);         // Segment 103
  sendTextCommand("OF  A  MAN", 12, 0, 400, 900, 0.12, 20);           // Segment 109
  sendTextCommand("OF  HIS  OWN  RANK", 30, 0, 800, 3000, 0.2, 150);  // Segment 113
  sendTextCommand("HIS TOOTH", 18, 0, 3180, 1200, 0.1, 20);           // Segment 120
  sendTextCommand("WILL  BE", 18, 0, 3250, 900, 0.12, 20);            // Segment 124
  sendTextCommand("KNOCKED  OUT", 40, 0, 500, 200, 0.3, 180);         // Segment 128
}