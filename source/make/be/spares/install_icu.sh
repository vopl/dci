#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh icu https://github.com/unicode-org/icu/archive/refs/tags/release-77-1.tar.gz ded3a96f6b7236d160df30af46593165b9c78a4ec72a414aa63cf50614e4c14e
cd ${WDIR}/icu

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../icu-release-77-1/icu4c/source/runConfigureICU Linux/gcc --prefix=${PREFIX} --libdir=${LIBDIR}
    make -j`nproc`
    make install

    ESC="$(printf '%s' "$LOCAL_LDFLAGS" | sed 's/[.[\*^$ -]/\\&/g')"
    grep "${ESC}" -l -r ${LIBDIR}/icu | xargs -L1 sed -i "s/${ESC}//g" 2>/dev/null

    popd

    touch install.stamp
fi
