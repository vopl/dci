#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh readline ftp://ftp.gnu.org/pub/gnu/readline/readline-8.3.tar.gz fe5383204467828cd495ee8d1d3c037a7eba1389c22bc6a041f627976f9061cc
cd ${WDIR}/readline

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../readline-8.3/configure --prefix ${PREFIX} --with-curses --with-shared-termcap-library

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
