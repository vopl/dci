// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#include "pch.hpp"
#include "none.hpp"

namespace dci::module::www::http::compress
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::expected<void, ExceptionPtr> None::initialize()
    {
        return {};
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    std::expected<Bytes, ExceptionPtr> None::exec(Bytes&& content, bool /*finish*/)
    {
        return {std::move(content)};
    }
}
