#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh pugixml https://github.com/zeux/pugixml/releases/download/v1.12.1/pugixml-1.12.1.tar.gz dcf671a919cc4051210f08ffd3edf9e4247f79ad583c61577a13ee93af33afc7
cd ${WDIR}/pugixml

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" cmake ../pugixml-1.12 -DCMAKE_INSTALL_PREFIX=${PREFIX} -DCMAKE_INSTALL_LIBDIR=${LIBDIR} -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=On
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
