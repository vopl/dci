#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
${CDIR}/prepareBuild.sh icu https://github.com/unicode-org/icu/archive/release-68-2.tar.gz f790b0202facbbf19c5581a7a5f21b2b4b6ed70ba3e4bef8d5560868e5e82476
cd ${WDIR}/icu

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" ../icu-release-68-2/icu4c/source/runConfigureICU Linux/gcc --prefix=${PREFIX} --libdir=${LIBDIR}
    make -j`nproc`
    make install

    ESC="$(printf '%s' "$LOCAL_LDFLAGS" | sed 's/[.[\*^$ -]/\\&/g')"
    grep "${ESC}" -l -r ${LIBDIR}/icu | xargs -L1 sed -i "s/${ESC}//g" 2>/dev/null

    popd

    touch install.stamp
fi
