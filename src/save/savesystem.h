#ifndef SAVE_SYSTEM_H
#define SAVE_SYSTEM_H

#include <Arduboy2.h>
// #include <EEPROM.h>

// Arduboy-safe EEPROM start
constexpr uint16_t EEPROM_SAFE_START = 32;
constexpr uint16_t EEPROM_TOTAL_SIZE = 1024;

// ---------- Save Block Descriptor ----------

struct SaveBlock {
    uint8_t id;
    uint16_t offset;
    uint16_t size;
};

// ---------- Provided by the game ----------
extern const uint8_t SAVE_LAYOUT_VERSION;
extern const SaveBlock SAVE_BLOCK_TABLE[];
extern const uint8_t SAVE_BLOCK_COUNT;

// ---------- Header ----------

struct SaveHeader {
    uint8_t layoutVersion;
};

// ---------- Save System ----------

class SaveSystem {
public:
    static void init() {
        SaveHeader hdr;
        EEPROM.get(EEPROM_SAFE_START, hdr);

        if (hdr.layoutVersion != SAVE_LAYOUT_VERSION) {
            clearAll();
            hdr.layoutVersion = SAVE_LAYOUT_VERSION;
            EEPROM.put(EEPROM_SAFE_START, hdr);
        }
    }

    template <typename T>
    static bool load(uint8_t id, T& out) {
        const SaveBlock* b = find(id);
        if (!b || b->size != sizeof(T)) return false;
        EEPROM.get(b->offset, out);
        return true;
    }

    template <typename T>
    static bool save(uint8_t id, const T& in) {
        const SaveBlock* b = find(id);
        if (!b || b->size != sizeof(T)) return false;
        EEPROM.put(b->offset, in);
        return true;
    }

private:
    static const SaveBlock* find(uint8_t id) {
        for (uint8_t i = 0; i < SAVE_BLOCK_COUNT; ++i) {
            if (SAVE_BLOCK_TABLE[i].id == id) {
                return &SAVE_BLOCK_TABLE[i];
            }
        }
        return nullptr;
    }

    static void clearAll() {
        uint16_t end = EEPROM_SAFE_START + sizeof(SaveHeader);
        for (uint8_t i = 0; i < SAVE_BLOCK_COUNT; ++i) {
            end += SAVE_BLOCK_TABLE[i].size;
        }

        for (uint16_t i = EEPROM_SAFE_START; i < end; ++i) {
            EEPROM.update(i, 0);
        }
    }
};

#endif
