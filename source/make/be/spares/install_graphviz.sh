#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

stage=$1
if [[ "$stage" == "" ]]; then
    stage=1
fi

#################################
${CDIR}/prepareBuild.sh graphviz https://gitlab.com/api/v4/projects/4207231/packages/generic/graphviz-releases/12.2.1/graphviz-12.2.1.tar.xz 85e34b5c982777c30f01dfab9ea7c713b4335a2f584e62c0abb9868413eb915b
cd ${WDIR}/graphviz

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../graphviz-12.2.1/configure --prefix=${PREFIX}
    make -j`nproc`
    make install

    popd
    
    touch install.stamp
fi
