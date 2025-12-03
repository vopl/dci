// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include <dci/test.hpp>
#include <dci/host.hpp>
#include "ppn/node/rdb.hpp"

using namespace dci;
using namespace dci::host;

using namespace dci::idl::gen::ppn::node::rdb::pql;

namespace
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Value vNull     {};

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Value vBool     {false};

    const Value vUint8    {uint8{}};
    const Value vUint16   {uint16{}};
    const Value vUint32   {uint32{}};
    const Value vUint64   {uint64{}};

    const Value vInt8     {int8{}};
    const Value vInt16    {int16{}};
    const Value vInt32    {int32{}};
    const Value vInt64    {int64{}};

    const Value vReal32   {real32{}};
    const Value vReal64   {real64{}};

    const Value vString   {String{}};

    const Value vId128    {id128{}};
    const Value vId256    {id256{}};
    const Value vId512    {id512{}};

    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    const Value vBool_1     {true};

    const Value vUint8_1    {uint8{1}};
    const Value vUint16_1   {uint16{1}};
    const Value vUint32_1   {uint32{1}};
    const Value vUint64_1   {uint64{1}};

    const Value vInt8_1     {int8{1}};
    const Value vInt16_1    {int16{1}};
    const Value vInt32_1    {int32{1}};
    const Value vInt64_1    {int64{1}};

    const Value vReal32_1   {real32{1}};
    const Value vReal64_1   {real64{1}};

    const Value vString_1   {String{"1"}};

    const Value vId128_1    {id128{0x1}};
    const Value vId256_1    {id256{0x1}};
    const Value vId512_1    {id512{0x1}};


    template <fun f, class T>
    T evalTo(const auto&...args)
    {
        ExpressionEvaluator4Tests<> e = testManager()->createService<ExpressionEvaluator4Tests<>>().value();
        Value v = e->evaluate(Expression{Node{f, List<Expression>{        args...       }}}).value();

        if(!v.data.holds<T>())
        {
            throw "unexpected result type";
        }

        return v.data.get<T>();
    }

    template <fun f>
    bool evalToBool(const auto&...args)
    {
        return evalTo<f, bool>(args...);
    }

    template <fun f>
    int evalToInt(const auto&...args)
    {
        return evalTo<f, int>(args...);
    }

    template <fun f>
    real64 evalToReal(const auto&...args)
    {
        return evalTo<f, real64>(args...);
    }

    template <fun f>
    String evalToString(const auto&...args)
    {
        return evalTo<f, String>(args...);
    }

    template <fun f>
    Null evalToNull(const auto&...args)
    {
        return evalTo<f, Null>(args...);
    }

}
