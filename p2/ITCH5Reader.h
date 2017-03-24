#pragma once
#include "EngineTM.h"
#include "FlatHash.h"
#include "ITCH5Messages.h"
#include "L2Book.h"

#include <unordered_map>

class ITCH5Reader
{
public:
    typedef std::unordered_map<uint64_t, uint16_t> FilterMap;

    ITCH5Reader(FilterMap& m, EngineTM& tm)
    : m_symbolFilter(m), m_tm(tm)
    {}

    size_t processMessage(const char* data);
    uint64_t m_lastUpdateTime = 0;
    L2Book* m_lastBook = NULL;

private:
    static const uint32_t SymbolMapSize = 1 << 14;
    FlatHash<uint16_t, L2Book, SymbolMapSize, pow2hash<uint64_t, SymbolMapSize>> m_bookMap;

    FilterMap& m_symbolFilter;
    EngineTM& m_tm;

    L2Book* parseMessage(const SystemEvent& m);
    L2Book* parseMessage(const StockDirectory& m);
    L2Book* parseMessage(const AddOrder& m);
    L2Book* parseMessage(const AddOrderWithMPID& m)
    { return parseMessage(reinterpret_cast<const AddOrder&>(m)); }
    L2Book* parseMessage(const ExecuteOrder& m);
    L2Book* parseMessage(const ExecuteOrderWithPrice& m)
    { return parseMessage(reinterpret_cast<const ExecuteOrder&>(m)); }
    L2Book* parseMessage(const CancelReplaceOrder& m);
    L2Book* parseMessage(const DeleteOrder& m);
    L2Book* parseMessage(const ReplaceOrder& m);
};

