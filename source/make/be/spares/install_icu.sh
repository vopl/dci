#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh icu https://github.com/unicode-org/icu/archive/refs/tags/release-71-1.tar.gz d88a4ea7a4a28b445bb073a6cfeb2a296bf49a4a2fe5f1b49f87ecb4fc55c51d
cd ${WDIR}/icu

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../icu-release-71-1/icu4c/source/runConfigureICU Linux/gcc --prefix=${PREFIX} --libdir=${LIBDIR}
    make -j`nproc`
    make install

    ESC="$(printf '%s' "$LOCAL_LDFLAGS" | sed 's/[.[\*^$ -]/\\&/g')"
    grep "${ESC}" -l -r ${LIBDIR}/icu | xargs -L1 sed -i "s/${ESC}//g" 2>/dev/null

    popd

    touch install.stamp
fi
