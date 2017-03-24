#include "ITCH5Reader.h"
#include "ITCH5Messages.h"
#include "BucketTimer.h"
#include "Logging.h"
#include "StatsCounter.h"

#include <assert.h>

#ifdef WRITE_FEED
static FILE* feedcsv = fopen("feed.csv", "w");
#define FEEDDBG(t, m, s, ...) do { \
    fprintf(feedcsv, "%c,%u,%u,%lu," s "\n", \
            t, bswap(m.locate), bswap(m.trackingNumber), m.timestamp.ns(), ##__VA_ARGS__); \
} while(0)

#else
#define FEEDDBG(...)
#endif

static const size_t TimerSamples = 1024;

#define PREAMBLE(msg) \
    incrementStat(Stat::Messages);          \
    incrementStat(Stat:: msg);              \
    auto book = m_bookMap.find(bswap(m.locate));   \
    if(book == NULL) return NULL;           \
    TIMER_SCOPE(msg, TimerSamples);         \
    m_lastUpdateTime = m.timestamp.ns();


size_t ITCH5Reader::processMessage(const char* data)
{
    const ITCHHeader* msg = reinterpret_cast<const ITCHHeader*>(data);
    size_t msgSize = bswap(msg->length) + 2;
    assert(msgSize != 0);

    L2Book* lastBook = 0;

#define DISPATCH(msgType) case msgType ::MessageType: \
    lastBook = parseMessage(reinterpret_cast<const msgType&>(*msg)); break;

    switch(msg->type)
    {
        DISPATCH(SystemEvent);
        DISPATCH(StockDirectory);
        DISPATCH(AddOrder);
        DISPATCH(AddOrderWithMPID);
        DISPATCH(ExecuteOrder);
        DISPATCH(ExecuteOrderWithPrice);
        DISPATCH(CancelReplaceOrder);
        DISPATCH(DeleteOrder);
        DISPATCH(ReplaceOrder);
default:
        return msgSize;
    }
#undef DISPATCH

    if(likely(lastBook))
    {
        m_tm.bookUpdate(*this, *lastBook);
        m_lastBook = lastBook;
    }

    return msgSize;
}

static constexpr Side side(char s)
{
    return s == 'B' ? Side:: bid : Side::ask;
}

L2Book* ITCH5Reader::parseMessage(const StockDirectory& m)
{
    //printf("StockDirectory: %.8s %u\n", m.symbol.s, bswap(m.locate)); // write symbols list to a file?
    auto it = m_symbolFilter.find(m.symbol.n);
    if(it != m_symbolFilter.end())
    {
        incrementStat(Stat::NumSymbols);
        dbg("Found locate %u for symbol %.8s\n", bswap(m.locate), m.symbol.s);
        m_symbolFilter[m.symbol.n] = bswap(m.locate);
        m_bookMap.initialize(bswap(m.locate), bswap(m.locate), m.symbol);
    }

    return NULL;
}

L2Book* ITCH5Reader::parseMessage(const SystemEvent& m)
{
    FEEDDBG(m.type, m, "%c", m.eventCode);

    m_lastUpdateTime = m.timestamp.ns();
    m_tm.systemUpdate(*this, m_lastBook);
    return NULL;
}

L2Book* ITCH5Reader::parseMessage(const AddOrder& m)
{
    PREAMBLE(AddOrder);
    FEEDDBG('A', m, "%lu,%c,%u,%.8s,%u",
            bswap(m.orderRef), m.buySell, bswap(m.qty), m.symbol.truncate().s, bswap(m.price));

    book->insert(bswap(m.orderRef), side(m.buySell), bswap(m.price), bswap(m.qty));
    return book;
}

L2Book* ITCH5Reader::parseMessage(const ExecuteOrder& m)
{
    PREAMBLE(ExecuteOrder);
    FEEDDBG('E', m, "%lu,%u,%lu", bswap(m.orderRef), bswap(m.qty), bswap(m.matchID));

    book->reduce(bswap(m.orderRef), bswap(m.qty));
    return book;
}

L2Book* ITCH5Reader::parseMessage(const CancelReplaceOrder& m)
{
    PREAMBLE(CancelReplaceOrder);
    FEEDDBG(m.type, m, "%lu,%u", bswap(m.orderRef), bswap(m.qty));

    book->reduce(bswap(m.orderRef), bswap(m.qty));
    return book;
}

L2Book* ITCH5Reader::parseMessage(const DeleteOrder& m)
{
    PREAMBLE(DeleteOrder);
    FEEDDBG(m.type, m, "%lu", bswap(m.orderRef));

    book->reduce(bswap(m.orderRef), -1);
    return book;
}

L2Book* ITCH5Reader::parseMessage(const ReplaceOrder& m)
{
    PREAMBLE(ReplaceOrder);
    FEEDDBG(m.type, m, "%lu,%lu,%u,%u", bswap(m.origOrderRef), bswap(m.orderRef), bswap(m.qty), bswap(m.price));

    book->replace(bswap(m.origOrderRef), bswap(m.orderRef), bswap(m.price), bswap(m.qty));
    return book;
}
#undef PREAMBLE
