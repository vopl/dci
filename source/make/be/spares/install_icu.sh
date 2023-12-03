#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh icu https://github.com/unicode-org/icu/archive/refs/tags/release-74-1.tar.gz ca464bfa73bc00ebdb850546514d01f3a983159fda0f7682ff6bf4d3de56844c
cd ${WDIR}/icu

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../icu-release-74-1/icu4c/source/runConfigureICU Linux/gcc --prefix=${PREFIX} --libdir=${LIBDIR}
    make -j`nproc`
    make install

    ESC="$(printf '%s' "$LOCAL_LDFLAGS" | sed 's/[.[\*^$ -]/\\&/g')"
    grep "${ESC}" -l -r ${LIBDIR}/icu | xargs -L1 sed -i "s/${ESC}//g" 2>/dev/null

    popd

    touch install.stamp
fi
