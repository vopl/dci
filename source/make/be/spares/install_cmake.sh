#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh cmake https://github.com/Kitware/CMake/releases/download/v3.20.0/cmake-3.20.0.tar.gz 9c06b2ddf7c337e31d8201f6ebcd3bba86a9a033976a9aee207fe0c6971f4755
cd ${WDIR}/cmake

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" CMAKE_PREFIX_PATH=${PREFIX} ../cmake-3.20.0/configure --prefix=${PREFIX} --parallel=10
    make -j`nproc`
    make install

    echo 'message("${CMAKE_ROOT}")' > tmp.cmake
    UPC=`${PREFIX}/bin/cmake -P tmp.cmake 2>&1`/Modules/Platform/UnixPaths.cmake
    echo "" >> $UPC
    echo "# patch for dci" >> $UPC
    echo "list(PREPEND CMAKE_SYSTEM_PREFIX_PATH \"${PREFIX}\")" >> $UPC
    echo "list(PREPEND CMAKE_SYSTEM_INCLUDE_PATH \"${PREFIX}/include\")" >> $UPC
    echo "list(PREPEND CMAKE_SYSTEM_LIBRARY_PATH \"${PREFIX}/lib\")" >> $UPC
    echo "list(PREPEND CMAKE_PREFIX_PATH \"${PREFIX}\")" >> $UPC
    echo "list(PREPEND CMAKE_INCLUDE_PATH \"${PREFIX}/include\")" >> $UPC
    echo "list(PREPEND CMAKE_LIBRARY_PATH \"${PREFIX}/lib\")" >> $UPC

    popd

    touch install.stamp
fi
