#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh ncurses https://ftp.gnu.org/pub/gnu/ncurses/ncurses-6.5.tar.gz 136d91bc269a9a5785e5f9e980bc76ab57428f604ce3e5a5a90cebc767971cc6
#${CDIR}/prepareBuild.sh ncurses https://ftp.gnu.org/pub/gnu/ncurses/ncurses-6.6.tar.gz 355b4cbbed880b0381a04c46617b7656e362585d52e9cf84a67e2009b749ff11
cd ${WDIR}/ncurses

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS} -std=gnu17" CXXFLAGS="${LOCAL_CXXFLAGS} -std=gnu17" ../ncurses-6.5/configure \
        --prefix ${PREFIX} \
        --with-shared \
        --disable-widec \
        --enable-pc-files \
        --with-pkg-config-libdir=$PREFIX/lib/pkgconfig \
        --enable-termcap \
        --with-termlib \
        --enable-ext-colors \
        --enable-ext-mouse

    make -j`nproc`
    make install

    popd

    touch install.stamp
fi
