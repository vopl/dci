#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh python https://www.python.org/ftp/python/3.13.2/Python-3.13.2.tar.xz d984bcc57cd67caab26f7def42e523b1c015bbc5dc07836cf4f0b63fa159eb56
cd ${WDIR}/python

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build
    
    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../Python-3.13.2/configure \
        --prefix ${PREFIX} \
        --enable-optimizations --enable-lto \
        --with-computed-gotos \
        --disable-test-modules \
        --enable-shared \
        --enable-ipv6 \
        --enable-loadable-sqlite-extensions \
        --with-system-libmpdec \
        --with-system-expat \
        --with-openssl=${PREFIX} \
        --with-ssl-default-suites=openssl \
        --with-readline=yes \

    make -j`nproc`
    make install
    [ -e ${PREFIX}/bin/python ] || ln -r -s ${PREFIX}/bin/python3 ${PREFIX}/bin/python

    popd

    touch install.stamp
fi
