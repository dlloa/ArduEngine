#ifndef UTILITIES_H
#define UTILITIES_H

// Draws a square border using alternating white/black pixels along the perimeter.
void drawDottedRect(Arduboy2 arduboy, byte x, byte y, byte size );
// Increments or decrements *val, wrapping within [0, max).
void WrapValue(uint8_t* val, uint8_t max, bool inc);

#endif