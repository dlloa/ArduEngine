#ifndef GAME_SAVES_H
#define GAME_SAVES_H

#include "save/savesystem.h"

// ---------- Version ----------
const uint8_t SAVE_LAYOUT_VERSION = 124;

// ---------- IDs ----------
enum SaveID : uint8_t {
    SAVED_BOARD,
    SAVED_COUNTER,
    SAVE_COUNT
};

// ---------- Save structs ----------

struct SavedPicrossBoard{
    uint16_t board[15];
    uint8_t boardsize;
};

struct CounterSave{
    uint32_t count;
};

// ---------- Layout ----------
// Each block is placed immediately after the previous one:
//   [SafeStart + Header] → SAVED_BOARD → SAVED_COUNTER

const SaveBlock SAVE_BLOCK_TABLE[SAVE_COUNT] = {
    { SAVED_BOARD,    EEPROM_SAFE_START + sizeof(SaveHeader),                             sizeof(SavedPicrossBoard) },
    { SAVED_COUNTER,  EEPROM_SAFE_START + sizeof(SaveHeader) + sizeof(SavedPicrossBoard), sizeof(CounterSave)       },
};

const uint8_t SAVE_BLOCK_COUNT = SAVE_COUNT;

// ---------- Compile-time safety ----------

constexpr uint16_t TOTAL_SAVE_BYTES = sizeof(SaveHeader) +
    sizeof(SavedPicrossBoard) + sizeof(CounterSave);

static_assert(
    EEPROM_SAFE_START + TOTAL_SAVE_BYTES <= EEPROM_TOTAL_SIZE,
    "EEPROM overflow"
);

#endif
