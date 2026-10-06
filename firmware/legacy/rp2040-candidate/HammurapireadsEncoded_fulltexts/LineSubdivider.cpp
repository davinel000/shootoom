// LineSubdivider.cpp
#include "LineSubdivider.h"
#include "CommandEncoder.h" 

// Subdivide a line from (x1,y1) -> (x2,y2) into `steps` increments.
// laserOn => sets bit 15 in X => "laser on" at the Slave.
// delayMs => how long to wait between sending each sub-point.
void drawLineSegmentMaster(int x1, int y1,
                                  int x2, int y2,
                                  bool laserOn,
                                  int steps,
                                  int delayMs)
{
    for (int i = 0; i <= steps; i++) {
        float t = (float)i / (float)steps;
        int X = x1 + (int)((x2 - x1) * t);
        int Y = y1 + (int)((y2 - y1) * t);

        // Laser ON => set bit 15 in X
        unsigned short xVal = laserOn ? (0x8000 | (X & 0x7FFF))
                                      : (X & 0x7FFF);
        unsigned short yVal = (unsigned short)(Y & 0xFFFF);

        // Send the usual packet: start byte, 0x02 cmd, length=4, ...
        sendLaserPoint(xVal, yVal);

        delay(delayMs); // Pace the sending
    }
}
