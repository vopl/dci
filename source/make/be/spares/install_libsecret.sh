#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

stage=$1
if [[ "$stage" == "" ]]; then
    stage=1
fi

#################################
${CDIR}/prepareBuild.sh libsecret https://github.com/GNOME/libsecret/archive/refs/tags/0.21.7.tar.gz 944d8a6072b6f285db40b8e9927dbe4dde81dcc7d177f84271fb167ccc297f65
cd ${WDIR}/libsecret

if [ ! -f "install.stamp" ]; then

    (
        cd libsecret-0.21.7
        LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" meson setup ../build --prefix ${PREFIX} \
            -Dgtk_doc=false -Dmanpage=false \
            -Dpam=false
    )
    (
        cd build
        meson compile
        #meson test
        meson install
    )
    touch install.stamp
fi
