// LineSubdivider.h
#ifndef LINE_SUBDIVIDER_H
#define LINE_SUBDIVIDER_H

#include <Arduino.h>
#include "CommandEncoder.h"

void drawLineSegmentMaster(int x1, int y1,
                           int x2, int y2,
                           bool laserOn,
                           int steps,
                           int delayMs);

#endif
