#ifndef UTILITIES
#define UTILITIES
#include <Arduboy2.h>
void drawDottedRect(Arduboy2 arduboy, byte x, byte y, byte size ){
  for( byte i = 0; i < size; i++ ){
    byte pixelColor = WHITE;
    if( i % 2 ){
      pixelColor = BLACK;
    }

    arduboy.drawPixel( x + i, y, pixelColor);
    arduboy.drawPixel( x + i, y + size, pixelColor);
    arduboy.drawPixel( x , y + i, pixelColor);
    arduboy.drawPixel( x + size, y + i, pixelColor);
  }
}


void WrapValue(uint8_t* val, uint8_t max, bool inc) {
    if (inc) {
        *val = (*val + 1) % max;
    } else {
        *val = (*val == 0) ? (max - 1) : (*val - 1);
    }
}
#endif