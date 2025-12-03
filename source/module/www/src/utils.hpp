// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::www::utils
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void split(std::string_view str, std::string_view delims, auto&& f)
    {
        std::string_view::size_type pos{};
        for(;;)
        {
            std::string_view::size_type next = str.find_first_of(delims, pos);
            std::string_view sub = str.substr(pos, next-pos);
            if(!sub.empty())
                f(sub);
            if(std::string_view::npos == next)
                break;
            pos = next+1;
        }
    };
}
