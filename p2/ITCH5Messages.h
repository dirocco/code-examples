#pragma once

#include "BaseTypes.h"

#include <stdint.h>
#include <ctype.h>

#pragma pack(push)
#pragma pack(1)

template<typename IntType>
inline IntType bswap(IntType in)
{
    static_assert(sizeof(IntType) == 0, "Undefined bswap called");
}

template<> inline uint64_t bswap(uint64_t in) { return __builtin_bswap64(in); }
template<> inline uint32_t bswap(uint32_t in) { return __builtin_bswap32(in); }
template<> inline uint16_t bswap(uint16_t in) { return __builtin_bswap16(in); }

struct ITCH5Timestamp
{
    union
    {
        char data[6];
        uint64_t n : 48;
    };

    uint64_t ns() const
    {
        return (((n & 0x0000000000ff) << 40) |
                ((n & 0x00000000ff00) << 24) |
                ((n & 0x000000ff0000) << 8)  |
                ((n & 0x0000ff000000) >> 8)  |
                ((n & 0x00ff00000000) >> 24) |
                ((n & 0xff0000000000) >> 40));
    }
};
static_assert(sizeof(ITCH5Timestamp) == 6, "Symbol miscompiled");

struct ITCHHeader
{
    uint16_t length;
    char type;
    uint16_t locate;
    uint16_t trackingNumber;
    ITCH5Timestamp timestamp;
};

struct SystemEvent : ITCHHeader
{
    static constexpr char MessageType = 'S';

    char eventCode;
};

struct StockDirectory : ITCHHeader
{
    static constexpr char MessageType = 'R';

    Symbol symbol;
    // etc
};

struct StockTradingAction : ITCHHeader
{
    static constexpr char MessageType = 'H';

    Symbol symbol;
    char tradingState;
    char reserved;
    char reason[4];
};

struct AddOrder : ITCHHeader
{
    static constexpr char MessageType = 'A';

    uint64_t orderRef;
    char buySell;
    uint32_t qty;
    Symbol symbol;
    uint32_t price;
};

struct AddOrderWithMPID : AddOrder
{
    static constexpr char MessageType = 'F';
};

struct ExecuteOrder : ITCHHeader
{
    static constexpr char MessageType = 'E';

    uint64_t orderRef;
    uint32_t qty;
    uint64_t matchID;
};

struct ExecuteOrderWithPrice : ExecuteOrder
{
    static constexpr char MessageType = 'C';
};

struct CancelReplaceOrder : ITCHHeader
{
    static constexpr char MessageType = 'X';

    uint64_t orderRef;
    uint32_t qty;
};

struct DeleteOrder : ITCHHeader
{
    static constexpr char MessageType = 'D';

    uint64_t orderRef;
};

struct ReplaceOrder : ITCHHeader
{
    static constexpr char MessageType = 'U';

    uint64_t origOrderRef;
    uint64_t orderRef;
    uint32_t qty;
    uint32_t price;
};

#pragma pack(pop)

