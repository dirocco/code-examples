#include "L2Book.h"
#include "StatsCounter.h"

#include <cmath>

L2Book::OrdersMap L2Book::s_orders;

template<typename Book>
void L2Book::insert(uint64_t orderRef, Book& book, Side buySell, Price price, Quantity qty)
{
    PriceNode node = {price, qty};
    auto pos = book.find(node);
    if(pos == book.end())
        pos = book.insert(pos, node);
    else pos->qty += qty;

    s_orders[orderRef] = Order(orderRef, buySell, qty, pos);

    auto bidTop = top(Side::bid);
    auto askTop = top(Side::ask);
    if(askTop.price != 0 && bidTop.price >= askTop.price)
        critical_err(-1, "book crossed on order %lx\n", orderRef);
}

template<typename Book, typename PriceLevel>
void L2Book::reduce(Order* o, Book& book, PriceLevel& priceLevel, Quantity qty)
{
    if(qty == -1 || o->qty == qty)
    {
        priceLevel->qty -= o->qty;
        o->m_initialized = false;
    }
    else
        priceLevel->qty -= qty;

    o->qty -= qty;

    if(priceLevel->qty == 0)
        book.erase(priceLevel);
}

void L2Book::insert(uint64_t orderRef, Side buySell, Price price, Quantity qty)
{
    if(buySell == Side::bid)
        insert(orderRef, m_bidBook, buySell, price, qty);
    else
        insert(orderRef, m_askBook, buySell, price, qty);
}

void L2Book::replace(uint64_t origOrderRef, uint64_t newOrderRef, Price newPrice, Quantity newQty)
{
    auto it = s_orders.find(origOrderRef);
    if(unlikely(it == s_orders.end()))
        critical_err(-1, "Couldn't find order %lx", origOrderRef);
    auto& o = it->second;

    if(o.side() == Side::bid)
        reduce(&o, m_bidBook, o.bidLevel, -1);
    else
        reduce(&o, m_askBook, o.askLevel, -1);

    insert(newOrderRef, o.side(), newPrice, newQty);
}

void L2Book::reduce(uint64_t orderRef, Quantity qty)
{
    auto it = s_orders.find(orderRef);
    if(unlikely(it == s_orders.end()))
        critical_err(-1, "Couldn't find order %lx", orderRef);
    auto& o = it->second;

    if(o.side() == Side::bid)
        reduce(&o, m_bidBook, o.bidLevel, qty);
    else
        reduce(&o, m_askBook, o.askLevel, qty);
}

L2Book::PriceNode L2Book::top(Side buySell) const
{
    if(buySell == Side::bid)
        return *m_bidBook.begin();
    return *m_askBook.begin();
}

