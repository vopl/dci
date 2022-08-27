#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh sqlite https://sqlite.org/2022/sqlite-autoconf-3390200.tar.gz 852be8a6183a17ba47cee0bbff7400b7aa5affd283bf3beefc34fcd088a239de
cd ${WDIR}/sqlite

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS} -DSQLITE_ENABLE_COLUMN_METADATA=1" CXXFLAGS="${LOCAL_CXXFLAGS}" ../sqlite-autoconf-3390200/configure --prefix=${PREFIX} --libdir=${LIBDIR}
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
