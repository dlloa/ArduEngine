#ifndef GAME_SAVES_H
#define GAME_SAVES_H

#include "save/savesystem.h"

// ---------- Version ----------
const uint8_t SAVE_LAYOUT_VERSION = 123;

// ---------- IDs ----------
enum SaveID : uint8_t {
    SAVED_BOARD,
    SAVED_COUNTER,
    // SAVE_SYSTEM,
    // SAVE_GAME,
    // SAVE_SETTINGS,
    // SAVE_STATS,
    SAVE_COUNT
};

// ---------- Save structs ----------
// struct SystemSave {
//     uint8_t audioOn;
// };

// struct GameSave {
//     uint16_t highScore;
//     uint8_t unlocked;
// };

// struct SettingsSave {
//     uint8_t brightness;
//     uint8_t volume;
// };

// struct StatsSave {
//     uint32_t playTime;
// };

struct SavedPicrossBoard{
    uint16_t board[15];
    uint8_t boardsize;
};

struct CounterSave{
    uint32_t count;
};

// ---------- Layout (packed automatically) ----------

// constexpr SaveBlock makeBlock(
//     uint8_t id,
//     uint8_t index,
//     uint16_t size
// ) {
//     uint16_t offset = EEPROM_SAFE_START + sizeof(SaveHeader);
//     for (uint8_t i = 0; i < index; ++i) {
//         offset += SAVE_BLOCK_TABLE[i].size;
//     }
//     return { id, offset, size };
// }

const SaveBlock SAVE_BLOCK_TABLE[SAVE_COUNT] = {

    { SAVED_BOARD, EEPROM_SAFE_START + sizeof(SaveHeader), sizeof(SavedPicrossBoard) },
    { SAVED_COUNTER, EEPROM_SAFE_START + sizeof(SaveHeader) + sizeof(CounterSave), sizeof(CounterSave)}

    // { SAVE_GAME,     EEPROM_SAFE_START + sizeof(SaveHeader) + sizeof(SystemSave), sizeof(GameSave) },
    // { SAVE_SETTINGS, EEPROM_SAFE_START + sizeof(SaveHeader) + sizeof(SystemSave) + sizeof(GameSave), sizeof(SettingsSave) },
    // { SAVE_STATS,    EEPROM_SAFE_START + sizeof(SaveHeader) + sizeof(SystemSave) + sizeof(GameSave) + sizeof(SettingsSave), sizeof(StatsSave) },
};

const uint8_t SAVE_BLOCK_COUNT = SAVE_COUNT;

// ---------- Compile-time safety ----------

constexpr uint16_t TOTAL_SAVE_BYTES = sizeof(SaveHeader) +
    sizeof(SavedPicrossBoard) + sizeof(CounterSave);
    // sizeof(GameSave) +
    // sizeof(SettingsSave) +
    // sizeof(StatsSave);

static_assert(
    EEPROM_SAFE_START + TOTAL_SAVE_BYTES <= EEPROM_TOTAL_SIZE,
    "EEPROM overflow"
);

#endif
