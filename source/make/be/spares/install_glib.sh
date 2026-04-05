#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

stage=$1
if [[ "$stage" == "" ]]; then
    stage=1
fi

#################################
${CDIR}/prepareBuild.sh glib https://download.gnome.org/sources/glib/2.88/glib-2.88.0.tar.xz 3546251ccbb3744d4bc4eb48354540e1f6200846572bab68e3a2b7b2b64dfd07
cd ${WDIR}/glib

if [ ! -f "install-${stage}.stamp" ]; then

    if [[ "${stage}" == "1" ]]; then
        SETUP_ARGS=-Dintrospection=disabled
    else
        SETUP_ARGS=-Dintrospection=enabled
    fi

    (
        cd glib-2.88.0
        LDFLAGS="${LOCAL_LDFLAGS}" CFLAGS="${LOCAL_CFLAGS}" CXXFLAGS="${LOCAL_CXXFLAGS}" meson setup ../build-${stage} --prefix ${PREFIX} ${SETUP_ARGS}
        exit
    )
    (
        cd build-${stage}
        meson compile
        #meson test
        meson install
    )

    touch install-${stage}.stamp
fi
