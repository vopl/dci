#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh libgpg-error https://gnupg.org/ftp/gcrypt/libgpg-error/libgpg-error-1.51.tar.bz2 be0f1b2db6b93eed55369cdf79f19f72750c8c7c39fc20b577e724545427e6b2
cd ${WDIR}/libgpg-error

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../libgpg-error-1.51/configure --prefix=${PREFIX}
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
