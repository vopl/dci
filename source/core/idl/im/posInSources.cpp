// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "posInSources.hpp"

namespace dci::idl::im
{
    void PosInSources::add(const std::string& file, int line, int column)
    {
        _entries.emplace_back(file, line, column);
    }

    std::string PosInSources::toString() const
    {
        std::string res;

        bool first = true;
        for(const Entry& entry : _entries)
        {
            if(first)
            {
                first = false;
            }
            else
            {
                res += ": included from here\n";
            }

            res += entry._file;

            if(entry._line>=0)
            {
                res += ":";
                res += std::to_string(entry._line);

                if(entry._column>=0)
                {
                    res += ":";
                    res += std::to_string(entry._column);
                }
            }
        }

        return res;
    }
}
