#!/bin/bash

set -e

export PREFIX=${HOME}/dci-be
export LIBDIR=${PREFIX}/lib

export WDIR=${HOME}/dci-be-build
export DISTRDIR=${WDIR}/distr

if [[ "$PATH" != *"${PREFIX}/bin"* ]]; then
    export PATH=${PREFIX}/bin:$PATH
fi

if [[ "$LD_LIBRARY_PATH" != *"${PREFIX}/lib"* ]]; then
    PREV_LD_LIBRARY_PATH=$LD_LIBRARY_PATH
    LD_LIBRARY_PATH=${PREFIX}/lib:${PREFIX}/lib64
    if [[ "$PREV_LD_LIBRARY_PATH" != "" ]]; then
        LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$PREV_LD_LIBRARY_PATH
    fi
    export LD_LIBRARY_PATH
fi

export LOCAL_LDFLAGS="-O2 -g -Wl,--enable-new-dtags,-z,origin,-rpath,________________________________________________________________________________________________________________________________,--build-id=sha1"
export LOCAL_CFLAGS="-O2 -g"
export LOCAL_CXXFLAGS="-O2 -g"
