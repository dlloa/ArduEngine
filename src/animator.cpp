#include "animator.h"

Animator::Animator(Arduboy2* inArduboy, FrameCounter* inFrameCounter,
                   const unsigned char** inSprite, uint8_t inSize, uint8_t inFrameRate)
    : Renderable(inArduboy), Updateable(inFrameCounter)
{
    sprite    = inSprite;
    size      = inSize;
    frameRate = inFrameRate;
}

void Animator::setPosition(int16_t x, int16_t y) {
    posX = x;
    posY = y;
}

void Animator::setSprite(const unsigned char** newSprite) {
    sprite     = newSprite;
    clipStart  = 0;
    clipLength = 0;
    clipFrame  = 0;
    frameCounter = 0;
    playing  = false;
    finished = false;
}

void Animator::play(uint8_t start, uint8_t length, AnimMode inMode) {
    clipStart  = start;
    clipLength = length;
    mode       = inMode;
    clipFrame  = 0;
    frameCounter = 0;
    playing  = true;
    finished = false;
}

void Animator::pause() {
    playing = false;
}

void Animator::resume() {
    if (finished) finished = false;
    playing = true;
}

void Animator::stop() {
    playing      = false;
    clipFrame    = 0;
    frameCounter = 0;
    finished     = false;
}

void Animator::update() {
    if (!playing || clipLength == 0) return;

    frameCounter++;
    if (frameCounter < frameRate) return;
    frameCounter = 0;

    clipFrame++;
    if (clipFrame >= clipLength) {
        if (mode == AnimMode::LOOP) {
            clipFrame = 0;
        } else {
            clipFrame = clipLength - 1;
            finished  = true;
            playing   = false;
        }
    }
}

void Animator::render() {
    if (sprite == nullptr || clipLength == 0) return;
    arduboy->drawBitmap(posX, posY, sprite[clipStart + clipFrame], size, size, WHITE);
}
