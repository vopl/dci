#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh sqlite https://sqlite.org/2021/sqlite-autoconf-3350400.tar.gz 7771525dff0185bfe9638ccce23faa0e1451757ddbda5a6c853bb80b923a512d
cd ${WDIR}/sqlite

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS} -DSQLITE_ENABLE_COLUMN_METADATA=1" CXXFLAGS="${LOCAL_CXXFLAGS}" ../sqlite-autoconf-3350400/configure --prefix=${PREFIX} --libdir=${LIBDIR}
    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
