// e46c3fd261d639a831722481db0207e8183df2bb2ca1bc825fe853fd61e4b777

#pragma once

#include "api.hpp"

namespace dci::host
{
    class Manager;

    enum class TestStage
    {
        null,
        noenv,  //ничего нет, только самостоятельные функционалы подключеы, logger, himpl, mm, ...
        mnone,  //запущены активные функционалы, poller, cmt. Модулей нет
        mstart  //после старта модулей
    };

    API_DCI_HOST TestStage testStage();
    API_DCI_HOST Manager* testManager();
}
