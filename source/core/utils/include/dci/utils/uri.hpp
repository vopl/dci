/* This file is part of the the dci project. Copyright (C) 2013-2026 vopl, shtoba.
   This program is free software: you can redistribute it and/or modify it under the terms of the GNU Affero General Public
   License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
   This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more details.
   You should have received a copy of the GNU Affero General Public License along with this program. If not, see <https://www.gnu.org/licenses/>. */

// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777
// b15a37183c32a03cae506ae094d1894df6baf99664684d8534c56d9acdecdccf

#pragma once

#include "api.hpp"
#include <string>
#include <string_view>
#include <variant>
#include <optional>

namespace dci::utils::uri
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class String = std::string_view>
    struct Unknown
    {
        String _content{};
        auto operator<=>(const Unknown&) const = default;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class String = std::string_view>
    struct Generic
    {
        String                _scheme{};
        String                _hierPart{};
        std::optional<String> _query{};
        std::optional<String> _fragment{};
        auto operator<=>(const Generic&) const = default;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class String = std::string_view>
    struct Mailto : Generic<String>
    {
        String _value{};
        auto operator<=>(const Mailto&) const = default;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class String = std::string_view>
    struct Inproc : Generic<String>
    {
        String _auth{};
        auto operator<=>(const Inproc&) const = default;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class String = std::string_view>
    struct Local : Generic<String>
    {
        String _auth{};
        auto operator<=>(const Local&) const = default;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class String = std::string_view>
    struct File : Generic<String>
    {
        std::optional<String> _auth{};
        String                _path{};
        auto operator<=>(const File&) const = default;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace networkNode
    {
        template <class String = std::string_view>
        struct Ip4 : String
        {
            auto operator<=>(const Ip4&) const = default;
        };

        template <class String = std::string_view>
        struct Ip6 : String
        {
            auto operator<=>(const Ip6&) const = default;
        };

        template <class String = std::string_view>
        struct IpFuture : String
        {
            auto operator<=>(const IpFuture&) const = default;
        };

        template <class String = std::string_view>
        struct RegName : String
        {
            auto operator<=>(const RegName&) const = default;
        };

        template <class String = std::string_view>
        using Host = std::variant<Ip4<String>, Ip6<String>, IpFuture<String>, RegName<String>>;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class String = std::string_view>
    struct NetworkNode
    {
        networkNode::Host<String> _host{};
        std::optional<String>     _port{};
        auto operator<=>(const NetworkNode&) const = default;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class String = std::string_view>
    struct TCP : Generic<String>
    {
        NetworkNode<String> _auth{};
        auto operator<=>(const TCP&) const = default;
    };

    template <class String = std::string_view>
    struct TCP4 : TCP<String>
    {
        auto operator<=>(const TCP4&) const = default;
    };

    template <class String = std::string_view>
    struct TCP6 : TCP<String>
    {
        auto operator<=>(const TCP6&) const = default;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class String = std::string_view>
    struct UDP : Generic<String>
    {
        NetworkNode<String> _auth{};
        auto operator<=>(const UDP&) const = default;
    };

    template <class String = std::string_view>
    struct UDP4 : UDP<String>
    {
        auto operator<=>(const UDP4&) const = default;
    };

    template <class String = std::string_view>
    struct UDP6 : UDP<String>
    {
        auto operator<=>(const UDP6&) const = default;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    namespace www
    {
        template <class String = std::string_view>
        struct Userinfo
        {
            String                _name{};
            std::optional<String> _password{};
            auto operator<=>(const Userinfo&) const = default;
        };

        template <class String = std::string_view>
        struct Authority
        {
            std::optional<www::Userinfo<String>> _userinfo{};
            NetworkNode<String>                  _networkNode{};
            auto operator<=>(const Authority&) const = default;
        };
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class String = std::string_view>
    struct WWW : Generic<String>
    {
        www::Authority<String> _auth{};
        String                 _path{};
        auto operator<=>(const WWW&) const = default;
    };

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class String = std::string_view>
    struct HTTP  : WWW<String>
    {
        auto operator<=>(const HTTP&) const = default;
    };

    template <class String = std::string_view>
    struct HTTPS : WWW<String>
    {
        auto operator<=>(const HTTPS&) const = default;
    };

    template <class String = std::string_view>
    struct FTP   : WWW<String>
    {
        auto operator<=>(const FTP&) const = default;
    };

    template <class String = std::string_view>
    struct FTPS  : WWW<String>
    {
        auto operator<=>(const FTPS&) const = default;
    };
}

namespace dci::utils
{
    template <class String = std::string_view>
    using URI = std::variant
    <
        uri::Unknown<String>,
        uri::Generic<String>,

        uri::Mailto<String>,

        uri::Inproc<String>,
        uri::Local<String>,
        uri::File<String>,

        uri::TCP<String>,
        uri::TCP4<String>,
        uri::TCP6<String>,

        uri::UDP<String>,
        uri::UDP4<String>,
        uri::UDP6<String>,

        uri::WWW<String>,
        uri::HTTP<String>,
        uri::HTTPS<String>,
        uri::FTP<String>,
        uri::FTPS<String>
    >;
}

namespace dci::utils::uri
{
    API_DCI_UTILS bool parse(std::string_view src, URI<std::string_view>& dst);
    API_DCI_UTILS bool parse(std::string_view src, URI<std::string>&      dst);

    API_DCI_UTILS bool parse(std::string_view src, NetworkNode<std::string_view>& dst);
    API_DCI_UTILS bool parse(std::string_view src, NetworkNode<std::string>& dst);

    API_DCI_UTILS bool parse(std::string_view src, networkNode::Host<std::string_view>& dst);
    API_DCI_UTILS bool parse(std::string_view src, networkNode::Host<std::string>& dst);

    API_DCI_UTILS bool parse(std::string_view src, networkNode::Ip4<std::string_view>& dst);
    API_DCI_UTILS bool parse(std::string_view src, networkNode::Ip4<std::string>& dst);

    API_DCI_UTILS bool parse(std::string_view src, networkNode::Ip6<std::string_view>& dst);
    API_DCI_UTILS bool parse(std::string_view src, networkNode::Ip6<std::string>& dst);

    API_DCI_UTILS bool parse(std::string_view src, networkNode::RegName<std::string_view>& dst);
    API_DCI_UTILS bool parse(std::string_view src, networkNode::RegName<std::string>& dst);

    API_DCI_UTILS bool valid(std::string_view src);
    template <class... Alts> bool valid(std::string_view src);

    API_DCI_UTILS std::string_view scheme(std::string_view src);
    API_DCI_UTILS std::string_view scheme(const URI<std::string_view>& src);
    API_DCI_UTILS std::string_view scheme(const URI<std::string>& src);
    API_DCI_UTILS std::string_view host(std::string_view src);
    API_DCI_UTILS std::string_view host(const URI<std::string_view>& src);
    API_DCI_UTILS std::string_view host(const URI<std::string>& src);
    API_DCI_UTILS std::string hostPort(std::string_view src);
    API_DCI_UTILS std::string hostPort(const URI<std::string_view>& src);
    API_DCI_UTILS std::string hostPort(const URI<std::string>& src);
    API_DCI_UTILS bool isCover(std::string_view base, std::string_view target);
    API_DCI_UTILS bool isCover(const URI<std::string_view>& base, const URI<std::string_view>& target);
    API_DCI_UTILS bool isCover(const URI<std::string>& base, const URI<std::string>& target);
}

namespace dci::utils::uri
{
    template <class Out, class String> Out& operator<<(Out& out, const Unknown<String>& in);
    template <class Out, class String> Out& operator<<(Out& out, const Generic<String>& in);
    template <class Out, class String> Out& operator<<(Out& out, const Mailto<String>& in);
    template <class Out, class String> Out& operator<<(Out& out, const File<String>& in);
    template <class Out, class String> Out& operator<<(Out& out, const NetworkNode<String>& in);
    template <class Out, class String> Out& operator<<(Out& out, const TCP<String>& in);
    template <class Out, class String> Out& operator<<(Out& out, const UDP<String>& in);
    template <class Out, class String> Out& operator<<(Out& out, const www::Userinfo<String>& in);
    template <class Out, class String> Out& operator<<(Out& out, const www::Authority<String>& in);
    template <class Out, class String> Out& operator<<(Out& out, const WWW<String>& in);
}

namespace dci::utils
{
    template <class Out, class String> Out& operator<<(Out& out, const URI<String>& in);
}

namespace dci::utils::uri
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Out, class String> Out& operator<<(Out& out, const Unknown<String>& in)
    {
        out << in._content;
        return out;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Out, class String> Out& operator<<(Out& out, const Generic<String>& in)
    {
        out << in._scheme << ':';
        out << in._hierPart;
        if(in._query) out << '?' << *in._query;
        if(in._fragment) out << '#' << *in._fragment;
        return out;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Out, class String> Out& operator<<(Out& out, const Mailto<String>& in)
    {
        using namespace std::string_view_literals;
        out << "mailto:"sv;
        out << in._value;
        if(in._query) out << '?' << *in._query;
        if(in._fragment) out << '#' << *in._fragment;
        return out;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Out, class String> Out& operator<<(Out& out, const File<String>& in)
    {
        using namespace std::string_view_literals;
        out << "file://"sv;
        if(in._auth)
            out << *in._auth;
        out << in._path;
        if(in._query) out << '?' << *in._query;
        if(in._fragment) out << '#' << *in._fragment;
        return out;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Out, class String> Out& operator<<(Out& out, const NetworkNode<String>& in)
    {
        using namespace networkNode;

        std::visit([&]<class Alt>(const Alt& alt)
        {
            if constexpr (std::is_same_v<Alt, Ip6<String>> || std::is_same_v<Alt, IpFuture<String>>)
                out << '[' << alt << ']';
            else
                out << alt;
        }, in._host);

        if(in._port) out << ':' << *in._port;
        return out;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Out, class String> Out& operator<<(Out& out, const TCP<String>& in)
    {
        using namespace std::string_view_literals;
        out << in._scheme << "://"sv;
        out << in._auth;
        if(in._query) out << '?' << *in._query;
        if(in._fragment) out << '#' << *in._fragment;
        return out;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Out, class String> Out& operator<<(Out& out, const UDP<String>& in)
    {
        using namespace std::string_view_literals;
        out << in._scheme << "://"sv;
        out << in._auth;
        if(in._query) out << '?' << *in._query;
        if(in._fragment) out << '#' << *in._fragment;
        return out;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Out, class String> Out& operator<<(Out& out, const www::Userinfo<String>& in)
    {
        out << in._name;
        if(in._password) out << ':' << *in._password;
        return out;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Out, class String> Out& operator<<(Out& out, const www::Authority<String>& in)
    {
        if(in._userinfo) out << *in._userinfo << '@';
        out <<in._networkNode;
        return out;
    }

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Out, class String> Out& operator<<(Out& out, const WWW<String>& in)
    {
        using namespace std::string_view_literals;
        out << in._scheme << "://"sv;
        out << in._auth;
        out << in._path;
        if(in._query) out << '?' << *in._query;
        if(in._fragment) out << '#' << *in._fragment;
        return out;
    }
}

namespace dci::utils
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class Out, class String> Out& operator<<(Out& out, const URI<String>& in)
    {
        std::visit([&](const auto& alt) { out << alt; }, in);
        return out;
    }
}

namespace dci::utils::uri
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    template <class... Alts> bool valid(std::string_view src)
    {
        URI<std::string_view> tmp;
        return parse(src, tmp) && (std::holds_alternative<Alts>(tmp) || ...);
    }
}
