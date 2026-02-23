#ifndef ENUM_PTR_MAP_H
#define ENUM_PTR_MAP_H

#include <stdint.h>

/*
    REQUIREMENTS FOR Enum:

    enum class MyEnum {
        STATE_MIN,
        ...
        STATE_MAX = <last valid state>
    };
*/

// ---------- Enum helpers ----------

template <typename Enum>
constexpr uint8_t enumMin() {
    return static_cast<uint8_t>(Enum::STATE_MIN);
}

template <typename Enum>
constexpr uint8_t enumMax() {
    return static_cast<uint8_t>(Enum::STATE_MAX);
}

template <typename Enum>
constexpr uint8_t enumCount() {
    return enumMax<Enum>() - enumMin<Enum>() + 1;
}

template <typename Enum>
constexpr uint8_t enumIndex(Enum e) {
    return static_cast<uint8_t>(e) - enumMin<Enum>();
}

template <typename Enum>
constexpr Enum stateFromIndex(uint8_t index) {
    return static_cast<Enum>(enumMin<Enum>() + index);
}

template <typename Enum>
constexpr uint8_t indexFromState(Enum state) {
    return enumIndex(state);
}

// ---------- Enum → Pointer Map ----------

template <typename Enum, typename PtrType>
class EnumPtrMap {
public:
    static constexpr uint8_t COUNT = enumCount<Enum>();

    PtrType* table[COUNT] = { nullptr };

    void bind(Enum key, PtrType* value) {
        table[enumIndex(key)] = value;
    }

    PtrType* get(Enum key) const {
        return table[enumIndex(key)];
    }

    bool has(Enum key) const {
        return table[enumIndex(key)] != nullptr;
    }

    void clear(Enum key) {
        table[enumIndex(key)] = nullptr;
    }

    void clearAll() {
        for (uint8_t i = 0; i < COUNT; ++i) {
            table[i] = nullptr;
        }
    }
};

// ---------- Compile-time safety ----------

template <typename Enum>
struct EnumPtrMapSanity {
    static_assert(enumMin<Enum>() == 0,
        "Enum must start with STATE_MIN = 0");

    static_assert(enumCount<Enum>() <= 32,
        "EnumPtrMap size too large for embedded use");
};

#endif // ENUM_PTR_MAP_H
