#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh git https://mirrors.edge.kernel.org/pub/software/scm/git/git-2.37.0.tar.xz 9f7fa1711bd00c4ec3dde2fe44407dc13f12e4772b5e3c72a58db4c07495411f
cd ${WDIR}/git

if [ ! -f "install.stamp" ]; then

    pushd git-2.37.0

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ./configure \
        --prefix=${PREFIX} --libdir=${LIBDIR} --with-curl=yes \
exit
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
