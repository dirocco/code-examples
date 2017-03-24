#pragma once

#include "L2Book.h"

#include <iostream>
#include <fstream>

class TMBookPrint
{
public:
    std::unordered_map<uint64_t, std::ofstream> m_outfiles;
    std::ofstream m_outfile;

    struct Args
    {
        const char* filename;
        bool printSymbol;
    };

    TMBookPrint(const Args* args)
    : m_printSymbol(args->printSymbol)
    {
        m_outfile.open(args->filename);
    }

    bool m_printSymbol = false;

    template<typename FeedHandler>
    inline void bookUpdate(const FeedHandler& fh, const L2Book& book)
    {
        auto bidTop = book.top(Side::bid);
        auto askTop = book.top(Side::ask);

        m_outfile << fh.m_lastUpdateTime << ",";
        if(m_printSymbol)
            m_outfile << stackString(book.m_symbol) << ",";
        if(bidTop.price)
            m_outfile << bidTop.price;
        m_outfile << ",";
        if(bidTop.qty)
            m_outfile << bidTop.qty;
        m_outfile << ",";
        if(askTop.price)
            m_outfile << askTop.price;
        m_outfile << ",";
        if(askTop.qty)
            m_outfile << askTop.qty;
        m_outfile << std::endl;
    }

    template<typename FeedHandler>
    inline void systemUpdate(const FeedHandler& fh, L2Book* book)
    {
        if(!book)
            m_outfile << fh.m_lastUpdateTime << ",,,," << std::endl;
        else bookUpdate(fh, *book);
    }
};

