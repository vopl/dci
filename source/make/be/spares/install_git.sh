#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh git https://mirrors.edge.kernel.org/pub/software/scm/git/git-2.49.0.tar.xz 618190cf590b7e9f6c11f91f23b1d267cd98c3ab33b850416d8758f8b5a85628
cd ${WDIR}/git

if [ ! -f "install.stamp" ]; then

    pushd git-2.49.0

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ./configure \
        --prefix=${PREFIX} --libdir=${LIBDIR} --with-curl=yes \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
