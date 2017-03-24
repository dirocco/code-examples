#pragma once

#include "BaseTypes.h"
#include "FlatHash.h"

#include <stddef.h>
#include <assert.h>
#include <set>
#include <unordered_map>

class L2Book : public FlatHashable
{
private:
    L2Book& operator=(const L2Book&) = delete;
    L2Book(const L2Book&) = delete;
public:
    struct PriceNode
    {
        Price price;
        mutable Quantity qty;
    } __attribute__((packed));
    static_assert(sizeof(PriceNode) <= 8, "PriceNode packing failure");

public:
    struct BidCompare
    {
        bool operator()(const PriceNode& l, const PriceNode& r)
        {
            return l.price > r.price;
        }
    };

    struct AskCompare
    {
        bool operator()(const PriceNode& l, const PriceNode& r)
        {
            return l.price < r.price;
        }
    };

    typedef std::set<PriceNode, BidCompare> BidBook;
    typedef std::set<PriceNode, AskCompare> AskBook;

    BidBook m_bidBook __attribute((aligned(64)));
    AskBook m_askBook __attribute((aligned(64)));

    static constexpr int OrderMapSize = 1 * 1024 * 1024;
    struct Order : FlatHashable
    {
        Quantity qty : 31;
        uint8_t buySell: 1;
        uint64_t orderRef;

        inline Side side() { return buySell ? Side::bid : Side::ask; }

        union
        {
            BidBook::iterator bidLevel;
            AskBook::iterator askLevel;
        };

        Order() {}
        explicit Order(uint64_t orderRef, Side buySell, Quantity qty, BidBook::const_iterator priceLevel)
        : orderRef(orderRef), buySell((int)buySell), qty(qty), bidLevel(priceLevel)
        {}

        using hash = pow2hash<uint64_t, OrderMapSize>;

        struct equal
        {
            bool operator()(const Order& o1, const Order& o2)
            {
                return o1.orderRef == o2.orderRef;
            }
        };
    };
    typedef std::unordered_map<uint64_t, Order> OrdersMap;
    static OrdersMap s_orders;

    template<typename Book>
    void insert(uint64_t orderRef, Book& book, Side buySell, Price price, Quantity qty);

    template<typename Book, typename PriceLevel>
    void reduce(Order* o, Book& book, PriceLevel& priceLevel, Quantity qty);

    uint16_t m_locate = 0;

    void insert(uint64_t orderRef, Side buySell, Price p, Quantity q);
    void replace(uint64_t origOrderRef, uint64_t newOrderRef, Price newPrice, Quantity newQty);
    void reduce(uint64_t orderRef, Quantity q);

    Symbol m_symbol;

    L2Book(uint16_t locate = 0, Symbol symbol = Symbol::None())
    : m_locate(locate), m_symbol(symbol)
    {}

    bool operator==(uint16_t locate)
    {
        assert(m_locate == locate);  // should never hash collide
        return true;
    }

    bool initialized() const
    {
        return m_locate != 0;
    }

    PriceNode top(Side buySell) const;

} __attribute__((aligned(64)));

