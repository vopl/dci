#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh git https://mirrors.edge.kernel.org/pub/software/scm/git/git-2.43.0.tar.xz 5446603e73d911781d259e565750dcd277a42836c8e392cac91cf137aa9b76ec
cd ${WDIR}/git

if [ ! -f "install.stamp" ]; then

    pushd git-2.43.0

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ./configure \
        --prefix=${PREFIX} --libdir=${LIBDIR} --with-curl=yes \

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
