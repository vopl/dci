// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <string>
#include <deque>

namespace dci::idl::im
{
    struct PosInSources
    {
    public:
        void add(const std::string& file, int line, int column);
        std::string toString() const;

    private:
        struct Entry
        {
            std::string     _file;
            int             _line;
            int             _column;

            Entry(auto&& file, auto&& line, auto&& column)
                : _file{std::forward<decltype(file)>(file)}
                , _line{static_cast<int>(line)}
                , _column{static_cast<int>(column)}
            {}
        };

        std::deque<Entry> _entries;
    };
}
