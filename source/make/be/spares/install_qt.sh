#!/bin/bash
set -e
CDIR=`realpath ${BASH_SOURCE%/*}`
source ${CDIR}/env.sh

#################################
#${CDIR}/prepareBuild.sh qt https://qt-mirror.dannhauer.de/official_releases/qt/6.6/6.6.1/single/qt-everywhere-src-6.6.1.tar.xz dd3668f65645fe270bc615d748bd4dc048bd17b9dc297025106e6ecc419ab95d
${CDIR}/prepareBuild.sh qt https://mirror.accum.se/mirror/qt.io/qtproject/official_releases/qt/6.8/6.8.2/single/qt-everywhere-src-6.8.2.tar.xz 659d8bb5931afac9ed5d89a78e868e6bd00465a58ab566e2123db02d674be559

cd ${WDIR}/qt

if [ ! -f "install.stamp" ]; then

    mkdir -p build
    pushd build

    export CFLAGS=${LOCAL_CFLAGS}
    export CXXFLAGS=${LOCAL_CXXFLAGS}
    export LDFLAGS=${LOCAL_LDFLAGS}

    ../qt-everywhere-src-6.8.2/configure \
        \
        -prefix         ${PREFIX} \
        -bindir         ${PREFIX}/bin \
        -headerdir      ${PREFIX}/include \
        -libdir         ${LIBDIR} \
        -archdatadir    ${LIBDIR}/qt6 \
        -datadir        ${LIBDIR}/qt6 \
        -plugindir      ${LIBDIR}/qt6/plugins \
        -libexecdir     ${LIBDIR}/qt6/libexec \
        -qmldir         ${LIBDIR}/qt6/qml \
        -docdir         ${LIBDIR}/qt6/doc \
        -translationdir ${LIBDIR}/qt6/translations \
        -sysconfdir     ${LIBDIR}/qt6/etc/xdg \
        -examplesdir    ${LIBDIR}/qt6/examples \
        -testsdir       ${LIBDIR}/qt6/tests \
        \
        -opensource -confirm-license \
        -release -force-debug-info \
        -shared \
        -sse2 \
        -no-sse3 -no-ssse3 -no-sse4.1 -no-sse4.2 -no-avx -no-avx2 -no-avx512 \
        -system-sqlite \
        -no-opengl \
        \
        -make libs \
        -make tools \
        \
        -nomake examples \
        -nomake tests \
        -nomake benchmarks \
        -nomake manual-tests \
        -nomake minimal-static-tests \
        \
        -skip qtwebengine \
        -skip qtwebview \
        \
        
        #-skip qt3d \
        #-skip qt5compat \
        #-skip qtactiveqt \
        #-skip qtcanvas3d \
        #-skip qtcoap \
        #-skip qtconnectivity \
        #-skip qtdatavis3d \
        #-skip qtdoc \
        #-skip qtfeedback \
        #-skip qtgamepad \
        #-skip qtlocation \
        #-skip qtlottie \
        #-skip qtmqtt \
        #-skip qtmultimedia \
        #-skip qtnetworkauth \
        #-skip qtopcua \
        #-skip qtpim \
        #-skip qtpurchasing \
        #-skip qtqa \
        #-skip qtquick3d \
        #-skip qtquickcontrols \
        #-skip qtremoteobjects \
        #-skip qtrepotools \
        #-skip qtscxml \
        #-skip qtsensors \
        #-skip qtserialbus \
        #-skip qtserialport \
        #-skip qtspeech \
        #-skip qtvirtualkeyboard \
        #-skip qtwayland \
        #-skip qtwebchannel \
        #-skip qtwebglplugin \
        #-skip qtwebsockets \
        #-skip qtmultimedia \
        #-skip qtquick3d \
        #-skip qtwebengine \
        #-skip qtwayland \
        #\

        #-release -force-debug-info \
        #-recheck-all

    ninja -j`nproc`
    ninja install

    popd

    touch install.stamp
fi
