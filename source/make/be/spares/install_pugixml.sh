#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh pugixml https://github.com/zeux/pugixml/releases/download/v1.14/pugixml-1.14.tar.gz 2f10e276870c64b1db6809050a75e11a897a8d7456c4be5c6b2e35a11168a015
cd ${WDIR}/pugixml

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" cmake ../pugixml-1.14 -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_INSTALL_LIBDIR=${LIBDIR} -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=On
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
