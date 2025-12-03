#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

stage=$1
if [[ "$stage" == "" ]]; then
    stage=1
fi

#################################
${CDIR}/prepareBuild.sh gir https://download.gnome.org/sources/gir-repository/0.6/gir-repository-0.6.5.tar.bz2 cbeadc6c701f376134c9fe288fe0d95a725d9fa398daaeeb6621c35e8bafcae1
cd ${WDIR}/gir

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../gir-repository-0.6.5/configure \
        --prefix=${PREFIX} \

    make -j`nproc`
    make install

    popd
    
    touch install.stamp
fi
