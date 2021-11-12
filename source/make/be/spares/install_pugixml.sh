#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh pugixml https://github.com/zeux/pugixml/releases/download/v1.11.4/pugixml-1.11.4.tar.gz 8ddf57b65fb860416979a3f0640c2ad45ddddbbafa82508ef0a0af3ce7061716
cd ${WDIR}/pugixml

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" cmake ../pugixml-1.11.4 -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_INSTALL_LIBDIR=${LIBDIR} -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=On
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
