#include "L2Book.h"
#include "FlatHash.h"
#include "L2Book.h"

#include <set>

int main(int argc, char** argv)
{
    {
        printf("Testing FlatHash\n");
        static const int mapSize = 8;
        struct Order : FlatHashable
        {
            Order() {}
            Order(int k) : orderRef(k) {}

            uint64_t orderRef = 0;
            using hash = pow2hash<uint64_t, mapSize>;
            bool operator==(const uint64_t& k)
            {
                return orderRef == k;
            }
        };

        FlatHash<uint64_t, Order, mapSize, Order::hash> m_flathash;

        for(int i = 1; i < mapSize + 1; i++)
        {
            bool collision = m_flathash.initialize(i, i);
            if(collision)
                err("collided on %d for size %d\n", i, mapSize);
        }

        for(int i = 1; i < mapSize + 1; i++)
        {
            auto o = m_flathash.find(i);
            if(o == NULL)
                err("couldn't find element %d\n", i);
        }

        for(int i = 1; i < mapSize + 1; i++)
        {
            m_flathash.erase(i);

            auto o = m_flathash.find(i);
            if(o != NULL)
                err("element %d was not erased\n", i);
        }

    }

    {
        printf("Testing L2Book\n");
        L2Book book;
        uint64_t orderCounter = 1;
        auto expectTop = [&](Side buySell, Price p, Quantity q)
        {
            auto level = book.top(buySell);
            if(level.price != p || level.qty != q)
                err("%s book mismatch, expected %u@%u got %u@%u\n",
                       stackString(buySell), q, p, level.qty, level.price);
        };

        // bid tests
        book.insert(orderCounter++, Side::bid, 100, 5);
        expectTop(Side::bid, 100, 5);
        book.insert(orderCounter++, Side::bid, 99, 1);
        expectTop(Side::bid, 100, 5);
        book.insert(orderCounter++, Side::bid, 101, 1);
        expectTop(Side::bid, 101, 1);

        book.reduce(3, 1);
        expectTop(Side::bid, 100, 5);

        book.reduce(1, 2);
        expectTop(Side::bid, 100, 3);

        book.reduce(1, -1);
        expectTop(Side::bid, 99, 1);

        // ask tests
        book.insert(orderCounter++, Side::ask, 100, 5);
        expectTop(Side::ask, 100, 5);
        book.insert(orderCounter++, Side::ask, 101, 1);
        expectTop(Side::ask, 100, 5);
        book.insert(orderCounter++, Side::ask, 99, 1);
        expectTop(Side::ask, 99, 1);

        book.reduce(6, 1);
        expectTop(Side::ask, 100, 5);

        book.reduce(4, 2);
        expectTop(Side::ask, 100, 3);

        book.reduce(4, -1);
        expectTop(Side::ask, 101, 1);

        // multiple order on same level test
        book.insert(orderCounter++, Side::bid, 99, 2);
        book.insert(orderCounter++, Side::ask, 101, 2);
        expectTop(Side::bid, 99, 3);
        expectTop(Side::ask, 101, 3);

        // replace test
        book.replace(7, orderCounter++, 98, 1);
        expectTop(Side::bid, 99, 1);

        book.replace(8, orderCounter++, 102, 1);
        expectTop(Side::ask, 101, 1);
    }
}

