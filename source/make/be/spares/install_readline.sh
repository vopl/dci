#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh readline http://git.savannah.gnu.org/cgit/readline.git/snapshot/readline-8.2.tar.gz a3d4637cdbd76f3cbc9566db90306a6af7bef90b291f7c9bc5fd8b0b0db9c686
cd ${WDIR}/readline

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../readline-8.2/configure --prefix ${PREFIX}

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
