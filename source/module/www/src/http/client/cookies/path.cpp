// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "path.hpp"

namespace dci::module::www::http::client::cookies::path
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void directoryFromResource(std::string& path)
    {
        if(path.empty() || path[0] != '/')
        {
            path = "/";
            return;
        }

        for(std::size_t i{1}; i<path.size(); )
        {
            if(path[i-1] == '/' && path[i] == '/')
                path.erase(i, 1);
            else
                ++i;
        }

        while(!path.empty() && path.back() != '/')
            path.pop_back();
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    void canonicalize(std::string& path, std::string_view default_)
    {
        if(path.empty() || path[0] != '/')
        {
            path = default_;
            return;
        }

        for(std::size_t i{1}; i<path.size(); )
        {
            if(path[i-1] == '/' && path[i] == '/')
                path.erase(i, 1);
            else
                ++i;
        }

        if(path.back() != '/')
            path.push_back('/');
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    bool matched(std::string_view target, std::string_view pattern)
    {
        dbgAssert(!target.empty());
        dbgAssert(target.front() == '/');
        dbgAssert(target.back() == '/');

        dbgAssert(!pattern.empty());
        dbgAssert(pattern.front() == '/');
        dbgAssert(pattern.back() == '/');

        return target.starts_with(pattern);
    }
}
