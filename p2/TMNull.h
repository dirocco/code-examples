#pragma once

class L2Book;

class TMNull
{
public:
    TMNull(const void*)
    {}

    template<typename FeedHandler>
    inline void bookUpdate(const FeedHandler& fh, const L2Book& book)
    {}

    template<typename FeedHandler>
    inline void systemUpdate(const FeedHandler& fh, L2Book* book)
    {}
};

