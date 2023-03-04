# Distributed Components Environment

Среда для построения распределенных программных систем

## Как собрать
    git clone https://github.com/vopl/dci
    cd dci
    git submodule update --init
    cd ..
    mkdir build
    cd build
    cmake ../dci
    make
    
    cp out/etc/ppn-node.conf.template out/etc/ppn-node.conf
    cd out/bin
    ./dci-host --run ppn-node @../etc/ppn-node.conf

## Обзор на youtube
[![Обзор на youtube](https://img.youtube.com/vi/4yMB9yPSc0I/maxresdefault.jpg)](https://youtu.be/4yMB9yPSc0I)
