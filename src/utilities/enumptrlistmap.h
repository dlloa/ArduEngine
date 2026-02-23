#ifndef ENUM_PTR_LIST_MAP_H
#define ENUM_PTR_LIST_MAP_H

#include <stdint.h>
#include "enumptrmap.h"

// ---------- Enum → List of Pointers ----------

template <typename Enum, typename PtrType, uint8_t MAX_PER_ENUM>
class EnumPtrListMap {
public:
    static constexpr uint8_t ENUM_COUNT = enumCount<Enum>();

    struct Bucket {
        PtrType* items[MAX_PER_ENUM] = {};
        uint8_t count = 0;
    };

    Bucket table[ENUM_COUNT];

    bool add(Enum key, PtrType* value) {
        Bucket& b = table[enumIndex(key)];
        if (b.count >= MAX_PER_ENUM) {
            return false; // overflow
        }
        b.items[b.count] = value;
        b.count++;
        return true;
    }

    uint8_t size(Enum key) const {
        return table[enumIndex(key)].count;
    }

    bool has(Enum key, uint8_t i){
        return table[enumIndex(key)].items[i] != nullptr;
    }

    PtrType* get(Enum key, uint8_t i) const {
        return table[enumIndex(key)].items[i];
    }

    bool empty(Enum key) const {
        return table[enumIndex(key)].count == 0;
    }

    void clear(Enum key) {
        table[enumIndex(key)].count = 0;
    }

    void clearAll() {
        for (uint8_t i = 0; i < ENUM_COUNT; ++i) {
            table[i].count = 0;
        }
    }
};

// ---------- Compile-time safety ----------

template <typename Enum>
struct EnumPtrListMapSanity {
    static_assert(enumMin<Enum>() == 0,
        "Enum must start with STATE_MIN = 0");

    static_assert(enumCount<Enum>() <= 32,
        "Enum too large for EnumPtrListMap");
};

#endif
