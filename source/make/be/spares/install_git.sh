#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh git https://mirrors.edge.kernel.org/pub/software/scm/git/git-2.32.0.tar.xz 68a841da3c4389847ecd3301c25eb7e4a51d07edf5f0168615ad6179e3a83623
cd ${WDIR}/git

if [ ! -f "install.stamp" ]; then

    pushd git-2.32.0

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ./configure \
        --prefix=${PREFIX} --libdir=${LIBDIR} --with-curl=yes \
exit
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
