#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

stage=$1
if [[ "$stage" == "" ]]; then
    stage=1
fi

#################################
${CDIR}/prepareBuild.sh gobject-introspection https://download.gnome.org/sources/gobject-introspection/1.84/gobject-introspection-1.84.0.tar.xz 945b57da7ec262e5c266b89e091d14be800cc424277d82a02872b7d794a84779
cd ${WDIR}/gobject-introspection

if [ ! -f "install.stamp" ]; then

    (
        cd gobject-introspection-1.84.0
        LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" meson setup ../build --prefix ${PREFIX}
    )
    (
        cd build
        meson compile
        #meson test
        meson install
    )
    touch install.stamp
fi
