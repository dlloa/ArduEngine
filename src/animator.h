#ifndef ANIMATOR_H
#define ANIMATOR_H

#include "base.h"
#include "framecounter.h"

// LOOP  — wraps back to the first frame of the clip when the last is reached.
// ONCE  — holds on the last frame and sets finished=true; call resume() to restart.
enum class AnimMode : uint8_t { LOOP, ONCE };

class Animator : public Renderable, public Updateable {
public:
    const unsigned char** sprite = nullptr; // pointer to frame array (RAM pointers → PROGMEM bitmaps)
    uint8_t size      = 0;  // sprite width and height in pixels (square)

    // Active clip
    uint8_t clipStart  = 0;  // index into sprite[] where the clip begins
    uint8_t clipLength = 0;  // number of frames in the clip
    uint8_t clipFrame  = 0;  // current position within the clip (0 = clipStart)

    uint8_t frameCounter = 0; // game-frames elapsed since last sprite-frame advance
    uint8_t frameRate    = 4; // game-frames per sprite-frame advance

    int16_t posX = 0;
    int16_t posY = 0;

    AnimMode mode    = AnimMode::LOOP;
    bool playing     = false;
    bool finished    = false; // true once a ONCE clip plays all the way through

    Animator(Arduboy2* ab, FrameCounter* fc,
             const unsigned char** inSprite, uint8_t inSize, uint8_t inFrameRate);

    void setPosition(int16_t x, int16_t y);

    // Swap the sprite array (resets clip state).
    void setSprite(const unsigned char** newSprite);

    // Begin playing a clip defined by a start index and frame count within sprite[].
    // Resets to the first frame of the clip immediately.
    void play(uint8_t start, uint8_t length, AnimMode inMode = AnimMode::LOOP);

    void pause();    // freeze on the current frame
    void resume();   // continue from the current frame (re-arms a finished ONCE clip)
    void stop();     // return to clip frame 0 and stop; clears finished

    bool isPlaying()  const { return playing; }
    bool isFinished() const { return finished; }

    void update() override;
    void render() override;
};

#endif
