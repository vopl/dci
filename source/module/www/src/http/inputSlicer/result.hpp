// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "pch.hpp"

namespace dci::module::www::http::inputSlicer
{
    /////////0/////////1/////////2/////////3/////////4/////////5/////////6/////////7
    enum class Result
    {
        needMore,

        internalError, //500 Internal Server Error

        badEntity,  //400 Bad Request
        badMethod,  //405 Method Not Allowed
        badVersion, //505 HTTP Version Not Supported
        badStatus,

        tooBigContent,          //413 Content Too Large
        tooBigUri,              //414 URI Too Long
        tooBigHeaders,          //431 Request Header Fields Too Large
        unprocessableContent,   //422 Unprocessable Content

        done,
    };
}
