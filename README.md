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

[00:00:00](https://youtu.be/4yMB9yPSc0I&t=0s) приветствие<br>
[00:00:40](https://youtu.be/4yMB9yPSc0I&t=40s) вводная<br>
[00:04:13](https://youtu.be/4yMB9yPSc0I&t=253s) о структуре дальнейшего повествования<br>
[00:06:05](https://youtu.be/4yMB9yPSc0I&t=365s) инструменты для структурирования/систематизации ПО<br>
[00:11:01](https://youtu.be/4yMB9yPSc0I&t=661s) - модульность<br>
[00:18:30](https://youtu.be/4yMB9yPSc0I&t=1110s) - построение API - концепция<br>
[00:19:28](https://youtu.be/4yMB9yPSc0I&t=1168s) - - типы данных<br>
[00:26:03](https://youtu.be/4yMB9yPSc0I&t=1563s) - - декларация интерфейса<br>
[00:38:34](https://youtu.be/4yMB9yPSc0I&t=2314s) - - рантайм<br>
[00:44:05](https://youtu.be/4yMB9yPSc0I&t=2645s) - - - транспорт<br>
[00:47:52](https://youtu.be/4yMB9yPSc0I&t=2872s) - - - управление временем жизни<br>
[00:49:38](https://youtu.be/4yMB9yPSc0I&t=2978s) - - отличия от COM/CORBA<br>
[00:52:30](https://youtu.be/4yMB9yPSc0I&t=3150s) - шлюзы в другие среды<br>
[00:58:03](https://youtu.be/4yMB9yPSc0I&t=3483s) - способ построения программных систем<br>
[01:01:55](https://youtu.be/4yMB9yPSc0I&t=3715s) параллелизация<br>
[01:03:15](https://youtu.be/4yMB9yPSc0I&t=3795s) - о проблемах вытесняющего многопотока<br>
[01:07:40](https://youtu.be/4yMB9yPSc0I&t=4060s) - альтенативы вытесняющему многопотоку<br>
[01:11:17](https://youtu.be/4yMB9yPSc0I&t=4277s) - предлагаемая схема для обеспечения параллелизации<br>
[01:19:58](https://youtu.be/4yMB9yPSc0I&t=4798s) - вариант использования - на многопоточке<br>
[01:22:10](https://youtu.be/4yMB9yPSc0I&t=4930s) - вариант использования - на продолжениях<br>
[01:25:55](https://youtu.be/4yMB9yPSc0I&t=5155s) помогалки<br>
[01:26:20](https://youtu.be/4yMB9yPSc0I&t=5180s) - aup (автоматизация деплоя)<br>
[01:32:05](https://youtu.be/4yMB9yPSc0I&t=5525s) - gui<br>
[01:37:37](https://youtu.be/4yMB9yPSc0I&t=5857s) - ppn (peer-peer network)<br>
[01:45:15](https://youtu.be/4yMB9yPSc0I&t=6315s) - himpl (фаервол компиляции)<br>
[01:50:07](https://youtu.be/4yMB9yPSc0I&t=6607s) - криптуха<br>
[01:51:55](https://youtu.be/4yMB9yPSc0I&t=6715s) - сборочная/интеграционная инфраструктура<br>
[01:56:08](https://youtu.be/4yMB9yPSc0I&t=6968s) - фреймворк для юнит-тестирования<br>
[01:56:40](https://youtu.be/4yMB9yPSc0I&t=7000s) кроссплатформа<br>
[01:58:11](https://youtu.be/4yMB9yPSc0I&t=7091s) как собрать/посмотреть<br>
[02:00:49](https://youtu.be/4yMB9yPSc0I&t=7249s) лицензирование<br>
